#ifndef SHADER_DX_H
#define SHADER_DX_H
#include "../../resource/shader_asset.h"
#include "buffer_constant_generic_dx.h"
#include <wrl.h>
#include <glm/mat4x4.hpp>

namespace modelViewer::render
{
	class shader_dx {
	public:
		explicit shader_dx(std::shared_ptr<res::shader_asset>& asset);
		void compileToByteCode();
		Microsoft::WRL::ComPtr<ID3DBlob> getByteCode() const;
		modelViewer::res::shaderType getType() const;
		std::string getTypeName() const;
		
	private:
		
		std::shared_ptr<res::shader_asset> m_Asset;
		Microsoft::WRL::ComPtr<ID3DBlob> m_ByteCode = nullptr;
	};
}


#endif //SHADER_DX_H
