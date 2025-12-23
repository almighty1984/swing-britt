module;
#include <filesystem>
#include <vector>
#include <string>
#include <map>
#include <list>

export module state.edit;

export import state;
import bitmap_text;
import console;
import input;
import sprite;
import transform;
import types;
import window;

namespace history {
    enum class Act { NONE = 0, PLACE, REPLACE, ERASE, MOVE };
    const std::string_view as_string(const Act& act) {
        switch (act) {
            case history::Act::PLACE:   return "PLACE";
            case history::Act::REPLACE: return "REPLACE";
            case history::Act::ERASE:   return "ERASE";
            case history::Act::MOVE:    return "MOVE";
            default: return "";
        }
    }
    struct Info {
        I32   transform_id = -1;
        U8    layer        =  0,
              tile_set     =  0;
        RectI source_rect  = { 0, 0, 0, 0 };
        Vec2F offset       = { 0.0F, 0.0F };

        bool operator ==(const Info& other) {
            return (layer == other.layer && tile_set == other.tile_set && source_rect == other.source_rect && offset == other.offset);
        }
        bool operator !=(const Info& other) {
            return !operator ==(other);
        }
    };
};

namespace entity {
    struct Info {
        U8  set    = 0;
        U16 number = 0;

        bool operator <(const Info& other)  const {
            return set < other.set ? true : number < other.number;
        }
    };
}

namespace menu {
    /*enum class Label {
        none = 0, Open, Import
    };
    using cLabel = const Label;
    std::string to_string(cLabel l) {
        switch (l) {
        case Label::Open:   return "Open";
        case Label::Import: return "Import";
        default:            return "";       }
    }*/
    struct List {
        I32 transform_id = -1,
            bg_sprite_id = -1;
        U16 bg_w = 8,
            bg_h = 8;
        std::vector<std::unique_ptr<BitmapText>> text_items;

        ~List() {
            transform::erase(transform_id);
            sprite::erase(bg_sprite_id);
            text_items.clear();
        }

        void draw(std::unique_ptr<Window>& window) {
            sprite::draw(window, bg_sprite_id);
            
            for (auto& i : text_items) {
                if (i) i->draw(window);
            }
        }
    };
}

export namespace state {
    constexpr U8 GRID_LAYER                  = 10,
                 GRID_MAP_LAYER              = 10,
                 SELECTION_ON_LEVEL_LAYER    = 10,
                 TILE_SET_BG_LAYER           = 11,
                 TILE_SET_LAYER              = 11,
                 SELECTION_ON_TILE_SET_LAYER = 11,
                 MENU_LAYER                  = 12,
                 MOUSE_LAYER                 = 13;

    class Edit : public Object {
    public:
        Edit(cU16 window_w, cU16 window_h, const std::filesystem::path& path);
        /*for (U8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }*/

        /*Edit(cU16 window_w, cU16 window_h, const std::filesystem::path& path) {
            Edit(window_w, window_h);
            load_level_sprites(path);
        }*/
        ~Edit();

    private:        
        I32 m_mouse_transform_id           = -1,
            m_level_transform_id           = -1,
            m_tile_set_transform_id        = -1,
            m_tile_set_bg_transform_id     = -1,
            m_grid_transform_id            = -1, m_grid_map_transform_id   = -1,
            m_menu_up_transform_id         = -1,
            m_menu_down_transform_id       = -1, m_menu_right_transform_id = -1;

        I32 m_menu_down_bg_sprite_id             = -1, m_menu_right_bg_sprite_id = -1,
            m_position_on_grid_map_sprite_id     = -1,
            m_save_sprite_id                     = -1,
            m_tile_set_sprite_id                 = -1, m_tile_set_bg_sprite_id   = -1,
            m_current_tile_sprite_id             = -1,
            m_grid_icon_sprite_id                = -1,
            m_active_layer_sprite_id             = -1,
            m_text_bar_bg_sprite_id              = -1,
            m_text_current_tile_set_bg_sprite_id = -1;

        std::vector<I32> m_is_hidden_layer_sprite_ids,
                         m_mouse_sprite_ids,
                         m_selection_on_tile_set_sprite_ids,
                         m_selection_on_level_sprite_ids,
                         m_moving_sprite_ids;

        std::list<I32> m_level_sprite_ids;

        std::vector<I32> m_grid_sprite_ids,
                         m_grid_map_sprite_ids;

        std::vector<I32> m_menu_up_bar_bg_sprite_ids;

        std::string                     m_menu_up_labels[2] = { "Open ", "Import" };

        //std::vector<menu::Label> m_menu_labels;


        std::map<std::string, menu::List> m_menu_up_lists;

        size_t m_typing_pos = 0;
        U16 m_time_typing_text_bar = 0;

        std::vector<std::unique_ptr<BitmapText>> m_menu_up_bar;

        BitmapText m_text_current_tile_set,
                   m_text_bar,
                   m_info_message;

        Vec2F  m_mouse_tile_position;
        
        bool m_is_moving               = false,
             m_is_mouse_on_menu_up_bar = false, m_is_mouse_on_menu_up_list = false, m_is_mouse_on_menu_down = false,
             m_is_hidden_menu_down     = false, m_is_hidden_menu_right     = false,
             m_is_hidden_grid          = false, m_is_hidden_grid_map       = true,
             m_is_showing_tile_set     = false,
             m_is_typing_text_bar      = false;

        std::pair<bool, size_t> m_is_asked_to_remove_level = { false, 0 };

        U8   m_level_brush_size    = 1,
             m_tile_set_brush_size = 1,
             m_tile_set            = 0, m_prev_tile_set = 255,
             m_layer               = 0;

        U16  m_tile_number = 0;
            
        U16  m_menu_up_w        = 0,
             m_timer            = 0,
             m_time_left_saving = 0, m_time_to_save = 10;

        std::vector<history::Info> m_undo_info_placed,
                                   m_undo_info_replaced,
                                   m_undo_info_erased,
                                   m_undo_info_moved;
        std::vector<history::Info> m_redo_info_placed,
                                   m_redo_info_replaced,
                                   m_redo_info_erased,
                                   m_redo_info_moved;
        
        std::vector<U16>        m_undo_counts,
                                m_redo_counts;
        U16                     m_last_undo_count = 0,
                                m_last_redo_count = 0;

        std::vector<history::Act>  m_undo_acts,
                                   m_redo_acts;

        std::filesystem::path m_grid_texture_path     = "res/texture/editor_grid_block.png",
                              m_grid_map_texture_path = "res/texture/tile_blue.png",
                              m_mouse_texture_path    = "res/texture/tile_selection.png",
                              m_level_path            = "";

        std::map<entity::Info, std::string> m_types;
        
        void handle_menu_up();
        bool add_to_menu_up_bar(const std::string& text);
        void load_menu_up_list(const std::string& menu, const std::filesystem::path& level_dir_path);
        bool add_to_menu_up_list(const std::string& menu, const std::filesystem::path& level_path);
        bool remove_level(const std::filesystem::path& path);
        
        void handle_menu_down();
        void handle_menu_right();

        bool init_brush(U8 size);

        cI32 grid_sprite_id_at_offset(cVec2F offset);
        cI32 grid_map_sprite_id_at_offset(cVec2F offset);
        bool add_grid_at_offset(cVec2F offset);
        bool erase_grid_at_offset(cVec2F offset);
        bool clear_grid_sprites();

        //void clear_mouse_sprites();
                
        void init_tile_set(cU8 layer);
        void enter_tile_set();
        void exit_tile_set();
        void move_tile_set(cVec2F amount);
        bool select_on_tile_set(cVec2F offset);
        bool deselect_on_tile_set(cVec2F offset);
        bool move_selection_on_tile_set(cVec2F amount);
        bool deselect_all_on_tile_set();

        bool init_typing_text_bar();
        bool quit_typing_text_bar();
        void handle_typing_text_bar();
        bool shift_text_bar_typing_pos_left();
        bool shift_text_bar_typing_pos_right();
        bool save_typed_text_bar();

        bool save_types_to_bin();
        bool load_types_from_bin();
        void load_types_from_text_file(const std::filesystem::path& path);

        void move_level(cVec2F amount);
        void move_level_to(cVec2F position);
        bool select_on_level(cVec2F offset);
        bool deselect_on_level(cVec2F offset);

        std::vector<I32> find_sprites_in_selection_on_level();

        void copy_selected_start_moving_on_level();
        void start_moving_selected_on_level();
        void clear_selected_on_level();
        void move_selected_on_level(cVec2F amount);
        void finish_moving_selected_on_level();
        void deselect_all_on_level();
        bool eyedropper_on_level(cVec2F offset);
        cI32 level_sprite_id_at_offset(cU8 layer, cVec2F offset);

        history::Act place_sprite_on_level(cU8 layer, cU8 tile_set, cRectI source_rect, cVec2F offset);
        history::Act erase_sprite_on_level(cU8 layer, cVec2F offset);
        
        void undo_last_act();
        void redo_last_act();
        void clear_level_sprites();
        void load_level_sprites(const std::filesystem::path& path);

        void update(cF32 ts) override;

        void draw(std::unique_ptr<Window>& window, cU8 layer) override {
            if (sprite::layer(m_menu_down_bg_sprite_id) == layer)             sprite::draw(window, m_menu_down_bg_sprite_id);
            if (sprite::layer(m_menu_right_bg_sprite_id) == layer)            sprite::draw(window, m_menu_right_bg_sprite_id);
            if (sprite::layer(m_position_on_grid_map_sprite_id) == layer)     sprite::draw(window, m_position_on_grid_map_sprite_id);
            if (sprite::layer(m_save_sprite_id) == layer)                     sprite::draw(window, m_save_sprite_id);
            if (sprite::layer(m_tile_set_bg_sprite_id) == layer)              sprite::draw(window, m_tile_set_bg_sprite_id);
            if (sprite::layer(m_tile_set_sprite_id) == layer)                 sprite::draw(window, m_tile_set_sprite_id);
            if (sprite::layer(m_current_tile_sprite_id) == layer)             sprite::draw(window, m_current_tile_sprite_id);
            if (sprite::layer(m_grid_icon_sprite_id) == layer)                sprite::draw(window, m_grid_icon_sprite_id);
            if (sprite::layer(m_active_layer_sprite_id) == layer)             sprite::draw(window, m_active_layer_sprite_id);
            if (sprite::layer(m_text_bar_bg_sprite_id) == layer)              sprite::draw(window, m_text_bar_bg_sprite_id);
            if (sprite::layer(m_text_current_tile_set_bg_sprite_id) == layer) sprite::draw(window, m_text_current_tile_set_bg_sprite_id);

            for (auto& i : m_is_hidden_layer_sprite_ids)       if (sprite::layer(i) == layer) sprite::draw(window, i);            
            for (auto& i : m_mouse_sprite_ids)                 if (sprite::layer(i) == layer) sprite::draw(window, i);            
            for (auto& i : m_selection_on_tile_set_sprite_ids) if (sprite::layer(i) == layer) sprite::draw(window, i);            
            for (auto& i : m_selection_on_level_sprite_ids)    if (sprite::layer(i) == layer) sprite::draw(window, i);            
            for (auto& i : m_moving_sprite_ids)                if (sprite::layer(i) == layer) sprite::draw(window, i);            
            for (auto& i : m_grid_sprite_ids)                  if (sprite::layer(i) == layer) sprite::draw(window, i);
            for (auto& i : m_grid_map_sprite_ids)              if (sprite::layer(i) == layer) sprite::draw(window, i);

            if (m_is_mouse_on_menu_up_bar || m_is_mouse_on_menu_up_list) {
                for (auto& i : m_menu_up_bar_bg_sprite_ids) {
                    if (sprite::layer(i) == layer) sprite::draw(window, i);
                }
                for (auto& text_label : m_menu_up_bar) {
                    if (text_label && text_label->layer == layer) {
                        text_label->draw(window);
                    }
                }
            }
            if (m_text_current_tile_set.layer == layer) {
                m_text_current_tile_set.draw(window);
            }
            if (m_text_bar.layer == layer) {
                m_text_bar.draw(window);
            }
            if (m_info_message.layer == layer) {
                m_info_message.draw(window);
            }

            for (auto& i : m_level_sprite_ids) {
                if (sprite::layer(i) == layer) sprite::draw(window, i);
            }

            for (auto& i : m_menu_up_lists) {
                i.second.draw(window);
            }
        }
    };
}