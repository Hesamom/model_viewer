#include <imgui/imgui.h>
#include "modelviewer_app.h"
#include "regex"
#include "../common/stopwatch.h"
#include "../render/renderer_simple.h"
#include "../resource/shader_asset.h"

using namespace modelViewer::res;
using namespace modelViewer::render;
using namespace modelViewer::common;

glm::vec3 getPosition(float pitch, float yaw, float zoomLevel);

void modelviewer_app::onRender(float elapsed) {
    addNewModels();
	m_Renderer->render(m_Scene, m_Camera, true, true);
}


void modelviewer_app::addModel(model_info& info) {
    
    m_NewModelsQueue.push_back(info);
}


modelviewer_app::modelviewer_app(std::shared_ptr<window>& window, std::shared_ptr<gfx_device>& device,std::shared_ptr<modelViewer::debug::tracer>& tracer, bool simpleRenderer) : m_ObjectFactory(device)
{
	m_Window = window;
	m_Device = device;
	m_Tracer = tracer;

	if (simpleRenderer) {
		m_Renderer = std::make_unique<renderer_simple>(m_Device);
		m_Renderer->setClearFlag(glm::vec4{1,0,0,1});
	}
	else {
		m_Renderer = std::make_unique<renderer_forward>(m_Device, m_ObjectFactory);
		m_Renderer->setClearFlag(glm::vec4{0,0,0,1});
		m_Renderer->setReflectionPosition({0,0,0});
		m_Renderer->setReflectionClearFlag({0,0,0,0});

		model_platform_info info;
		info.sizeZ = 12;
		info.sizeX = 12;
		info.lineSpace = 1;

		auto plane = m_Platform.generatePlane(m_ObjectFactory, info, m_Device);
		auto grid = m_Platform.generateGrid(m_ObjectFactory, info, m_Device);
		m_Scene.addStaticObject(plane);
		m_Scene.addStaticObject(grid);
	}

	m_Window->setOnMouseButtonCallback(std::bind(&modelviewer_app::onMouseButtonCallback, this, std::placeholders::_1));
	m_Window->setOnSizeChangedCallback(std::bind(&modelviewer_app::onSizeChanged, this, std::placeholders::_1, std::placeholders::_2));
	m_Window->setOnMouseScrollCallback(std::bind(&modelviewer_app::onScrollChanged, this, std::placeholders::_1));
	
	setTargetFrameRate(360);
    updateCameraPosition();
	m_Camera.setViewPort(m_Window->getWidth(), m_Window->getHeight());
	m_Device->onInitImGUI();
}

modelviewer_app::~modelviewer_app() {
    
	m_Device->onShutdownImGUI();
	m_Window->onShutdownImGUI();
	ImGui::DestroyContext();
}

std::string getLightObjectName(int id) {
	return "light_object_" + std::to_string(id);
}

void modelviewer_app::addSpotLight(light_spot spot) {

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

void modelviewer_app::addPointLight(light_point point) {

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


glm::vec3 getPosition(float pitch, float yaw, float zoomLevel)
{
    float xPos = cos(glm::radians(pitch)) * sin(glm::radians(yaw)) * zoomLevel;
    float yPos = sin(glm::radians(pitch)) * zoomLevel;
    float zPos = cos(glm::radians(pitch)) * cos(glm::radians(yaw)) * zoomLevel;
    return {xPos,yPos, zPos};
}

void modelviewer_app::onScrollChanged(int yOffset) {
    
    //down -1, up 1
    m_ZoomLevel += yOffset * -1 * 3;
    m_ZoomLevel = std::clamp<float>(m_ZoomLevel, 1, 50);
    
    updateCameraPosition();
}

void modelviewer_app::onMouseButtonCallback(window::mouse_event event) {
	
	/*std::cout<< "onMouseButtonCallback, left: " << event.leftPressed << " right: " << event.rightPressed << " middle: " 
	<< event.middlePressed<< " x: " << event.x
	 << ", y: " << event.y << std::endl;*/
	
    m_IsMouseButtonDown = event.middlePressed;

	if (m_IsMouseButtonDown)
	{
		auto deltaY = m_LastMousePosition.y - event.y;
		m_PitchAngle += -deltaY * AngleChangeMul;
		m_PitchAngle = std::clamp<float>(m_PitchAngle, PitchAngleRange.x,PitchAngleRange.y);

		auto deltaX = m_LastMousePosition.x - event.x;
		m_YawAngle += deltaX * AngleChangeMul;

		updateCameraPosition();
	}

	m_LastMousePosition.x = event.x;
	m_LastMousePosition.y = event.y;
}


std::string modelviewer_app::label(std::string str, int id) {
	return str + "##" + std::to_string(id);
}

void modelviewer_app::drawReflectionSettings()
{
	int biasedIndex = 2567;
	ImGui::Text("Reflection");
	auto& reflectionPos = m_Renderer->getReflectionPosition();
	ImGui::SliderFloat3(label("position", biasedIndex).c_str(), &reflectionPos.x, -10,10, "%.2f");
}


void modelviewer_app::drawSpotLightSettings() {

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

void modelviewer_app::drawPointLightSettings() {
	
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

void modelviewer_app::drawDirectionalLightSettings() {
	auto& directionalLight = m_Scene.getDirectionalLight();
	ImGui::Text("Directional light");
	ImGui::ColorEdit3("ambient", &directionalLight.ambient.x, ImGuiColorEditFlags_NoAlpha);
	ImGui::ColorEdit3("diffuse", &directionalLight.diffuse.x, ImGuiColorEditFlags_NoAlpha);
	ImGui::SliderFloat3("direction", &directionalLight.direction.x, -1,1, "%.2f");
}

void modelviewer_app::displayLightPanel() {

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

	drawReflectionSettings();
	drawDirectionalLightSettings();
	drawSpotLightSettings();
	drawPointLightSettings();
	
	ImGui::End();
	ImGui::PopStyleColor();
}

void modelviewer_app::updateCameraPosition() {
    auto pos = getPosition(m_PitchAngle, m_YawAngle, m_ZoomLevel);
	m_Camera.setPosition(pos);
}


void modelviewer_app::displayMenubar() {
	
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

			if (ImGui::BeginMenu("Specular mapping")) {
				if(ImGui::MenuItem("wall"))
				{
					openSpecularMapModel();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Complex models")) {
				if(ImGui::MenuItem("backpack (multi mesh)"))
				{
					openModel(modelViewer::res::literals::models::complex_backpack);
				}
				ImGui::EndMenu();
			}
			

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("clear"))
		{
			if(ImGui::MenuItem("color"))
			{
				m_Renderer->setClearMode(clear_mode::color);
			}
			if(ImGui::MenuItem("skybox"))
			{
				m_Renderer->setClearMode(clear_mode::skybox);
			}
			ImGui::EndMenu();
		}
	  
		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void modelviewer_app::onPreRenderImGUI() {

	m_Device->onPreRenderImGUI();
	m_Window->onNewImGUIFrame();
	ImGui::NewFrame();
}

void modelviewer_app::onSizeChanged(int width, int height)
{
	if(height == 0 || width == 0) {
		return;
	}
	
	m_Device->resize(width, height);
	m_Camera.setViewPort(width, height);
}

void modelviewer_app::addSpotLight() {
	
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

void modelviewer_app::addPointLight() {
	light_point point{};
	point.position =  glm::vec3{0,20,0};
	point.ambient =  glm::vec3{0.2f,0,0};
	point.diffuse =  glm::vec3{1,0,0};
	point.setRange(100,1);
	point.id = m_LastLightId;
	m_LastLightId++;
	
	addPointLight(point);
}

void modelviewer_app::displayFilePicker() {

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

void modelviewer_app::openWallParallaxMap() {

	model_info wallModel;
	shader_asset_info fragShader { modelViewer::res::literals::shaders::parallax_frag, shaderType::fragment};
	shader_asset_info vertShader { modelViewer::res::literals::shaders::parallax_vert, shaderType::vertex};

	auto material = std::make_shared<material_asset>();
	material->shaders.push_back(fragShader);
	material->shaders.push_back(vertShader);

	texture_asset_info diffuseTexture;
	diffuseTexture.type = texture_asset_type::texture2D;
	diffuseTexture.paths.emplace_back(modelViewer::res::literals::textures::parallax_diffuse);
	diffuseTexture.samplerName = "diffuseSampler";
	material->textures.push_back(diffuseTexture);

	texture_asset_info normalTexture;
	normalTexture.type = texture_asset_type::texture2D;
	normalTexture.paths.emplace_back(modelViewer::res::literals::textures::parallax_normal);
	normalTexture.samplerName = "normalSampler";
	material->textures.push_back(normalTexture);

	texture_asset_info parallaxMappingTexture;
	parallaxMappingTexture.type = texture_asset_type::texture2D;
	parallaxMappingTexture.paths.emplace_back(modelViewer::res::literals::textures::parallax_displacement);
	parallaxMappingTexture.samplerName = "u_depthSampler";
	parallaxMappingTexture.isHightMap = true;
	material->textures.push_back(parallaxMappingTexture);
	
	wallModel.materials.push_back(material);
	wallModel.path = modelViewer::res::literals::models::primitive_plane;
	wallModel.name = "wall";
	material->propertySet.colors.push_back({Literals::DiffuseAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::AmbientAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::SpecularAlbedo, glm::vec3 (0.2f)});
	material->propertySet.floats.push_back({Literals::Shininess, 10});
	material->propertySet.floats.push_back({Literals::Opacity, 1});
	material->propertySet.cullFaceMode = cull_face_mode::disabled;
	addModel(wallModel);
}

void modelviewer_app::openWallNormalMap() {
	model_info wallModel;
	shader_asset_info fragShader { modelViewer::res::literals::shaders::normal_map_frag, shaderType::fragment};
	shader_asset_info vertShader { modelViewer::res::literals::shaders::normal_map_vert, shaderType::vertex};

	auto material = std::make_shared<material_asset>();
	material->shaders.push_back(fragShader);
	material->shaders.push_back(vertShader);

	texture_asset_info diffuseTexture;
	diffuseTexture.type = texture_asset_type::texture2D;
	diffuseTexture.paths.emplace_back(modelViewer::res::literals::textures::normal_diffuse);
	diffuseTexture.samplerName = "diffuseSampler";
	material->textures.push_back(diffuseTexture);

	texture_asset_info normalTexture;
	normalTexture.type = texture_asset_type::texture2D;
	normalTexture.paths.emplace_back(modelViewer::res::literals::textures::normal_normal);
	normalTexture.samplerName = "normalSampler";
	material->textures.push_back(normalTexture);


	wallModel.materials.push_back(material);
	wallModel.path = modelViewer::res::literals::models::primitive_plane;
	wallModel.name = "wall";
	material->propertySet.colors.push_back({Literals::DiffuseAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::AmbientAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::SpecularAlbedo, glm::vec3 (0.2f)});
	material->propertySet.floats.push_back({Literals::Shininess, 32});
	material->propertySet.floats.push_back({Literals::Opacity, 1});
	material->propertySet.cullFaceMode = cull_face_mode::disabled;
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

void modelviewer_app::addNewModels()
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

void modelviewer_app::openDemoModel(std::string name)
{
	model_info info = getDemoModel(name);
	addModel(info);
}

void modelviewer_app::openModel(std::string path)
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

model_info modelviewer_app::getDemoModel(const std::string& name) const
{
	model_info info;
	shader_asset_info fragShader { modelViewer::res::literals::shaders::lit_frag, shaderType::fragment};
	shader_asset_info vertShader { modelViewer::res::literals::shaders::lit_vert, shaderType::vertex};
	
	auto material = std::make_shared<material_asset>();
	material->shaders.push_back(fragShader);
	material->shaders.push_back(vertShader);
	addDefaults(material->propertySet);

	texture_asset_info textureAssetInfo;
	textureAssetInfo.samplerName = "diffuseSampler";
	textureAssetInfo.paths.emplace_back(modelViewer::res::literals::textures::uv_checker);
	material->textures.push_back(textureAssetInfo);

	info.materials.push_back(material);
	info.path =  modelViewer::res::literals::models::primitive_path + name + ".fbx";
	info.name = name;
	return info;
}


void modelviewer_app::openSpecularMapModel()
{
	model_info wallModel;
	shader_asset_info fragShader { modelViewer::res::literals::shaders::lit_frag, shaderType::fragment};
	shader_asset_info vertShader { modelViewer::res::literals::shaders::lit_vert, shaderType::vertex};

	auto material = std::make_shared<material_asset>();
	material->shaders.push_back(fragShader);
	material->shaders.push_back(vertShader);

	texture_asset_info diffuseTexture;
	diffuseTexture.type = texture_asset_type::texture2D;
	diffuseTexture.paths.emplace_back(modelViewer::res::literals::textures::spec_diffuse);
	diffuseTexture.samplerName = "diffuseSampler";
	material->textures.push_back(diffuseTexture);

	texture_asset_info specTexture;
	specTexture.type = texture_asset_type::texture2D;
	specTexture.paths.emplace_back(modelViewer::res::literals::textures::spec_spec);
	specTexture.samplerName = "specularSampler";
	material->textures.push_back(specTexture);

	wallModel.materials.push_back(material);
	wallModel.path = modelViewer::res::literals::models::primitive_plane;
	wallModel.name = "wall_spec";
	material->propertySet.colors.push_back({Literals::DiffuseAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::AmbientAlbedo, glm::vec3 (1.0f)});
	material->propertySet.colors.push_back({Literals::SpecularAlbedo, glm::vec3 (1.0f)});
	material->propertySet.floats.push_back({Literals::Shininess, 256});
	material->propertySet.floats.push_back({Literals::Opacity, 1});
	material->propertySet.cullFaceMode = cull_face_mode::disabled;
	addModel(wallModel);
}

void capTargetFrameRate(double elapsed, int targetFps)
{
	if(targetFps == -1)
	{
		return;
	}

	int fps = 1/elapsed;
	if (fps > targetFps)
	{
		double targetElapsedTime = 1.0/targetFps ;
		double remaining = (targetElapsedTime - elapsed);
		stopwatch stopwatch;
		stopwatch.start();
		do{
			stopwatch.stop();
		}
		while(stopwatch.getSeconds() < remaining);

		stopwatch.stop();
	}
}

void modelviewer_app::loop()
{
	stopwatch watch;
	watch.start();
	while (!m_Window->shouldClose())
	{
		if (m_Window->isPaused())
		{
			m_Window->pollEvents();
			//TODO fix this 
			//std::this_thread::sleep_for(100);
			continue;
		}
		m_Window->pollEvents();
		
		if (m_Window->getWidth() == 0)
		{
			m_Window->pollEvents();
			continue;
		}

		watch.stop();
		double elapsed = watch.getSeconds();
		watch.start();

		m_Window->pollEvents();
		//onPreRenderImGUI();
		//onRenderImGUI();
		m_Device->onStartRender();
		onRender((float)elapsed);
		//onPostRenderImGUI();
		m_Device->swapBuffers();
		
		watch.stop();
		m_elapsedTimeSinceStart += watch.getSeconds();
		capTargetFrameRate(watch.getSeconds(), m_TargetFrameRate);
	}
}

void modelviewer_app::setTargetFrameRate(int fps)
{
	m_TargetFrameRate = fps;
}

int modelviewer_app::getTargetFrameRate()
{
	return m_TargetFrameRate;
}

void modelviewer_app::onKeyPressed(int key, int action, int mods)
{

}

void modelviewer_app::onPostRenderImGUI()
{
	m_Device->onPostRenderImGUI();
}

void modelviewer_app::onRenderImGUI()
{
	//ImGui::ShowDemoWindow();
	displayLightPanel();
	displayMenubar();
	
	ImGui::Render();
}
