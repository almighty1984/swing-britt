module;
#include <sstream>

export module entity.bug;
import anim;
import console;
import entity;
import collider.aabb;
import health;
import sound;
import transform;
import random;
import sprite;

export namespace entity {
    class Bug : public Object {
        //U8 m_time_left_turning = 0;
    public:
        Bug() {
            m_state = m_next_state = m_start_state = entity::State::WALK;
            m_time_to_interact = 15;
        }        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
        bool hurt(entity::Object* culprit) override {
            if (!culprit) return false;
            //m_state = entity::State::UPENDED;
            m_next_state = entity::State::HURT;
                        
            if (m_time_left_hurt > 0) {
                return false;
            }
            m_time_left_hurt = m_time_to_hurt;

            if (culprit->type() == entity::Type::PARTICLE_MELEE) {
                health::add_amount(m_health_id, -16.0f);
            } else {
                health::add_amount(m_health_id, -8.0f);
            }
            sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));

            sound::position(m_melee_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound::play(m_melee_sound_id);            

            return true;
        }

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
            }

            //console::log("entity::Bug::update state: ", entity::to_string(m_state), "\n");
            
            

            if (!m_is_on_slope && (velocity().y < 0.0F || velocity().y > acceleration().y)) {
                m_is_on_ground = false;
            }

            if (m_state == entity::State::UPENDED) {
                //console::log("entity::Bug::update is_near_wall: ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");
            }
            if (velocity().x <= 0.0F) {
                m_is_near_wall_R = false;
            }
            if (velocity().x >= 0.0F) {
                m_is_near_wall_L = false;
            }
            //console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");

            if (m_time_left_hurt        > 0) --m_time_left_hurt;
            //if (m_time_left_turning     > 0) --m_time_left_turning;
            if (m_time_left_interacting > 0) --m_time_left_interacting;

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            if (!is_dead()) {
                add_velocity({ 0.0F,acceleration().y });
            }
            switch (m_state) {
                case entity::State::BOUNCE:  bounce();  break;
                case entity::State::CARRIED: carried(); break;
                case entity::State::DEAD:    dead();    break;
                case entity::State::HURT:    hurt();    break;
                case entity::State::SWIM:    swim();    break;
                case entity::State::TOSSED:  tossed();  break;
                case entity::State::UPENDED: upended(); break;
                case entity::State::WALK:    walk();    break;
                default: break;
            }

            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));

            if (health::amount(m_health_id) <= 0.0f) {
                m_next_state = entity::State::DEAD;
            }
            health::layer(m_health_id, m_start_layer);
        }
        void bounce() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                sound::position(m_bounce_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_bounce_sound_id);
                
                reset_anim(m_bounce_anim_id);
            }
            //int num_frames = anim()->texture_size.x / anim()->source.w;
            //console::log("num_frames: ", anim()->current_frame(), "\n");
            if (anim::current_frame(m_current_anim_id) + 1 == anim::num_frames(m_current_anim_id)) {
                m_next_state = entity::State::UPENDED;
            }            
        }
        void carried() {
            if (!m_parent) {
                m_next_state = entity::State::WALK;
                return;
            }
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
            }
            m_is_on_ground = false;

            console::log("entity::Bug::carried ", velocity().x, " ", velocity().y, "\n");

            
            velocity(m_parent->velocity());
            moved_velocity({});
            if (sprite::is_leftward(m_parent->sprite_id())) {
                if (position().x < m_parent->position().x - 12.0F) {
                    position_x(m_parent->position().x - 12.0F);
                }
                if (position().x > m_parent->position().x - 12.0F) {
                    cF32 diff_x = (m_parent->position().x - 12.0F) - position().x;
                    add_velocity_x(diff_x / 4.0F);
                } else {
                    position_x(m_parent->position().x - 12.0F);
                }
            } else {
                if (position().x > m_parent->position().x + 12.0F) {
                    position_x(m_parent->position().x + 12.0F);
                }

                if (position().x < m_parent->position().x + 12.0F) {
                    cF32 diff_x = (m_parent->position().x + 12.0F) - position().x;
                    add_velocity_x(diff_x / 4.0F);
                } else {
                    position_x(m_parent->position().x + 12.0F);
                }
            }

            sprite::is_leftward(m_sprite_id, position().x + 16.0F < m_parent->position().x + 8.0F);

            position_y(m_parent->position().y + 6.0F);
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
                velocity({ velocity().x, -1.0F });

                
                m_next_state = m_parent->is_on_ground() ? entity::State::WALK : entity::State::UPENDED;
                

                m_parent = nullptr;
                m_is_first_state_update = true;                
            }
        }
        void dead() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_dead = m_time_to_be_dead;
                reset_anim(m_dead_anim_id);
                //console::log("prev state: ", entity::to_string(m_prev_state), "\n");
                if (m_prev_state == entity::State::UPENDED || m_prev_state == entity::State::BOUNCE) {
                    sprite::is_upended(m_sprite_id, true);
                }
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, false);
                }
                spawn_fan(0.0F, 360.0F, 8, particle::Type::DROP_BLOOD, position() - Vec2F{ 16.0F, 8.0F }, velocity(), 3.0F);

                //particle::spawn({ this, particle::Type::HEALTH, position() , {} });
                spawn(particle::Type::HEALTH, position(), {});

                sound::position(m_dead_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_dead_sound_id);
                
                if (m_parent) {
                    m_parent->is_carrying(false);
                    m_parent = nullptr;
                }
            }
            if (m_time_left_dead > 0 && m_time_to_be_dead != U16_MAX) {
                --m_time_left_dead;
                if (m_time_left_dead == 0) {
                    console::log("entity::Bug::dead done being dead\n");
                }
            }
            velocity( {} );
            moved_velocity( {} );
        }        
        void hurt() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;                
                reset_anim(m_hurt_anim_id);
                m_parent = nullptr;
            }
            //console::log("entity::Bug::hurt()\n");
            deceleration({});
            set_anim(m_hurt_anim_id);
            if (anim::is_last_frame(m_current_anim_id)) {
                m_next_state = entity::State::UPENDED;
            }            
        }
        void swim() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_sensed_objects.clear();
                m_is_on_ground = false;
                //reset_anim(m_swim_anim_id);
            }            
            set_anim(m_upended_anim_id);
            acceleration({ });
            deceleration({ 0.1F, 0.1F });

            F32 diff_y = m_water_line_y - position_on_level().y;

            console::log("entity::Bug::swim() water line: ", m_water_line_y, " diff y: ", diff_y, "\n");

            add_velocity_y( diff_y / 10.0F );
        }
        void tossed() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_is_on_ground = false;

                deceleration( {} );
            }
            if (m_is_on_ground) {
                console::log("entity::Bug::tossed, on ground\n");
                //m_next_state = entity::State::WALK;
                //m_next_state = entity::State::UPENDED;
                m_next_state = m_prev_state;
            } else {
                //console::log("entity::Bug toss not on ground\n");
            }
        }
        void upended() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
            }
            set_anim(m_upended_anim_id);

            if (m_is_on_ground) {
                deceleration( {0.2F, 0.0F} );
            } else {
                deceleration( {0.0F, 0.0F} );
            }

            if (m_time_left_in_state > 0) {
                --m_time_left_in_state;
                if (m_time_left_in_state == 0) {
                    m_next_state = entity::State::WALK;
                }
            }
        }
        void walk() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                sprite::is_hidden(m_sprite_id, false);
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, true);
                }
                if (m_prev_state == entity::State::DEAD) {
                    health::reset(m_health_id);
                    sprite::is_leftward(m_sprite_id, random::number(0, 1) ? true : false);
                }
            }
            deceleration({});

            if (m_is_on_ground) {
                //console::log("entity::Bug walk on ground\n");
                moved_velocity({});
                velocity({ sprite::is_leftward(m_sprite_id) ? -0.5F : 0.5F, velocity().y });
            } else {
                //console::log("entity::Bug walk not on ground\n");
            }
            m_current_anim_id = m_idle_anim_id;
        }
    };
}