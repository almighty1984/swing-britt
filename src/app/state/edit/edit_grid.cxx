module;
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    cI32 Edit::grid_sprite_id_at_offset(cVec2F offset) {
        if (m_grid_sprite_ids.empty()) return -1;

        cF32 x = offset.x - std::fmodf(offset.x, 256.0F);
        cF32 y = offset.y - std::fmodf(offset.y, 256.0F);

        for (auto& i : m_grid_sprite_ids) {
            if (sprite::offset(i) == Vec2F{ x, y }) {
                return i;
            }
        }
        return -1;
    }
    cI32 Edit::grid_map_sprite_id_at_offset(cVec2F offset) {
        if (m_grid_map_sprite_ids.empty()) return -1;

        cF32 x = offset.x - std::fmodf(offset.x, 16.0F);
        cF32 y = offset.y - std::fmodf(offset.y, 16.0F);

        for (auto& i : m_grid_map_sprite_ids) {
            if (sprite::offset(i) == Vec2F{ x, y }) {
                return i;
            }
        }
        return -1;
    }
    bool Edit::add_grid_at_offset(cVec2F offset) {
        for (auto& i : m_grid_sprite_ids) {
            if (sprite::offset(i) == offset) {
                //console::log("state::Edit::add_grid_at_offset ", offset.x, " ", offset.y, " already added\n");
                return false;
            }
        }
        cI32 grid_sprite_id = sprite::make(m_grid_texture_path);
        m_grid_sprite_ids.emplace_back(grid_sprite_id);
        //sprite::id(grid_sprite_id, grid_sprite_id);
        sprite::source_rect(grid_sprite_id, { 0, 0, 256, 256 });
        sprite::offset(grid_sprite_id, offset);
        sprite::transform_id(grid_sprite_id, m_grid_transform_id);
        sprite::layer(grid_sprite_id, GRID_LAYER);
        sprite::is_hidden(grid_sprite_id, m_is_hidden_grid);
        
        cI32 grid_map_sprite_id = sprite::make(m_grid_map_texture_path);
        m_grid_map_sprite_ids.emplace_back(grid_map_sprite_id);
        //sprite::id(grid_map_sprite_id, grid_map_sprite_id);
        sprite::source_rect(grid_map_sprite_id, { 0, 0, 16, 16 });
        sprite::offset(grid_map_sprite_id, offset / 16.0F);
        sprite::transform_id(grid_map_sprite_id, m_grid_map_transform_id);
        sprite::layer(grid_map_sprite_id, GRID_MAP_LAYER);
        sprite::is_hidden(grid_map_sprite_id, m_is_hidden_grid_map);

        //console::log("state::Edit::add_grid_at_offset ", offset.x, " ", offset.y, " sprite id: ", grid_sprite_id, " map sprite id: ", grid_map_sprite_id, "\n");
        return true;
    }
    bool Edit::erase_grid_at_offset(cVec2F offset) {
        if (m_grid_sprite_ids.empty() || m_grid_sprite_ids.size() < 2) {
            //console::log("state::Edit::erase_grid_at_offset grid size is 1");
            return false;
        }
        cI32 grid_sprite_id = Edit::grid_sprite_id_at_offset(offset);
        if (grid_sprite_id == -1) return false;

        //console::log("state::Edit::erase_grid_at_offset sprite_id: ", grid_sprite_id, "\n");

        bool is_found = false;
        std::vector<I32> resized_grid_sprite_ids;
        for (auto& i : m_grid_sprite_ids) {
            if (i == grid_sprite_id) {
                is_found = true;
            } else {
                resized_grid_sprite_ids.emplace_back(i);
            }
        }
        if (!is_found) {
            //console::log("state::Edit::erase_grid_at_offset grid_sprite_id not found ", grid_sprite_id, "\n");
            return false;
        }
        m_grid_sprite_ids.clear();
        m_grid_sprite_ids = resized_grid_sprite_ids;
        //console::log("state::Edit::erase_grid_at_offset grid_sprite_ids size: ", m_grid_sprite_ids.size(), "\n");
        sprite::erase(grid_sprite_id);

        Vec2F grid_offset = offset / 16.0F;
        //console::log("state::Edit::erase_grid_at_offset grid_offset: ", grid_offset.x, " ", grid_offset.y, "\n");        

        cI32 grid_map_sprite_id = Edit::grid_map_sprite_id_at_offset(grid_offset);
        if (grid_map_sprite_id == -1) return false;
        if (!is_found) {
            console::log("state::Edit::erase_grid_at_offset grid_map_sprite_id not found ", grid_map_sprite_id, "\n");
            return false;
        }
        is_found = false;
        std::vector<I32> resized_grid_map_sprite_ids;
        for (auto& i : m_grid_map_sprite_ids) {
            if (i == grid_map_sprite_id) {
                is_found = true;
            } else {
                resized_grid_map_sprite_ids.emplace_back(i);
            }
        }

        m_grid_map_sprite_ids.clear();
        m_grid_map_sprite_ids = resized_grid_map_sprite_ids;
        //console::log("state::Edit::erase_grid_at grid_map_sprite_ids size: ", m_grid_map_sprite_ids.size(), "\n");
        sprite::erase(grid_map_sprite_id);

        return true;
    }
    bool Edit::clear_grid_sprites() {
        /*if (m_grid_transform_id != -1) {
            transform::erase(m_grid_transform_id);
            m_grid_transform_id = -1;
        }*/

        /*for (size_t i = 1; i < m_grid_sprite_ids.size(); ++i) {
            sprite::erase(m_grid_sprite_ids.at(i));
        }
        m_grid_sprite_ids.resize(1);
        for (size_t i = 1; i < m_grid_map_sprite_ids.size(); ++i) {
            sprite::erase(m_grid_sprite_ids.at(i));
        }
        m_grid_map_sprite_ids.resize(1);*/

        for (auto& i : m_grid_sprite_ids) {            
            if (sprite::transform_id(i) == m_grid_transform_id) {
                sprite::erase(i);
            }
        }
        for (auto& i : m_grid_map_sprite_ids) {            
            if (sprite::transform_id(i) == m_grid_map_transform_id) {
                sprite::erase(i);
            }
        }
        m_grid_sprite_ids.clear();
        m_grid_map_sprite_ids.clear();
        return true;
    }
}