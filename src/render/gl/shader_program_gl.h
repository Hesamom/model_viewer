#ifndef SHADER_PROGRAM_GL_H
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
  

        explicit shader_program_gl(std::vector<shader_gl>& shaders);
        ~shader_program_gl() override;

     
        void bind() override;
		void bindTexture(int slotIndex, std::shared_ptr<render::texture> &texture) override;

        void bindUniformBuffers();

        bool bindTexture(const std::string &name, std::shared_ptr<render::texture> &texture) override;

    	void setUniformBuffer(std::shared_ptr<uniform_buffer>& buffer, const std::string& name) override;
    	bool hasUniformBufferSlot(const std::string& name) const override;
    	
		int getTextureSlot(const std::string &textureName) override;
		const std::vector<shader_texture_slot> & getTextureSlots() override;
        int getAttributeLocation(const std::string& attributeName);
        int getUniformLocation(const std::string& uniformName, bool optional = true) const;
		
		bool hasUniform(const std::string &name) const override;
    	void setCullFaceMode(res::cull_face_mode mode) override;
		void setAlphaBlending(bool enabled) override;
		void setDepthMap(res::depth_buffer_options& options) override;
		void setTopology(topology_mode topology) override;
		topology_mode getTopology() override;
		
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

    	void applyPipelineState() const;
        void reflectUniformBlocks();
    	void reflectUniforms();
    	void reflectConsts();
    	int getActiveUniformsCount();
        int getActiveUniformBlockCount();
        void bindTextures();

    	unsigned int m_ProgramId = 0;
        res::depth_buffer_options m_DepthOptions;
		res::cull_face_mode m_FaceMode = res::cull_face_mode::back;
		std::vector<shader_texture_slot> m_TextureSlots;
		std::vector<std::shared_ptr<texture_gl>> m_BoundTextures;
		bool m_AlphaBlending = false;
		topology_mode m_Topology = topology_mode::triangles;


    	struct uniform_buffer_slot
    	{
    		int size = 0;
    		int slot = 0;
    	};
    	
    	std::map<std::string,uniform_buffer_slot> m_BufferSlots;
    	std::map<std::string, std::shared_ptr<uniform_buffer>> m_BoundBuffers;
		int m_MaxUniformName;
    	GLchar* m_NameBuffer;
	}; 
}


#endif
