#include "..\ecs.h"


ShaderRegistry& ShaderRegistry::get()
{
	static ShaderRegistry instance;
	return instance;
}

void ShaderRegistry::registerShader(const std::string& name, ShaderProc_t shaderProc)
{
	registry_[name] = shaderProc;
}

ShaderProc_t ShaderRegistry::getShader(const std::string& name) const
{
	auto it = registry_.find(name);
	assert(it != registry_.end());

	return it->second;
}
