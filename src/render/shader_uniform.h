#ifndef SHADER_UNIFORM_H
#define SHADER_UNIFORM_H

namespace modelViewer::render
{
	template<typename T>
	class shader_uniform {
	private:
		unsigned int m_Location = -1;
		std::string m_Name;
	public:
		explicit shader_uniform(std::string name, std::string blockName, int blockIndex = -1)
		{
			if (name.empty())
			{
				throw std::runtime_error("name can not be empty");
			}
			if(!blockName.empty())
			{
				if (blockIndex >= 0)
				{
					m_Name = blockName + '[' + std::to_string(blockIndex) + ']' + "." + name;
				}
				else
				{
					m_Name = blockName + "." + name;
				}
			}
			else
			{
				m_Name = name;
			}
		}
		
		void getLocation(modelViewer::render::shader_program& program)
		{
			m_Location = program.getUniformLocation(m_Name);
		}
		
		void setValue(T& value, modelViewer::render::shader_program& program)
		{
			if (m_Location < 0)
			{
				return;
			}
			
			if constexpr (std::is_same<T,  float>::value)
			{
				program.setUniform(m_Location, value);
			}
			if constexpr (std::is_same<T,  int>::value)
			{
				program.setUniform(m_Location, value);
			}
			if constexpr (std::is_same<T,  glm::mat4>::value)
			{
				program.setUniform(m_Location, value);
			}
			if constexpr (std::is_same<T,  glm::vec3>::value)
			{
				program.setUniform(m_Location, value);
			}
			if constexpr (std::is_same<T,  glm::vec4>::value)
			{
				program.setUniform(m_Location, value);
			}
			else
			{
				static_assert("type not supported!");
			}
		}
	};
}

#endif 
