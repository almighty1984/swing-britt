module;
#include <filesystem>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>

export module entity;
import anim;
import camera;
//import collider.aabb;
import console;
import health;
import sound;
import sprite;
import transform;
import types;
import window;

namespace entity {
    class Object;
}
export namespace aabb {
    struct Info {
        entity::Object* owner = nullptr;
        I32     id = -1;
        //RectF   rect = { 0.0F, 0.0F, 0.0F, 0.0F };
    };
    using cInfo = const Info;
}

export namespace entity {    
    class Object {
    protected:
        Object* m_parent = nullptr;

        Type m_type = Type::NONE;
        U8 m_number = 0;

        State m_state       = State::NONE,
              m_next_state  = State::NONE,
              m_prev_state  = State::NONE,
              m_start_state = State::NONE;

        I32 m_transform_id = -1,
            m_sprite_id    = -1;

        I32 m_health_id = -1;

        I32 m_current_anim_id = -1,
            m_bounce_anim_id  = -1,
            m_dead_anim_id    = -1,
            m_enter_anim_id   = -1,
            m_exit_anim_id    = -1,
            m_hurt_anim_id    = -1,
            m_idle_anim_id    = -1,
            m_jump_anim_id    = -1,
            m_melee_anim_id   = -1,
            m_shoot_anim_id   = -1,
            m_stunned_anim_id = -1,
            m_upended_anim_id = -1;

        I32 m_block_sound_id       = -1,
            m_bounce_sound_id      = -1,
            m_bump_head_sound_id   = -1,
            m_dead_sound_id        = -1,
            m_hurt_sound_id        = -1,
            m_melee_sound_id       = -1,
            m_shoot_sound_id       = -1,
            m_water_enter_sound_id = -1,
            m_water_exit_sound_id  = -1;

        std::vector<I32> m_aabb_ids;

        std::list<Object*> m_sensed_objects;

        U16 m_time_left_alive          =  U16_MAX, m_time_to_be_alive    = U16_MAX,
            m_time_left_blocked        =  0,
            m_time_left_dead           =  0,       m_time_to_be_dead     = 0,
            m_time_left_hurt           =  0,       m_time_to_hurt        = 10,
            m_time_left_in_state       =  0,
            m_time_in_state            =  0,
            m_time_to_be_in_state      =  0,
            m_time_left_interacting    =  0,       m_time_to_interact    = 10,
            m_time_left_to_spawn_sense =  0,       m_time_to_spawn_sense = 10,
            m_time_left_to_react       =  0;

        Vec2F m_start_offset      = { 0.0F, 0.0F };

        U8 m_num_jumps   = 0,
           m_start_layer = 0;        

        Vec2F m_direction = { 0.0F, 0.0F };

        bool m_is_first_update       = true,
             m_is_first_state_update = true;

        bool m_is_carrying    = false,
             m_is_ducking     = false,
             m_is_near_wall_L = false,
             m_is_near_wall_R = false,
             m_is_on_ground   = false,
             m_is_on_slope    = false,
             m_is_powered     = false;

        U16 m_input_limit = U16_MAX;

        F32 m_water_line_y = 0.0F;
        F32 m_rotation_speed = 1.0F;
    public:
        RectI start_source_rect = { 0, 0, 16, 16 };
        Object* parent() const { return m_parent; } void parent(Object* p) { m_parent = p; }

        virtual ~Object();

        virtual bool load_config(const std::filesystem::path& path);
        virtual void collide_x(aabb::cInfo our, aabb::cInfo other) {}
        virtual void collide_y(aabb::cInfo our, aabb::cInfo other) {}

        virtual bool hurt(Object* culprit) { return false; }
        virtual void interact(Object* object) {}
        virtual void update() {
            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));

            if (m_time_left_dead > 0) {
                //console::log("m_time_left_dead: ", (int)m_time_left_dead, "\n");
                if (m_time_left_dead != U16_MAX) {
                    --m_time_left_dead;
                }
                if (m_current_anim_id != m_dead_anim_id) {
                    m_current_anim_id = m_dead_anim_id;
                    anim::source_x(m_dead_anim_id, anim::first_frame(m_dead_anim_id));                    
                }
            } else {
                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
                if (m_current_anim_id != m_idle_anim_id) {
                    m_current_anim_id = m_idle_anim_id;
                    
                    anim::source_x(m_idle_anim_id, anim::first_frame(m_idle_anim_id));                    
                }
            }
        }
        virtual void draw(std::unique_ptr<Window>& window) {
            sprite::draw(window, m_sprite_id);
        }
        virtual void draw_debug(std::unique_ptr<Window>& window);

        bool add_input_object(Object* object) {
            if (input_objects.size() >= m_input_limit) {
                return false;
            }
            if (std::find(input_objects.begin(), input_objects.end(), object) != input_objects.end()) {
                return false;
            }
            if (object->is_powered()) {
                m_is_powered = true;
            }
            input_objects.emplace_back(object);
            return true;
        }
        std::vector<Object*> input_objects;

        U16 num_inputs() const { return input_objects.size(); }

        bool add_sensed_object(Object* object) {            
            if (std::find(m_sensed_objects.begin(), m_sensed_objects.end(), object) != m_sensed_objects.end()) {
                return false;
            }
            m_sensed_objects.emplace_back(object);
            return true;
        }

        bool is_blocked()     const { return m_time_left_blocked > 0; }
        bool is_carrying()    const { return m_is_carrying; }               void is_carrying(bool is) { m_is_carrying = is; }
        bool is_dead()        const { return m_time_left_dead > 0 && m_time_left_alive == 0; }
        bool is_ducking()     const { return m_is_ducking; }
        bool is_hurting()     const { return m_time_left_hurt > 0; }
        bool is_near_wall_L() const { return m_is_near_wall_L; }
        bool is_near_wall_R() const { return m_is_near_wall_R; }
        bool is_on_ground()   const { return m_is_on_ground; }              void is_on_ground(bool is) { m_is_on_ground = is; }
        bool is_interacting() const { return m_time_left_interacting > 0; }
        bool is_powered()     const { return m_is_powered; }                void is_powered(bool is) { m_is_powered = is; }
        bool is_tossed()      const { return m_state == State::TOSSED; }
        
        Type  type()   const { return m_type;   } void type(cType t)   { m_type = t;   }
        U8    number() const { return m_number; } void number(cU8 n)   { m_number = n; }
        State state()  const { return m_state;  } void state(cState s) { m_state = s;  }

        Vec2F position() const { return transform::position(m_transform_id); }
        void position(cVec2F p) { transform::position(m_transform_id, p); }
        void position_x(cF32 x) { transform::position_x(m_transform_id, x); }
        void position_y(cF32 y) { transform::position_y(m_transform_id, y); }
        void add_position(cVec2F v) { transform::add_position(m_transform_id, v); }
        void add_position_x(cF32 x) { transform::add_position_x(m_transform_id, x); }
        void add_position_y(cF32 y) { transform::add_position_y(m_transform_id, y); }

        Vec2F start_position() const { return transform::start_position(m_transform_id); }
        void start_position(cVec2F p) { transform::start_position(m_transform_id, p); }

        Vec2F velocity() const { return transform::velocity(m_transform_id);     }
        void velocity(cVec2F v)     { transform::velocity(m_transform_id, v);     }
        
        void velocity_x(cF32 x) { transform::velocity_x(m_transform_id, x); }
        void velocity_y(cF32 y) { transform::velocity_y(m_transform_id, y); }

        void add_velocity(cVec2F v) { transform::add_velocity(m_transform_id, v); }
        void add_velocity_x(cF32 x) { transform::add_velocity_x(m_transform_id, x); }
        void add_velocity_y(cF32 y) { transform::add_velocity_y(m_transform_id, y); }

        Vec2F moved_velocity() const      { return transform::moved_velocity(m_transform_id);   }
        void moved_velocity(cVec2F v)     { transform::moved_velocity(m_transform_id, v);       }
        void moved_velocity_x(cF32 x)     { transform::moved_velocity_x(m_transform_id, x);     }
        void moved_velocity_y(cF32 y)     { transform::moved_velocity_y(m_transform_id, y);     }
        void add_moved_velocity(cVec2F v) { transform::add_moved_velocity(m_transform_id, v);   }
        void add_moved_velocity_x(cF32 x) { transform::add_moved_velocity_x(m_transform_id, x); }
        void add_moved_velocity_y(cF32 y) { transform::add_moved_velocity_y(m_transform_id, y); }

        Vec2F acceleration() const { return transform::acceleration(m_transform_id); }
        void acceleration(cVec2F a) { transform::acceleration(m_transform_id, a); }
        void acceleration_x(cF32 a) { transform::acceleration_x(m_transform_id, a); }
        void acceleration_y(cF32 a) { transform::acceleration_y(m_transform_id, a); }

        Vec2F deceleration() const { return transform::deceleration(m_transform_id); }
        void deceleration(cVec2F d) { transform::deceleration(m_transform_id, d); }
        void deceleration_x(cF32 a) { transform::deceleration_x(m_transform_id, a); }
        void deceleration_y(cF32 a) { transform::deceleration_y(m_transform_id, a); }
        Vec2F max_velocity() const { return transform::max_velocity(m_transform_id); }
        void max_velocity(cVec2F m) { transform::max_velocity(m_transform_id, m); }
        void max_velocity_x(cF32 x) { transform::max_velocity_x(m_transform_id, x); }
        void max_velocity_y(cF32 y) { transform::max_velocity_y(m_transform_id, y); }

        Vec2F start_max_velocity() const { return transform::start_max_velocity(m_transform_id); }
        void start_max_velocity(cVec2F m) { transform::start_max_velocity(m_transform_id, m); }
        void start_max_velocity_x(cF32 x) { transform::start_max_velocity_x(m_transform_id, x); }
        void start_max_velocity_y(cF32 y) { transform::start_max_velocity_y(m_transform_id, y); }

        Vec2F position_on_level() const { return camera::position + position(); }

        F32 rotation_speed() const { return m_rotation_speed; }

        Vec2F direction() const { return m_direction; }    void direction(cVec2F d) { m_direction = d; }

        Vec2F start_offset() const { return m_start_offset; } void start_offset(cVec2F offset) { m_start_offset = offset; }
        U8    start_layer()  const { return m_start_layer;  } void start_layer(cU8 layer)      { m_start_layer = layer;   }

        U16 time_left_dead()   const { return m_time_left_dead;   } void time_left_dead(cU16 t)   { m_time_left_dead   = t; }
        U16 time_left_alive()  const { return m_time_left_alive;  } void time_left_alive(cU16 t)  { m_time_left_alive  = t; }
        U16 time_to_be_dead()  const { return m_time_to_be_dead;  } void time_to_be_dead(cU16 t)  { m_time_to_be_dead  = t; }
        U16 time_to_be_alive() const { return m_time_to_be_alive; } void time_to_be_alive(cU16 t) { m_time_to_be_alive = t; }

        U16 time_left_hurt() const { return m_time_left_hurt; }    void time_left_hurt(cU16 t) { m_time_left_hurt = t; }
        U16 time_to_hurt() const { return m_time_to_hurt; }

        U16 time_left_interacting() const { return m_time_left_interacting; } void time_left_interacting(cU16 t) { m_time_left_interacting = t; }
        U16 time_left_in_State()    const { return m_time_left_in_state;    } void time_left_in_state(cU16 t)    { m_time_left_in_state = t;    }

        I32 current_anim_id() { return m_current_anim_id; }

        void next_state(cState state) { m_next_state = state; }
        State start_state() { return m_start_state; }

        void set_anim(cI32 anim_id) {
            if (m_current_anim_id == anim_id || anim_id == -1) return;
            m_current_anim_id = anim_id;
        }
        void reset_anim(cI32 anim_id) {
            m_current_anim_id = anim_id;            
            anim::loop(anim_id, 0);
            //console::log("entity::reset_anim", " \n");
            //anim::source_x(anim_id, anim::is_reverse(anim_id) ? anim::texture_w(anim_id) - anim::source_w(anim_id) : 0);
            anim::source_x(anim_id, anim::first_frame(anim_id) * anim::source(anim_id).w);
        }

        cI32 sprite_id()    const { return m_sprite_id;    }    void sprite_id(cI32 id)    { m_sprite_id    = id; }
        cI32 transform_id() const { return m_transform_id; }    void transform_id(cI32 id) { m_transform_id = id; }
        cI32 health_id()    const { return m_health_id;    }    void health_id(cI32 id)    { m_health_id    = id; }
        
        std::vector<I32>& aabb_ids() { return m_aabb_ids; }

        void set_layer(cU8 layer) { sprite::layer(m_sprite_id, layer); }

        cRectI source_rect() const { return sprite::source_rect(m_sprite_id); }

        cU8 num_jumps() const { return m_num_jumps; }

        void spawn(particle::cType type, cVec2F position, cVec2F velocity);        
        void spawn_fan(cF32 start_angle, cF32 end_angle, cU8 num, particle::cType type, cVec2F position, cVec2F velocity, cF32 speed);

        bool is_in_view() { return position().x + sprite::source_rect(m_sprite_id).w > 0.0F && position().x < WINDOW_W &&
                                   position().y + sprite::source_rect(m_sprite_id).h > 0.0F && position().y < WINDOW_H; }

        bool is_start_in_view() { return start_position().x + sprite::source_rect(m_sprite_id).w > 0.0F && start_position().x < WINDOW_W &&
                                         start_position().y + sprite::source_rect(m_sprite_id).h > 0.0F && start_position().y < WINDOW_H; }

        //Vec2F sensed_position() const { return m_sensed_position; } void sensed_position(cVec2F p) { m_sensed_position = p; }
    };
}