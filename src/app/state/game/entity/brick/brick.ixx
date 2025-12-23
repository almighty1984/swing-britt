module;
#include <cmath>
#include <sstream>
export module entity.brick;
import anim;
import camera;
import console;
import collider.aabb;
import entity;
//import particle;
import sound;
import sprite;

export namespace entity {
    class Brick : public Object {
        Vec2F m_break_velocity = { 3.5F, 4.5F };
    public:
        Brick() {            
            m_state = m_next_state = m_start_state = entity::State::IDLE;
        }
        bool hurt(Object* culprit) override {
            if (!culprit) return false;
            console::log("entity::Brick::hurt() culprit: ", to_string(culprit->type()), "\n");
            m_next_state = entity::State::DEAD;
            
            sound::position(m_melee_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound::play(m_melee_sound_id);
            
            return true;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
            }
                        
            //console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");

            if (m_time_left_hurt > 0) --m_time_left_hurt;
            if (m_time_left_interacting > 0) --m_time_left_interacting;

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            switch (m_state) {
                case entity::State::CARRIED: carried(); break;
                case entity::State::DEAD:    dead();    break;
                case entity::State::IDLE:    idle();    break;
                case entity::State::TOSSED:  tossed();  break;
                default: break;
            }

            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));

            if (!is_dead()) {
                add_velocity_y(acceleration().y);
            }
            m_is_near_wall_L = m_is_near_wall_R = false;
            if (!m_is_on_slope && (velocity().y < 0.0F || velocity().y > acceleration().y)) {
                m_is_on_ground = false;
            }
        }
        void carried() {
            if (!m_parent) {
                m_next_state = entity::State::IDLE;
                return;
            }
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
            }
            m_is_on_ground = false;

            //console::log("entity::Brick::carried is near wall: ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");

            //console::log("entity::Brick::carried ", velocity().x, " ", velocity().y, "\n");

            velocity(m_parent->velocity());
            moved_velocity({});
            cF32 carry_offset_x = 10.0F;
            if (sprite::is_leftward(m_parent->sprite_id())) {
                if (position().x < m_parent->position().x - carry_offset_x) {
                    position_x(m_parent->position().x - carry_offset_x);
                }
                if (position().x > m_parent->position().x - carry_offset_x) {
                    cF32 diff_x = (m_parent->position().x - carry_offset_x) - position().x;
                    add_velocity_x(diff_x / 4.0F);
                } else {
                    position_x(m_parent->position().x - carry_offset_x);
                }
            } else {
                if (position().x > m_parent->position().x + carry_offset_x) {
                    position_x(m_parent->position().x + carry_offset_x);
                }

                if (position().x < m_parent->position().x + carry_offset_x) {
                    cF32 diff_x = (m_parent->position().x + carry_offset_x) - position().x;
                    add_velocity_x(diff_x / 4.0F);
                } else {
                    position_x(m_parent->position().x + carry_offset_x);
                }
            }

            sprite::is_leftward(m_sprite_id, position().x + 16.0F < m_parent->position().x + 8.0F);

            position_y(m_parent->position().y + 4.0F);
            /*if (m_parent->is_ducking()) {
                add_position_y(4.0F);
            }*/
            if (m_parent->is_ducking() || !m_parent->is_carrying()) {
                m_parent->is_carrying(false);
                velocity({ m_parent->velocity().x, velocity().y });
                if (sprite::is_leftward(m_sprite_id)) {
                    add_velocity_x(-0.5F);
                    add_position_x(-8.0F);
                } else {
                    add_velocity_x(0.5F);
                    add_position_x(8.0F);
                }
                velocity_y(-1.0F);
                m_parent = nullptr;
                m_is_first_state_update = true;
                m_next_state = entity::State::IDLE;
            }
        }
        void dead() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_dead = m_time_to_be_dead;
                m_time_left_alive = 0;
                console::log("entity::Brick::dead()\n");
                if (m_parent) {
                    m_parent->is_carrying(false);
                    m_parent = nullptr;
                }
                reset_anim(m_dead_anim_id);

                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, false);
                }
                sprite::is_hidden(m_sprite_id, true);
                sound::position(m_dead_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_dead_sound_id);
                spawn(particle::Type::HIT, position(), {});

                console::log("entity::Brick::dead velocity.x ", velocity().x, "\n");
                spawn_fan(0.0F, 360.0F, 8, particle::Type::BRICK, position() + Vec2F{ 6.0F, -4.0F }, velocity() / 4.0F, 2.0F);
            }
            velocity({});
            moved_velocity({});

            set_anim(m_dead_anim_id);

            //console::log("entity::Brick::dead() start position: ", start_position().x, " ", start_position().y, "\n");

            if (m_time_left_dead > 0 && m_time_to_be_dead != U16_MAX) {
                --m_time_left_dead;
                if (m_time_left_dead == 0) {
                    console::log("entity::Brick::dead done being dead\n");
                }
            }

        }
        void idle() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_alive = U16_MAX;
                reset_anim(m_idle_anim_id);
                m_parent = nullptr;

                sprite::is_hidden(m_sprite_id, false);
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, true);
                }
            }
            if (m_is_on_ground) {
                deceleration( {0.2F, 0.0F} );
            }
        }
        void tossed() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_is_on_ground = false;

                deceleration( {0.0F, 0.0F} );
            }
            if (m_is_on_ground) {
                deceleration( {0.1F, 0.0F} );
                m_next_state = entity::State::IDLE;
            } else {
                //console::log("entity::Brick toss not on ground\n");
            }
        }
    };
}