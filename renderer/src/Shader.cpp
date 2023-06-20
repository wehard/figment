#include "Shader.h"
#include "OpenGLShader.h"
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <map>

std::shared_ptr<Shader> Shader::Create(const std::string &vertPath, const std::string &fragPath)
{
	return std::make_shared<OpenGLShader>(vertPath, fragPath);
}
