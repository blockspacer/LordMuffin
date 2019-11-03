#include <skelpch.h>
#include "ResourceManager.h"
#include <Graphics/Model/ImportedModel.h>
#include <Graphics/OpenGL/Cubemap.h>
#include <Objects/Box/BoxMesh.h>
#include <Objects/Box/WoodenBoxModel.h>
#include <Resources/GameResources.h>

namespace Skel
{
	ResourceManager & Skel::ResourceManager::Instance()
	{
		static ResourceManager resources;
		return resources;
	}
	void ResourceManager::Init()
	{
		// Loads in game-specfic resources
		LoadResources(m_TextureTable, m_MeshTable, m_ModelTable, m_ShaderTable, m_FontTable);
		LOG("Initialized Resource Manager");
	}
	TexturePtr ResourceManager::GetTexture(std::string name)
	{
		ASSERT(m_TextureTable[name], "Cannot load texture: " + name);
		return m_TextureTable[name];
	}
	MeshPtr ResourceManager::GetMesh(std::string name)
	{
		ASSERT(m_MeshTable[name], "Cannot load mesh: " + name);
		return m_MeshTable[name];
	}
	ModelPtr ResourceManager::GetModel(std::string name)
	{
		ASSERT(m_ModelTable[name], "Cannot load model: " + name);
		return m_ModelTable[name];
	}
	ShaderPtr ResourceManager::GetShader(std::string name)
	{
		ASSERT(m_ShaderTable[name], "Cannot load shader: " + name);
		return m_ShaderTable[name];
	}
	FontPtr ResourceManager::GetFont(std::string name)
	{
		ASSERT(m_FontTable[name], "Cannot load font: " + name);
		return m_FontTable[name];
	}
}