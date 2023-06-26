#include "OpenGLShader.h"
#include "Utils.h"
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <map>

OpenGLShader::OpenGLShader(const std::string &vertPath, const std::string &fragPath)
{
    std::string vertSource = Utils::LoadFile(vertPath.c_str());
    std::string fragSource = Utils::LoadFile(fragPath.c_str());
    this->m_VertID = compileShader(vertSource, GL_VERTEX_SHADER);
    this->m_FragID = compileShader(fragSource, GL_FRAGMENT_SHADER);
    this->m_ID = createProgram(this->m_VertID, this->m_FragID);
    loadUniforms();
    loadAttributes();
}

OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_ID);
}

void OpenGLShader::Bind()
{
    glUseProgram(m_ID);
}

void OpenGLShader::Unbind()
{
    glUseProgram(0);
}

void OpenGLShader::SetInt(std::string name, int i)
{
    GLuint location = glGetUniformLocation(m_ID, name.c_str());
    glUniform1i(location, i);
}

void OpenGLShader::SetFloat(std::string name, float f)
{
    GLuint location = glGetUniformLocation(m_ID, name.c_str());
    glUniform1f(location, f);
}

void OpenGLShader::SetVec2(std::string name, glm::vec2 v)
{
    GLuint location = glGetUniformLocation(m_ID, name.c_str());
    glUniform2f(location, v[0], v[1]);
}

void OpenGLShader::SetVec3(std::string name, glm::vec3 v)
{
    GLuint location = glGetUniformLocation(m_ID, name.c_str());
    glUniform3f(location, v[0], v[1], v[2]);
}

void OpenGLShader::SetVec4(std::string name, glm::vec4 v)
{
    GLuint location = glGetUniformLocation(m_ID, name.c_str());
    glUniform4f(location, v[0], v[1], v[2], v[3]);
}

void OpenGLShader::SetMat4(std::string name, glm::mat4x4 m)
{
    GLuint location = glGetUniformLocation(m_ID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat *)&m[0]);
}

void OpenGLShader::loadUniforms()
{
    GLint count;

    glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &count);
    for (GLint i = 0; i < count; i++)
    {
        GLsizei len;
        GLint size;
        GLenum type;
        GLchar name[30];
        glGetActiveUniform(m_ID, i, 30, &len, &size, &type, &name[0]);
        m_Uniforms.insert(std::pair<std::string, GLuint>(name, (GLuint)i));
    }
}

void OpenGLShader::loadAttributes()
{
    GLint count;

    glGetProgramiv(m_ID, GL_ACTIVE_ATTRIBUTES, &count);
    for (GLint i = 0; i < count; i++)
    {
        GLsizei len;
        GLint size;
        GLenum type;
        GLchar name[30];
        glGetActiveAttrib(m_ID, i, 30, &len, &size, &type, &name[0]);
        m_Attributes.insert(std::pair<std::string, GLuint>(name, (GLuint)glGetAttribLocation(m_ID, name)));
    }
}

std::string OpenGLShader::loadShader(std::string path)
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
        printf("Error opening %s\n", path.c_str());
    return (source);
}

uint32_t OpenGLShader::compileShader(std::string src, uint32_t shader_type)
{
    uint32_t shader_id;

    shader_id = glCreateShader(shader_type);
    const char *source_ptr = src.c_str();
    glShaderSource(shader_id, 1, &source_ptr, NULL);
    glCompileShader(shader_id);
    GLint status = GL_TRUE;
    GLint log_len;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_len);
    if (status == GL_FALSE)
    {
        std::vector<char> error_message(log_len + 1);
        glGetShaderInfoLog(shader_id, log_len, NULL, &error_message[0]);
        printf("%s: %s\n", shader_type == GL_FRAGMENT_SHADER ? "frag" : "vert", &error_message[0]);
        m_IsValid = false;
    }
    return (shader_id);
}

uint32_t OpenGLShader::createProgram(uint32_t vert_id, uint32_t frag_id)
{
    uint32_t program_id = glCreateProgram();
    glAttachShader(program_id, vert_id);
    glAttachShader(program_id, frag_id);
    glLinkProgram(program_id);
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
    return (program_id);
}
