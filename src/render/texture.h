
#ifndef TEXTURE_H
#define TEXTURE_H

class texture
{
public:
	virtual void active(const int index) = 0;
};


struct texture_binding {
	std::shared_ptr<texture> texture;
	std::string samplerName;
};

#endif //TEXTURE_H
