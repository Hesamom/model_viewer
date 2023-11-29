
#include "model_platform_buffer.h"


using namespace modelViewer::render;
using namespace modelViewer::res;

std::shared_ptr<mesh> generateMesh(const model_platform_info& info)
{
    auto positions = std::make_shared<std::vector<glm::vec3>>();
    auto colors = std::make_shared<std::vector<glm::vec4>>();
    glm::vec4 mainLineColor(1,1,1,0.3f);
    glm::vec4 sideLineColor(1,1,1,0.1f);
    
    glm::vec3 startPos(-info.sizeX/2 * info.lineSpace, -1, info.sizeX/2 * info.lineSpace);
    glm::vec3 currentPos = startPos;
    for (int i = 0; i < info.sizeX; ++i) {

        positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
        positions->emplace_back(currentPos.x,currentPos.y, currentPos.z - (info.lineSpace * (info.sizeY - 1)));
        colors->emplace_back(mainLineColor);
        colors->emplace_back(mainLineColor);

        if (i == info.sizeX - 1)
            break;
        
        for (int j = 0; j < 10; ++j) {
            positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
            positions->emplace_back(currentPos.x,currentPos.y, currentPos.z - (info.lineSpace * (info.sizeY - 1)));
            colors->emplace_back(sideLineColor);
            colors->emplace_back(sideLineColor);
            currentPos.x += info.lineSpace/10;
        }
    }

    currentPos = startPos;
    for (int i = 0; i < info.sizeY; ++i) {

        positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
        positions->emplace_back(currentPos.x + (info.lineSpace * (info.sizeX - 1)),currentPos.y, currentPos.z);
        colors->emplace_back(mainLineColor);
        colors->emplace_back(mainLineColor);

        if (i == info.sizeY - 1)
            break;
        
        for (int j = 0; j < 10; ++j) {

            positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
            positions->emplace_back(currentPos.x + (info.lineSpace * (info.sizeX - 1)),currentPos.y, currentPos.z);
            colors->emplace_back(sideLineColor);
            colors->emplace_back(sideLineColor);
            currentPos.z -= info.lineSpace/10;
        }
    }

    auto indices = std::make_shared<std::vector<unsigned int>>();
    unsigned int index = 0;
    for (int i = 0; i < positions->size() ; ++i) {
        indices->push_back(index);
        indices->push_back(index + 1);
        index += 2;
    }
    
    auto meshAsset = std::make_shared<mesh_asset>();
    meshAsset->positions = positions;
    meshAsset->indices = indices;
    meshAsset->colors = colors;
    
    auto meshPtr = std::make_shared<mesh>(meshAsset);
    return meshPtr;
}

void model_platform_buffer::draw(glm::mat4 view, glm::mat4 projection) {
    m_Object->render(view, projection, render_mode::lines);
}

void model_platform_buffer::init(shader_loader &shaderLoader,
                                 const model_platform_info &info) {
    auto mesh = generateMesh(info);
    auto fragShaderAsset = shaderLoader.load(m_FragShaderPath, shaderType::fragment);
    auto vertexShaderAsset = shaderLoader.load(m_VertShaderPath, shaderType::vertex);
    shader fragShader (fragShaderAsset);
    fragShader.compile();
    
    shader vertexShader (vertexShaderAsset);
    vertexShader.compile();
    auto program = std::make_shared<shader_program>(std::initializer_list<shader>{fragShader, vertexShader});
    if (!program->isLinked())
    {
        std::cerr<< program->getLinkLog() << "\n";
    }
    
    
    
    m_Object = std::make_unique<render_object>(program, mesh, "platform");
}
