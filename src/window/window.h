
#ifndef WINDOW_H
#define WINDOW_H

class window {
	
public:
	
	struct mouse_event
	{
		bool leftPressed = false;
		bool rightPressed = false;
		bool middlePressed = false;
		int x = 0, y = 0;
	};
	
	virtual ~window()
	{
		
	};

	virtual void* getHandleRaw() = 0;
	virtual void setOnSizeChangedCallback(std::function<void(int, int)> callback) = 0;
	virtual void setOnMouseButtonCallback(std::function<void(mouse_event)> callback) = 0;
	
	virtual void setSize(int width, int height) = 0;
	virtual int getHeight() = 0;
	virtual int getWidth() = 0;
	virtual void setTitle(std::string title) = 0;
	virtual std::string  getTitle() = 0;
	virtual bool shouldClose() = 0;
	virtual bool isPaused() = 0;
	virtual void pollEvents() = 0;
};

#endif //WINDOW_H
