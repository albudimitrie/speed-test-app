#pragma once
#include <iostream>
#include <cstdint>
namespace utils{
uint64_t parseSizeWithSuffix(const std::string& input) {
    char suffix = std::toupper(input.back());
    uint64_t multiplier = 1;
    std::string number_part = input;

    if (!std::isdigit(suffix)) {
        number_part = input.substr(0, input.size() - 1);
        switch (suffix) {
            case 'K': multiplier = 1024ULL; break;
            case 'M': multiplier = 1024ULL * 1024; break;
            case 'G': multiplier = 1024ULL * 1024 * 1024; break;
            default:
                throw std::invalid_argument("invalid suffix\n");
        }
    }

    return std::stoull(number_part) * multiplier;
}
}