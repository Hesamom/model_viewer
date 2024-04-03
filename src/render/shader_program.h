﻿
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H


#include <glm/glm.hpp>
#include "../resource/model_info.h"
#include "texture.h"

namespace modelViewer::render {
	
	struct shader_texture_slot
	{
		std::string name;
		shader_texture_type type = shader_texture_type::none;
	};
    
    class shader_program {
    public:
        virtual ~shader_program() = default;
        virtual void bind() = 0;
		virtual void setCullFaceMode(res::cull_face_mode mode) = 0;
		virtual void setDepthMap(bool enable) = 0;
		
		virtual const std::vector<shader_texture_slot>& getTextureSlots() = 0;
        virtual bool hasUniform(const std::string& name) const = 0;
		virtual void bindTexture(int slotIndex, std::shared_ptr<render::texture>& texture) = 0;
		
        virtual void setUniform(const std::string& name, glm::vec3& vec3, bool optional) = 0;
        virtual void setUniform(const std::string& name, glm::vec4& vec4, bool optional) = 0;
        virtual void setUniform(const std::string& name, float value, bool optional) = 0;
        virtual void setUniform(const std::string& name, int value, bool optional) = 0;
		virtual void setUniform(const std::string& name, bool value, bool optional) = 0;
        virtual void setUniform(const std::string& name, glm::mat4& mat, bool optional) = 0;
    };
}
#endif //SHADER_PROGRAM_H
