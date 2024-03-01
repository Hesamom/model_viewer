
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#include <glm/glm.hpp>

namespace modelViewer::render {

    enum class shader_uniform_type {
        none,
        sampler2D,
        sampler2DShadow,
        samplerCube
    };

    enum class shader_texture_usage {
        none,
        diffuse,
        specular,
        normal
    };
	
    struct shader_uniform_texture_pair
    {
        shader_uniform_type type = shader_uniform_type::none;
        shader_texture_usage usage = shader_texture_usage::none;

        bool operator<(const shader_uniform_texture_pair& other) const
        {
            if (type != other.type)
                return type < other.type;
            return usage < other.usage;
        }
    };
    
    struct shader_uniform_info {
        std::string name;
        shader_uniform_type type = shader_uniform_type::none;
        shader_texture_usage textureUsage = shader_texture_usage::none;
		
        bool isSampler() const {
            return type == shader_uniform_type::sampler2D || type == shader_uniform_type::sampler2DShadow || type == shader_uniform_type::samplerCube;
        }

        bool isShadowSampler() const {
            return type == shader_uniform_type::sampler2DShadow;
        }
    };
    
    class shader_program {
    public:
        virtual ~shader_program() = default;
        virtual void validateLinking() = 0;
        virtual std::string getLinkLog() = 0;
        virtual bool isLinked() = 0;
        virtual void bind() = 0;
        virtual int getAttributeLocation(const std::string& attributeName) = 0;
        virtual int getUniformLocation(const std::string& uniformName) = 0;
        virtual void setUniform(int location, glm::vec3 vec3) = 0;
        virtual void setUniform(int location, glm::vec4 vec4) = 0;
        virtual void setUniform(int location, float value) = 0;
        virtual void setUniform(int location, int value) = 0;
        virtual void setUniform(int index, glm::mat4& mat) = 0;
        virtual std::vector<shader_uniform_info> getActiveUniforms() = 0;
        virtual int getActiveUniformsCount() = 0;

        
    };
}
#endif //SHADER_PROGRAM_H
