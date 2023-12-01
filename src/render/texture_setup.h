#ifndef MODEL_VIEWER_TEXTURE_SETUP_H
#define MODEL_VIEWER_TEXTURE_SETUP_H

#include "../resource/texture_asset.h"

namespace modelViewer::render {

    enum class texture_filtering_mode
    {
        nearest,
        linear,
        linear_nearest,
        nearest_linear,
        linear_linear,
        nearest_nearest
    };

    enum class texture_wrapping_mode
    {
        clamp_to_edge,
        mirror_clamp_to_edge,
        clamp_to_border,
        repeat,
        mirrored_repeat,
        wrap
    };

    class texture_setup {
    public:
        std::shared_ptr<modelViewer::res::texture_asset> m_Asset;
        unsigned int m_Mip_Map_Min_Level;
        unsigned int m_Mip_Map_Max_Level;
        bool m_Is_Mip_Map_Active;
        texture_filtering_mode m_Texture_Filtering_Min = texture_filtering_mode::linear;
        texture_filtering_mode m_Texture_Filtering_Mig = texture_filtering_mode::linear;
        texture_wrapping_mode m_Texture_Wrapping = texture_wrapping_mode::clamp_to_edge;

        texture_setup(std::shared_ptr<modelViewer::res::texture_asset> asset, texture_filtering_mode texture_filtering_min = texture_filtering_mode::linear, texture_filtering_mode texture_filtering_mig = texture_filtering_mode::linear, texture_wrapping_mode textureWrapping = texture_wrapping_mode::clamp_to_edge, bool is_mip_map_active = false, unsigned int mip_map_min_level = 0, unsigned int mip_map_max_level = 4);
    };
}


#endif
