#pragma once

#include "app/Terminal.hpp"

#include <functional>
#include <string>
#include <string_view>

// Line-based terminal UI: a scrolling output area above a single-line prompt.
// Each submitted line is passed through the transform and the result is appended to the output.
class Application
{
public:
    using Transform = std::function<std::string(std::string_view)>;

    Application(Terminal& terminal, Transform transform);

    // Runs until the user quits (Ctrl+C, or Ctrl+D on an empty line), input ends, or a stop signal arrives.
    void run();

private:
    enum class KeyAction
    {
        Continue,
        Quit
    };

    // All rows and columns are 1-based.
    struct Layout
    {
        int width;
        int outputTop;
        int outputBottom;
        int topSeparatorRow;
        int inputRow;
        int bottomSeparatorRow;
        int inputCol;
        int inputCapacity; // characters that fit on the input line without wrapping
    };

    [[nodiscard]] static Layout computeLayout(TerminalSize size);

    [[nodiscard]] KeyAction handleKey(char key);
    void skipEscapeSequence();
    void submitLine();
    void appendOutput(std::string_view text);
    void appendOutputLine(std::string_view line);

    void drawChrome();
    void drawSeparator(int row);
    void drawInput();

    Terminal& m_terminal;
    Transform m_transform;
    Layout m_layout;
    std::string m_input;
    int m_nextOutputRow;
};
