#ifndef MODEL_VIEWER_TEXTURE_SETUP_H
#define MODEL_VIEWER_TEXTURE_SETUP_H

#include "../resource/texture_asset.h"

namespace modelViewer::render {

    enum class texture_filtering_mode {
        nearest,
        linear,
        linear_nearest,
        nearest_linear,
        linear_linear,
        nearest_nearest
    };

    enum class texture_wrapping_mode {
        clamp_to_edge,
        mirror_clamp_to_edge,
        clamp_to_border,
        repeat,
        mirrored_repeat,
        wrap
    };

    struct texture_setup {
        std::shared_ptr<modelViewer::res::texture_asset> m_Asset;
        unsigned int m_Mip_Map_Min_Level;
        unsigned int m_Mip_Map_Max_Level;
        bool m_Is_Mip_Map_Active;
        texture_filtering_mode m_Texture_Filtering_Min = texture_filtering_mode::linear_linear;
        texture_filtering_mode m_Texture_Filtering_Mag = texture_filtering_mode::linear;
        texture_wrapping_mode m_Texture_Wrapping = texture_wrapping_mode::clamp_to_edge;
    };
}


#endif
