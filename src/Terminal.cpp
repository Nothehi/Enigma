#include "Terminal.hpp"

#include <cerrno>
#include <format>
#include <iterator>
#include <stdexcept>
#include <system_error>

#include <sys/ioctl.h>
#include <unistd.h>

namespace
{
    [[noreturn]] void throwErrno(const char* what)
    {
        throw std::system_error(errno, std::generic_category(), what);
    }

    TerminalSize querySize()
    {
        winsize size{};
        if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) != 0)
            throwErrno("ioctl(TIOCGWINSZ)");

        return {size.ws_row, size.ws_col};
    }
}

Terminal::Terminal()
{
    if (!::isatty(STDIN_FILENO) || !::isatty(STDOUT_FILENO))
        throw std::runtime_error("stdin and stdout must be a terminal");

    m_size = querySize();

    if (::tcgetattr(STDIN_FILENO, &m_originalMode) != 0)
        throwErrno("tcgetattr");

    // Keys arrive one at a time and unechoed; Ctrl+C / Ctrl+D reach us as plain bytes.
    termios raw = m_originalMode;
    raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (::tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0)
        throwErrno("tcsetattr");

    write("\033[?1049h\033[2J");
    flush();
}

Terminal::~Terminal()
{
    write("\033[r\033[?1049l");
    flush();
    ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_originalMode);
}

void Terminal::moveCursor(int row, int col)
{
    std::format_to(std::back_inserter(m_buffer), "\033[{};{}H", row, col);
}

void Terminal::setScrollRegion(int top, int bottom)
{
    std::format_to(std::back_inserter(m_buffer), "\033[{};{}r", top, bottom);
}

void Terminal::clearToEndOfLine()
{
    m_buffer += "\033[K";
}

void Terminal::setForeground(Rgb color)
{
    std::format_to(std::back_inserter(m_buffer), "\033[38;2;{};{};{}m", color.r, color.g, color.b);
}

void Terminal::resetStyle()
{
    m_buffer += "\033[0m";
}

void Terminal::write(std::string_view text)
{
    m_buffer += text;
}

void Terminal::flush() noexcept
{
    std::string_view pending = m_buffer;
    while (!pending.empty())
    {
        const ssize_t written = ::write(STDOUT_FILENO, pending.data(), pending.size());
        if (written < 0)
        {
            if (errno == EINTR)
                continue;
            break; // The terminal is gone; there is nowhere left to report this.
        }
        pending.remove_prefix(static_cast<std::size_t>(written));
    }
    m_buffer.clear();
}

std::optional<char> Terminal::readByte()
{
    char byte{};
    if (::read(STDIN_FILENO, &byte, 1) == 1)
        return byte;

    return std::nullopt;
}
