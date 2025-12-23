module state.edit;

import console;
import input;
import sprite;
import types;

namespace state {
    void Edit::handle_menu_right() {
        //console::log("handle_menu_right\n");
        cVec2F position = m_mouse_tile_position;
        input::Button button = input::Button::NONE;
        if (input::is_pressed(m_input_id, input::Button::LEFT)) {
            button = input::Button::LEFT;
        } else if (input::is_pressed(m_input_id, input::Button::RIGHT)) {
            button = input::Button::RIGHT;
        }

        if (button == input::Button::NONE) return;
        if (position.y > 144.0F) return;

        if (!m_is_showing_tile_set) {
            input::release(m_input_id, button);
            //console::log("state::Edit::handle_menu_right ", position.x, " ", position.y, "\n");
            if (position.x == transform::position(m_menu_right_transform_id).x) {
                sprite::offset_y(m_active_layer_sprite_id, position.y);
                m_layer = (U8)(position.y / 16.0F);
            } else if (position.x == transform::position(m_menu_right_transform_id).x + 16.0F) {
                U8 layer = (U8)(position.y / 16.0F);
                if (layer < m_is_hidden_layer_sprite_ids.size() && m_is_hidden_layer_sprite_ids.at(layer)) {
                    sprite::is_hidden(m_is_hidden_layer_sprite_ids.at(layer), !sprite::is_hidden(m_is_hidden_layer_sprite_ids.at(layer)));
                }
                if (sprite::is_hidden(m_is_hidden_layer_sprite_ids.at(layer))) {
                    m_visible_layers.erase(layer);                    
                } else {
                    m_visible_layers.insert(layer);                    
                }
                //console::log("state::Edit::handle_menu_right layer: ", (int)layer, "\n");
            }
        } else if (m_is_showing_tile_set) {
            console::log("state::Edit::handle_menu_right position: ", position.x, " ", position.y, "\n");
            cVec2F bg_sprite_pos = transform::position(m_menu_right_transform_id) +
                                   sprite::offset(m_text_current_tile_set_bg_sprite_id);
            cRectI bg_sprite_rect = sprite::source_rect(m_text_current_tile_set_bg_sprite_id);

            if (position.x >= bg_sprite_pos.x && position.x <= bg_sprite_pos.x + bg_sprite_rect.w &&
                position.y >= bg_sprite_pos.y && position.y <= bg_sprite_pos.y + bg_sprite_rect.h) {
                //console::log("state::Edit::handle_menu_down position: ", position.x, " ", position.y, "\n");
                console::log("state::Edit::handle_menu_down sprite pos: ", bg_sprite_pos.x, " ", bg_sprite_pos.y, "\n");

                input::release(m_input_id, button);
                switch (button) {
                case input::Button::LEFT:  init_tile_set(m_tile_set + 1); break;
                case input::Button::RIGHT: init_tile_set(m_tile_set - 1); break; }                
            }

        }

    }
}