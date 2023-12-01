#include "texture_setup.h"
#include <utility>

modelViewer::render::texture_setup::texture_setup(
        std::shared_ptr<modelViewer::res::texture_asset> asset, texture_filtering_mode texture_filtering_min ,texture_filtering_mode texture_filtering_mig ,texture_wrapping_mode textureWrapping ,bool is_mip_map_active ,unsigned int mip_map_min_level ,unsigned int mip_map_max_level) : m_Asset{std::move(asset)}, m_Texture_Filtering_Mig{texture_filtering_mig}, m_Texture_Filtering_Min{texture_filtering_min}, m_Texture_Wrapping{textureWrapping}, m_Is_Mip_Map_Active{is_mip_map_active}, m_Mip_Map_Min_Level{mip_map_min_level}, m_Mip_Map_Max_Level{mip_map_max_level}{

}
