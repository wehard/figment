#include "Core.h"
#include "Shader.h"
#ifdef FIGMENT_MACOS
#include "OpenGLShader.h"
#elif defined(FIGMENT_WEB)
#include "WebGPUShader.h"
#endif
#include <sstream>
#include <vector>

std::shared_ptr<Shader> Shader::Create(const std::string &vertPath, const std::string &fragPath)
{
#ifdef FIGMENT_MACOS
	return std::make_shared<OpenGLShader>(vertPath, fragPath);
#elif defined(FIGMENT_WEB)
    return nullptr; //std::make_shared<WebGPUShader>(vertPath, fragPath);
#endif
    return nullptr;
}
