
#ifndef MODEL_VIEWER_MODELASSET_H
#define MODEL_VIEWER_MODELASSET_H

#include "mesh_asset.h"
#include "../common/transform.h"
#include "shader_loader.h"
#include "texture_asset.h"

using color = glm::vec3;

namespace modelViewer::res
{
	enum class cull_face_mode {
		disabled,
		front,
		back
	};
	
    enum class texture_asset_type
    {
        texture2D,
        textureCube,
		texture2DArray
    };
    
    enum class texture_asset_mapping_mode
    {
        unsupported,
        uv
    };
    
    enum class texture_asset_wrapping_mode
    {
        unsupported,
        wrap,
        clamp,
        mirror
    };
	
	enum render_queue
	{
		render_queue_opaque = 0,
		render_queue_transparent = 1000
	};

	struct texture_embedded {
		unsigned char* data = nullptr;
		int dataSize = 0;
		int channelsCount = 3;
		int width = 0;
		int height = 0;
		bool isCompressed = false;
	};
	
    struct texture_asset_info
    {
        bool forceFlip = true;
    	bool isHightMap = false;
        std::vector<std::string> paths;
        unsigned int uvIndex = 0;
        texture_asset_mapping_mode mappingMode = texture_asset_mapping_mode::uv;
        texture_asset_wrapping_mode wrappingMode = texture_asset_wrapping_mode::wrap;
    	texture_asset_type type = texture_asset_type::texture2D;
    	std::string samplerName;
    	std::shared_ptr<texture_embedded> embedded = nullptr;
    };
    
    struct shader_asset_info
    {
        std::string path;
        shaderType type;
    };
	
	
	template<typename T>
	struct material_property
	{
		std::string  name;
		T value;
	};
    
    struct material_property_set
    {
		std::vector<material_property<float>> floats;
		std::vector<material_property<color>> colors;
		std::vector<material_property<bool>> booleans;
		std::vector<material_property<int>> ints;
		
	   bool depthWriteEnabled = true;
	   int renderQueue = render_queue_opaque;
	   cull_face_mode cullFaceMode = cull_face_mode::back;
	};
    
    struct material_asset
    {
        std::vector<texture_asset_info> textures;
        std::vector<shader_asset_info> shaders;
        material_property_set propertySet;
		int index = -1;
    };
    
    struct model_info {
        std::vector<std::shared_ptr<mesh_asset>> meshes;
		std::vector<std::shared_ptr<material_asset>> materials;
		common::transform transform;
        std::string name;
        std::string path;
    };

	namespace Literals
	{
		const std::string  AmbientAlbedo = "u_mat.ambient";
		const std::string  DiffuseAlbedo = "u_mat.diffuseAlbedo";
		const std::string  SpecularAlbedo = "u_mat.specularAlbedo";
		const std::string  Shininess = "u_mat.shininess";
		const std::string  Opacity = "u_mat.opacity";
		const std::string  Reflectivity = "u_mat.reflectivity";

		const color DefaultAmbientAlbedo{0.5f};
		const color DefaultDiffuseAlbedo{0.2f};
		const color DefaultSpecularAlbedo{0.5f};
		const float DefaultShininess = 16;
		const float DefaultOpacity = 1;
		const float DefaultReflectivity = 0;
	}
}

#endif
