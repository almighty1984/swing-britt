module;
#include <fstream>
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    void Edit::load_types_from_text_file(const std::filesystem::path& path) {
        std::ifstream in_file(path);

        std::ostringstream oss;
        oss << in_file.rdbuf();
        in_file.close();

        std::string file_text = oss.str();

        const size_t start = file_text.find("set_255", 0);
        if (start == std::string::npos) {
            console::error("state::Edit::load_types set_255 not found\n");
            return;
        }

        const size_t open_bracket = file_text.find("{", start);
        if (open_bracket == std::string::npos) {
            console::error("state::Edit::load_types set_255 open bracket not found\n");
            return;
        }
        const size_t close_bracket = file_text.find("}", open_bracket);
        if (close_bracket == std::string::npos) {
            console::error("state::Edit::load_types set_255 close bracket not found\n");
            return;
        }

        size_t label_start = file_text.find("\n", open_bracket);
        while (label_start < close_bracket) {
            while (file_text.at(label_start) == '	' || file_text.at(label_start) == ' ' || file_text.at(label_start) == '\n') {
                ++label_start;
            }
            const size_t label_end = file_text.find(" ",  label_start);
            const size_t equals    = file_text.find("=",  label_start);
            const size_t end_line  = file_text.find("\n", label_start);                        

            const std::string number_label = file_text.substr(label_start, label_end - label_start);

            cU16 number = std::stoi(number_label);

            size_t type_start = equals + 1;
            while (file_text.at(type_start) == '	' || file_text.at(type_start) == ' ' || file_text.at(type_start) == '\n') {
                ++type_start;
            }
            const std::string type = file_text.substr(type_start, end_line - type_start);

            m_types.emplace(entity::Info{ 255, number }, type);
            label_start = end_line + 1;
        }        
    }
    void Edit::init_tile_set(cU8 tile_set) {
        m_prev_tile_set = m_tile_set;
        m_tile_set = tile_set;
        const std::string tile_set_str = std::to_string((int)m_tile_set);
        m_text_current_tile_set.set_text(tile_set_str);
        sprite::texture(m_tile_set_sprite_id, "res/texture/set_" + tile_set_str + ".png");
        sprite::texture(m_current_tile_sprite_id, "res/texture/set_" + tile_set_str + ".png");

        if (!m_is_showing_tile_set) return;
        
        m_text_bar.clear_text();

        if (tile_set == 255) {
            if (!m_selection_on_tile_set_sprite_ids.empty() && 
                m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
            }
            sprite::is_hidden(m_save_sprite_id, false);
        } else {
            //m_text_bar.clear_text();
            sprite::is_hidden(m_save_sprite_id, true);
        }        
                
        console::log("state::Edit::init_tile_set tile_number: ", m_tile_number, "\n");
        
        //deselect_all_on_tile_set();
    }
    void Edit::enter_tile_set() {
        console::log("state::Edit::enter_tile_set\n");
        //transform::get(m_tile_set_bg_transform_id)->position.y = 0.0F;
        sprite::is_hidden(m_tile_set_bg_sprite_id, false);
        sprite::is_hidden(m_tile_set_sprite_id, false);

        sprite::is_hidden(m_text_current_tile_set_bg_sprite_id, true);
        
        for (auto& i : m_is_hidden_layer_sprite_ids) {
            sprite::layer(i, 0);
        }
        sprite::is_hidden(m_active_layer_sprite_id, true);
        sprite::is_hidden(m_position_on_grid_map_sprite_id, true);

        for (auto& i : m_grid_sprite_ids) {
            sprite::is_hidden(i, true);
        }
        for (auto& i : m_grid_map_sprite_ids) {
            sprite::is_hidden(i, true);
        }
        sprite::is_hidden(m_position_on_grid_map_sprite_id, true);
        sprite::is_hidden(m_grid_icon_sprite_id, true);

        sprite::is_hidden(m_text_current_tile_set_bg_sprite_id, false);
        m_text_current_tile_set.is_hidden(false);
        if (m_tile_set == 255) {
            if (m_types.find(entity::Info{ 255, m_tile_number }) == m_types.end()) {
                m_text_bar.clear_text();
            } else {
                m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
            }
            sprite::is_hidden(m_save_sprite_id, false);
        } else {
            m_text_bar.clear_text();
            sprite::is_hidden(m_save_sprite_id, true);
        }
        //sprite::get(m_tile_set_sprite_id)->is_hidden = false;

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            sprite::is_hidden(i, false);
        }
    }
    void Edit::exit_tile_set() {
        console::log("state::Edit::exit_tile_set\n");
        sprite::is_hidden(m_tile_set_bg_sprite_id, true);
        sprite::is_hidden(m_tile_set_sprite_id, true);
        //transform::get(m_tile_set_bg_transform_id)->position.y = 512.0F;

        //transform::get(m_tile_set_transform_id)->position.x = view().w - 32.0F;
        //transform::get(m_tile_set_transform_id)->position.y = view().w == 480.0F ? 240.0F : 160.0F;

        /*if (!m_selection_on_tile_set_sprite_ids.empty() && sprite::get(m_selection_on_tile_set_sprite_ids.back())) {
            cVec2F tile_offset = sprite::get(m_selection_on_tile_set_sprite_ids.back())->offset;
            sprite::get(m_tile_set_sprite_id)->source_rect = { (I32)tile_offset.x, (I32)tile_offset.y, 16, 16 };
            sprite::get(m_tile_set_sprite_id)->layer = MENU_LAYER + 1;
        }*/

        sprite::is_hidden(m_text_current_tile_set_bg_sprite_id, true);

        m_text_current_tile_set.is_hidden(true);
           
        for (auto& i : m_is_hidden_layer_sprite_ids) {
            sprite::layer(i, MENU_LAYER);
        }
        sprite::is_hidden(m_active_layer_sprite_id, false);
        sprite::is_hidden(m_save_sprite_id, false);
        sprite::is_hidden(m_grid_icon_sprite_id, false);
        
        for (auto& i : m_grid_sprite_ids) {
            sprite::is_hidden(i, m_is_hidden_grid);
        }
        for (auto& i : m_grid_map_sprite_ids) {
            sprite::is_hidden(i, m_is_hidden_grid_map);
        }
        sprite::is_hidden(m_position_on_grid_map_sprite_id, m_is_hidden_grid_map);
        

        if (m_level_path.empty()) {
            m_text_bar.clear_text();
        } else {
            m_text_bar.set_text(m_level_path.string());
        }

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            sprite::is_hidden(i, true);
        }
    }
    void Edit::move_tile_set(cVec2F amount) {
        //console::log("state::Edit::move_tile_set ", amount.x, " ", amount.y, " ", " position: ", transform::get(m_tile_set_transform_id)->position.x, " ", transform::get(m_tile_set_transform_id)->position.y, "\n");        

        //cF32 edge_y = m_is_showing_tile_set ? 0.0F : 512.0F;
        cF32 edge_y = 0.0F;
        cVec2F prev_position = transform::position(m_tile_set_transform_id);
        
        cVec2F limit = { view().w - 512.0F - 32.0F,
                         view().h == 270.0F ? -272.0F : -352.0F };

        transform::add_position(m_tile_set_transform_id, amount);
        if (transform::position(m_tile_set_transform_id).y > edge_y) {            
            transform::position_y(m_tile_set_transform_id, edge_y);
        } else if (transform::position(m_tile_set_transform_id).y < edge_y + limit.y) {
            transform::position_y(m_tile_set_transform_id, edge_y + limit.y);
        }
        if (transform::position(m_tile_set_transform_id).x > 0.0F) {
            transform::position_x(m_tile_set_transform_id, 0.0F);
        } else if (transform::position(m_tile_set_transform_id).x < limit.x) {
            transform::position_x(m_tile_set_transform_id, limit.x);
        }        
    }
    bool Edit::select_on_tile_set(cVec2F offset) {
        for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
            if (sprite::offset(sel_id) == offset) {
                console::log("state::Edit::select_on_tile_set", offset.x, " ", offset.y, " already selected\n");
                return false;
            }
        }
        //console::log("state::Edit::select_on_tile_set\n");
        cI32 sprite_id = sprite::make("res/texture/tile_selection.png");
        sprite::layer(sprite_id, SELECTION_ON_TILE_SET_LAYER);
        sprite::transform_id(sprite_id, m_tile_set_transform_id);
        sprite::offset(sprite_id, offset);
        m_selection_on_tile_set_sprite_ids.push_back(sprite_id);

        sprite::source_rect(m_current_tile_sprite_id, { (I32)offset.x, (I32)offset.y, 16,16 });
                
        if (m_tile_set == 255 && m_selection_on_tile_set_sprite_ids.size() == 1) {
            cVec2F tile_offset = offset / 16.0F;
            m_tile_number = std::fmodf(tile_offset.x, 32.0F) + tile_offset.y * 32.0F;

            //if (m_types.at(entity::Info{ 255, m_tile_number }) == m_text_bar.get_text()) {
                //return false;
            //}

            if (m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                if (m_text_bar.get_text() != m_types.at(entity::Info{ 255, m_tile_number })) {
                    m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
                }
                //console::log("state::Edit::select_on_tile_set() text bar: ", m_text_bar.get_text(), "\n");
                //int num = 0;
                //console::log("state::Edit::select_on_tile_set text_bar size: ", m_text_bar.get_sprite_ids().size(), "\n");
                
                //for (auto& i : m_text_bar.get_sprite_ids()) {
                //    if (!sprite::get(i)) {
                //        console::log("text_bar sprite_id not valid\n");
                //    }
                    //console::log(num, " ", sprite::get(i)->source_rect.x, " ", sprite::get(i)->source_rect.y, " ", sprite::get(i)->source_rect.w, " ", sprite::get(i)->source_rect.h, "\n");
                    //console::log(num, " ", (int)sprite::get(i)->layer, "\n");
                //}
                //++num;
                //}
            }            
        }
        return true;
    }
    bool Edit::deselect_on_tile_set(cVec2F offset) {
        for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
            if (sprite::transform_id(sel_id) != m_tile_set_transform_id) continue;
            if (sprite::offset(sel_id) == offset) {
                console::log("state::Edit::deselect_on_tile_set aready selected, deselecting\n");
                sprite::erase(sel_id);
                std::vector<I32> resized_selection;
                for (auto& i : m_selection_on_tile_set_sprite_ids) {
                    if (i != sel_id) {
                        resized_selection.push_back(i);
                    }
                }
                m_selection_on_tile_set_sprite_ids.clear();
                m_selection_on_tile_set_sprite_ids = resized_selection;
                return true;
            }
        }
        return false;
    }
    bool Edit::move_selection_on_tile_set(cVec2F amount) {
        console::log("state::Edit::move_selection_on_tile_set ", amount.x, " ", amount.y, "\n");
        if (m_selection_on_tile_set_sprite_ids.empty()) return false;
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            if (sprite::offset(i).x + amount.x < 0 || sprite::offset(i).y + amount.y < 0 ||
                sprite::offset(i).x + amount.x > 496.0F || sprite::offset(i).y + amount.y > 496.0F) {
                return false;
            }

            //cF32 edge_y = m_is_showing_tile_set ? 0.0F : 512.0F;
            if (m_is_showing_tile_set) {
                cF32 edge_y = 0.0F;
                if (amount.x < 0.0F && transform::position(m_tile_set_transform_id).x + sprite::offset(i).x + amount.x < 0.0F ||
                    amount.x > 0.0F && transform::position(m_tile_set_transform_id).x + sprite::offset(i).x + amount.x > view().w - 48.0F ||
                    amount.y < 0.0F && transform::position(m_tile_set_transform_id).y + sprite::offset(i).y + amount.y < edge_y ||
                    amount.y > 0.0F && transform::position(m_tile_set_transform_id).y + sprite::offset(i).y + amount.y > edge_y + view().h - 32.0F) {
                    move_tile_set(-amount);
                }
            }
        }
        //transform::get(m_tile_set_transform_id)->position += amount;

        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            sprite::add_offset(i, amount);
        }

        cVec2F offset = sprite::offset(m_selection_on_tile_set_sprite_ids.front());

        sprite::source_rect_x(m_current_tile_sprite_id, sprite::source_rect(m_current_tile_sprite_id).x + amount.x);
        sprite::source_rect_y(m_current_tile_sprite_id, sprite::source_rect(m_current_tile_sprite_id).y + amount.y);

        cVec2F tile_offset = offset / 16.0F;
        m_tile_number = std::fmodf(tile_offset.x, 32.0F) + tile_offset.y * 32.0F;

        if (m_is_showing_tile_set) {
            if (m_tile_set == 255 && m_selection_on_tile_set_sprite_ids.size() == 1) {
                if (m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                    m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
                }
            }
        }
        return true;
    }
    bool Edit::deselect_all_on_tile_set() {        
        if (m_selection_on_tile_set_sprite_ids.empty()) return false;
        console::log("state::Edit::deselect_all_on_tile_set\n");
        for (auto& i : m_selection_on_tile_set_sprite_ids) {
            sprite::erase(i);
        }
        m_selection_on_tile_set_sprite_ids.clear();
        return true;
    }
}