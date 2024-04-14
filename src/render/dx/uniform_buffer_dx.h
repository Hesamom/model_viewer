#ifndef UNIFORM_BUFFER_DX_H
#define UNIFORM_BUFFER_DX_H
#include "../uniform_buffer.h"
#include <d3d12.h>
#include "buffer_constant_generic_dx.h"

namespace modelViewer::render::dx
{
    class uniform_buffer_dx : public uniform_buffer
    {
    public:
        
        uniform_buffer_dx(ID3D12Device& device, int size, const char* name);
        D3D12_CONSTANT_BUFFER_VIEW_DESC getView();
        void setData(void* data, int offset, int size) override;
        int getSize() override;
        
    private:
        std::unique_ptr<buffer_constant_generic_dx> m_Buffer;
        int m_Size = 0;
    };
}


#endif //UNIFORM_BUFFER_DX_H
