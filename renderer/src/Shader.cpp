#include "Core.h"
#include "Shader.h"
#include "OpenGLShader.h"
#include <sstream>
#include <vector>

std::shared_ptr<Shader> Shader::Create(const std::string &vertPath, const std::string &fragPath)
{
#ifdef FIGMENT_MACOS
	return std::make_shared<OpenGLShader>(vertPath, fragPath);
#elif defined(FIGMENT_WEB)
    return std::make_shared<WebGPUShader>(vertPath, fragPath);
#endif
    return nullptr;
}
