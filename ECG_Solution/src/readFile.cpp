#include "readFile.hpp"
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>

// Reads a file and returns as string
// No considerations taken for efficiency
std::string readFile(std::filesystem::path p) {
    std::ifstream file(p);
    std::string str;
    std::string tmp;
    while (std::getline(file, tmp))
    {
        str += tmp;
        str.push_back('\n');
    }
    return str;
}
