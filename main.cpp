#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <thread>

std::atomic<bool> running{true};

void handleSignal(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        running = false;
    }
}

int main(void)
{
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    winsize size{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

    std::cout << "\033[?1049h\033[2J";
    std::cout << "\033[" << 1 << ";" << size.ws_row - 3 << "r";
    // print("test")->bottom()

    std::cout << "\033[" << size.ws_row - 2 << ";" << 1 << "H";
    for (int i = 0; i < size.ws_col; ++i)
        std::cout << "─";

    std::cout << "\033[" << size.ws_row - 1 << ";" << 1 << "H";
    std::cout << "❯ ";
    std::cout << "\033[38;2;50;50;50m";
    std::cout << "Enter message: ";
    std::cout << "\033[0m";

    std::cout << "\033[" << size.ws_row << ";" << 1 << "H";
    for (int i = 0; i < size.ws_col; ++i)
        std::cout << "─";

    int count = 0;

    while (running)
    {
        std::cout << "\033[" << size.ws_row - 1 << ";" << 18 << "H";
        for (int i = 0; i < size.ws_col - 18; ++i)
            std::cout << " ";

        std::cout << "\033[" << size.ws_row - 1 << ";" << 18 << "H";

        char c;
        std::string m_encryption;

        while (std::cin.get(c))
        {
            if (c == '\n')
            {
                count++;
                break;
            }

            m_encryption.push_back('a');
        }

        if (count <= size.ws_row - 3)
            std::cout << "\033[" << count << ";" << 1 << "H";
        else
            std::cout << "\033[" << size.ws_row - 3 << ";" << 1 << "H" << "\n";

        std::cout << m_encryption;
    }

    std::cout << "\nShutting down gracefully...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "\033[r\033[?1049l" << std::flush;
    std::_Exit(0);
    std::cout << "Shutdown complete.\n";

    return 0;
}