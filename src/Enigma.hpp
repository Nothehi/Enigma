#pragma once

#include <string>
#include <string_view>

// The cipher machine. Pure logic with no I/O, so it can be unit-tested in isolation.
class Enigma
{
public:
    // Encrypts a single keypress. Non-const because a real machine steps its rotors on every press.
    [[nodiscard]] char press(char plain);

    [[nodiscard]] std::string encrypt(std::string_view plain);
};
