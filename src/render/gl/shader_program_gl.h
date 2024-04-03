﻿#ifndef SHADER_PROGRAM_GL_H
#define SHADER_PROGRAM_GL_H

#include "../../resource/shader_loader.h"
#include "shader_gl.h"
#include "../shader_program.h"
#include "texture_gl.h"

namespace modelViewer::render
{
    class shader_program_gl : public shader_program
		{

    public:
        shader_program_gl(std::initializer_list<shader_gl> shaders);
        explicit shader_program_gl(std::vector<shader_gl>& shaders);
        ~shader_program_gl() override;

     
        void bind() override;
		void bindTexture(int slotIndex, std::shared_ptr<render::texture> &texture) override;
		const std::vector<shader_texture_slot> & getTextureSlots() override;
        int getAttributeLocation(const std::string& attributeName);
        int getUniformLocation(const std::string& uniformName, bool optional = true) const;
		
		bool hasUniform(const std::string &name) const override;
    	void setCullFaceMode(res::cull_face_mode mode) override;
		void setDepthMap(bool enable) override;
		
        void setUniform(const std::string& name, glm::vec3& vec3, bool optional) override;
        void setUniform(const std::string& name, glm::vec4& vec4, bool optional) override;
        void setUniform(const std::string& name, float value, bool optional) override;
        void setUniform(const std::string& name, int value, bool optional) override;
		void setUniform(const std::string& name, bool value, bool optional) override;
        void setUniform(const std::string& name, glm::mat4& mat, bool optional) override;

		void validateLinking();
		std::string getLinkLog();
		bool isLinked();

    private:
    	unsigned int m_ProgramId = 0;
		bool m_DepthEnabled = false;
		res::cull_face_mode m_FaceMode = res::cull_face_mode::back;
		std::vector<shader_texture_slot> m_TextureSlots;
		std::vector<std::shared_ptr<texture_gl>> m_BoundTextures;
		
		void applyPipelineState();
		void reflectTextures();
	
		int getActiveUniformsCount();

		void bindTextures();
	}; 
}


#endif
