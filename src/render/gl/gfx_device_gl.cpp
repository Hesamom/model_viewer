#include "gfx_device_gl.h"
#include <../../../libs/glew/include/GL/glew.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include "framebuffer_gl.h"
#include "mesh_gl.h"
#include "shader_gl.h"
#include "shader_program_gl.h"
#include "texture_gl.h"
#include "texture_cube.h"
#include "texture_2D_gl.h"

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


void gfx_device_gl::clearDepthBuffer() {
	glClear(GL_DEPTH_BUFFER_BIT);
}

void gfx_device_gl::resize(int width, int height) {
	//TODO maybe need to do more?
	setViewport(width, height);
}

void gfx_device_gl::clearColorBuffer(const glm::vec4& color) {
	glClearBufferfv(GL_COLOR, 0, &color.x);
}

void* gfx_device_gl::getDevice() {
	return m_Window->getContextRaw();
}

void gfx_device_gl::popDebugGroup() {
	glPopDebugGroup();
}

void gfx_device_gl::pushDebugGroup(const char* label) {
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, label);
}

int gfx_device_gl::getMaxSamplersPerProgram() {
	int max = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max);
	return  max;
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

	m_Window->setGlContext(4,6,8);
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
	std::shared_ptr<texture_gl> texturePtr = std::make_shared<texture_2D_gl>(setup);;
	return texturePtr;
}

std::shared_ptr<mesh> gfx_device_gl::createMesh(
	std::shared_ptr<mesh_asset>& asset) {
	
	std::shared_ptr<mesh_gl> m = std::make_shared<mesh_gl>(asset);
	return m;
}

std::shared_ptr<shader_program> gfx_device_gl::createProgram(std::vector<std::shared_ptr<shader_asset>>& assets)
{
	std::vector<shader_gl> shaders;
	for (auto& asset: assets) {
		auto shader = shader_gl(asset);
		shader.compile();
		shader.verify();
		shaders.push_back(shader);
	}
	
	auto program = std::make_unique<shader_program_gl>(shaders);
	program->validateLinking();
	return program;
}

std::shared_ptr<framebuffer> gfx_device_gl::createFramebuffer() {
	auto buffer = std::make_shared<framebuffer_gl>();
	return buffer;
}

std::shared_ptr<texture> gfx_device_gl::createTextureCube(texture_setup& setup)
{
	std::shared_ptr<texture_gl> texturePtr = std::make_shared<texture_cube>(setup);;
	return texturePtr;
}

void gfx_device_gl::onPreRenderImGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
}

void gfx_device_gl::onPostRenderImGUI()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void gfx_device_gl::onInitImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	 	
	m_Window->initOpenGLImGUI();
	ImGui_ImplOpenGL3_Init();
}

void gfx_device_gl::onShutdownImGUI()
{
	ImGui_ImplOpenGL3_Shutdown();
}

void gfx_device_gl::onStartRender()
{

}
