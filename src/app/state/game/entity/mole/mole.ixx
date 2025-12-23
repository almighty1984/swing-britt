module;
#include <sstream>
export module entity.mole;

import anim;
import camera;
import console;
import collider.aabb;
import entity;
import health;
import line;
import particle;
import random;
import sound;
import sprite;
import types;

export namespace entity {
    class Mole : public Object {
        U16   m_time_to_turn = 40;

        Vec2F m_sensed_position;
    public:
        Mole() {
            m_state = m_next_state = m_start_state = entity::State::IDLE;
        }
        bool hurt(entity::Object* culprit) override {
            if (!culprit) return false;
            
            if (m_time_left_hurt > 0) {
                return false;
            }
            m_time_left_hurt = m_time_to_hurt;

            if (culprit->type() == entity::Type::BRICK) {
                health::add_amount(m_health_id, -health::max(m_health_id));
                m_next_state = entity::State::ENTER;
            }
            else if (culprit->type() == entity::Type::PARTICLE_BRICK) {
                health::add_amount(m_health_id, -1.0f);
                m_next_state = entity::State::ENTER;
            }
            else if (culprit->type() == entity::Type::PARTICLE_DOWN_THRUST) {                
                if (culprit->parent()) {
                    F32 vel_x = (position().x - culprit->parent()->position().x) / 16.0F;
                    if      (vel_x < -1.5F) vel_x = -1.5F;
                    else if (vel_x >  1.5F) vel_x =  1.5F;
                    console::log("entity::Mole::hurt() vel_x: ", vel_x, "\n");

                    velocity_x(vel_x);
                }

                
                health::add_amount(m_health_id, -1.0f);
                m_next_state = State::JUMP;
            }
            else if (culprit->type() == entity::Type::PARTICLE_MELEE) {
                if (m_state == State::JUMP) {
                    if (culprit->parent()) {
                        if (sprite::is_leftward(culprit->parent()->sprite_id())) {
                            velocity_x(-5.0F);
                        } else {
                            velocity_x( 5.0F);
                        }
                    }
                    //velocity_y(-1.0F);
                }
                health::add_amount(m_health_id, -16.0f);
            }
            else {
                health::add_amount(m_health_id, -8.0f);
                m_next_state = entity::State::ENTER;
            }
            sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));

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
            if (health::amount(m_health_id) > 0.0F && m_time_left_to_spawn_sense > 0) {
                --m_time_left_to_spawn_sense;
                if (m_time_left_to_spawn_sense == 0) {
                    m_time_left_to_spawn_sense = m_time_to_spawn_sense;

                    spawn_fan(170.0F, 370.0F, 32, particle::Type::SENSE, position() + Vec2F{ 4.0F, 0.0F }, velocity(), 6.0F);

                    if (m_state == State::IDLE) {
                        particle::spawn({ .parent = this,
                                          .type = particle::Type::DUST,
                                          .position = position() + Vec2F{4.0F, 10.0F},
                                          .velocity = Vec2F{ 0.0F, 0.0F } });
                    }
                }
            }
            //console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");

            if (m_time_left_hurt > 0) --m_time_left_hurt;

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            switch (m_state) {
                case entity::State::DEAD:  dead();  break;
                case entity::State::ENTER: enter(); break;
                case entity::State::EXIT:  exit();  break;
                case entity::State::IDLE:  idle();  break;
                case entity::State::JUMP:  jump();  break;
                case entity::State::SHOOT: shoot(); break;
                default:                            break;
            }

            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));

            if (!is_dead()) {
                add_velocity_y(acceleration().y);
            }
            m_is_near_wall_L = m_is_near_wall_R = false;
            if (!m_is_on_slope && (velocity().y < 0.0F || velocity().y > acceleration().y)) {
                m_is_on_ground = false;
            }
            if (health::amount(m_health_id) <= 0.0f) {
                m_next_state = entity::State::DEAD;
            }
            health::layer(m_health_id, m_start_layer);
        }
        void dead() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_dead = m_time_to_be_dead;
                reset_anim(m_dead_anim_id);

                sprite::is_hidden(m_sprite_id, true);
                
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, false);
                }
                spawn_fan(0.0F, 360.0F, 8, particle::Type::DROP_BLOOD, position() - Vec2F{ 16.0F, 8.0F }, velocity(), 3.0F);

                spawn(particle::Type::HEALTH, position() + Vec2F{ 0.0F, -4.0F}, {});

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
                    console::log("entity::Mole::dead done being dead\n");
                }
            }
            velocity({});
            moved_velocity({});
        }
        void enter() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                reset_anim(m_enter_anim_id);
                m_time_in_state = 0;
            }
            //console::log("entity::Mole::enter()\n");
            velocity_x(0.0F);
            if (anim::is_last_frame(m_enter_anim_id)) {
                m_next_state = entity::State::IDLE;
            }
        }
        void exit() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                reset_anim(m_exit_anim_id);
            }
            //console::log("entity::Mole::exit()\n");
            velocity_x(0.0F);
            if (anim::is_last_frame(m_exit_anim_id)) {
                m_next_state = entity::State::SHOOT;
            }
        }
        void idle() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                sprite::rotation(m_sprite_id, 0.0F);   
                max_velocity(start_max_velocity());
                m_time_left_alive = U16_MAX;
                m_time_left_to_spawn_sense = m_time_to_spawn_sense;
                reset_anim(m_idle_anim_id);
                sprite::is_hidden(m_sprite_id, false);
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, true);
                }
                if (m_prev_state == entity::State::DEAD) {
                    health::reset(m_health_id);
                    sprite::is_leftward(m_sprite_id, random::number(0, 1) ? true : false);
                }
            }            

            ++m_time_in_state;
            if (m_time_in_state > m_time_to_turn) {
                m_time_in_state = 0;
                m_time_to_turn = 40 + random::number(0, 20);
                sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));
            }

            if (sprite::is_leftward(m_sprite_id)) {
                add_velocity_x(-acceleration().x);
            } else {
                add_velocity_x(acceleration().x);
            }
            
            for (auto& i : m_sensed_objects) {
                if (!(i->velocity().x > -0.2F && i->velocity().x < 0.2F && i->velocity().y > -1.0F && i->velocity().y < 1.0F) &&
                    (i->type() == Type::PLAYER || i->type() == Type::BRICK && i->is_tossed()) //||
                    //i->type() == Type::PARTICLE_BRICK
                    ) {
                    //m_sensed_position = i->position() + Vec2F{ sprite::source_rect(i->sprite_id()).w, sprite::source_rect(i->sprite_id()).h };
                    m_sensed_position = i->position();
                    if (m_time_left_to_react == 0) {
                        if (i->type() == Type::BRICK) {
                            m_time_left_to_react = 1;
                        } else {
                            m_time_left_to_react = random::number(10, 40);
                        }
                        console::log("entity::Mole::idle() sensed: ", to_string(i->type()), " time left to react : ", m_time_left_to_react, "\n");
                    }                    
                }
            }
            

            if (m_time_left_to_react > 0) {
                //console::log("entity::Mole::idle() time left to react: ", m_time_left_to_react, "\n");
                --m_time_left_to_react;
                F32 slowdown = 1.0F - (1.0F / m_time_left_to_react);
                if (slowdown < 0.0F) {
                    slowdown = 0.0F;
                }
                //console::log("entity::Mole::idle() slowdown: ", slowdown, "\n");                
                
                velocity_x(velocity().x * slowdown);
                if (m_time_left_to_react == 0) {
                    m_next_state = entity::State::EXIT;
                }
            }
            
        }
        void jump() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;                
                reset_anim(m_jump_anim_id);
                sprite::is_hidden(m_sprite_id, false);
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, true);
                }
                //velocity_x(0.0F);
                velocity_y(-4.0F);
                m_is_on_ground = false;
                max_velocity({ 10.0F, 10.0F });
            }

            F32 radians = std::atan2(velocity().y, velocity().x);
            if (radians < 0.0F) radians += 3.1415926535f * 2.0F;

            cF32 degrees = radians * 180.0F / 3.1415926535F;

            sprite::rotation(m_sprite_id, degrees + 90.0F);
            if (m_is_on_ground) {                
                m_next_state = State::IDLE;
            }
        }
        void shoot() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_in_state = 40;
                m_sensed_objects.clear();

                sprite::is_leftward(m_sprite_id, m_sensed_position.x < position().x + 4.0F);

                reset_anim(m_shoot_anim_id);

                sound::position(m_shoot_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_shoot_sound_id);

                cVec2F start_position = position() + Vec2F{ 4.0F, -2.0F };
                cVec2F end_position = m_sensed_position + Vec2F{ 8.0F, 8.0F };
                cVec2F v = (end_position - start_position);

                Vec2F shot_velocity = v / line::length(v) * 8.0F;
                if (shot_velocity.y > -3.0F) {
                    shot_velocity.y = -3.0F;
                }
                if (shot_velocity.x < 0.0F && shot_velocity.x > -2.0F) {
                    shot_velocity.x = -2.0F;
                } else if (shot_velocity.x > 0.0F && shot_velocity.x < 2.0F) {
                    shot_velocity.x = 2.0F;
                }
                particle::spawn({ .parent = this,
                                           .type = particle::Type::SHOT,
                                           .position = start_position,
                                           .velocity = shot_velocity });
            }
            velocity_x(0.0F);

            //console::log("entity::Mole::shoot() time left in state: ", m_time_left_in_state, "\n");

            if (m_time_left_in_state > 0) {
                --m_time_left_in_state;
                if (m_time_left_in_state == 0) {
                    m_next_state = entity::State::ENTER;
                }
            }
        }
    };
}