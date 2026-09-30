#pragma once

#include <iostream>

class Enigma
{
    char m_plainCharacter;
    std::string m_encryptedText;

public:
    std::string listen()
    {
        m_encryptedText.clear();

        while (std::cin.get(m_plainCharacter))
        {
            if (m_plainCharacter == '\n')
                break;

            m_encryptedText.push_back('a');
        }

        return m_encryptedText;
    }
};