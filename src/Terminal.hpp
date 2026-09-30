#pragma once

#include <iostream>
#include <sys/ioctl.h>

class Terminal
{
    static inline winsize s_winsize{};

public:
    Terminal()
    {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &s_winsize);
    }

    inline unsigned short int getRows() const
    {
        return s_winsize.ws_row;
    }

    inline unsigned short int getCols() const
    {
        return s_winsize.ws_col;
    }

    inline void moveCursor(uint row, uint col)
    {
        std::cout << "\033[" << row << ";" << col << "H";
    }

    inline void enterAltScreen()
    {
        std::cout << "\033[?1049h\033[2J";
    }

    inline void exitAltScreen()
    {
        std::cout << "\033[r\033[?1049l" << std::flush;
    }

    inline void scrollRegion(uint from, uint to)
    {
        std::cout << "\033[" << from << ";" << to << "r";
    }

    inline void clear(uint from, uint to, uint offset)
    {
        moveCursor(from, to);
        for (int i = 0; i < offset; ++i)
            std::cout << " ";
    }

    inline void print(std::string str)
    {
        std::cout << str;
    }

    inline void printLine(std::string str)
    {
        std::cout << str << std::endl;
    }

    inline void printLine(std::string str, uint r, uint g, uint b)
    {
        std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m" << str << "\033[0m" << std::endl;
    }

    inline void printSeparator(std::string separator = "─")
    {
        for (int i = 0; i < getCols(); ++i)
            std::cout << separator;
        std::cout << std::endl;
    }
};