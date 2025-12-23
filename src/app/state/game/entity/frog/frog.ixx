module;
#include <cmath>
#include <sstream>

export module entity.frog;
import anim;
import console;
import entity;
import collider.aabb;
import health;
import line;
import particle;
import random;
import sound;
import transform;
import sprite;

export namespace entity {
    class Frog : public Object {
        I32 m_tounge_line_id = -1;

        Vec2F m_tounge_start,
              m_tounge_end,
              m_sensed_position;

        State m_sensed_state;        
    public:
        Frog() {            
            m_state = m_next_state = m_start_state = State::IDLE;
            m_tounge_line_id = line::make(m_tounge_start, m_tounge_end);
            line::is_hidden(m_tounge_line_id, true);
        }
        ~Frog() {            
            line::erase(m_tounge_line_id);
        }        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
        bool hurt(Object* culprit) override {
            if (m_time_left_hurt > 0) return false;
            m_time_left_hurt = m_time_to_hurt;

            if (!culprit) return false;

            if (culprit->type() == Type::BRICK || culprit->type() == Type::BUG) {
                m_sensed_objects.clear();
                m_sensed_objects.emplace_back(culprit);
                
                //velocity() = culprit->velocity() * 0.25F;
                cF32 amount = std::abs(culprit->velocity().x) * 4.0F;
                health::add_amount(m_health_id, -amount);
                //health::get(m_health_id)->amount -= 32.0F;
                m_next_state = State::HURT;
            } else if (culprit->type() == Type::PARTICLE_BRICK) {
                m_sensed_objects.clear();
                m_sensed_objects.emplace_back(culprit);

                cF32 amount = std::sqrtf(std::abs(culprit->velocity().x * culprit->velocity().y));
                //console::log("amount: ", amount, "\n");
                
                health::add_amount(m_health_id, -amount);
                m_next_state = State::HURT;                
            } else  if (culprit->type() == Type::PARTICLE_DOWN_THRUST) {
                velocity_x(culprit->velocity().x * 0.7F);
                if (m_is_on_ground) {
                    velocity_y(-2.0f);
                }
                m_next_state = State::STUNNED;
            } else if (culprit->type() == Type::PARTICLE_MELEE) {
                m_sensed_objects.clear();
                m_sensed_objects.emplace_back(culprit->parent());

                velocity(culprit->velocity() * 0.25F);
                health::add_amount(m_health_id, -8.0f);
                m_next_state = State::HURT;
            } else if (culprit->type() == Type::PLAYER) {
                if (culprit->state() == State::RUN) {
                    if (culprit->velocity().y >= 6.0F) {
                        velocity(culprit->velocity() * 0.5F);
                        m_next_state = State::STUNNED;
                        m_time_to_be_in_state = 100;

                        console::log("Frog::hurt() m_time_to_be_in_state: ", m_time_to_be_in_state, "\n");

                        sound::position(m_bounce_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                        sound::play(m_bounce_sound_id);
                        
                        spawn(particle::Type::HIT, position(), {});
                    }
                } else if (culprit->state() == State::SWING) {
                    m_sensed_objects.clear();
                    health::add_amount(m_health_id, -8.0f);
                    m_next_state = State::STUNNED;
                    m_time_to_be_in_state = 20;
                                        
                    sound::position(m_bump_head_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound::play(m_bump_head_sound_id);
                }
            }
            return true;
        }
        void update() override {
            if (m_time_left_blocked > 0) --m_time_left_blocked;
            if (m_time_left_hurt    > 0) --m_time_left_hurt;

            if (m_is_first_update) {
                m_is_first_update = false;
                sprite::is_leftward(m_sprite_id, random::number(0, 1) ? true : false);
            }

            if (velocity().x < -0.5F) {
                sprite::is_leftward(m_sprite_id, true);
            } else if (velocity().x > 0.5F) {
                sprite::is_leftward(m_sprite_id, false);
            }

            /*if (velocity().y < 0.0F && !m_is_on_slope) {
                m_is_on_ground = false;
            }*/

            deceleration_x(m_is_on_ground ? 0.2F : 0.0F);

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }
            switch (m_state) {
                case State::BLOCKED: blocked(); break;
                case State::DEAD:    dead();    break;
                case State::HURT:    hurt();    break;
                case State::IDLE:    idle();    break;
                case State::JUMP:    jump();    break;
                case State::MELEE:   melee();   break;
                case State::STUNNED: stunned(); break;
                case State::SWIM:    swim();    break;
                default:                        break;
            }

            //console::log("Frog::update() state: ", to_string(m_state), "\n");

            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));

            if (health::amount(m_health_id) <= 0.0f) {
                m_next_state = State::DEAD;
            }
            health::layer(m_health_id, m_start_layer);
        }
        void draw(std::unique_ptr<Window>& window) override {
            sprite::draw(window, m_sprite_id);
            line::draw(window, m_tounge_line_id);
        }        
        void blocked() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_sensed_objects.clear();
                reset_anim(m_hurt_anim_id);
                for (auto& i : m_aabb_ids) {
                    if (aabb::name(i) == aabb::Name::BODY) {
                        aabb::is_active(i, true);
                    } else {
                        aabb::is_active(i, false);
                    }
                }
                m_time_in_state = 0;
                transform::velocity(m_transform_id, { sprite::is_leftward(m_sprite_id) ? 1.0f : -1.0f, -1.5f });

                m_is_on_ground = false;

                sound::position(m_block_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_block_sound_id);
            }
            //console::log("Frog::blocked\n");

            set_anim(m_hurt_anim_id);
            m_tounge_start = transform::position(m_transform_id) + Vec2F{ 8.0F, 6.0F };
            m_tounge_end = m_sensed_position + Vec2F{ 8.0F, 8.0F };

            line::max_length(m_tounge_line_id, 32.0F - m_time_in_state * 4.0f);
            if (line::max_length(m_tounge_line_id) < 1.0f) {
                line::max_length(m_tounge_line_id, 1.0f);
            }

            line::set(m_tounge_line_id, m_tounge_start, m_tounge_end);

            transform::add_velocity(m_transform_id, { 0.0F, transform::acceleration(m_transform_id).y });

            ++m_time_in_state;
            if (m_is_on_ground && m_time_in_state > 100) {
                m_time_in_state = 0;
                m_next_state = State::IDLE;
            }
        }
        void dead() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_dead = m_time_to_be_dead;
                reset_anim(m_dead_anim_id);
                line::is_hidden(m_tounge_line_id, true);
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, false);
                }
                spawn_fan(0.0F, 360.0F, 8, particle::Type::DROP_BLOOD, position() + Vec2F{ 4.0F, 0.0F }, velocity(), 3.0F);
                spawn(particle::Type::HEALTH, position() + Vec2F{ 4.0F, 0.0F }, {});

                sound::position(m_dead_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_dead_sound_id);

                //m_next_state = State::none;
                //m_sensed_state = State::none;
                m_sensed_objects.clear();
            }
            
            //console::log("Frog::dead() next state: ", to_string(m_next_state), "\n");

            velocity(velocity() * 0.9F);

            //velocity().x *= 0.9F;
            //velocity().y *= 0.9F;
            moved_velocity({});

            if (m_time_left_dead > 0) {
                --m_time_left_dead;
                if (m_time_left_dead == 0) {
                    console::log("Frog::dead() done being dead\n");
                }
            }
        }
        void hurt() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;

reset_anim(m_hurt_anim_id);
for (auto& i : m_aabb_ids) {
    if (aabb::name(i) == aabb::Name::BODY) {
        aabb::is_active(i, true);
    } else {
        aabb::is_active(i, false);
    }
}
m_sensed_state = State::IDLE;
            }

            set_anim(m_hurt_anim_id);
            line::is_hidden(m_tounge_line_id, true);

            console::log("Frog::hurt\n");

            add_velocity({ 0.0F, acceleration().y });
            if (/*anim()->is_last_frame() &&*/ m_is_on_ground) {
                //console::log("is last frame: ", anim()->is_last_frame(), "\n");                

                if (!m_sensed_objects.empty() && m_time_left_to_react == 0) {
                    for (auto& i : m_sensed_objects) {
                        if (m_is_on_ground && (i->type() == Type::BRICK ||
                            i->type() == Type::PARTICLE_BRICK ||
                            i->type() == Type::PARTICLE_MELEE ||
                            i->type() == Type::PLAYER ||
                            i->type() == Type::BUG)
                            ) {
                            m_time_left_to_react = 10;
                            console::log("Frog::hurt() sensed: ", to_string(i->type()), "\n");
                            m_sensed_state = State::MELEE;
                            m_sensed_position = i->position();
                            m_sensed_objects.clear();
                            break;
                        }
                    }
                }
                if (m_sensed_state == State::IDLE) {
                    m_time_left_to_react = 1;
                }
            }
            //console::log("time left to react: ", (int)m_time_left_to_react, "\n");
            if (m_time_left_to_react > 0) {
                --m_time_left_to_react;
                if (m_time_left_to_react == 0) {
                    m_next_state = m_sensed_state;
                }
            }
        }
        void idle() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                if (m_prev_state == State::DEAD) {
                    m_sensed_state = State::NONE;
                    health::reset(m_health_id);
                    sprite::is_leftward(m_sprite_id, random::number(0, 1) ? true : false);
                }
                max_velocity(start_max_velocity());
                m_time_left_to_spawn_sense = m_time_to_spawn_sense;
                m_time_left_to_react = 0;
                reset_anim(m_idle_anim_id);
                for (auto& i : m_aabb_ids) {
                    if (aabb::name(i) == aabb::Name::BODY) {
                        aabb::is_active(i, true);
                    } else {
                        aabb::is_active(i, false);
                    }
                }
                m_sensed_objects.clear();
            }

            //console::log("Frog::idle() time left to spawn sense: ", m_time_left_to_spawn_sense, "\n");

            if (m_time_left_to_spawn_sense > 0) {
                --m_time_left_to_spawn_sense;
                if (m_time_left_to_spawn_sense == 0) {
                    //console::log("Frog::idle() spawn sense\n");

                    m_time_left_to_spawn_sense = m_time_to_spawn_sense;

                    spawn_fan(0.0F, 360.0F, 32, particle::Type::SENSE, position() + Vec2F{ 4.0F, 0.0F }, {}, 3.0F);
                }
            }

            set_anim(m_idle_anim_id);

            line::is_hidden(m_tounge_line_id, true);

            deceleration_x(m_is_on_ground ? 0.2F : 0.0F);

            add_velocity({ 0.0F, acceleration().y });

            if (m_time_left_to_react > 0) {
                --m_time_left_to_react;
                if (m_time_left_to_react == 0) {
                    m_next_state = m_sensed_state;
                }
            } else if (m_is_on_ground) {
                for (auto& i : m_sensed_objects) {
                    if (i->type() == Type::PARTICLE_HEALTH) {
                        //if (health::amount(m_health_id) < health::max(m_health_id)) {
                            console::log("health amount: ", health::amount(m_health_id), " max: ", health::max(m_health_id), "\n");
                        //}
                    }
                    if (((i->type() == Type::PLAYER && !(i->state() == State::SWIM && i->num_jumps() == 0)) ||
                            (i->type() == Type::PARTICLE_HEALTH && health::amount(m_health_id) < health::max(m_health_id)))
                        ) {
                        if (i->is_ducking() && (i->velocity().x < -1.5F || i->velocity().x > 1.5F)) {
                            //console::log("velocity x: ", i->velocity().x, "\n");
                            //console::log("player sliding ground\n");
                            m_time_left_to_react = 1;
                            m_sensed_state = State::JUMP;

                            m_sensed_position = i->position() + Vec2F{ 0.0F, -32.0F };
                        } else {
                            if (i->state() == State::SWING) {               
                                m_time_left_to_react += std::abs(i->rotation_speed());
                            } else {
                                //m_time_left_to_react = 5 + random::number(0, 2);
                                m_time_left_to_react = std::abs(i->velocity().x / 2.0F);
                                console::log("entity::Frog::idle() time left to react: ", m_time_left_to_react, "\n");
                            }
                            if (m_time_left_to_react <= 0) {
                                m_time_left_to_react = 1;
                            }
                            m_sensed_state = State::MELEE;
                            m_sensed_position = i->position();
                        }
                        
                        m_sensed_objects.clear();
                        break;
                    }
                }
            }
        }
        void jump() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;

                m_sensed_objects.clear();

                cF32 diff_x = m_sensed_position.x - position().x;
                cF32 diff_y = m_sensed_position.y - position().y;

                //console::log("diff_y: ", diff_y, "\n");
                max_velocity(start_max_velocity());

                velocity_x(diff_x / 64.0F);
                velocity_y(diff_y / 10.0F);

                add_position_y(-2.0F);

                if (velocity().y < -4.0F) {
                    velocity_y(-4.0F);
                } else if (velocity().y > -2.0F) {
                    velocity_y(-2.0F);
                }
                m_is_on_ground = false;

                reset_anim(m_jump_anim_id);
            }

            line::is_hidden(m_tounge_line_id, true);

            set_anim(m_jump_anim_id);

            add_velocity_y(acceleration().y);

            if (m_is_on_ground && velocity().y > 0.0F) {
                m_next_state = State::IDLE;
                reset_anim(m_idle_anim_id);
            }
        }
        void melee() {
            if (!m_is_on_ground) return;

            set_anim(m_melee_anim_id);

            sprite::is_leftward(m_sprite_id, m_sensed_position.x < position().x);

            m_tounge_start = position() + Vec2F{ 8.0F, 6.0F };
            m_tounge_end = m_sensed_position + Vec2F{ 8.0F, 8.0F };
            //console::log("tounge start: ", m_tounge_start.x, " ", m_tounge_start.y, "\n");
            //console::log("tounge end: ", m_tounge_end.x, " ", m_tounge_end.y, "\n");

            line::size(m_tounge_line_id, 2);
            line::max_length(m_tounge_line_id, 48.0F - anim::source(m_current_anim_id).x / 16.0F);

            if (line::max_length(m_tounge_line_id) < 16.0F) {
                line::max_length(m_tounge_line_id, 16.0F);
            }

            //line::max_length(m_tounge_line_id, 64.0F);

            line::is_hidden(m_tounge_line_id, false);
            line::layer(m_tounge_line_id, sprite::layer(m_sprite_id) + 1);
            line::color(m_tounge_line_id, { 191, 0, 0 });
            line::set(m_tounge_line_id, m_tounge_start, m_tounge_end);
            //line::get(m_tounge_line_id)->update();

            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                reset_anim(m_melee_anim_id);

                sound::position(m_melee_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_melee_sound_id);

                cVec2F tounge_vector = (m_tounge_end - m_tounge_start);
                cF32 tounge_length = line::length(tounge_vector);

                //console::log("length: ", tounge_length, "\n");

                Vec2F melee_velocity = (tounge_vector / tounge_length) * 8.0f;

                console::log("Frog::melee melee velocity: ", melee_velocity.x, " ", melee_velocity.y, "\n");

                particle::spawn({ .parent = this,
                                           .type = particle::Type::MELEE,
                                           .position = { position().x + 4.0f, position().y + 4.0f },
                                           .velocity = melee_velocity });
            }


            if (anim::is_last_frame(m_current_anim_id)) {
                //console::log("is last frame: ", anim()->is_last_frame(), "\n");                
                //line::is_hidden(m_tounge_line_id, true);
                m_next_state = State::JUMP;
            }

        }
        void stunned() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_sensed_objects.clear();
                reset_anim(m_stunned_anim_id);
                for (auto& i : m_aabb_ids) {
                    if (aabb::name(i) == aabb::Name::BODY) {
                        aabb::is_active(i, true);
                    } else {
                        aabb::is_active(i, false);
                    }
                }
                m_time_left_in_state = m_time_to_be_in_state;
            }
            set_anim(m_stunned_anim_id);

            line::is_hidden(m_tounge_line_id, true);

            add_velocity_y(acceleration().y);

            console::log("Frog::stunned time left: ", m_time_left_in_state, "\n");

            if (m_time_left_in_state > 0) {
                --m_time_left_in_state;
            }
            if (m_is_on_ground && m_time_left_in_state == 0) {
                m_time_in_state = 0;
                m_next_state = State::IDLE;
            }
        }
        void swim() {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_sensed_objects.clear();
                m_is_on_ground = false;
                //reset_anim(m_swim_anim_id);

                for (auto& i : m_aabb_ids) {
                    if (aabb::name(i) == aabb::Name::BODY_SWIM) {
                        aabb::is_active(i, true);
                    } else {
                        aabb::is_active(i, false);
                    }
                }
            }
            line::is_hidden(m_tounge_line_id, true);
            set_anim(m_hurt_anim_id);

            deceleration({ 0.0F, 0.0F });

            if (position_on_level().y + 16 < m_water_line_y) {
                console::log("frog swim water line y: ", m_water_line_y, "\n");
                //transform()->max_velocity = transform()->start_max_velocity;
                max_velocity({ 0.0F, 4.0F });
            } else {
                max_velocity({ 0.0F, 2.0F });
                velocity_x(sprite::is_leftward(m_sprite_id) ? -0.8F : 0.8F);
            }

            velocity_y(velocity().y * 0.9F);
            //moved_velocity({});

            if (position_on_level().y > m_water_line_y) {
                add_velocity({ 0.0F, -0.3F });
            } else {
                add_velocity({ 0.0F, 0.3F });
            }
        }
    };
}