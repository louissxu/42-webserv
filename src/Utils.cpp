#include "Utils.hpp"

std::string Utils::getFirst(const std::string& str) {
    // Find the position of the first space
    size_t spacePos = str.find(' ');

    // If there's no space, the entire string is a single word
    if (spacePos == std::string::npos) {
        return str;
    }

    // Otherwise, return the substring from the beginning to the first space
    return str.substr(0, spacePos);
}

std::string Utils::getSecond(const std::string& str) {
    // Find the position of the first space
    size_t firstSpacePos = str.find(' ');
    if (firstSpacePos == std::string::npos) {
        // No spaces, return empty string as there's no second word
        return "";
    }

    // Find the position of the next space after the first
    size_t secondSpacePos = str.find(' ', firstSpacePos + 1);

    // If there's no second space, return the substring from the first space to the end
    if (secondSpacePos == std::string::npos) {
        return str.substr(firstSpacePos + 1);
    }

    // Otherwise, return the substring between the two spaces
    return str.substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);
}