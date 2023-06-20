#pragma once

#include <iostream>
#include <map>
#include <glm/glm.hpp>

class Shader
{
public:
	virtual ~Shader() = default;
	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual void setInt(std::string name, int i) = 0;
	virtual void setFloat(std::string name, float f) = 0;
	virtual void setVec2(std::string name, glm::vec2 v) = 0;
	virtual void setVec3(std::string name, glm::vec3 v) = 0;
	virtual void setVec4(std::string name, glm::vec4 v) = 0;
	virtual void setMat4(std::string name, glm::mat4x4 m) = 0;

	static std::shared_ptr<Shader> Create(const std::string &vertPath, const std::string &fragPath);
};
