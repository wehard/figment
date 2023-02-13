#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class Utils
{
private:
public:
    Utils() = delete;
    ~Utils() = delete;
    static std::string LoadFile(const char *path);
};

std::string Utils::LoadFile(const char *path)
{
    std::string source;

    std::ifstream shader_stream(path, std::ios::in);
    if (shader_stream.is_open())
    {
        std::stringstream sstr;
        sstr << shader_stream.rdbuf();
        source = sstr.str();
        shader_stream.close();
    }
    else
        printf("Error opening %s\n", path);
    return source;
}
