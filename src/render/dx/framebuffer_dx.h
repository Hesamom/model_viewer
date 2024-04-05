
#ifndef FRAMEBUFFER_DX_H
#define FRAMEBUFFER_DX_H


#include <wrl/client.h>
#include "../framebuffer.h"
#include "texture_dx.h"
#include "descriptor_heap.h"

namespace modelViewer::render::dx
{
	class framebuffer_dx : public framebuffer {
	public:
		
		framebuffer_dx(Microsoft::WRL::ComPtr<ID3D12Device>& device,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, std::string& name);

		void unbind() override;
		
		void attachDepthTexture() override;
		void attachCubeMapFace(int index) override;
		void attachDepthTextureArray(int layer) override;
		
		void createDepthTexture(int width, int height, bool enableDepthCompare) override;
		void createArrayDepthTexture(int width, int height, int layers, bool enableDepthCompare) override;
		void createCubeMap(int size) override;
		void createColorBuffer(int width, int height) override;
		
		
		void clearDepthBuffer() override;
		void clearColorBuffer(const glm::vec4 &color) override;
		
		
		std::shared_ptr<texture> getDepthMap() override;
		std::shared_ptr<texture> getDepthMapArray() override;
		std::shared_ptr<texture> getColorCubeMap() override;
		
	private:
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		std::shared_ptr<texture_dx> m_DepthTexture;
		D3D12_VIEWPORT m_DepthViewport;
		D3D12_RECT m_DepthScissorRect;
		
		std::shared_ptr<descriptor_heap> m_DepthDescriptorHeap;
		std::string m_Name;
		bool m_BoundDepth = false;
	};
}

#endif //FRAMEBUFFER_DX_H
