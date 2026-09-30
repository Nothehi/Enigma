#include "Enigma.hpp"

char Enigma::press(char /*plain*/)
{
    // Placeholder until the rotors, reflector and plugboard are implemented.
    return 'a';
}

std::string Enigma::encrypt(std::string_view plain)
{
    std::string cipher;
    cipher.reserve(plain.size());

    for (const char c : plain)
        cipher.push_back(press(c));

    return cipher;
}
