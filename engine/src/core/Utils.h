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
    static std::string LoadFile(const char *path)
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
    static std::string *LoadFile2(const char *filename)
    {
        FILE *file = fopen(filename, "rb");
        if (!file)
        {
            printf("Failed to open file: %s\n", filename);
            return nullptr;
        }
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);
        auto *result = new std::string(size, '\0');
        fread(result->data(), 1, size, file);
        fclose(file);
        return result;
    }
};
