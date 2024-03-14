#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>

class Utils
{
private:
public:
    Utils() = delete;
    ~Utils() = delete;
    static std::string LoadFile(const std::string &filePath)
    {
        std::string result;

        std::ifstream shaderStream(filePath, std::ios::in);
        if (shaderStream.is_open())
        {
            std::stringstream stream;
            stream << shaderStream.rdbuf();
            result = stream.str();
            shaderStream.close();
        }
        else
            printf("Error opening %s\n", filePath.c_str());
        return result;
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

    static void *LoadFileBytes(const char *filePath, size_t *size)
    {
        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        *size = fileSize;
        file.seekg(0, std::ios::beg);
        char *bytes = new char[fileSize];
        file.read(bytes, fileSize);
        file.close();
        return bytes;
    }
};
