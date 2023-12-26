#pragma once

#include "Shader.h"
#include <iostream>
#include <map>
#include <glm/glm.hpp>

class OpenGLShader : public Shader
{
private:
    uint32_t m_VertID;
    uint32_t m_FragID;
    std::map<std::string, uint32_t> m_Uniforms;
    std::map<std::string, uint32_t> m_Attributes;

    std::string loadShader(std::string path);
    uint32_t compileShader(std::string src, uint32_t shader_type);
    uint32_t createProgram(uint32_t vert_id, uint32_t frag_id);
    void loadUniforms();
    void loadAttributes();
    bool m_IsValid = true;

    uint32_t m_ID;

public:
    OpenGLShader(const std::string &vertPath, const std::string &fragPath);
    ~OpenGLShader();
    void Bind();
    void Unbind();
    void SetInt(std::string name, int i);
    void SetFloat(std::string name, float f);
    void SetVec2(std::string name, glm::vec2 v);
    void SetVec3(std::string name, glm::vec3 v);
    void SetVec4(std::string name, glm::vec4 v);
    void SetMat4(std::string name, glm::mat4x4 m);
    bool IsValid() { return m_IsValid; }
};
