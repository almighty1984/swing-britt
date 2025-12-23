module;
#include <cmath>
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

module entity;
import camera;
import collider.aabb;
import console;
import health;
import sound;
import sprite;
import particle;

void entity::Object::draw_debug(std::unique_ptr<Window>& window) {
    sprite::draw(window, m_sprite_id);
    for (auto& i : m_aabb_ids) {
        aabb::draw(window, i);
    }
}

entity::Object::~Object() {
    //console::log("~Object() ", entity::to_string(m_type), "\n");

    camera::remove_transform_id(m_transform_id);
    for (auto& i : m_aabb_ids) {
        aabb::erase(i);
    }
    m_aabb_ids.clear();
    input_objects.clear();

    anim::erase(m_bounce_anim_id);
    anim::erase(m_dead_anim_id);
    anim::erase(m_hurt_anim_id);
    anim::erase(m_idle_anim_id);
    anim::erase(m_jump_anim_id);
    anim::erase(m_melee_anim_id);
    anim::erase(m_stunned_anim_id);
    anim::erase(m_upended_anim_id);
    m_current_anim_id = -1,

    health::erase(m_health_id);

    sound::erase(m_block_sound_id);
    sound::erase(m_bounce_sound_id);
    sound::erase(m_bump_head_sound_id);
    sound::erase(m_dead_sound_id);
    sound::erase(m_hurt_sound_id);
    sound::erase(m_melee_sound_id);
    
    sprite::erase(m_sprite_id);
    
    //if (m_start_offset == Vec2F{ 0.0F, 0.0F}) {
    if (!transform::is_level(m_transform_id)) {
        transform::erase(m_transform_id);
        m_transform_id = -1;
    }
    //}    
}
bool entity::Object::load_config(const std::filesystem::path& path) {
    std::ifstream in_file(path);
    if (!in_file) {
        console::error("config::load ", path, " not found\n");
        return false;
    }
    std::ostringstream oss{};
    oss << in_file.rdbuf();

    const std::string text = oss.str();

    const size_t transform_label = text.find("Transform", 0);
    if (transform_label == std::string::npos || m_transform_id != -1) {
        goto transform_block_end;
    }

    if (text.find("{", transform_label) != std::string::npos) {
        const size_t transform_open = text.find("{", transform_label) + 1;

        m_transform_id = transform::make();

        //console::log("entity ", entity::to_string(m_type), " camera add transform: ", m_transform_id, "\n");
        camera::add_transform_id(m_transform_id);

        const size_t transform_close = text.find("\n}", transform_open);
        if (transform_close != std::string::npos) {
            const size_t velocity_limit_label = text.find("max_velocity", transform_open);
            size_t end_line = text.find("\n", velocity_limit_label);
            size_t velocity_limit_start = text.find("=", velocity_limit_label);
            if (velocity_limit_start < end_line) {
                const size_t velocity_limit_open = text.find("{", velocity_limit_start);
                const size_t velocity_limit_close = text.find("}", velocity_limit_start);

                if (velocity_limit_open < end_line && velocity_limit_close < end_line) {
                    Vec2F max_velocity = {};
                    size_t value_0 = velocity_limit_open + 1;
                    while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                    const size_t comma_0 = text.find(",", value_0);
                    if (comma_0 < end_line) {
                        max_velocity.x = std::stof(text.substr(value_0, comma_0 - value_0));
                    }
                    size_t value_1 = comma_0 + 1;
                    while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                    size_t value_1_end = velocity_limit_close;
                    while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                    max_velocity.y = std::stof(text.substr(value_1, value_1_end - value_1));

                    transform::max_velocity(m_transform_id, max_velocity);
                    transform::start_max_velocity(m_transform_id, max_velocity);
                }
            }


            const size_t acceleration_label = text.find("acceleration", transform_open);
            end_line = text.find("\n", acceleration_label);
            size_t acceleration_start = text.find("=", acceleration_label);
            if (acceleration_start < end_line) {
                const size_t acceleration_open = text.find("{", acceleration_start);
                const size_t acceleration_close = text.find("}", acceleration_start);

                if (acceleration_open < end_line && acceleration_close < end_line) {
                    Vec2F acceleration = {};
                    size_t value_0 = acceleration_open + 1;
                    while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                    const size_t comma_0 = text.find(",", value_0);
                    if (comma_0 < end_line) {
                        acceleration.x = std::stof(text.substr(value_0, comma_0 - value_0));
                    }
                    size_t value_1 = comma_0 + 1;
                    while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                    size_t value_1_end = acceleration_close;
                    while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                    acceleration.y = std::stof(text.substr(value_1, value_1_end - value_1));

                    transform::acceleration(m_transform_id, acceleration);
                }
            }

            const size_t deceleration_label = text.find("deceleration", transform_open);
            end_line = text.find("\n", deceleration_label);
            size_t deceleration_start = text.find("=", deceleration_label);
            if (deceleration_start < end_line) {
                const size_t deceleration_open = text.find("{", deceleration_start);
                const size_t deceleration_close = text.find("}", deceleration_start);

                if (deceleration_open < end_line && deceleration_close < end_line) {
                    Vec2F deceleration = {};

                    size_t value_0 = deceleration_open + 1;
                    while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                    const size_t comma_0 = text.find(",", value_0);
                    if (comma_0 < end_line) {
                        deceleration.x = std::stof(text.substr(value_0, comma_0 - value_0));
                    }
                    size_t value_1 = comma_0 + 1;
                    while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                    size_t value_1_end = deceleration_close;
                    while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                    deceleration.y = std::stof(text.substr(value_1, value_1_end - value_1));

                    transform::deceleration(m_transform_id, deceleration);
                }
            }

        }
    }
transform_block_end:

    const size_t aabb_set_label = text.find("AABB_Set", 0);
    const size_t aabb_colors_label = text.find("AABB_Colors", 0);

    if (aabb_set_label == std::string::npos) {
        goto aabb_block_end;
    }
    if (text.find("{", aabb_set_label) != std::string::npos) {
        const size_t aabb_set_open = text.find("{", aabb_set_label) + 1;
        const size_t aabb_set_close = text.find("\n}", aabb_set_open);

        if (aabb_set_close != std::string::npos) {
            std::vector<I32> aabb_ids{};

            size_t aabb_open = text.find("{", aabb_set_open + 1);

            while (aabb_open < aabb_set_close) {

                RectF aabb_rect{ 0.0F, 0.0F, 16.0F, 16.0F };
                aabb::Name aabb_name = aabb::Name::NONE;

                size_t end_line = text.find("\n", aabb_open);
                size_t aabb_close = text.find("}", aabb_open);

                size_t prev_endl = text.rfind("\n", aabb_open);
                if (aabb_close < end_line) {

                    // See if there's a name in front
                    size_t name_end = text.rfind("=", aabb_open);

                    if (name_end > prev_endl && name_end != std::string::npos) {
                        //console::log("\n\nname end: ", text.at(name_end), "\n\n");
                        //--name_end;
                        while (text.at(name_end - 1) == '	' || text.at(name_end - 1) == ' ') {
                            --name_end;
                            //console::log("\nname end: ", text.at(name_end), "\n");
                            //console::log("name_end: ", name_end, "\n");
                        }
                        if (name_end > prev_endl) {
                            size_t name_start = name_end;
                            while (text.at(name_start - 1) != '	' && text.at(name_start - 1) != ' ' && name_start > prev_endl) {
                                --name_start;
                            }
                            const std::string aabb_name_str = text.substr(name_start, name_end - name_start);

                            aabb_name = aabb::string_to_name(aabb_name_str);

                            //console::log("entity::Object::load_config(", path, ") aabb_name_str: ", aabb_name_str, "\n");
                        }
                    }
                    size_t value_0 = aabb_open + 1;
                    while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                    size_t comma_0 = text.find(",", value_0);
                    aabb_rect.x = std::stof(text.substr(value_0, comma_0 - value_0));

                    size_t value_1 = comma_0 + 1;
                    while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                    size_t comma_1 = text.find(",", value_1);
                    aabb_rect.y = std::stof(text.substr(value_1, comma_1 - value_1));

                    size_t value_2 = comma_1 + 1;
                    while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                    size_t comma_2 = text.find(",", value_2);
                    aabb_rect.w = std::stof(text.substr(value_2, comma_2 - value_2));

                    size_t value_3 = comma_2 + 1;
                    while (text.at(value_3) == '	' || text.at(value_3) == ' ') ++value_3;
                    size_t comma_3 = text.find(",", value_3);
                    aabb_rect.h = std::stof(text.substr(value_3, comma_3 - value_3));

                    //out_rects.emplace_back(rect);
                    aabb_open = text.find("{", end_line);
                }

                cI32 aabb_id = aabb::make(m_transform_id, { m_start_offset.x + aabb_rect.x,
                                                            m_start_offset.y + aabb_rect.y,
                                                            aabb_rect.w,
                                                            aabb_rect.h });                
                aabb::name(aabb_id, aabb_name);
                aabb::owner(aabb_id, this);
                //aabb::get(aabb_id)->update();
                aabb_ids.emplace_back(aabb_id);

                //aabb_set_label = aabb_set_close;
            }
            m_aabb_ids = aabb_ids;
        }
    }
    if (aabb_colors_label == std::string::npos) {
        goto aabb_block_end;
    }
    if (text.find("{", aabb_colors_label) != std::string::npos) {
        const size_t aabb_colors_open = text.find("{", aabb_colors_label) + 1;
        const size_t aabb_colors_close = text.find("\n}", aabb_colors_open);

        if (aabb_colors_close != std::string::npos) {
            std::vector<I32> aabb_ids{};

            size_t color_open = text.find("{", aabb_colors_open + 1);

            while (color_open < aabb_colors_close) {
                aabb::Name aabb_name = aabb::Name::NONE;
                Color aabb_color;

                size_t end_line = text.find("\n", color_open);
                size_t color_close = text.find("}", color_open);

                size_t prev_endl = text.rfind("\n", color_open);
                if (color_close < end_line) {
                    size_t name_end = text.rfind("=", color_open);

                    if (name_end > prev_endl && name_end != std::string::npos) {
                        //console::log("\n\nname end: ", text.at(name_end), "\n\n");
                        //--name_end;
                        while (text.at(name_end - 1) == '	' || text.at(name_end - 1) == ' ') {
                            --name_end;
                            //console::log("\nname end: ", text.at(name_end), "\n");
                            //console::log("name_end: ", name_end, "\n");
                        }
                        if (name_end > prev_endl) {
                            size_t name_start = name_end;
                            while (text.at(name_start - 1) != '	' && text.at(name_start - 1) != ' ' && name_start > prev_endl) {
                                --name_start;
                            }

                            const std::string aabb_name_str = text.substr(name_start, name_end - name_start);

                            //console::log("entity::Object::load_config(", path, ") aabb_name_str: ", aabb_name_str, "\n");

                            aabb_name = aabb::string_to_name(aabb_name_str);
                        }
                    }

                    size_t value_0 = color_open + 1;
                    while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                    size_t comma_0 = text.find(",", value_0);
                    aabb_color.r = std::stoi(text.substr(value_0, comma_0 - value_0));

                    size_t value_1 = comma_0 + 1;
                    while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                    size_t comma_1 = text.find(",", value_1);
                    aabb_color.g = std::stoi(text.substr(value_1, comma_1 - value_1));

                    size_t value_2 = comma_1 + 1;
                    while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                    size_t comma_2 = text.find(",", value_2);
                    aabb_color.b = std::stoi(text.substr(value_2, comma_2 - value_2));

                    color_open = text.find("{", end_line);
                }

                for (auto& aabb_id : m_aabb_ids) {
                    if (aabb::name(aabb_id) == aabb_name) {
                        aabb::color(aabb_id, aabb_color);
                        aabb::start_color(aabb_id, aabb_color);
                    }
                }
                //aabb_set_label = aabb_colors_close;
            }
        }
    }
aabb_block_end:

    const size_t config_label = text.find("Config", 0);
    if (config_label == std::string::npos) {
        goto config_block_end;
    }   
    if (text.find("{", config_label) != std::string::npos) {
        const size_t config_open = text.find("{", config_label) + 1;
        const size_t config_close = text.find("\n}", config_open);
        if (config_close != std::string::npos) {
            const size_t time_to_be_alive_label = text.find("time_to_be_alive", config_open);
            if (time_to_be_alive_label < config_close) {
                const size_t end_line = text.find("\n", time_to_be_alive_label);
                size_t value_start = text.find("=", time_to_be_alive_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' || text.at(value_start) == ' ') ++value_start;
                    m_time_to_be_alive = std::stoi(text.substr(value_start, end_line - value_start));
                    m_time_left_alive = m_time_to_be_alive;
                }
            }
            const size_t time_to_be_dead_label = text.find("time_to_be_dead", config_open);
            if (time_to_be_dead_label < config_close) {
                const size_t end_line = text.find("\n", time_to_be_dead_label);
                size_t value_start = text.find("=", time_to_be_dead_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' || text.at(value_start) == ' ') ++value_start;
                    m_time_to_be_dead = std::stoi(text.substr(value_start, end_line - value_start));
                    //m_time_left_dead = m_time_to_be_dead;
                }
            }
            const size_t time_to_hurt_label = text.find("time_to_hurt", config_open);
            if (time_to_hurt_label < config_close) {
                const size_t end_line = text.find("\n", time_to_hurt_label);
                size_t value_start = text.find("=", time_to_hurt_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' || text.at(value_start) == ' ') ++value_start;
                    m_time_to_hurt = std::stoi(text.substr(value_start, end_line - value_start));
                }
            }
            const size_t time_to_spawn_sense_label = text.find("time_to_spawn_sense", config_open);
            if (time_to_spawn_sense_label < config_close) {
                const size_t end_line = text.find("\n", time_to_spawn_sense_label);
                size_t value_start = text.find("=", time_to_spawn_sense_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' || text.at(value_start) == ' ') ++value_start;
                    m_time_to_spawn_sense = std::stoi(text.substr(value_start, end_line - value_start));
                }
            }
        }
    }
config_block_end:
    
    const size_t sounds_label = text.find("Sounds", 0);
    if (sounds_label == std::string::npos) {
        goto sound_block_end;
    }
    if (text.find("{", sounds_label) != std::string::npos) {
        const size_t sounds_open = text.find("{", sounds_label) + 1;
        const size_t sounds_close = text.find("\n}", sounds_open);
        if (sounds_close != std::string::npos) {
            size_t label_start = sounds_open + 1;
            while (1) {
                const size_t end_line = text.find("\n", label_start + 1);
                if (end_line > sounds_close) {
                    break;
                }
                while ((text.at(label_start) == '	' || text.at(label_start) == ' ' || text.at(label_start) == '\n') && label_start < end_line) {
                    ++label_start;
                }
                size_t current_equals = text.find("=", label_start);
                if (current_equals > sounds_close) {
                    break;
                }
                size_t label_end = text.find("=", label_start);
                while ((text.at(label_end - 1) == '	' || text.at(label_end - 1) == ' ') && label_end > label_start) {
                    --label_end;
                }
                const std::string sound_label = text.substr(label_start, label_end - label_start);

                size_t value_start = current_equals + 1;
                while ((text.at(value_start) == '	' || text.at(value_start) == ' ') && value_start < end_line) {
                    ++value_start;
                }
                size_t value_end = end_line;
                while ((text.at(value_end - 1) == '	' || text.at(value_end - 1) == ' ') && value_end > value_start) {
                    --value_end;
                }
                const std::filesystem::path sound_path = "res/sound/" + text.substr(value_start, value_end - value_start);

                //console::log("entity::load_config ", path, " sound: ", sound_path, "\n\n");
                if      (sound_label == "block")       m_block_sound_id       = sound::make(sound_path);
                else if (sound_label == "bounce")      m_bounce_sound_id      = sound::make(sound_path);
                else if (sound_label == "bump_head")   m_bump_head_sound_id   = sound::make(sound_path);
                else if (sound_label == "dead")        m_dead_sound_id        = sound::make(sound_path);
                else if (sound_label == "hurt")        m_hurt_sound_id        = sound::make(sound_path);
                else if (sound_label == "melee")       m_melee_sound_id       = sound::make(sound_path);
                else if (sound_label == "shoot")       m_shoot_sound_id       = sound::make(sound_path);
                else if (sound_label == "water_enter") m_water_enter_sound_id = sound::make(sound_path);
                else if (sound_label == "water_exit")  m_water_exit_sound_id  = sound::make(sound_path);
                label_start = end_line;
            }
        }
    }
sound_block_end:

    if (text.find("Sprite", 0) == std::string::npos) {
        goto sprite_and_animations_block_end;
    }
    if (text.find("{", text.find("Sprite", 0)) != std::string::npos) {
        const size_t sprite_label = text.find("Sprite", 0);
        const size_t sprite_open = text.find("{", sprite_label) + 1;
        const size_t sprite_close = text.find("\n}", sprite_open);
        if (sprite_close != std::string::npos) {

            bool sprite_is_debug = false,
                sprite_is_hidden = false,
                sprite_is_leftward = false,
                sprite_is_upended = false;

            std::filesystem::path sprite_texture_path{};

            RectI sprite_source_rect = { 0, 0, 16, 16 };
            Vec2F sprite_offset{},
                  sprite_origin{};

            const size_t texture_label = text.find("texture", sprite_open);
            if (texture_label < sprite_close) {
                const size_t end_line = text.find("\n", texture_label);
                size_t texture_start = text.find("=", texture_label);
                if (texture_start < end_line) {
                    ++texture_start;
                    while (text.at(texture_start) == '	' || text.at(texture_start) == ' ') ++texture_start;
                    sprite_texture_path = "res/texture/" + text.substr(texture_start, end_line - texture_start);
                    //console::log("texture: ", texture_path, "\n");
                }
            }
            const size_t is_debug_label = text.find("is_debug", sprite_open);
            if (is_debug_label < sprite_close) {
                const size_t end_line = text.find("\n", is_debug_label);
                size_t is_debug_start = text.find("=", is_debug_label);
                if (is_debug_start < end_line) {
                    ++is_debug_start;
                    while (text.at(is_debug_start) == '	' || text.at(is_debug_start) == ' ') ++is_debug_start;
                    sprite_is_debug = text.substr(is_debug_start, end_line - is_debug_start) == "true" ? true : false;
                }
            }
            const size_t is_hidden_label = text.find("is_hidden", sprite_open);
            if (is_hidden_label < sprite_close) {
                const size_t end_line = text.find("\n", is_hidden_label);
                size_t is_hidden_start = text.find("=", is_hidden_label);
                if (is_hidden_start < end_line) {
                    ++is_hidden_start;
                    while (text.at(is_hidden_start) == '	' || text.at(is_hidden_start) == ' ') ++is_hidden_start;
                    sprite_is_hidden = text.substr(is_hidden_start, end_line - is_hidden_start) == "true" ? true : false;
                }
            }
            const size_t is_leftward_label = text.find("is_leftward", sprite_open);
            if (is_leftward_label < sprite_close) {
                const size_t end_line = text.find("\n", is_leftward_label);
                size_t is_leftward_start = text.find("=", is_leftward_label);
                if (is_leftward_start < end_line) {
                    ++is_leftward_start;
                    while (text.at(is_leftward_start) == '	' || text.at(is_leftward_start) == ' ') ++is_leftward_start;
                    sprite_is_leftward = text.substr(is_leftward_start, end_line - is_leftward_start) == "true" ? true : false;
                }
            }
            const size_t is_upended_label = text.find("is_upended", sprite_open);
            if (is_upended_label < sprite_close) {
                const size_t end_line = text.find("\n", is_upended_label);
                size_t is_upended_start = text.find("=", is_upended_label);
                if (is_upended_start < end_line) {
                    ++is_upended_start;
                    while (text.at(is_upended_start) == '	' || text.at(is_upended_start) == ' ') ++is_upended_start;
                    sprite_is_upended = text.substr(is_upended_start, end_line - is_upended_start) == "true" ? true : false;
                }
            }
            const size_t source_rect_label = text.find("source_rect", sprite_open);
            if (source_rect_label < sprite_close) {
                const size_t end_line = text.find("\n", source_rect_label);
                const size_t source_rect_start = text.find("=", source_rect_label);
                if (source_rect_start < end_line) {
                    const size_t source_rect_open = text.find("{", source_rect_start);
                    const size_t source_rect_close = text.find("}", source_rect_start);

                    if (source_rect_open < end_line && source_rect_close < end_line) {
                        size_t value_0 = source_rect_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            sprite_source_rect.x = std::stoi(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        const size_t comma_1 = text.find(",", value_1);
                        if (comma_1 < end_line) {
                            sprite_source_rect.y = std::stoi(text.substr(value_1, comma_1 - value_1));
                        }
                        size_t value_2 = comma_1 + 1;
                        while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                        const size_t comma_2 = text.find(",", value_2);
                        if (comma_2 < end_line) {
                            sprite_source_rect.w = std::stoi(text.substr(value_2, comma_2 - value_2));
                        }
                        size_t value_3 = comma_2 + 1;
                        while (text.at(value_3) == '	' || text.at(value_3) == ' ') ++value_3;
                        size_t value_3_end = source_rect_close;
                        while (text.at(value_3_end) == '	' || text.at(value_3_end) == ' ') --value_3_end;

                        sprite_source_rect.h = std::stoi(text.substr(value_3, value_3_end - value_3));
                        //console::log("entity::Object::load_config source_rect: ", sprite_source_rect.x, " ", sprite_source_rect.y, " ", sprite_source_rect.w, " ", sprite_source_rect.h, "\n");                            
                    }
                }
            }
            const size_t offset_label = text.find("offset", sprite_open);
            if (offset_label < sprite_close) {
                const size_t end_line = text.find("\n", offset_label);
                size_t offset_start = text.find("=", offset_label);
                if (offset_start < end_line) {
                    const size_t offset_open = text.find("{", offset_start);
                    const size_t offset_close = text.find("}", offset_start);

                    if (offset_open < end_line && offset_close < end_line) {
                        size_t value_0 = offset_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            sprite_offset.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = offset_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        sprite_offset.y = std::stof(text.substr(value_1, value_1_end - value_1));
                        //console::log("entity::Object::load_config offset: ", offset.x, " ", offset.y, "\n");
                    }
                }
            }
            const size_t origin_label = text.find("origin", sprite_open);
            if (origin_label < sprite_close) {
                const size_t end_line = text.find("\n", origin_label);
                size_t origin_start = text.find("=", origin_label);
                if (origin_start < end_line) {
                    const size_t origin_open = text.find("{", origin_start);
                    const size_t origin_close = text.find("}", origin_start);

                    if (origin_open < end_line && origin_close < end_line) {
                        size_t value_0 = origin_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            sprite_origin.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = origin_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        sprite_origin.y = std::stof(text.substr(value_1, value_1_end - value_1));
                    }
                }
            }

            if (sprite_texture_path.empty()) {
                sprite_texture_path = "res/texture/set_255.png";
            }
            /*if (m_sprite_id != -1) {
                sprite::erase(m_sprite_id);
            }*/
            m_sprite_id = sprite::make(sprite_texture_path);

            sprite::layer(m_sprite_id, m_start_layer);
            sprite::is_debug(m_sprite_id, sprite_is_debug);
            sprite::is_hidden(m_sprite_id, sprite_is_hidden);
            sprite::is_leftward(m_sprite_id, sprite_is_leftward);
            sprite::is_upended(m_sprite_id, sprite_is_upended);
            sprite::source_rect(m_sprite_id, sprite_source_rect);
                
            if (sprite_origin == Vec2F{ 0.0f, 0.0f }) {
                sprite::origin(m_sprite_id, { sprite_source_rect.w / 2.0F, sprite_source_rect.h / 2.0F });
            } else {
                sprite::origin(m_sprite_id, sprite_origin);
            }

            sprite::offset(m_sprite_id, m_start_offset + sprite_offset); // Add to already set level offset
            sprite::transform_id(m_sprite_id, m_transform_id);
            //console::log(path, " is_hidden: ", sprite_is_hidden, "\n");
        }
    }        
    if (text.find("Animations", 0) != std::string::npos) {
        const size_t animations_label = text.find("Animations", 0);

        if (text.find("{", animations_label) != std::string::npos) {
            const size_t animations_open = text.find("{", animations_label) + 1;
            const size_t animations_close = text.find("\n}", animations_open);
            if (animations_close != std::string::npos) {
                size_t current_equals = animations_open;

                while (1) {
                    current_equals = text.find("=", current_equals + 1);
                    if (current_equals > animations_close) {
                        break;
                    }
                    const size_t end_line = text.find("\n", current_equals);
                    const size_t current_open = text.find("{", current_equals);
                    const size_t current_close = text.find("}", current_equals);

                    if (current_open < end_line && current_close < end_line) {
                        U16 source_y = 0;
                        F32 speed = 0.0F;
                        U16 num_loops = 0;

                        size_t value_0 = current_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            source_y = std::stoi(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        const size_t comma_1 = text.find(",", value_1);
                        if (comma_1 < end_line) {
                            speed = std::stof(text.substr(value_1, comma_1 - value_1));
                        }
                        size_t value_2 = comma_1 + 1;
                        while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                        size_t value_2_end = current_close;
                        while (text.at(value_2_end) == '	' || text.at(value_2_end) == ' ') --value_2_end;
                        num_loops = std::stoi(text.substr(value_2, value_2_end - value_2));

                        size_t current_label_start = text.rfind("\n", current_equals);
                        while (text.at(current_label_start) == '	' || text.at(current_label_start) == ' ' || text.at(current_label_start) == '\n') ++current_label_start;
                        size_t current_label_end = current_equals;
                        while (text.at(current_label_end - 1) == '	' || text.at(current_label_end - 1) == ' ') --current_label_end;

                        std::string current_label_str = text.substr(current_label_start, current_label_end - current_label_start);

                        m_current_anim_id = -1;
                        if (current_label_str == "bounce" || current_label_str == "BOUNCE") {
                            m_bounce_anim_id = anim::make();   m_current_anim_id = m_bounce_anim_id;
                        } else if (current_label_str == "dead" || current_label_str == "DEAD") {
                            m_dead_anim_id = anim::make();     m_current_anim_id = m_dead_anim_id;
                        } else if (current_label_str == "enter" || current_label_str == "ENTER") {
                            m_enter_anim_id = anim::make();    m_current_anim_id = m_enter_anim_id;
                        } else if (current_label_str == "exit" || current_label_str == "EXIT") {
                            m_exit_anim_id = anim::make();     m_current_anim_id = m_exit_anim_id;
                        } else if (current_label_str == "hurt" || current_label_str == "HURT") {
                            m_hurt_anim_id = anim::make();     m_current_anim_id = m_hurt_anim_id;
                        } else if (current_label_str == "idle" || current_label_str == "IDLE") {
                            m_idle_anim_id = anim::make();     m_current_anim_id = m_idle_anim_id;
                        } else if (current_label_str == "jump" || current_label_str == "JUMP") {
                            m_jump_anim_id = anim::make();     m_current_anim_id = m_jump_anim_id;
                        } else if (current_label_str == "melee" || current_label_str == "MELEE") {
                            m_melee_anim_id = anim::make();    m_current_anim_id = m_melee_anim_id;
                        } else if (current_label_str == "shoot" || current_label_str == "SHOOT") {
                            m_shoot_anim_id = anim::make();    m_current_anim_id = m_shoot_anim_id;
                        } else if (current_label_str == "stunned" || current_label_str == "STUNNED") {
                            m_stunned_anim_id = anim::make();  m_current_anim_id = m_stunned_anim_id;
                        } else if (current_label_str == "upended" || current_label_str == "UPENDED") {
                            m_upended_anim_id = anim::make();  m_current_anim_id = m_upended_anim_id;
                        }
                        
                        anim::texture_size(m_current_anim_id, sprite::texture_size(m_sprite_id));
                        anim::source(m_current_anim_id, { 0, source_y, sprite::source_rect(m_sprite_id).w, sprite::source_rect(m_sprite_id).h });                        
                        anim::loops(m_current_anim_id, num_loops);
                        anim::type(m_current_anim_id, anim::string_to_type(current_label_str));

                        anim::first_frame(m_current_anim_id, 0);
                        int num_frames = anim::texture_w(m_current_anim_id) / sprite::source_rect(m_sprite_id).w;
                        anim::last_frame(m_current_anim_id, num_frames - 1);
                        if (speed < 0.0F) {
                            speed = -speed;
                            anim::is_reverse(m_current_anim_id, true);
                            
                            anim::first_frame(m_current_anim_id, num_frames - 1);
                            anim::last_frame(m_current_anim_id, 0);
                            //console::log("anim: ", anim::to_string(anim::type(m_current_anim_id)), " speed: ", speed, "\n");                            
                        }
                        
                        anim::speed(m_current_anim_id, speed);
                    }
                }
            }
        }
    }

sprite_and_animations_block_end:

    const size_t health_label = text.find("Health", 0);
    if (health_label == std::string::npos) {
        goto health_block_end;
    }
    m_health_id = health::make(m_transform_id);
    if (text.find("{", health_label) != std::string::npos) {
        const size_t health_open = text.find("{", health_label) + 1;
        const size_t health_close = text.find("\n}", health_open);
        if (health_close != std::string::npos) {
            const size_t amount_label = text.find("amount", health_open);
            if (amount_label < health_close) {
                const size_t end_line = text.find("\n", amount_label);
                size_t value_start = text.find("=", amount_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' || text.at(value_start) == ' ') ++value_start;
                    cF32 amount = std::stoi(text.substr(value_start, end_line - value_start));
                    health::max(m_health_id, amount);
                    health::amount(m_health_id, amount);
                }
            }
            const size_t regen_label = text.find("regen", health_open);
            if (regen_label < health_close) {
                const size_t end_line = text.find("\n", regen_label);
                size_t value_start = text.find("=", regen_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' || text.at(value_start) == ' ') ++value_start;
                    cF32 regen = std::stof(text.substr(value_start, end_line - value_start));
                    health::regen(m_health_id, regen);
                }
            }
            const size_t offset_label = text.find("offset", health_open);
            size_t end_line = text.find("\n", offset_label);
            size_t offset_start = text.find("=", offset_label);
            if (offset_start < end_line) {
                const size_t offset_open = text.find("{", offset_start);
                const size_t offset_close = text.find("}", offset_start);

                if (offset_open < end_line && offset_close < end_line) {
                    Vec2F offset{};
                    size_t value_0 = offset_open + 1;
                    while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                    const size_t comma_0 = text.find(",", value_0);
                    if (comma_0 < end_line) {
                        offset.x = std::stof(text.substr(value_0, comma_0 - value_0));
                    }
                    size_t value_1 = comma_0 + 1;
                    while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                    size_t value_1_end = offset_close;
                    while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                    offset.y = std::stof(text.substr(value_1, value_1_end - value_1));
                    health::offset(m_health_id, offset);
                }
            }
        }
    }
health_block_end:
    return true;
}

void entity::Object::spawn(particle::cType type, cVec2F position, cVec2F velocity) {
    particle::spawn({ .parent = this,
                               .type = type,
                               .position = position,
                               .velocity = velocity });
}
void entity::Object::spawn_fan(cF32 start_angle, cF32 end_angle, cU8 num, particle::cType type, cVec2F position, cVec2F velocity, cF32 speed) {
    //cVec2F p = { position.x - dimensions.x / 2.0F, position.y - dimensions.y / 2.0F };

    /*Vec2F size   = { 16.0F, 16.0F };
    Vec2F offset = {   0.0F, 0.0F };
    
    switch (type) {
    case particle::Type::BRICK:      size = { 6.0F, 6.0F }; break;
    case particle::Type::DROP_BLOOD: size = { 4.0F, 8.0F }; break;
    case particle::Type::SENSE:      size = { 8.0F, 8.0F }; break;
    }*/
    //cVec2F p = position - size / 2.0F;

    cF32 end_angle_adjusted = start_angle > end_angle ? end_angle + 360.0F : end_angle;

    cF32 angle_to_radians = (3.1415926535F / 180.0F);
    cF32 end_radian = end_angle_adjusted * angle_to_radians;
    cF32 start_radian = start_angle * angle_to_radians;
    F32 radians = start_radian;

    F32 radian_range = end_radian - start_radian;

    //console::log("entity::Object::spawn_fan() velocity: ", velocity.x, " ", velocity.y, "\n");

    for (U8 i = 0; i <= num; ++i) {
        particle::spawn({ .parent = this,
                                   .type = type,
                                   .position = position,
                                   .velocity = velocity + Vec2F{ std::cosf(radians), std::sinf(radians) } * speed });
        radians += (radian_range / num);
    }
}