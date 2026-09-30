#include "Application.hpp"

#include "Signals.hpp"

#include <format>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace
{
    constexpr std::string_view kPromptSymbol = "❯ ";
    constexpr int kPromptSymbolWidth = 2; // display columns; the string itself is multi-byte UTF-8
    constexpr std::string_view kPromptLabel = "Enter message: ";
    constexpr Rgb kPromptLabelColor{50, 50, 50};
    constexpr std::string_view kSeparator = "─";

    constexpr int kMinRows = 5;
    constexpr int kMinCols = 20;

    constexpr char kCtrlC = 0x03;
    constexpr char kCtrlD = 0x04;
    constexpr char kCtrlH = 0x08;
    constexpr char kEscape = 0x1B;
    constexpr char kDelete = 0x7F;

    [[nodiscard]] constexpr bool isPrintableAscii(char c) noexcept
    {
        return c >= 0x20 && c < 0x7F;
    }
}

Application::Application(Terminal& terminal, Transform transform)
    : m_terminal(terminal),
      m_transform(std::move(transform)),
      m_layout(computeLayout(terminal.size())),
      m_nextOutputRow(m_layout.outputTop)
{
}

Application::Layout Application::computeLayout(TerminalSize size)
{
    if (size.rows < kMinRows || size.cols < kMinCols)
        throw std::runtime_error(std::format("terminal must be at least {}x{}", kMinCols, kMinRows));

    const int inputCol = 1 + kPromptSymbolWidth + static_cast<int>(kPromptLabel.size());

    return {
        .width = size.cols,
        .outputTop = 1,
        .outputBottom = size.rows - 4,
        .topSeparatorRow = size.rows - 3,
        .inputRow = size.rows - 2,
        .bottomSeparatorRow = size.rows - 1,
        .inputCol = inputCol,
        .inputCapacity = size.cols - inputCol, // keep the last column free so the cursor never wraps
    };
}

void Application::run()
{
    drawChrome();
    drawInput();
    m_terminal.flush();

    while (!signals::stopRequested())
    {
        const auto key = m_terminal.readByte();
        if (!key || handleKey(*key) == KeyAction::Quit)
            break;
    }
}

Application::KeyAction Application::handleKey(char key)
{
    switch (key)
    {
    case kCtrlC:
        return KeyAction::Quit;
    case kCtrlD:
        if (m_input.empty())
            return KeyAction::Quit;
        break;
    case '\r':
    case '\n':
        submitLine();
        break;
    case kDelete:
    case kCtrlH:
        if (!m_input.empty())
            m_input.pop_back();
        break;
    case kEscape:
        skipEscapeSequence();
        break;
    default:
        if (isPrintableAscii(key) && std::ssize(m_input) < m_layout.inputCapacity)
            m_input.push_back(key);
        break;
    }

    drawInput();
    m_terminal.flush();
    return KeyAction::Continue;
}

void Application::skipEscapeSequence()
{
    // Arrow and function keys arrive as ESC '[' <params> <final byte> or ESC 'O' <byte>.
    // Line editing isn't supported, so drop them instead of inserting the raw bytes.
    const auto introducer = m_terminal.readByte();
    if (introducer == 'O')
    {
        (void)m_terminal.readByte();
        return;
    }
    if (introducer != '[')
        return;

    while (const auto byte = m_terminal.readByte())
    {
        if (*byte >= 0x40 && *byte <= 0x7E)
            return;
    }
}

void Application::submitLine()
{
    appendOutput(m_transform(m_input));
    m_input.clear();
}

void Application::appendOutput(std::string_view text)
{
    // Wrap long lines ourselves so the row bookkeeping stays accurate.
    const auto width = static_cast<std::size_t>(m_layout.width);
    do
    {
        const std::string_view line = text.substr(0, width);
        text.remove_prefix(line.size());
        appendOutputLine(line);
    } while (!text.empty());
}

void Application::appendOutputLine(std::string_view line)
{
    if (m_nextOutputRow <= m_layout.outputBottom)
    {
        m_terminal.moveCursor(m_nextOutputRow++, 1);
    }
    else
    {
        // The output area is full: a newline on its bottom row scrolls only the scroll region.
        m_terminal.moveCursor(m_layout.outputBottom, 1);
        m_terminal.write("\n");
    }
    m_terminal.write(line);
}

void Application::drawChrome()
{
    m_terminal.setScrollRegion(m_layout.outputTop, m_layout.outputBottom);
    drawSeparator(m_layout.topSeparatorRow);
    drawSeparator(m_layout.bottomSeparatorRow);

    m_terminal.moveCursor(m_layout.inputRow, 1);
    m_terminal.write(kPromptSymbol);
    m_terminal.setForeground(kPromptLabelColor);
    m_terminal.write(kPromptLabel);
    m_terminal.resetStyle();
}

void Application::drawSeparator(int row)
{
    m_terminal.moveCursor(row, 1);
    for (int col = 0; col < m_layout.width; ++col)
        m_terminal.write(kSeparator);
}

void Application::drawInput()
{
    m_terminal.moveCursor(m_layout.inputRow, m_layout.inputCol);
    m_terminal.write(m_input);
    m_terminal.clearToEndOfLine();
}
