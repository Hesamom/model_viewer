#include <imgui/imgui.h>
#include "glm/glm.hpp"
#include "modelviewer_window.h"
#include "glfw/glfw3.h"
#include "regex"

using namespace modelViewer::res;
using namespace modelViewer::render;
using namespace modelViewer::common;

glm::vec3 getPosition(float pitch, float yaw, float zoomLevel);

void modelviewer_window::onRender(float elapsed) {
    addNewModels();
	m_Renderer.render(m_Scene, m_Camera, true);
}


void modelviewer_window::addModel(modelViewer::res::model_info& info) {
    
    m_NewModelsQueue.push_back(info);
}


modelviewer_window::modelviewer_window(int width, int height, std::string title, bool fullscreen) : window(width,
                                                                                                           height,
                                                                                                           title,
                                                                                                           fullscreen)
    {
    
    setVsync(false);
    setTargetFrameRate(360);
    updateCameraPosition();
	//TODO set viewport when the window size changes too
	m_Camera.setViewPort(getWidth(), getHeight());
	m_Renderer.init(m_ObjectFactory);
    
    model_platform_info info;
    info.sizeZ = 12;
    info.sizeX = 12;
    info.lineSpace = 1;
	
	auto plane = m_Platform.generatePlane(m_ObjectFactory, info);
	auto grid = m_Platform.generateGrid(m_ObjectFactory, info);
	m_Scene.addStaticObject(plane);
	m_Scene.addStaticObject(grid);
}

modelviewer_window::~modelviewer_window() {
    
}

std::string getLightObjectName(int id) {
	return "light_object_" + std::to_string(id);
}

void modelviewer_window::addSpotLight(light_spot spot) {

	auto spotModel1 = getDemoModel("sphere");
	spotModel1.transform.setPosition(spot.position);
	spotModel1.transform.setScale(glm::vec3(0.1f));
	spotModel1.name = getLightObjectName(spot.id);
	auto spotObject = m_ObjectFactory.createObject(spotModel1);
	spotObject->setCastShadow(false);
	m_Scene.addStaticObject(spotObject);
	spot.object = spotObject;
	
	m_Scene.addSpotLight(spot);
}

void modelviewer_window::addPointLight(light_point point) {

	auto lightObjectModel = getDemoModel("cube");
	lightObjectModel.transform.setPosition(point.position);
	lightObjectModel.transform.setScale(glm::vec3(0.1f));
	lightObjectModel.name = getLightObjectName(point.id);
	
	auto lightObject = m_ObjectFactory.createObject(lightObjectModel);
	lightObject->setCastShadow(false);
	m_Scene.addStaticObject(lightObject);
	point.object = lightObject;
	
	m_Scene.addPointLight(point);
}

void modelviewer_window::setClearFlag(glm::vec4 color) {
    
	m_Renderer.setClearFlag(color);
}

glm::vec3 getPosition(float pitch, float yaw, float zoomLevel)
{
    float xPos = cos(glm::radians(pitch)) * sin(glm::radians(yaw)) * zoomLevel;
    float yPos = sin(glm::radians(pitch)) * zoomLevel;
    float zPos = cos(glm::radians(pitch)) * cos(glm::radians(yaw)) * zoomLevel;
    return {xPos,yPos, zPos};
}

void modelviewer_window::onScrollChanged(double yOffset) {
    
    //down -1, up 1
    m_ZoomLevel += yOffset * -1 * 3;
    m_ZoomLevel = std::clamp<float>(m_ZoomLevel, 1, 50);
    
    updateCameraPosition();
}

void modelviewer_window::onMouseButtonChanged(int button, int action, int mods) {
    
    if (button !=  GLFW_MOUSE_BUTTON_MIDDLE)
    {
        return;
    }

    m_IsMouseButtonDown = action == GLFW_REPEAT || action == GLFW_PRESS;
}


void modelviewer_window::onMousePositionChanged(double xpos, double ypos) {
    
    if (m_IsMouseButtonDown)
    {
        auto deltaY = m_LastMousePosition.y - ypos;
        m_PitchAngle += -deltaY * AngleChangeMul;
        m_PitchAngle = std::clamp<float>(m_PitchAngle, PitchAngleRange.x,PitchAngleRange.y);

        auto deltaX = m_LastMousePosition.x - xpos;
        m_YawAngle += deltaX * AngleChangeMul;
        
        updateCameraPosition();
    }

    m_LastMousePosition.x = xpos;
    m_LastMousePosition.y = ypos;
}

std::string modelviewer_window::label(std::string str, int id) {
	return str + "##" + std::to_string(id);
}


void modelviewer_window::drawSpotLightSettings() {

	ImGui::Dummy(ImVec2(0.0f,20.0f));
	ImGui::Text("Spot lights");
	
	auto& spotLights = m_Scene.getSpotLights();
	
	ImGui::Dummy(ImVec2(0.0f,20.0f));
	
	if (spotLights.empty()) {
		ImGui::Text("No active spot lights");
	}

	int index = 0;
	constexpr int bias = 1000;
	int biasedIndex = index + bias;
	std::vector<int> removedLights;

	if (spotLights.size() < 4 && ImGui::Button(label("add new", biasedIndex).c_str())) {
		addSpotLight();
	}
	
	for (auto& light: spotLights) {
		
		ImGui::Text(label("Light @" + std::to_string(index), biasedIndex).c_str());
		
		ImGui::ColorEdit3(label("ambient", biasedIndex).c_str(), &light.ambient.x, ImGuiColorEditFlags_NoAlpha);
		ImGui::ColorEdit3(label("diffuse",biasedIndex).c_str(),  &light.diffuse.x, ImGuiColorEditFlags_NoAlpha);
		
		ImGui::SliderFloat3(label("direction",biasedIndex).c_str(), &light.direction.x, -1,1, "%.2f");
		ImGui::SliderFloat3(label("position", biasedIndex).c_str(), &light.position.x, -10,10, "%.2f");

		ImGui::SliderFloat(label("outter cutoff angle",biasedIndex).c_str(), &light.outerCutoff, 0,360, "%.2f");
		ImGui::SliderFloat(label("inner cutoff angle", biasedIndex).c_str(), &light.innerCutoff, 0,light.outerCutoff, "%.2f");

		const auto object = light.object;
		object->getTransform().setPosition(light.position);
		
		if (ImGui::Button(label("remove", biasedIndex).c_str())) {
			removedLights.push_back(index);
		}
		index++;
		biasedIndex = index + bias;
		ImGui::Dummy(ImVec2(0.0f,20.0f));
	}

	for (int i = removedLights.size() - 1; i >= 0; --i) {
		
		const int lightIndex = removedLights[i];
		const auto light = m_Scene.getSpotLights()[lightIndex];
		
		auto object = light.object;
		if (object != nullptr) {
			m_Scene.removeStaticObject(object);
		}
		
		m_Scene.removeSpotLight(lightIndex);
	}
}

void modelviewer_window::drawPointLightSettings() {
	
	ImGui::Dummy(ImVec2(0.0f,20.0f));
	ImGui::Text("Point lights");

	auto& pointLights = m_Scene.getPointLights();
	if (pointLights.empty()) {
		ImGui::Text("No active point lights");
	
	}
	
	
	
	int index = 0;
	constexpr int bias = 5000;
	int biasedIndex = index + bias;

	if (pointLights.size() < 4 && ImGui::Button(label("add new", biasedIndex).c_str())) {
		addPointLight();
	}

	std::vector<int> removedLights;
	for (auto& light: pointLights) {
		ImGui::Text(label("Light#" + std::to_string(index), biasedIndex).c_str());
		
		ImGui::ColorEdit3(label("ambient", biasedIndex).c_str(), &light.ambient.x, ImGuiColorEditFlags_NoAlpha);
		ImGui::ColorEdit3(label("diffuse", biasedIndex).c_str(),  &light.diffuse.x, ImGuiColorEditFlags_NoAlpha);
		
		ImGui::SliderFloat3(label("position", biasedIndex).c_str(), &light.position.x, -10,10 ,"%.2f");

		float range = 0;
		float insideRange = 0;
		light.getRanges(range,insideRange);
		ImGui::SliderFloat(label("range", biasedIndex).c_str(), &range, 0,1000, "%.2f");
		ImGui::SliderFloat(label("inside range", biasedIndex).c_str(), &insideRange, 0,range, "%.2f");
		insideRange = std::clamp<float>(insideRange, 0, range);
		light.setRange(range, insideRange);

		const auto object = light.object;
		object->getTransform().setPosition(light.position);

		if (ImGui::Button(label("remove", biasedIndex).c_str())) {
			removedLights.push_back(index);
		}
		
		index++;
		biasedIndex = index + bias;
		ImGui::Dummy(ImVec2(0.0f,20.0f));
	}

	for (int i = removedLights.size() - 1; i >= 0; --i) {
		
		const int lightIndex = removedLights[i];
		const auto light = m_Scene.getPointLights()[lightIndex];
		
		auto object = light.object;
		if (object != nullptr) {
			m_Scene.removeStaticObject(object);
		}
		
		m_Scene.removePointLight(lightIndex);
	}
}

void modelviewer_window::drawDirectionalLightSettings() {
	auto& directionalLight = m_Scene.getDirectionalLight();
	ImGui::Text("Directional light");
	ImGui::ColorEdit3("ambient", &directionalLight.ambient.x, ImGuiColorEditFlags_NoAlpha);
	ImGui::ColorEdit3("diffuse", &directionalLight.diffuse.x, ImGuiColorEditFlags_NoAlpha);
	ImGui::SliderFloat3("direction", &directionalLight.direction.x, -1,1, "%.2f");
}

void modelviewer_window::displayLightPanel() {

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	constexpr int size = 300;
	ImGui::SetNextWindowPos(ImVec2( main_viewport->WorkSize.x - size, main_viewport->WorkPos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(size, main_viewport->WorkSize.y), ImGuiCond_Always);

	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoCollapse
	| ImGuiWindowFlags_NoMove
	| ImGuiWindowFlags_NoResize
	| ImGuiWindowFlags_MenuBar;
	
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0,0,0,0.7f));
	if (!ImGui::Begin("Light panel", &m_isImGUIOpen, window_flags))
	{
		ImGui::PopStyleColor();
		ImGui::End();
		return;
	}

	drawDirectionalLightSettings();
	drawSpotLightSettings();
	drawPointLightSettings();
	
	ImGui::End();
	ImGui::PopStyleColor();
}

void modelviewer_window::updateCameraPosition() {
    auto pos = getPosition(m_PitchAngle, m_YawAngle, m_ZoomLevel);
	m_Camera.setPosition(pos);
}


void modelviewer_window::displayMenubar() {
	
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, 0), ImGuiCond_Once);

	ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | 
			ImGuiWindowFlags_NoCollapse | 
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBackground;

	if (!ImGui::Begin("Main window", nullptr, window_flags))
	{
		ImGui::End();
		return;
	}

	// Menu Bar
	if (ImGui::BeginMenuBar()) 
	{
		if (ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Open"))
			{
				displayFilePicker();
			}
			ImGui::EndMenu();
		}
        

		if (ImGui::BeginMenu("Demo"))
		{

			
			if (ImGui::BeginMenu("Simple lighting")) {
				
				std::string demoObjects[4] = {"cube","sphere","cylinder","plane"};

				for (auto& model : demoObjects)
				{
					if(ImGui::MenuItem(model.data()))
					{
						openDemoModel(model);
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Normal mapping")) {
				if(ImGui::MenuItem("wall"))
				{
					openWallNormalMap();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Parallax mapping")) {
				if(ImGui::MenuItem("wall"))
				{
					openWallParallaxMap();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("complex models")) {
				if(ImGui::MenuItem("backpack (multi mesh)"))
				{
					openModel("res/models/complex/backpack/backpack.obj");
				}
				ImGui::EndMenu();
			}
			

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("clear"))
		{
			if(ImGui::MenuItem("color"))
			{
				setClearMode(clear_mode::color);
			}
			if(ImGui::MenuItem("skybox"))
			{
				setClearMode(clear_mode::skybox);
			}
			ImGui::EndMenu();
		}
	  
		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void modelviewer_window::onRenderImGUI() {
	
	//ImGui::ShowDemoWindow();
	displayLightPanel();
	displayMenubar();
}

void modelviewer_window::onSizeChanged(int height, int width)
{
	window::onSizeChanged(height, width);
	
	if(height == 0 || width == 0) {
		return;
	}
	glViewport(0, 0, width, height);
	m_Camera.setViewPort(width, height);
}

void modelviewer_window::addSpotLight() {
	
	light_spot spot;
	spot.outerCutoff = 24;
	spot.innerCutoff = 12;
	spot.position = glm::vec3{0,50,0};
	spot.direction = glm::vec3{0,-1,0};
	spot.diffuse = glm::vec3{0.8f};
	spot.ambient = glm::vec3{0.2f};
	spot.id = m_LastLightId;
	m_LastLightId++;

	addSpotLight(spot);
}

void modelviewer_window::addPointLight() {
	light_point point{};
	point.position =  glm::vec3{0,20,0};
	point.ambient =  glm::vec3{0.2f,0,0};
	point.diffuse =  glm::vec3{1,0,0};
	point.setRange(100,1);
	point.id = m_LastLightId;
	m_LastLightId++;
	
	addPointLight(point);
}

void modelviewer_window::displayFilePicker() {

	std::vector<file_filter> filters;
	filters.push_back({"fbx files", "fbx"});
	filters.push_back({"obj files", "obj"});
	filters.push_back({"stl files", "stl"});
	
    std::string path;
    path.reserve(256);
    if(m_FilePicker.tryOpenPicker(path,filters))
    {
		openModel(path.c_str());
    }
}

void modelviewer_window::openWallParallaxMap() {

	model_info wallModel;
	shader_asset_info fragShader { "res/shaders/sample/phong_phong_parallex_mapping_frag.glsl", shaderType::fragment};
	shader_asset_info vertShader { "res/shaders/sample/phong_phong_normal_map_vert.glsl", shaderType::vertex};

	auto material = std::make_shared<material_asset>();
	material->shaders.push_back(fragShader);
	material->shaders.push_back(vertShader);

	texture_asset_info diffuseTexture;
	diffuseTexture.type = texture_asset_type::texture2D;
	diffuseTexture.paths.emplace_back("res/textures/ParallexMapping/bricks2.jpg");
	diffuseTexture.samplerName = "u_diffuseSampler";
	material->textures.push_back(diffuseTexture);

	texture_asset_info normalTexture;
	normalTexture.type = texture_asset_type::texture2D;
	normalTexture.paths.emplace_back("res/textures/ParallexMapping/bricks2_normal.jpg");
	normalTexture.samplerName = "u_normalSampler";
	material->textures.push_back(normalTexture);

	texture_asset_info parallaxMappingTexture;
	parallaxMappingTexture.type = texture_asset_type::texture2D;
	parallaxMappingTexture.paths.emplace_back("res/textures/ParallexMapping/bricks2_disp.jpg");
	parallaxMappingTexture.samplerName = "u_depthSampler";
	parallaxMappingTexture.isHightMap = true;
	material->textures.push_back(parallaxMappingTexture);
	
	wallModel.materials.push_back(material);
	wallModel.path = "res/models/primitives/plane.fbx";
	wallModel.name = "wall";
	material->propertySet.colors.push_back({Literals::DiffuseAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::AmbientAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::SpecularAlbedo, glm::vec3 (0.2f)});
	material->propertySet.floats.push_back({Literals::Shininess, 10});
	material->propertySet.floats.push_back({Literals::Opacity, 1});
	material->propertySet.cullFaceEnabled = false;
	addModel(wallModel);
}

void modelviewer_window::openWallNormalMap() {
	model_info wallModel;
	shader_asset_info fragShader { "res/shaders/sample/phong_phong_normal_map_frag.glsl", shaderType::fragment};
	shader_asset_info vertShader { "res/shaders/sample/phong_phong_normal_map_vert.glsl", shaderType::vertex};

	auto material = std::make_shared<material_asset>();
	material->shaders.push_back(fragShader);
	material->shaders.push_back(vertShader);

	texture_asset_info diffuseTexture;
	diffuseTexture.type = texture_asset_type::texture2D;
	diffuseTexture.paths.emplace_back("res/textures/wall.jpg");
	diffuseTexture.samplerName = "u_diffuseSampler";
	material->textures.push_back(diffuseTexture);

	texture_asset_info normalTexture;
	normalTexture.type = texture_asset_type::texture2D;
	normalTexture.paths.emplace_back("res/textures/wall_normal.png");
	normalTexture.samplerName = "u_normalSampler";
	material->textures.push_back(normalTexture);


	wallModel.materials.push_back(material);
	wallModel.path = "res/models/primitives/plane.fbx";
	wallModel.name = "wall";
	material->propertySet.colors.push_back({Literals::DiffuseAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::AmbientAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::SpecularAlbedo, glm::vec3 (0.2f)});
	material->propertySet.floats.push_back({Literals::Shininess, 10});
	material->propertySet.floats.push_back({Literals::Opacity, 1});
	material->propertySet.cullFaceEnabled = false;
	addModel(wallModel);
}


void transformModel(const std::shared_ptr<object_renderer>& object) {
	object->computeBoundingBox();
	auto box = object->getBoundingBox();
	auto boxSize = box.getSize();
	float largestSize = std::max(boxSize.x,  std::max(boxSize.y, boxSize.z));
	const float MaxSize = 4;
	if (largestSize > MaxSize) {
		 object->getTransform().setScale(glm::vec3{MaxSize/largestSize});
	}
	
	if (box.getMin().y < 0)
	{
		 object->getTransform().translate(glm::vec3{0, -box.getMin().y, 0});
	}
	
	object->computeBoundingBox();
}

void modelviewer_window::addNewModels()
{
    //TODO the current imp is not efficient since it first loads the objects then applies limit, a better one should apply the limit when adding objects 
    for (auto& info: m_NewModelsQueue)
    {
		auto object = m_ObjectFactory.createObject(info);
    	transformModel(object);
		m_Scene.addModelObject(object);
    }

    m_NewModelsQueue.clear();
}

void addDefaults(material_property_set& set)
{
	set.floats.push_back({Literals::Opacity, Literals::DefaultOpacity});
	set.floats.push_back({Literals::Shininess, Literals::DefaultShininess});

	set.colors.push_back({Literals::AmbientAlbedo, Literals::DefaultAmbientAlbedo});
	set.colors.push_back({Literals::DiffuseAlbedo, Literals::DefaultDiffuseAlbedo});
	set.colors.push_back({Literals::SpecularAlbedo, Literals::DefaultSpecularAlbedo});
}

void modelviewer_window::openDemoModel(std::string name)
{
	model_info info = getDemoModel(name);
	addModel(info);
}

void modelviewer_window::openModel(std::string path)
{
	model_info info;
	info.path = path;
	info.name = path;
	
	try
	{
		m_ObjectFactory.getModelLoader().load(info.path, info);
	}
	catch (std::runtime_error& error)
	{
		std::cerr<< error.what() << std::endl;
		return;
	}
	
	addModel(info);
}

model_info modelviewer_window::getDemoModel(const std::string& name) const
{
	model_info info;
	shader_asset_info fragShader { "res/shaders/sample/phong_phong_frag.glsl", shaderType::fragment};
	shader_asset_info vertShader { "res/shaders/sample/phong_phong_vert.glsl", shaderType::vertex};
	
	auto material = std::make_shared<material_asset>();
	material->shaders.push_back(fragShader);
	material->shaders.push_back(vertShader);
	addDefaults(material->propertySet);

	texture_asset_info textureAssetInfo;
	textureAssetInfo.paths.emplace_back("res/textures/Transparent.png");
	material->textures.push_back(textureAssetInfo);

	info.materials.push_back(material);
	info.path = "res/models/primitives/" + name + ".fbx";
	info.name = name;
	return info;
}

void modelviewer_window::setClearMode(clear_mode mode)
{
	m_Renderer.setClearMode(mode);
}
