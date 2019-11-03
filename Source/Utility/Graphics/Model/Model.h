#pragma once
#include <Graphics/OpenGL/Shader.h>
namespace Skel
{
	/**
		Interface to draw a 3D model.
		Model may encompass multiple meshes.
	**/
	class Model
	{
	public:
		virtual void Draw(const ShaderPtr& shader, const Matrix4x4& model) = 0;
	};

	typedef std::shared_ptr<Model> ModelPtr;
}