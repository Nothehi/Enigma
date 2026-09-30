#pragma once

#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include <array>
#include <functional>
#include <Terminal.hpp>

class Application
{
    static inline std::atomic<bool> s_running{true};

    static inline Terminal terminal;

    static void handleSignal(int signal)
    {
        if (signal == SIGINT || signal == SIGTERM)
            s_running = false;
    }

    void render()
    {
        terminal.scrollRegion(outputRegion()[0], outputRegion()[1]);
        terminal.moveCursor(terminal.getRows() - 3, 0);
        terminal.printSeparator();
        terminal.moveCursor(inputRegion()[0], 0);
        terminal.print("❯ ");
        terminal.printLine("Enter message: ", 50, 50, 50);
        terminal.printSeparator();
        terminal.moveCursor(inputRegion()[0], inputRegion()[1]);
    }

    std::array<int, 2> outputRegion()
    {
        return {1, terminal.getRows() - 4};
    }

    std::array<int, 2> inputRegion()
    {
        return {terminal.getRows() - 2, 18};
    }

    bool isInsideOutputRegion(uint row)
    {
        return row >= outputRegion()[0] && row <= outputRegion()[1];
    }

public:
    Application()
    {
        std::signal(SIGINT, handleSignal);
        std::signal(SIGTERM, handleSignal);

        terminal.enterAltScreen();
        render();
    }

    void run(const std::function<std::string()>& callback)
    {
        int outputLinesCount = 0;

        while (s_running)
        {
            terminal.clear(inputRegion()[0], inputRegion()[1], terminal.getCols() - inputRegion()[1]);
            terminal.moveCursor(inputRegion()[0], inputRegion()[1]);

            std::string output = callback();

            outputLinesCount++;
            if (isInsideOutputRegion(outputLinesCount))
                terminal.moveCursor(outputLinesCount, 0);
            else
            {
                terminal.moveCursor(outputRegion()[1], 0);
                terminal.print("\n");
            }

            terminal.print(output);
        }
    }

    ~Application()
    {
        terminal.printLine("Shutting down gracefully...");

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        terminal.exitAltScreen();

        terminal.printLine("Shutdown complete.");
        std::_Exit(0);
    }
};
