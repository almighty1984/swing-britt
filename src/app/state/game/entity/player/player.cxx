module;
#include <fstream>
#include <sstream>
module entity.player;
import anim;
import collider.aabb;
import console;
import health;
import input;
import particle;
import sound;
import sprite;
import transform;
import types;

namespace entity {
    Player::Player() {
        console::log("Player()\n");
        m_input_id = input::make();

        m_type = Type::PLAYER;
        m_state = m_next_state = State::RUN;

        load_config("res/entity/player/PLAYER.cfg");
    }

    Player::~Player() {
        console::log("Player::~Player() input\n");
        input::erase(m_input_id);

        sound::erase(m_down_thrust_sound_id);
        sound::erase(m_hit_ground_sound_id);
        sound::erase(m_hover_sound_id);
        sound::erase(m_jump_sound_id);
        sound::erase(m_ledge_climb_sound_id);
        sound::erase(m_ledge_grab_sound_id);
        sound::erase(m_lever_sound_id);
        sound::erase(m_pick_up_sound_id);
        sound::erase(m_skid_sound_id);
        sound::erase(m_slide_wall_sound_id);
        sound::erase(m_step_grass_sound_id);
        sound::erase(m_swing_detach_sound_id);
        sound::erase(m_toss_sound_id);

        anim::erase(m_down_thrust_anim_id);
        anim::erase(m_duck_anim_id);
        anim::erase(m_crawl_anim_id);
        anim::erase(m_fall_anim_id);
        anim::erase(m_hit_ground_anim_id);
        anim::erase(m_hover_anim_id);
        anim::erase(m_jump_skid_anim_id);
        anim::erase(m_jump_spin_anim_id);
        anim::erase(m_ledge_climb_anim_id);
        anim::erase(m_ledge_grab_anim_id);
        anim::erase(m_lever_anim_id);
        anim::erase(m_rise_anim_id);
        anim::erase(m_run_anim_id);
        anim::erase(m_skid_anim_id);
        anim::erase(m_slide_ground_anim_id);
        anim::erase(m_slide_wall_anim_id);
        anim::erase(m_swim_anim_id);
        anim::erase(m_swing_anim_id);
        anim::erase(m_walk_anim_id);
    }

    bool Player::load_config(const std::filesystem::path& path) {
        if (!Object::load_config(path)) {
            return false;
        }
        m_start_sprite_offset = sprite::offset(m_sprite_id);

        console::log("entity::Player::load_config start_sprite_offset: ", m_start_sprite_offset.x, " ", m_start_sprite_offset.y, "\n");

        std::ifstream in_file(path);
        if (!in_file) {
            console::error("entity::Player::load_config::load ", path, " not found\n");
            return false;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();

        const std::string text = oss.str();

        const size_t animations_label = text.find("Animations", 0);
        if (animations_label != std::string::npos) {
            size_t animations_open = text.find("{", animations_label);
            if (animations_open != std::string::npos) {
                ++animations_open;
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
                            //console::log("Object::load_config anim: ", current_label_str, " ", source_y, " ", speed, " ", num_loops, "\n");

                            //console::log("Object::load_config anim: ", current_label_str, ".\n");

                            m_current_anim_id = -1;
                            if (current_label_str == "CRAWL") {
                                m_crawl_anim_id = anim::make();
                                m_current_anim_id = m_crawl_anim_id;
                            } else if (current_label_str == "DOWN_THRUST") {
                                m_down_thrust_anim_id = anim::make();
                                m_current_anim_id = m_down_thrust_anim_id;
                            } else if (current_label_str == "DUCK") {
                                m_duck_anim_id = anim::make();
                                m_current_anim_id = m_duck_anim_id;
                            } else if (current_label_str == "FALL") {
                                m_fall_anim_id = anim::make();
                                m_current_anim_id = m_fall_anim_id;
                            } else if (current_label_str == "RUN") {
                                m_run_anim_id = anim::make();
                                m_current_anim_id = m_run_anim_id;
                            } else if (current_label_str == "HIT_GROUND") {
                                m_hit_ground_anim_id = anim::make();
                                m_current_anim_id = m_hit_ground_anim_id;
                            } else if (current_label_str == "HOVER") {
                                m_hover_anim_id = anim::make();
                                m_current_anim_id = m_hover_anim_id;
                            } else if (current_label_str == "JUMP_SPIN") {
                                m_jump_spin_anim_id = anim::make();
                                m_current_anim_id = m_jump_spin_anim_id;
                            } else if (current_label_str == "JUMP_SKID") {
                                m_jump_skid_anim_id = anim::make();
                                m_current_anim_id = m_jump_skid_anim_id;
                            } else if (current_label_str == "LEDGE_GRAB") {
                                m_ledge_grab_anim_id = anim::make();
                                m_current_anim_id = m_ledge_grab_anim_id;
                            } else if (current_label_str == "LEDGE_CLIMB") {
                                m_ledge_climb_anim_id = anim::make();
                                m_current_anim_id = m_ledge_climb_anim_id;
                            } else if (current_label_str == "LEVER") {
                                m_lever_anim_id = anim::make();
                                m_current_anim_id = m_lever_anim_id;
                            } else if (current_label_str == "RISE") {
                                m_rise_anim_id = anim::make();
                                m_current_anim_id = m_rise_anim_id;
                            } else if (current_label_str == "SKID") {
                                m_skid_anim_id = anim::make();
                                m_current_anim_id = m_skid_anim_id;
                            } else if (current_label_str == "SLIDE_GROUND") {
                                m_slide_ground_anim_id = anim::make();
                                m_current_anim_id = m_slide_ground_anim_id;
                            } else if (current_label_str == "SLIDE_WALL") {
                                m_slide_wall_anim_id = anim::make();
                                m_current_anim_id = m_slide_wall_anim_id;
                            } else if (current_label_str == "SWIM") {
                                m_swim_anim_id = anim::make();
                                m_current_anim_id = m_swim_anim_id;
                            } else if (current_label_str == "SWING") {
                                m_swing_anim_id = anim::make();
                                m_current_anim_id = m_swing_anim_id;
                            } else if (current_label_str == "WALK") {
                                m_walk_anim_id = anim::make();
                                m_current_anim_id = m_walk_anim_id;
                            }
                            if (m_current_anim_id != -1) {
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
        }


        const size_t sounds_label = text.find("Sounds", 0);
        if (sounds_label != std::string::npos) {
            size_t sounds_open = text.find("{", sounds_label);
            if (sounds_open != std::string::npos) {
                ++sounds_open;
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
                        //console::log("Player::load_config ", sound_label, "=", sound_path, "\n");
                        //console::log("Player::load_config ", sound_label, "\n");

                        if (sound_label == "down_thrust")  m_down_thrust_sound_id = sound::make(sound_path);
                        else if (sound_label == "hit_ground")   m_hit_ground_sound_id = sound::make(sound_path);
                        else if (sound_label == "hover") {
                            m_hover_sound_id = sound::make(sound_path);
                            sound::is_looped(m_hover_sound_id, true);
                        } else if (sound_label == "jump")         m_jump_sound_id = sound::make(sound_path);
                        else if (sound_label == "ledge_climb")  m_ledge_climb_sound_id = sound::make(sound_path);
                        else if (sound_label == "ledge_grab")   m_ledge_grab_sound_id = sound::make(sound_path);
                        else if (sound_label == "lever")        m_lever_sound_id = sound::make(sound_path);
                        else if (sound_label == "pick_up")      m_pick_up_sound_id = sound::make(sound_path);
                        else if (sound_label == "skid") {
                            m_skid_sound_id = sound::make(sound_path);
                            sound::is_looped(m_skid_sound_id, false);
                        } else if (sound_label == "slide_wall")   m_slide_wall_sound_id = sound::make(sound_path);
                        else if (sound_label == "step_grass")   m_step_grass_sound_id = sound::make(sound_path);
                        else if (sound_label == "swing_detach") m_swing_detach_sound_id = sound::make(sound_path);
                        else if (sound_label == "toss")         m_toss_sound_id = sound::make(sound_path);
                        label_start = end_line;
                    }
                }
            }
        }
        return true;
    }

    bool Player::hurt(Object* culprit) {
        if (!culprit) return false;
        if (m_time_left_hurt > 0) {
            console::log("entity::Player::hurt() still hurting: ", m_time_left_hurt, "\n");
            return false;
        }
        m_time_left_hurt = m_time_to_hurt;

        if (culprit->type() == Type::PARTICLE_MELEE) {
            if (!culprit->parent() || culprit->parent() == this) {
                return false;
            }
            if (culprit->parent()->type() == Type::FROG) {
                health::add_amount(m_health_id, -16.0f);
            }
            console::log("entity::Player::hurt() culprit: ", to_string(culprit->parent()->type()), " health: ", health::amount(m_health_id), "\n");
        } else if (culprit->type() == Type::PARTICLE_SHOT) {            
            health::add_amount(m_health_id, -16.0f);
            console::log("\n\nentity::Player::hurt() culprit: ", to_string(culprit->type()), " health: ", health::amount(m_health_id), "\n");
        }
        return true;
    }

    void Player::interact(Object* object) {
        if (!object || m_time_left_interacting > 0) return;

        m_time_left_interacting = m_time_to_interact;

        if (object->type() == Type::BRICK || object->type() == Type::BUG) {
            if (object->state() == State::SWIM) return;

            if (!m_is_carrying && !object->parent()) {
                console::log("entity::Player::interact pick up\n");
                m_is_carrying = true;
                object->parent(this);
                object->next_state(State::CARRIED);
                sprite::layer(object->sprite_id(), sprite::layer(m_sprite_id) + 1);
                sound::position(m_pick_up_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_pick_up_sound_id);
            } else if (object->parent() && m_is_carrying) {
                console::log("Player::interact toss\n");
                if (sprite::is_leftward(m_sprite_id) && object->is_near_wall_L()) {
                    console::log("entity::Player::interact nah, near wall L\n");
                    return;
                }
                if (!sprite::is_leftward(m_sprite_id) && object->is_near_wall_R()) {
                    console::log("entity::Player::interact nah, near wall R\n");
                    return;
                }
                m_is_carrying = false;
                sprite::layer(object->sprite_id(), object->start_layer());
                object->parent(nullptr);
                object->velocity(velocity() + moved_velocity());
                //object->add_position_x(object->velocity().x);
                if (sprite::is_leftward(m_sprite_id)) {
                    object->add_velocity_x(-2.0F);
                    object->add_position_x(-8.0F);
                } else {
                    object->add_velocity_x(2.0F);
                    object->add_position_x(8.0F);
                }
                //console::log("object->velocity().x: ", object->velocity().x, "\n");

                object->add_velocity_y(-1.5F);
                object->add_position_y(-1.0F);
                object->next_state(State::TOSSED);
                //object->time_left_interacting(10);

                sound::position(m_toss_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_toss_sound_id);
            }
        } else if (object->type() == Type::CLIP_LEDGE) {
            m_is_on_ledge = true;
            m_is_climbing_ledge = true;
            m_is_sliding_wall = false;
            input::press(m_input_id, key_up);
            m_is_wall_to_left = sprite::is_leftward(m_sprite_id);
            //position().y = object->other_rect.y - 8;
            velocity_y(0.0F);
            reset_anim(m_ledge_climb_anim_id);
        } else if (object->type() == Type::TRIGGER) {

            m_time_left_lever = m_time_to_lever;
            reset_anim(m_lever_anim_id);
            sprite::is_leftward(m_sprite_id, object->is_dead());

            if (object->is_dead()) {
                object->time_left_alive(U16_MAX);
                object->time_left_dead(0);
            } else {
                object->time_left_dead(U16_MAX);
                object->time_left_alive(0);
            }

            sound::position(m_lever_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound::play(m_lever_sound_id);
        }

    }

    void Player::spawn_down_thrust(cVec2F position) {
        cVec2F dimensions = { 32.0F, 32.0F };
        cF32 speed = 3.0F;

        particle::cType type = particle::Type::DOWN_THRUST;

        cVec2F p = { position.x - dimensions.x / 2.0F, position.y - dimensions.y / 2.0F };

        particle::spawn({ .parent = this,
                          .type = type,
                          .position = p,
                          .velocity = { -speed, 0.0F } });
        particle::spawn({ .parent = this,
                          .type = type,
                          .position = p,
                          .velocity = {  speed, 0.0F } });

    }


    void Player::update() {
        if (m_time_left_hurt > 0) {
            //console::log("entity::Player::update() time left hurt: ", m_time_left_hurt, "\n");
            --m_time_left_hurt;
        }
        if (m_time_left_interacting > 0) {
            --m_time_left_interacting;
        }
        if (m_is_sliding_wall) {
            ++m_time_sliding_wall;
        } else {
            m_time_sliding_wall = 0;
        }

        if (m_next_state != m_state) {
            m_prev_state = m_state;
            m_state = m_next_state;
            m_is_first_state_update = true;
        }
        switch (m_state) {
            case State::DEAD:  dead();  break;
            case State::RUN:   run();   break;
            case State::SWIM:  swim();  break;
            case State::SWING: swing(); break;
            default:                    break;
        }

        //console::log("Player::update() anim source x: ", anim::source(m_current_anim_id).x, "\n");
        sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));

        health::layer(m_health_id, m_start_layer);
        if (health::amount(m_health_id) <= 0.0f) {
            m_next_state = State::DEAD;
        }

        //console::log("is_carrying: ", m_is_carrying, "\n");
        //console::log("is to write save: ", m_is_to_write_save, "\n");

        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
            //console::log("time_left_in_state: ", (int)m_time_left_in_state, "\n");
        }
        //console::log("level pos: ", position_difference(level_transform_id).x, "\n");
    }
}