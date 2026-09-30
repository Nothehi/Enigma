#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <termios.h>

struct TerminalSize
{
    int rows;
    int cols;
};

struct Rgb
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

// RAII owner of the controlling terminal: switches to raw mode and the alternate
// screen on construction and restores both on destruction.
// Output is buffered; nothing reaches the screen until flush().
class Terminal
{
public:
    Terminal();
    ~Terminal();

    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;
    Terminal(Terminal&&) = delete;
    Terminal& operator=(Terminal&&) = delete;

    [[nodiscard]] TerminalSize size() const noexcept { return m_size; }

    // Coordinates are 1-based, as in ANSI escape sequences.
    void moveCursor(int row, int col);
    void setScrollRegion(int top, int bottom);
    void clearToEndOfLine();
    void setForeground(Rgb color);
    void resetStyle();
    void write(std::string_view text);
    void flush() noexcept;

    // Blocks until one byte of input arrives. Returns nullopt on end of input,
    // on error, or when a signal interrupts the read.
    [[nodiscard]] std::optional<char> readByte();

private:
    termios m_originalMode{};
    TerminalSize m_size{};
    std::string m_buffer;
};
