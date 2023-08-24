#pragma once
#include "Shader.h"

class WebGPUShader : public Shader
{
public:
    WebGPUShader(const std::string &vertPath, const std::string &fragPath);

    void Bind() override;
    void Unbind() override;
    void SetInt(std::string name, int i) override;
    void SetFloat(std::string name, float f) override;
    void SetVec2(std::string name, glm::vec2 v) override;
    void SetVec3(std::string name, glm::vec3 v) override;
    void SetVec4(std::string name, glm::vec4 v) override;
    void SetMat4(std::string name, glm::mat4x4 m) override;
};

