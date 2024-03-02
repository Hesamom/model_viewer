#ifndef MODEL_VIEWER_TEXTURE_FORMAT_H
#define MODEL_VIEWER_TEXTURE_FORMAT_H

#include "../../../libs/glew/include/GL/glew.h"
#include "../../resource/texture_asset.h"

namespace modelViewer::render {

    struct texture_format_type{
        std::function<unsigned char()> isSupported;
        GLint format_rgb = 0;
        GLint format_rgba = 0;
        std::string name;
    };

    class texture_format {
    private:
       static texture_format_type types[4];
    public:
        static GLint
        getOptimalFormat(modelViewer::res::texture_channel_type channelType, bool compress);
    };
}

#endif
