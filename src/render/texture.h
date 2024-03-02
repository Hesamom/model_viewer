
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

	class texture
	{
	public:
		virtual void active(int index) = 0;
		virtual ~texture() = default;
	};


	struct texture_binding {
		std::shared_ptr<texture> texture;
		std::string samplerName;
	};
}

#endif //TEXTURE_H
