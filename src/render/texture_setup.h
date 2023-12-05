#ifndef MODEL_VIEWER_TEXTURE_SETUP_H
#define MODEL_VIEWER_TEXTURE_SETUP_H

#include "../resource/texture_asset.h"
#include "../resource/model_info.h"

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
        std::shared_ptr<modelViewer::res::texture_asset> asset;
        unsigned int mipMapMinLevel = 0;
        unsigned int mipMapMaxLevel = 1000;
        bool isMipMapActive = true;
        texture_filtering_mode filteringMin = texture_filtering_mode::linear_linear;
        texture_filtering_mode filteringMag = texture_filtering_mode::linear;
        texture_wrapping_mode wrapping = texture_wrapping_mode::clamp_to_edge;
        res::texture_asset_type type =  res::texture_asset_type::diffuse;
        bool compress = true;
    };
}


#endif
