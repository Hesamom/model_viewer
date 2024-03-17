
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H


#include <glm/glm.hpp>
#include "../resource/model_info.h"

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
        virtual void bind() = 0;
		virtual void setCullFaceMode(res::cull_face_mode mode) = 0;
		virtual void setDepthMap(bool enable) = 0;
		
        virtual bool hasUniform(const std::string& name) const = 0;
		
        virtual void setUniform(const std::string& name, glm::vec3& vec3, bool optional) = 0;
        virtual void setUniform(const std::string& name, glm::vec4& vec4, bool optional) = 0;
        virtual void setUniform(const std::string& name, float value, bool optional) = 0;
        virtual void setUniform(const std::string& name, int value, bool optional) = 0;
		virtual void setUniform(const std::string& name, bool value, bool optional) = 0;
        virtual void setUniform(const std::string& name, glm::mat4& mat, bool optional) = 0;
		
        virtual std::vector<shader_uniform_info> getActiveUniforms() = 0;
        virtual int getActiveUniformsCount() = 0;
    };
}
#endif //SHADER_PROGRAM_H
