#include <Application.hpp>
#include <Enigma.hpp>
#include <iostream>

int main(void)
{
    Application app;
    Enigma enigma;

    app.run([&enigma]
            { return enigma.listen(); });

    return 0;
}