module;
#include <cmath>
#include <filesystem>
#include <string>
#include <memory>

module state.edit;

namespace state {
    void Edit::handle_menu_up() {
        size_t mouse_on_menu_bar_item = -1;
        for (size_t i = 0; i < m_menu_up_bar_bg_sprite_ids.size(); ++i) {
            if (input::mouse.x >= sprite::offset(m_menu_up_bar_bg_sprite_ids.at(i)).x && input::mouse.x <= sprite::offset(m_menu_up_bar_bg_sprite_ids.at(i)).x + sprite::source_rect(m_menu_up_bar_bg_sprite_ids.at(i)).w &&
                input::mouse.y >= sprite::offset(m_menu_up_bar_bg_sprite_ids.at(i)).y && input::mouse.y <= sprite::offset(m_menu_up_bar_bg_sprite_ids.at(i)).y + sprite::source_rect(m_menu_up_bar_bg_sprite_ids.at(i)).h) {
                mouse_on_menu_bar_item = i;
            }
        }
        m_is_mouse_on_menu_up_list = false;
        bool is_mouse_on_menu_up_list[2] = { false };
        if (!input::is_pressed(m_input_id, input::Key::CTRL)) {
            if (input::mouse.y >= transform::position(m_menu_up_lists[m_menu_up_labels[0]].transform_id).y - 2 &&
                input::mouse.y <= transform::position(m_menu_up_lists[m_menu_up_labels[0]].transform_id).y + m_menu_up_lists[m_menu_up_labels[0]].bg_h &&
                input::mouse.x >= transform::position(m_menu_up_lists[m_menu_up_labels[0]].transform_id).x &&
                input::mouse.x <= transform::position(m_menu_up_lists[m_menu_up_labels[0]].transform_id).x + m_menu_up_lists[m_menu_up_labels[0]].bg_w) {
                m_is_mouse_on_menu_up_list = true;
                is_mouse_on_menu_up_list[0] = true;
            } else if (input::mouse.y >= transform::position(m_menu_up_lists[m_menu_up_labels[1]].transform_id).y - 2 &&
                input::mouse.y <= transform::position(m_menu_up_lists[m_menu_up_labels[1]].transform_id).y + m_menu_up_lists[m_menu_up_labels[1]].bg_h &&
                input::mouse.x >= transform::position(m_menu_up_lists[m_menu_up_labels[1]].transform_id).x &&
                input::mouse.x <= transform::position(m_menu_up_lists[m_menu_up_labels[1]].transform_id).x + m_menu_up_lists[m_menu_up_labels[1]].bg_w)

            {
                m_is_mouse_on_menu_up_list = true;
                is_mouse_on_menu_up_list[1] = true;
            }
        }

        for (auto& i : m_menu_up_bar) {
            i.get()->texture("res/texture/font_8_gray.png");
        }
        //console::log("m_is_mouse_on_menu_up_open_list: ", m_is_mouse_on_menu_up_open_list, "\n");
        if (mouse_on_menu_bar_item == 0 || is_mouse_on_menu_up_list[0]) {
            m_menu_up_bar.at(0).get()->texture("res/texture/font_8_white.png");
            transform::position(m_menu_up_lists[m_menu_up_labels[0]].transform_id, { 0.0F, 8.0F });

            cF32 level_list_entry_y = input::mouse.y - transform::position(m_menu_up_lists[m_menu_up_labels[0]].transform_id).y;
            const size_t level_list_entry = (size_t)(level_list_entry_y - std::fmodf(level_list_entry_y, 8.0F)) / 8;
            if (level_list_entry < m_menu_up_lists[m_menu_up_labels[0]].text_items.size()) {
                m_menu_up_bar.at(0).get()->texture("res/texture/font_8_white.png");
                for (size_t i = 0; i < m_menu_up_lists[m_menu_up_labels[0]].text_items.size(); ++i) {
                    if (i != level_list_entry) {
                        m_menu_up_lists[m_menu_up_labels[0]].text_items.at(i).get()->texture("res/texture/font_8_gray.png");
                    }
                }
                m_menu_up_lists[m_menu_up_labels[0]].text_items.at(level_list_entry).get()->texture("res/texture/font_8_white.png");

                if (input::is_pressed(m_input_id, input::Button::LEFT)) {
                    input::release(m_input_id, input::Button::LEFT);
                    console::log("state::Edit::update menu up clicked on ", level_list_entry, " ", m_menu_up_lists[m_menu_up_labels[0]].text_items.at(level_list_entry).get()->get_text(), "\n");

                    m_level_path = m_menu_up_lists[m_menu_up_labels[0]].text_items.at(level_list_entry).get()->get_text();

                    clear_grid_sprites();
                    clear_level_sprites();

                    load_level_sprites(m_level_path);
                }
                if (input::is_pressed(m_input_id, input::Key::DEL)) {
                    input::release(m_input_id, input::Key::DEL);
                    m_info_message.offset({ m_menu_up_lists[m_menu_up_labels[0]].text_items.at(level_list_entry).get()->offset().x + m_menu_up_lists[m_menu_up_labels[0]].bg_w,
                                                m_menu_up_lists[m_menu_up_labels[0]].text_items.at(level_list_entry).get()->offset().y + 8.0F });

                    m_info_message.is_hidden(false);
                    m_info_message.set_text("(y/n)");

                    m_is_asked_to_remove_level = { true, level_list_entry };
                    return;
                }
            }
            transform::position(m_menu_up_lists[m_menu_up_labels[1]].transform_id, { 0.0F, m_menu_up_lists[m_menu_up_labels[1]].bg_h * -1.0F });
        }
        if (mouse_on_menu_bar_item == 1 || is_mouse_on_menu_up_list[1]) {
            m_menu_up_bar.at(1).get()->texture("res/texture/font_8_white.png");
            transform::position(m_menu_up_lists[m_menu_up_labels[0]].transform_id, { 0.0F, m_menu_up_lists[m_menu_up_labels[0]].bg_h * -1.0F });
            transform::position(m_menu_up_lists[m_menu_up_labels[1]].transform_id, { (F32)sprite::source_rect(m_menu_up_bar_bg_sprite_ids.at(0)).w, 8.0F });

            cF32 list_entry_y = input::mouse.y - transform::position(m_menu_up_lists[m_menu_up_labels[1]].transform_id).y;
            const size_t list_entry = (size_t)(list_entry_y - std::fmodf(list_entry_y, 8.0F)) / 8;

            if (list_entry < m_menu_up_lists[m_menu_up_labels[1]].text_items.size()) {
                for (size_t i = 0; i < m_menu_up_lists[m_menu_up_labels[1]].text_items.size(); ++i) {
                    if (i != list_entry) {
                        m_menu_up_lists[m_menu_up_labels[1]].text_items.at(i).get()->texture("res/texture/font_8_gray.png");
                    }
                }
                m_menu_up_lists[m_menu_up_labels[1]].text_items.at(list_entry).get()->texture("res/texture/font_8_white.png");
            }
        }
    }
    bool Edit::add_to_menu_up_bar(const std::string& text) {
        std::unique_ptr<BitmapText> bitmap_text = std::make_unique<BitmapText>();

        F32 offset_x = 0.0F;
        for (auto& i : m_menu_up_bar_bg_sprite_ids) {
            offset_x += (F32)sprite::source_rect(i).w;
        }

        bitmap_text->transform_id = m_menu_up_transform_id;
        bitmap_text->layer = MENU_LAYER;
        bitmap_text->offset({ offset_x, 0.0F });
        bitmap_text->set_text(text);

        cU16 bg_w = text.size() * (int)bitmap_text->font_size;
        cU16 bg_h = (int)bitmap_text->font_size;

        m_menu_up_w += bg_w;

        m_menu_up_bar_bg_sprite_ids.push_back(sprite::make("res/texture/editor_menu_bg.png"));
        sprite::source_rect(m_menu_up_bar_bg_sprite_ids.back(), { 0, 0, bg_w, bg_h });
        sprite::layer(m_menu_up_bar_bg_sprite_ids.back(), MENU_LAYER - 1);
        sprite::transform_id(m_menu_up_bar_bg_sprite_ids.back(), m_menu_up_transform_id);
        sprite::offset(m_menu_up_bar_bg_sprite_ids.back(), { offset_x, 0.0F });

        m_menu_up_bar.emplace_back(std::move(bitmap_text));
        return true;
    }

    void Edit::load_menu_up_list(const std::string& menu, const std::filesystem::path& dir_path) {
        m_menu_up_lists[menu].text_items.clear();
        for (auto const& dir_entry : std::filesystem::directory_iterator{ dir_path }) {            
            add_to_menu_up_list(menu, dir_entry);            
        }
        transform::position(m_menu_up_lists[menu].transform_id, { 0.0F, m_menu_up_lists[menu].bg_h * -1.0F });
    }
    bool Edit::add_to_menu_up_list(const std::string& menu, const std::filesystem::path& path) {
        if (path.string().substr(path.string().size() - 3, 3) != "bin") {
            return false;
        }
        if (menu != m_menu_up_labels[0] && menu != m_menu_up_labels[1]) {
            console::log("state::Edit::add_to_menu_up_list ", menu, " not implemented\n");
            return false;
        }
        std::string path_str = path.string();
        std::string separator_str;
        separator_str = (char)std::filesystem::path::preferred_separator;

        const std::string folder_name = menu == m_menu_up_labels[1] ? "prefab" : "level";        

        size_t res_path_pos = path_str.find("res" + separator_str, 0);
        if (res_path_pos == std::string::npos) {
            console::log("state::Edit::add_to_menu_up_list res" + separator_str + " not found!\n");
            return false;
        }        
        path_str.erase(0, res_path_pos);    // keep last part of the path (res/level/...)        

        std::unique_ptr<BitmapText> bitmap_text = std::make_unique<BitmapText>();
        bitmap_text->transform_id = m_menu_up_lists[menu].transform_id;
        bitmap_text->layer = MENU_LAYER;
        bitmap_text->offset({ 0.0F, m_menu_up_lists[menu].text_items.size() * (F32)bitmap_text->font_size });
        bitmap_text->set_text(path_str);

        cU16 bg_w = path_str.size() * (int)bitmap_text->font_size;
        if (bg_w > m_menu_up_lists[menu].bg_w) {
            m_menu_up_lists[menu].bg_w = bg_w;
        }
        m_menu_up_lists[menu].bg_h = (int)(bitmap_text->font_size * (m_menu_up_lists[menu].text_items.size() + 1));
        sprite::source_rect(m_menu_up_lists[menu].bg_sprite_id, { 0, 0, m_menu_up_lists[menu].bg_w, m_menu_up_lists[menu].bg_h });

        m_menu_up_lists[menu].text_items.emplace_back(std::move(bitmap_text));
        return true;
    }
}