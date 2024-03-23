#include "texture_format.h"
#include "../../resource/texture_asset.h"

using namespace modelViewer::render;
using namespace modelViewer::res;

const int unspecified = 0;

texture_format_type texture_format::types[]={
        texture_format_type{
            [](){return GLEW_KHR_texture_compression_astc_ldr;},
            unspecified,
            GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
            "ASTC"},
            
            texture_format_type{
            [](){return GLEW_EXT_texture_compression_s3tc;},
            GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 
            GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, "S3TC"},
            
            texture_format_type{
            [](){return GLEW_EXT_texture_compression_bptc;}, 
            unspecified, GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, "BPTC"},
            
            texture_format_type{
            [](){return GL_TRUE;}, GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA, "General compressed"}
};

bool tryGetFormat(const texture_format_type& type, texture_channel_type channelType, GLint& format, std::string& name)
{
    if (!type.isSupported())
    {
        return false;
    }

    switch (channelType) {
        case texture_channel_type::RGB:
            if (type.format_rgb) {
                format = type.format_rgb;
                name = type.name;
                return true;
            }
            return false;
        case texture_channel_type::RGBA:
            if (type.format_rgba) {
                format = type.format_rgba;
                name = type.name;
                return true;
            }
            return false;
        case texture_channel_type::NormalMap:
            throw std::runtime_error("not supported");
    }    
    
}

GLint
modelViewer::render::texture_format::getOptimalFormat(texture_channel_type channelType, bool compress)
{
    if (!compress)
    {
		//TODO maybe we should use sized internal formats to be more precise 
        switch (channelType) {

            case texture_channel_type::RGB:
                return GL_RGB;
            case texture_channel_type::RGBA:
                return GL_RGBA;
            case texture_channel_type::NormalMap:
                //TODO set a better one later
                return GL_RGB;
        }
    }
    
    
    GLint optimalFormat;
    std::string  optimalFormatName;
    
    for (const auto& type : types)
    {
        if (tryGetFormat(type, channelType, optimalFormat, optimalFormatName))
        {
            break;
        }
    }

    return optimalFormat;
}
