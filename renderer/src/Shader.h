#pragma once

#include <iostream>
#include <map>
#include <glm/glm.hpp>

class Shader
{
public:
	virtual ~Shader() = default;
	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	virtual void SetInt(std::string name, int i) = 0;
	virtual void SetFloat(std::string name, float f) = 0;
	virtual void SetVec2(std::string name, glm::vec2 v) = 0;
	virtual void SetVec3(std::string name, glm::vec3 v) = 0;
	virtual void SetVec4(std::string name, glm::vec4 v) = 0;
	virtual void SetMat4(std::string name, glm::mat4x4 m) = 0;

	static std::shared_ptr<Shader> Create(const std::string &vertPath, const std::string &fragPath);
};
