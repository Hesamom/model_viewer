
#ifndef TEXTURE_H
#define TEXTURE_H

namespace modelViewer::render {
	enum texture_cube_face
	{
		x_positive,
		x_negative,
		y_positive,
		y_negative,
		z_positive,
		z_negative
	};

	enum class shader_texture_type {
		none,
		texture1D,
		texture2D,
		texture3D,
		textureCube,
		texture1DArray,
		texture2DArray
	};

	enum class shader_texture_usage {
		none,
		diffuse,
		specular,
		normal
	};

	struct shader_uniform_texture_pair
	{
		shader_texture_type type = shader_texture_type::none;
		shader_texture_usage usage = shader_texture_usage::none;

		bool operator<(const shader_uniform_texture_pair& other) const
		{
			if (type != other.type)
				return type < other.type;
			return usage < other.usage;
		}
	};
	
	class texture
	{
	public:
		virtual ~texture() = default;
		virtual shader_texture_type getType() const = 0;
	};


	struct texture_binding {
		std::shared_ptr<texture> texture;
		std::string samplerName;
	};
}

#endif //TEXTURE_H
