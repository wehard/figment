#pragma once

#include <iostream>
#include <map>
#include <glm/glm.hpp>

class Shader
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
	Shader(const char *vertSource, const char *fragSource);
	~Shader();
	void use();
	void setInt(std::string name, int i);
	void setFloat(std::string name, float f);
	void setVec2(std::string name, glm::vec2 v);
	void setVec3(std::string name, glm::vec3 v);
	void setVec4(std::string name, glm::vec4 v);
	void setMat4(std::string name, glm::mat4x4 m);
	bool IsValid() { return m_IsValid; }
};
