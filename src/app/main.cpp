#include "app/Application.hpp"
#include "app/Signals.hpp"
#include "app/Terminal.hpp"
#include "core/Enigma.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string_view>

int main()
{
    try
    {
        signals::installStopHandlers();

        Enigma enigma;
        {
            Terminal terminal;
            Application app(terminal, [&enigma](std::string_view line) { return enigma.encrypt(line); });
            app.run();
        } // Terminal restored here, before anything is printed to the normal screen.

        std::cout << "Shutdown complete.\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "enigma: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
