module;
#include <cmath>
module entity.player;
import console;
import random;
import particle;

U8 prev_num_jumps = 0;

namespace entity {
    void Player::swim() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_num_jumps = 0;
            console::log("entity::Player::swim velocity: ", velocity().x, " ", velocity().y, "\n");;
            sound::stop(m_hover_sound_id);
            sound::stop(m_slide_wall_sound_id);
        }

        if (m_num_jumps != prev_num_jumps) {
            m_time_in_state = 0;
            prev_num_jumps = m_num_jumps;
            //console::log("reset time in state\n");
        }

        ++m_time_in_state;
        //console::log("m_time_in_state: ", m_time_in_state, "\n");

        m_current_anim_id = m_swim_anim_id;

        moved_velocity({});

        if (m_time_in_state > 5) {
            max_velocity(input::is_pressed(m_input_id, key_sprint) || m_num_jumps == 1 ? Vec2F{ 3.0F, 3.0F } : Vec2F{ 2.0F, 2.0F });
        }

        cVec2F swim_acc = { 0.15F, 0.15F };
        cVec2F jump_acc = { 0.05F, 0.01F };

        acceleration(m_num_jumps == 1 ? jump_acc : swim_acc);
        deceleration({ 0.02F, 0.02F });


        //console::log("entity::Player::swim is_on_ground: ", m_is_on_ground, "\n");
        if (m_num_jumps == 0) {
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
                if (aabb::name(i) == aabb::Name::BODY_SWIM) {
                    aabb::is_active(i, true);
                }
            }
            cF32 hypotenuse = std::sqrtf(velocity().x * velocity().x + velocity().y * velocity().y);
            anim::speed(m_current_anim_id, hypotenuse / 5.0F);
            if (anim::speed(m_current_anim_id) < 0.1F) {
                anim::speed(m_current_anim_id, 0.1F);
            }
            cU16 largest_vel = std::abs(velocity().x) > std::abs(velocity().y) ? std::abs(velocity().x) : std::abs(velocity().y);
            //console::log("entity::Player::swim largest_vel: ", largest_vel, "\n");

            if (m_time_in_state > 100 - largest_vel * 10) {
                //cU32 speed = (I32)std::sqrtf(velocity().x * velocity().x + velocity().y * velocity().y);
                //console::log("speed: ", speed, "\n");
                m_time_in_state = random::number(0, 10);
                cF32 vel_x = sprite::is_upended(m_sprite_id) ? -0.5F : 0.5F;

                particle::spawn({ .parent = this,
                                  .type = particle::Type::BUBBLE,
                                  .position = position() + Vec2F{sprite::is_upended(m_sprite_id) ? -6.0F : 6.0F, 0.0F},
                                  .velocity = Vec2F{velocity().x / 10.0F + vel_x, -0.5F}
                    }
                );
            }

        } else if (m_num_jumps == 1) {
            if (m_time_in_state < 4 && velocity().y > 0.0F) {
                velocity_y(-1.0F); // prevent falling out of water when jumping straight into ceiling
            }
            if (velocity().y > 0.0F) {
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, false);
                    if (aabb::name(i) == aabb::Name::BODY) {
                        aabb::is_active(i, true);
                    }
                }
            }
            add_velocity({ 0.0F, 0.2F });

            anim::speed(m_current_anim_id, 0.0F);
            anim::source_x(m_current_anim_id, anim::source(m_current_anim_id).w * 7);

            if ((m_is_on_ground || input::is_pressed(m_input_id, key_jump) || input::is_pressed(m_input_id, key_melee))
                &&
                m_next_state != State::RUN) {
                console::log("entity::Player::swim landed on ground\n");
                m_next_state = State::RUN;
                return;
                //next_state(State::RUN);
                //console::log("is_on_ground: ", m_is_on_ground, "\n");
            }
        }

        if (input::is_pressed(m_input_id, key_up)) {
            add_velocity_y(-acceleration().y);
        }
        if (input::is_pressed(m_input_id, key_down)) {
            add_velocity_y(acceleration().y);
        }
        if (input::is_pressed(m_input_id, key_left)) {
            //sprite::is_leftward(m_sprite_id, true);        
            add_velocity_x(-acceleration().x);
        }
        if (input::is_pressed(m_input_id, key_right)) {
            //sprite::is_leftward(m_sprite_id, false);
            add_velocity_x(acceleration().x);
        }


        F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) {
            radians += 3.1415926535F * 2.0F;
        }
        F32 degrees = radians * 180.0F / 3.1415926535F;
        //console::log("degrees: ", degrees, "\n");
        sprite::origin(m_sprite_id, { sprite::source_rect(m_sprite_id).w / 2.0F, 48.0F });

        if (input::is_pressed(m_input_id, key_jump) && m_num_jumps == 0) {
            return;
        }

        if (velocity().x < -deceleration().x || velocity().x > deceleration().x ||
            velocity().y < -deceleration().y || velocity().y > deceleration().y ||
            input::is_pressed(m_input_id, key_left) || input::is_pressed(m_input_id, key_right) ||
            input::is_pressed(m_input_id, key_up) || input::is_pressed(m_input_id, key_down)
            ) {

            //if (degree_diff > 10.0F) {
                //sprite::rotate(m_sprite_id, degree_diff * 0.1F);
            //} else {
            sprite::rotation(m_sprite_id, degrees);
            //}
        }

        sprite::is_leftward(m_sprite_id, false);
        if (velocity().x < 0.0F) {
            sprite::is_upended(m_sprite_id, true);
        } else if (velocity().x > 0.0F) {
            sprite::is_upended(m_sprite_id, false);
        }

        if (!input::is_pressed(m_input_id, key_up) && !input::is_pressed(m_input_id, key_down) && !input::is_pressed(m_input_id, key_left) && !input::is_pressed(m_input_id, key_right)) {
            if (sprite::is_upended(m_sprite_id)) {
                cF32 from_target_dir = std::abs(180.0F - sprite::rotation(m_sprite_id));
                //console::log("from_target_dir: ", from_target_dir, "\n");

                if (sprite::rotation(m_sprite_id) >= 90.0F && sprite::rotation(m_sprite_id) < 180.0F) {
                    sprite::rotation(m_sprite_id, sprite::rotation(m_sprite_id) + 1.0F * from_target_dir * 0.1F);
                } else if (sprite::rotation(m_sprite_id) <= 270.0F && sprite::rotation(m_sprite_id) > 180.0F) {
                    sprite::rotation(m_sprite_id, sprite::rotation(m_sprite_id) - 1.0F * from_target_dir * 0.1F);
                }
            } else {
                cF32 target_dir = sprite::rotation(m_sprite_id) >= 270.0F && sprite::rotation(m_sprite_id) <= 360.0F ? 360.0F : 0.0F;

                cF32 from_target_dir = std::abs(target_dir - sprite::rotation(m_sprite_id));

                //console::log("from_target_dir: ", from_target_dir, "\n");

                if (sprite::rotation(m_sprite_id) >= 270.0F && sprite::rotation(m_sprite_id) < 360.0F) {
                    sprite::rotation(m_sprite_id, sprite::rotation(m_sprite_id) + 1.0F * from_target_dir * 0.1F);
                } else if (sprite::rotation(m_sprite_id) <= 90.0F && sprite::rotation(m_sprite_id) > 0.0F) {
                    sprite::rotation(m_sprite_id, sprite::rotation(m_sprite_id) - 1.0F * from_target_dir * 0.1F);
                }
            }
        }

        if (m_next_state == State::RUN) {
            if (m_time_left_in_state > 0) --m_time_left_in_state;
            if (m_time_left_in_state == 0) {
                m_is_first_state_update = true;
                unlock(key_melee);
                m_is_ducking = false;
                m_time_left_melee = 0;
                m_state = State::RUN;
                sprite::is_leftward(m_sprite_id, sprite::is_upended(m_sprite_id));
                sprite::is_upended(m_sprite_id, false);
                sprite::rotation(m_sprite_id, 0.0F);
            }
        }
    }
}