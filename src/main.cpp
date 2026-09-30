#include <Application.hpp>
#include <iostream>

std::string enigma()
{
    char c;
    std::string m_encryption;

    while (std::cin.get(c))
    {
        if (c == '\n')
        {
            break;
        }

        m_encryption.push_back('a');
    }

    return m_encryption;
}

int main(void)
{
    Application app;

    app.run(enigma);

    return 0;
}