#include "texture_format.h"
#include "../resource/texture_asset.h"

using namespace modelViewer::render;
using namespace modelViewer::res;

const int unspecified = 0;

GLint
modelViewer::render::texture_format::getOptimalFormat(
        texture_channel_type channelType){
     texture_format_type types[3]={
             texture_format_type{GLEW_EXT_texture_compression_s3tc, unspecified, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, "S3TC"},
             texture_format_type{GLEW_EXT_texture_compression_bptc, unspecified, GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, "BPTC"},texture_format_type{GL_TRUE, GL_RGB, GL_RGBA, "Default"}
    };

    for (const auto& type : types) {
        if (type.extension)
        {
            switch (channelType) {
                case texture_channel_type::RGB:
                    if (type.format_rgb)
                    {
                        std::cout<< type.name << "_rgb"<< std::endl;
                        return type.format_rgb;
                    }
                    break;
                case texture_channel_type::RGBA:
                    if (type.format_rgba)
                    {
                        std::cout<< type.name << "_rgba"<< std::endl;
                        return type.format_rgba;
                    }
                    break;
                case texture_channel_type::NormalMap:
                    throw std::runtime_error("not supported");
                    break;
            }
        }
    }

    throw std::runtime_error("current driver doesn't support defined and base format");
}
