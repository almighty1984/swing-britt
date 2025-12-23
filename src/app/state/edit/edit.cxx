module;
#include <filesystem>
#include <sstream>

module state.edit;

import console;
import transform;
import sprite;
import types;

namespace state {
    Edit::Edit(cU16 window_w, cU16 window_h, const std::filesystem::path& level_path) {
        console::log("state::Edit()\n");
        for (U8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            m_visible_layers.insert(i);
        }
        m_current = m_next = Type::EDIT;

        //load_types_from_text_file("edit.cfg");

        load_types_from_bin();

        m_input_id = input::make();
        m_level_transform_id = transform::make();

        console::log("state::Edit level transform id: ", m_level_transform_id, "\n");

        //m_tile_set_scroll_transform_id = transform::make();
        //transform::position(m_tile_set_scroll_transform_id, { 0.0F, 0.0F });
        //m_selection_on_tile_set_sprite_ids.emplace_back(sprite::make("res/texture/tile_selection.png"));
        //sprite::get(m_selection_on_tile_set_sprite_ids.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        //sprite::get(m_selection_on_tile_set_sprite_ids.back())->transform_id = m_tile_set_scroll_transform_id;

        m_tile_set_bg_transform_id = transform::make();
        transform::position(m_tile_set_bg_transform_id, { 0.0F, 0.0F });
        m_tile_set_bg_sprite_id = sprite::make("res/texture/editor_tile_set_bg.png");
        sprite::source_rect(m_tile_set_bg_sprite_id, { 0, 0, 512, 240 });
        sprite::layer(m_tile_set_bg_sprite_id, TILE_SET_BG_LAYER);
        sprite::is_hidden(m_tile_set_bg_sprite_id, true);
        sprite::transform_id(m_tile_set_bg_sprite_id, m_tile_set_bg_transform_id);

        m_tile_set_transform_id = transform::make();
        //transform::position(m_tile_set_transform_id, { 304.0F, 240.0F });
        m_tile_set_sprite_id = sprite::make(tile_set_texture_path(0));        
        sprite::source_rect(m_tile_set_sprite_id, { 0, 0, 512, 512 });
        sprite::layer(m_tile_set_sprite_id, TILE_SET_LAYER);
        sprite::is_hidden(m_tile_set_sprite_id, true);
        sprite::transform_id(m_tile_set_sprite_id, m_tile_set_transform_id);

        m_menu_up_transform_id = transform::make();
        transform::position(m_menu_up_transform_id, { 0.0F, -8.0F });

        add_to_menu_up_bar(m_menu_up_labels[0]);
        add_to_menu_up_bar(m_menu_up_labels[1]);

        m_menu_up_lists[m_menu_up_labels[0]].transform_id = transform::make();        
        m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id = sprite::make("res/texture/editor_menu_bg.png");        

        console::log("state::Edit::Edit bg_h: ", m_menu_up_lists[m_menu_up_labels[0]].bg_h, "\n");

        sprite::layer(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id, MENU_LAYER);
        sprite::transform_id(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite_id, m_menu_up_lists[m_menu_up_labels[0]].transform_id);

        m_menu_up_lists[m_menu_up_labels[1]].transform_id = transform::make();
        m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id = sprite::make("res/texture/editor_menu_bg.png");        
        sprite::layer(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id, MENU_LAYER);
        sprite::transform_id(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite_id, m_menu_up_lists[m_menu_up_labels[1]].transform_id);

        load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "level");
        load_menu_up_list(m_menu_up_labels[1], std::filesystem::current_path() / "res" / "prefab");                

        m_menu_down_transform_id = transform::make();
        transform::position(m_menu_down_transform_id, { 0.0F, 160.0F });
        m_menu_down_bg_sprite_id = sprite::make("res/texture/editor_menu_bg.png");
        sprite::offset(m_menu_down_bg_sprite_id, { 0.0F, 0.0F });
        sprite::source_rect(m_menu_down_bg_sprite_id, { 0, 0, 512, 32 });
        sprite::layer(m_menu_down_bg_sprite_id, MENU_LAYER);
        sprite::transform_id(m_menu_down_bg_sprite_id, m_menu_down_transform_id);

        m_current_tile_sprite_id = sprite::make(tile_set_texture_path(0));
        sprite::source_rect(m_current_tile_sprite_id, { 0, 0, 16, 16 });
        sprite::layer(m_current_tile_sprite_id, MENU_LAYER + 1);
        sprite::transform_id(m_current_tile_sprite_id, m_menu_down_transform_id);
        sprite::offset_x(m_current_tile_sprite_id, view().w - 32.0f);

        m_grid_icon_sprite_id = sprite::make("res/texture/editor_tile_grid.png");
        sprite::source_rect(m_grid_icon_sprite_id, { 0, 0, 16, 16 });
        sprite::layer(m_grid_icon_sprite_id, MENU_LAYER + 1);
        sprite::transform_id(m_grid_icon_sprite_id, m_menu_down_transform_id);

        m_menu_right_transform_id = transform::make();
        transform::position(m_menu_right_transform_id, { 288.0F, 0.0F });
        m_menu_right_bg_sprite_id = sprite::make("res/texture/editor_menu_bg.png");
        sprite::offset(m_menu_right_bg_sprite_id, { 0, 0 });
        sprite::source_rect(m_menu_right_bg_sprite_id, { 0, 0, 32, 512 });
        sprite::layer(m_menu_right_bg_sprite_id, MENU_LAYER);
        sprite::transform_id(m_menu_right_bg_sprite_id, m_menu_right_transform_id);

        m_save_sprite_id = sprite::make("res/texture/editor_save.png");
        sprite::offset(m_save_sprite_id, { 272.0F, 0.0F });
        sprite::layer(m_save_sprite_id, MENU_LAYER);
        sprite::transform_id(m_save_sprite_id, m_menu_down_transform_id);

        m_active_layer_sprite_id = sprite::make("res/texture/editor_layer_active.png");
        sprite::offset(m_active_layer_sprite_id, { 0.0F, 0.0F });
        sprite::source_rect(m_active_layer_sprite_id, { 0, 0, 16, 16 });
        sprite::layer(m_active_layer_sprite_id, MENU_LAYER);
        sprite::transform_id(m_active_layer_sprite_id, m_menu_right_transform_id);

        for (U8 i = 0; i < 10; ++i) {
            m_is_hidden_layer_sprite_ids.emplace_back(sprite::make("res/texture/editor_layer_hidden.png"));
            sprite::offset(m_is_hidden_layer_sprite_ids.back(), { 16.0F, i * 16.0F });
            sprite::source_rect(m_is_hidden_layer_sprite_ids.back(), { 0, 0, 16, 16 });
            sprite::layer(m_is_hidden_layer_sprite_ids.back(), MENU_LAYER);
            sprite::transform_id(m_is_hidden_layer_sprite_ids.back(), m_menu_right_transform_id);
        }



        m_text_bar_bg_sprite_id = sprite::make("res/texture/editor_text_bar_bg.png");
        sprite::offset(m_text_bar_bg_sprite_id, { 0.0F, 0.0F });
        sprite::source_rect(m_text_bar_bg_sprite_id, { 0, 0, 272, 16 });
        sprite::layer(m_text_bar_bg_sprite_id, MENU_LAYER);
        sprite::transform_id(m_text_bar_bg_sprite_id, m_menu_down_transform_id);

        m_text_bar.transform_id = m_menu_down_transform_id;
        m_text_bar.offset({ 0.0F, 4.0F });
        m_text_bar.layer = MENU_LAYER + 1;
        m_text_bar.texture_path = "res/texture/font_8_black.png";
        m_text_bar.is_hidden(false);
        //m_text_bar.clear_text();
        //m_text_bar.set_text(m_level_path.string());

        m_text_current_tile_set_bg_sprite_id = sprite::make("res/texture/editor_text_current_tile_set_bg.png");
        sprite::offset(m_text_current_tile_set_bg_sprite_id, { 0.0F, 0.0F });
        sprite::source_rect(m_text_current_tile_set_bg_sprite_id, { 0, 0, 32, 16 });
        sprite::layer(m_text_current_tile_set_bg_sprite_id, MENU_LAYER);
        sprite::transform_id(m_text_current_tile_set_bg_sprite_id, m_menu_right_transform_id);
        sprite::is_hidden(m_text_current_tile_set_bg_sprite_id, true);

        m_text_current_tile_set.transform_id = m_menu_right_transform_id;
        m_text_current_tile_set.layer = MENU_LAYER;
        m_text_current_tile_set.texture_path = "res/texture/font_8_black.png";
        m_text_current_tile_set.is_hidden(true);
        m_text_current_tile_set.offset( {4.0F, 4.0F} );
        m_text_current_tile_set.set_text("0");

        m_info_message.transform_id = m_menu_up_transform_id;
        m_info_message.is_hidden(true);


        m_grid_transform_id = transform::make();
        m_grid_map_transform_id = transform::make();
        add_grid_at_offset({ 0.0F, 0.0F });


        m_position_on_grid_map_sprite_id = sprite::make("res/texture/tile_yellow.png");
        sprite::source_rect(m_position_on_grid_map_sprite_id, { 0, 0, 1, 1 });
        sprite::offset(m_position_on_grid_map_sprite_id, Vec2F{ 7.0F, 7.0F });
        sprite::transform_id(m_position_on_grid_map_sprite_id, m_grid_map_transform_id);
        sprite::layer(m_position_on_grid_map_sprite_id, GRID_LAYER + 1);

        sprite::is_hidden(m_position_on_grid_map_sprite_id, m_is_hidden_grid_map);

        m_mouse_transform_id = transform::make();
        m_mouse_sprite_ids.emplace_back(sprite::make(m_mouse_texture_path));
        sprite::transform_id(m_mouse_sprite_ids.back(), m_mouse_transform_id);
        sprite::layer(m_mouse_sprite_ids.back(), MOUSE_LAYER);

        load_level_sprites(level_path);

        m_level_path = level_path;
    }
    Edit::~Edit() {
        console::log("state::Edit::~Edit()\n");
        //input::clear();

        input::erase(m_input_id);

        transform::erase(m_mouse_transform_id);
        transform::erase(m_level_transform_id);
        transform::erase(m_tile_set_transform_id);
        transform::erase(m_tile_set_bg_transform_id);
        transform::erase(m_grid_transform_id);
        transform::erase(m_grid_map_transform_id);
        transform::erase(m_menu_up_transform_id);
        transform::erase(m_menu_down_transform_id);
        transform::erase(m_menu_right_transform_id);

        sprite::erase(m_menu_down_bg_sprite_id);
        sprite::erase(m_menu_right_bg_sprite_id);
        sprite::erase(m_position_on_grid_map_sprite_id);
        sprite::erase(m_save_sprite_id);
        sprite::erase(m_tile_set_sprite_id);
        sprite::erase(m_tile_set_bg_sprite_id);
        sprite::erase(m_current_tile_sprite_id);
        sprite::erase(m_grid_icon_sprite_id);
        sprite::erase(m_active_layer_sprite_id);
        sprite::erase(m_text_bar_bg_sprite_id);
        sprite::erase(m_text_current_tile_set_bg_sprite_id);

        for (auto& i : m_is_hidden_layer_sprite_ids)       sprite::erase(i);
        for (auto& i : m_mouse_sprite_ids)                 sprite::erase(i);
        for (auto& i : m_selection_on_tile_set_sprite_ids) sprite::erase(i);
        for (auto& i : m_selection_on_level_sprite_ids)    sprite::erase(i);
        for (auto& i : m_moving_sprite_ids)                sprite::erase(i);

        for (auto& i : m_level_sprite_ids)                 sprite::erase(i);
        
        for (auto& i : m_grid_sprite_ids)                  sprite::erase(i);
        for (auto& i : m_grid_map_sprite_ids)              sprite::erase(i);

        for (auto& i : m_menu_up_bar_bg_sprite_ids)        sprite::erase(i);
                
        m_menu_up_lists.clear();

        m_types.clear();
    }
    /*void Edit::clear_mouse_sprites() {
        console::log("state::Edit::clear_mouse_sprites\n");
        for (auto& i : m_mouse_sprite_ids) {
            sprite::erase(i);
        }
        m_mouse_sprite_ids.clear();        
    }*/     
}