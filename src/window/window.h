
#ifndef WINDOW_H
#define WINDOW_H

class window {
	
public:
	
	virtual ~window()
	{
		
	};

	virtual void setSize(int width, int height) = 0;
	virtual int getHeight() = 0;
	virtual int getWidth() = 0;
	virtual int getTargetFrameRate() = 0;
	virtual void setTargetFrameRate(int fps) = 0;
	virtual void setTitle(std::string title) = 0;
	virtual void setVsync(bool enabled) = 0;
	virtual std::string  getTitle() = 0;
	virtual void draw() = 0;
};

#endif //WINDOW_H
