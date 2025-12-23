module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
export module entity.player;
import anim;
import collider.aabb;
import console;
import health;
import input;
import sound;
import sprite;
import transform;
import entity;
import types;

//export U8 NUM_ADDED_PLAYERS = 0;

export namespace entity {
    class Player : public Object {
    private:
        start::Type   m_next_level = start::Type::CENTER;

        start::Info m_next_start = { start::Type::CENTER, 0 };

        bool m_is_to_write_save = false;

        I32 m_input_id = -1;

        I32 m_down_thrust_sound_id  = -1,
            m_hit_ground_sound_id   = -1,
            m_hover_sound_id        = -1,
            m_jump_sound_id         = -1,
            m_ledge_climb_sound_id  = -1,
            m_ledge_grab_sound_id   = -1,
            m_lever_sound_id        = -1,
            m_pick_up_sound_id      = -1,
            m_skid_sound_id         = -1,
            m_slide_wall_sound_id   = -1,
            m_step_grass_sound_id   = -1,
            m_swing_detach_sound_id = -1,
            m_toss_sound_id         = -1;

        I32 m_down_thrust_anim_id  = -1,
            m_duck_anim_id         = -1,
            m_crawl_anim_id        = -1,
            m_fall_anim_id         = -1,
            m_hit_ground_anim_id   = -1,
            m_hover_anim_id        = -1,
            m_jump_skid_anim_id    = -1,
            m_jump_spin_anim_id    = -1,
            m_ledge_climb_anim_id  = -1,
            m_ledge_grab_anim_id   = -1,
            m_lever_anim_id        = -1,
            m_rise_anim_id         = -1,
            m_run_anim_id          = -1,
            m_skid_anim_id         = -1,
            m_slide_ground_anim_id = -1,
            m_slide_wall_anim_id   = -1,
            m_swim_anim_id         = -1,
            m_swing_anim_id        = -1,
            m_walk_anim_id         = -1;

        U8 m_time_left_until_down_thrust = 0, m_time_until_down_thrust = 16,
           m_time_left_bouncing          = 0, m_time_to_bounce         = 20,
           m_time_left_ducking           = 0, m_time_to_duck           = 10,
           m_time_left_rising            = 0, m_time_to_rise           = 10,
           m_time_left_hitting_ground    = 0, m_time_to_hit_ground     = 20,
           m_time_left_holding_jump      = 0, m_time_to_hold_jump      = 10,
           m_time_left_jumping_wall      = 0, m_time_to_jump_wall      =  8,
           m_time_left_jump_again        = 0, m_time_to_jump_again     =  8,
           m_time_left_lever             = 0, m_time_to_lever          = 10,
           m_time_left_melee             = 0, m_time_to_melee          =  8,
           m_time_left_skidding          = 0, m_time_to_skid           = 20,
           m_time_sliding_wall           = 0;

        bool m_is_wall_to_left      = false,
             m_is_to_wall_jump_left = false;

        bool m_is_down_thrusting = false,                
             m_is_sliding_ground = false,
             m_is_sliding_wall   = false,
             m_is_on_ledge       = false,
             m_is_climbing_ledge = false,
             m_is_hovering       = false;

        Vec2F m_ground_velocity_limit = { 2.0F, 4.0F };
        Vec2F m_swing_velocity_limit  = { 4.0F, 5.0F };

        Vec2F m_slide_ground_velocity_limit = { 1.4F, 0.0F };

        U8 m_time_left_in_state = 0;

        Vec2F m_start_sprite_offset = { 0.0F, 0.0F };
                
    public:
        Player();
        ~Player();

        bool load_config(const std::filesystem::path& path) override;
        U8 id = 0;
                
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        input::Key key_up     = input::Key::UP,
                   key_down   = input::Key::DOWN,
                   key_left   = input::Key::LEFT,
                   key_right  = input::Key::RIGHT,
                   key_jump   = input::Key::X,
                   key_melee  = input::Key::C,
                   key_sprint = input::Key::Z;

        bool is_pressed(input::Key k) { return input::is_pressed(m_input_id, k); }
        bool is_locked(input::Key k)  { return input::is_locked(m_input_id, k);  }
        void press(input::Key k)      { input::press(m_input_id, k);             }
        void release(input::Key k)    { input::release(m_input_id, k);           }
        void lock(input::Key k)       { input::lock(m_input_id, k);              }
        void unlock(input::Key k)     { input::unlock(m_input_id, k);            }
                
        start::cType next_level() const { return m_next_level; }
        start::cInfo next_start() const { return m_next_start; }

        bool is_to_write_save() const { return m_is_to_write_save; }    void is_to_write_save(bool is) { m_is_to_write_save = is; }

        void set_layer(cU8 layer) { sprite::layer(m_sprite_id, layer); }

        cI32 get_transform_id() const { return m_transform_id; }
        cI32 get_sprite_id()    const { return m_sprite_id;    }

        cRectI source_rect() const { return sprite::source_rect(m_sprite_id); }

        bool hurt(entity::Object* culprit) override;
        void interact(entity::Object* object) override;

        void spawn_down_thrust(cVec2F position);

        void update() override;
        void dead();
        void run();
        void swim();
        void swing();
    };
}