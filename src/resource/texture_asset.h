
#ifndef MODEL_VIEWER_TEXTURE_ASSET_H
#define MODEL_VIEWER_TEXTURE_ASSET_H

namespace modelViewer::res
{
    using byte = unsigned char;

    enum class texture_channel_type{
        RGB,
        RGBA,
        NormalMap
    };
    
    struct textureInfo
    {
        bool forceFlip = true;
        int width = 0;
        int height = 0;
        int channels = 0;
    };
    
    class texture_asset {
    private:
        std::unique_ptr<byte[]> m_Content;
        textureInfo m_Info;
        std::string  m_Name;
    public: 
        texture_asset(byte* content, textureInfo info, std::string name);
        byte* getContent(); 
        int getWidth();
        int getHeight();
        int getChannelCount();
        texture_channel_type getChannelType();

        std::string& getName();
    };

}


#endif 
