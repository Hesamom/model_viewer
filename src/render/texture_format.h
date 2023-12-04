#ifndef MODEL_VIEWER_TEXTURE_FORMAT_H
#define MODEL_VIEWER_TEXTURE_FORMAT_H

#include "GL/glew.h"
#include "../resource/texture_asset.h"

namespace modelViewer::render {

    struct texture_format_type{
        GLboolean extension;
        GLint format_rgb = 0;
        GLint format_rgba = 0;
        std::string name;
    };

    class texture_format {
    public:
        static GLint
        getOptimalFormat(
                modelViewer::res::texture_channel_type channelType);
    };
}

#endif
