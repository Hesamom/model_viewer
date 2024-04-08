
#ifndef LITERALS_H
#define LITERALS_H

namespace modelViewer::res::literals
{
	namespace models
	{
		const std::string default_path = "res/models";
		const std::string primitive_path = "res/models/primitives/";

		const std::string primitive_cube = default_path + "/primitives/cube.fbx";
		const std::string primitive_plane = default_path +  "/primitives/plane.fbx";
		const std::string primitive_sphere = default_path +  "/primitives/sphere.obj";
		const std::string primitive_cylinder = default_path +  "/primitives/cylinder.obj";

		const std::string complex_backpack = default_path +  "/complex/backpack/backpack.obj";
	}
	
	namespace textures
	{
		const std::string base_path = "res/textures";
		const std::string default_path = base_path + "/default";
		
		const std::string default_black = default_path + "/black.png";
		const std::string default_normal = default_path + "/normal.png";
		const std::string default_white = default_path + "/white.png";

		const std::string default_shadowmap = default_white;
		const std::string uv_checker = base_path + "/uv_checker.png";
		const std::string uv_checker_dds = base_path + "/uv_checker.dds";

		const std::string cat = base_path + "/sample.png";
		const std::string bar = base_path + "/bar_test.png";
		
		const std::string parallax_diffuse = base_path + "/parallax/diffuse.jpg";
		const std::string parallax_displacement = base_path + "/parallax/displacement.jpg";
		const std::string parallax_normal = base_path + "/parallax/normal.jpg";

		const std::string normal_diffuse = base_path + "/normal/diffuse.jpg";
		const std::string normal_normal = base_path + "/normal/normal.jpg";

		const std::string spec_diffuse = base_path + "/specular/diffuse.png";
		const std::string spec_spec = base_path + "/specular/spec.png";

		const std::string skybox_sample_path =  base_path + "/skybox/sample";
		
		const std::string skybox_right = skybox_sample_path + "/right.jpg";
		const std::string skybox_left = skybox_sample_path + "/left.jpg";
		const std::string skybox_top = skybox_sample_path + "/top.jpg";
		const std::string skybox_bottom = skybox_sample_path + "/bottom.jpg";
		const std::string skybox_front = skybox_sample_path + "/front.jpg";
		const std::string skybox_back = skybox_sample_path + "/back.jpg";
	}
	
	
	namespace shaders
	{
#ifdef GFX_DX
		const std::string default_path = "res/shaders/dx";
		const std::string include_path = "res/shaders/include/";
			
		const std::string skybox_vert = default_path + "/skybox.hlsl";
		const std::string skybox_frag = skybox_vert;

		const std::string sample_vert = default_path + "/sample.hlsl";
		const std::string sample_frag = sample_vert;
		
		const std::string lit_vert = sample_vert;
		const std::string lit_frag = sample_frag;

		const std::string line_vert = default_path + "/lines.hlsl";
		const std::string line_frag = line_vert;

		//TODO port these shader to hlsl:
		const std::string shadow_vert = default_path + "/simple_depth_vert.glsl";
		const std::string shadow_frag = default_path + "/simple_depth_frag.glsl";

		const std::string lit_vertex_vert = default_path + "/phong_gouraud_vert.glsl";
		const std::string lit_vertex_frag = default_path + "/phong_gouraud_frag.glsl";
		
		const std::string normal_map_vert = default_path + "/phong_phong_normal_map_vert.glsl";
		const std::string normal_map_frag = default_path + "/phong_phong_normal_map_frag.glsl";

		const std::string parallax_vert = normal_map_vert;
		const std::string parallax_frag = default_path + "/phong_phong_parallax_mapping_frag.glsl";;
		
#endif
#ifdef GFX_GL
	
		const std::string default_path = "res/shaders/gl/sample";
		const std::string include_path = "res/shaders/gl/include/";
		
		const std::string shadow_vert = default_path + "/simple_depth_vert.glsl";
		const std::string shadow_frag = default_path + "/simple_depth_frag.glsl";
		
		const std::string sample_vert = default_path + "/sample_vert.glsl";
		const std::string sample_frag = default_path + "/sample_frag.glsl";

		const std::string lit_vertex_vert = default_path + "/phong_gouraud_vert.glsl";
		const std::string lit_vertex_frag = default_path + "/phong_gouraud_frag.glsl";
		
		const std::string lit_vert = default_path + "/phong_phong_vert.glsl";
		const std::string lit_frag = default_path + "/phong_phong_frag.glsl";

		const std::string line_vert = default_path + "/lines_vert.glsl";
		const std::string line_frag = default_path + "/lines_frag.glsl";

		const std::string normal_map_vert = default_path + "/phong_phong_normal_map_vert.glsl";
		const std::string normal_map_frag = default_path + "/phong_phong_normal_map_frag.glsl";

		const std::string parallax_vert = normal_map_vert;
		const std::string parallax_frag = default_path + "/phong_phong_parallax_mapping_frag.glsl";;

		const std::string skybox_vert = default_path + "/skybox_vert.glsl";
		const std::string skybox_frag = default_path + "/skybox_frag.glsl";
#endif
	}
	
}

#endif //LITERALS_H
