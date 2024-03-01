#include "gfx_device_gl.h"
#include <GL/glew.h>
#include "texture_setup.h"
#include "gl/texture_gl.h"
#include "gl/texture_cube.h"
#include "gl/texture_2D.h"

using namespace modelViewer::render;
using namespace modelViewer::res;

void gfx_device_gl::swapBuffers()
{
	m_Window->swapBuffers();
}

void gfx_device_gl::setViewport(int width, int height)
{
	glViewport(0, 0, width, height);
}

void gfx_device_gl::setClearColor(glm::vec3& color)
{
	glClearColor(color.r, color.g, color.b, 1);
}

gfx_device_gl::gfx_device_gl(std::shared_ptr<window_glfw>& window)
{
	m_Window = window;
	initDevice();
}

void __stdcall debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cerr << "OpenGL Debug Message: " << message << std::endl;
			break;
	}
}

void gfx_device_gl::initDevice() {

	m_Window->setGlContext(4,6,4);
	glewExperimental = GL_TRUE; // Enable experimental features
	glewInit();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	if (GLEW_KHR_debug) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(debugCallback, nullptr);
		// Enable only high-severity error messages
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
	}
	else
	{
		std::cerr << "KHR_debug extension not supported" << std::endl;
	}

	// Setup Dear ImGui context
	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init();*/

	
	std::cout << glGetString(GL_VERSION) << " OpenGL Driver Version \n";

	int maxSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
	int maxTexturesFrag;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexturesFrag);
	int maxTexturesVert;
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxTexturesVert);
	std::cout << "max samplers: " << maxSamples << ", max texture units in frag: " << maxTexturesFrag <<
			  ",max texture units in vert: " << maxTexturesVert << "\n";
}


std::shared_ptr<texture> gfx_device_gl::createTexture2D(texture_setup& setup)
{
	std::shared_ptr<texture_gl> texturePtr = std::make_shared<texture_2D>(setup);;
	return texturePtr;
}

std::shared_ptr<texture> gfx_device_gl::createTextureCube(texture_setup& setup)
{
	std::shared_ptr<texture_gl> texturePtr = std::make_shared<texture_cube>(setup);;
	return texturePtr;
}
