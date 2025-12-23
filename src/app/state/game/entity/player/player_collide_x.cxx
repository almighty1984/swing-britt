module;
#include <sstream>
module entity.player;
import console;
import particle;

namespace entity {
    void Player::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (other.owner->parent() == this) return;

        aabb::cName our_name = aabb::name(our.id);

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        /*if (other_rect.x < our_rect.x && velocity().x > 0.0F ||
            other_rect.x > our_rect.x && velocity().x < 0.0F) return;*/

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cState other_state = other.owner->state();

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

        if (m_state == State::SWING) {
            //add_position_x( -overlap_x );
            //velocity_x(-velocity().x;

            if (is_arch(other_type)) {
                collide_y(our, other);
            } else if (is_clip(other_type)) {
                if (!sound::is_playing(m_bump_head_sound_id)) {
                    sound::stop(m_bump_head_sound_id);
                    sound::position(m_bump_head_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound::play(m_bump_head_sound_id);

                    m_rotation_speed *= -1.0F;
                }
            }
            return;
        }

        //console::log(sprite::to_string(other_type), " overlap_x: ", overlap_x, "\n");


        if (is_arch(other_type)) {
            if (m_is_climbing_ledge) {
                return;
            }
            if (m_is_on_ground && m_is_on_slope) {
                add_position_x(-overlap_x);
                velocity_x(velocity().x * 0.0F);
                //return;
            }
            //if (m_state == State::SWIM) {
            //    add_position_x( -overlap_x );
                /*if (other_type == Type::ARCH_L_1X1 && velocity().x >= 0.0F)
                    velocity_y(std::abs(velocity().x);
                else if (other_type == Type::ARCH_R_1X1 && velocity().x <= 0.0F)
                    velocity_y(std::abs(velocity().x);
                else if (other_type == Type::ARCH_L_2X1_0 || other_type == Type::ARCH_L_2X1_1 && velocity().x >= 0.0F)
                    velocity_y(std::abs(velocity().x) * 0.75F;
                else if (other_type == Type::ARCH_R_2X1_0 || other_type == Type::ARCH_R_2X1_1 && velocity().x <= 0.0F)
                    velocity_y(std::abs(velocity().y) * 0.75F;*/
                    //    moved_velocity_x(0.0F);

                    //    velocity().x *= 0.9F;
                    //    if (other_type == Type::ARCH_L_1X1) {
                    //        //if (velocity().x > 0.0F) {
                    //            velocity_y(velocity().x);
                    //        //}
                    //    }
                    //    else if (other_type == Type::ARCH_R_1X1) {
                    //        //if (velocity().x < 0.0F) {
                    //            velocity_y(-velocity().x;
                    //        //}
                    //    }
                    //    else if (other_type == Type::ARCH_L_2X1_0 || other_type == Type::ARCH_L_2X1_1) {
                    //        //if (velocity().x > 0.0F) {
                    //            velocity_y(velocity().x * 0.5F;
                    //        //}
                    //    }            
                    //    else if (other_type == Type::ARCH_R_2X1_0 || other_type == Type::ARCH_R_2X1_1) {
                    //        //if (velocity().x < 0.0F) {
                    //            velocity_y(-velocity().x * 0.5F;
                    //        //}
                    //    }            
                    //    return;
                    //} else {
            collide_y(our, other);
            //}
        } else if (is_slope(other_type) && other_type != Type::SLOPE_U && m_state == State::SWIM) {
            collide_y(our, other);
        } else if (other_type == Type::BRICK) {
            if (our_velocity.y >= 6.0F) {
                collide_y(our, other);
                return;
            }
            if (our_rect.h < other_rect.y + 2.0F ||
                our_rect.y > other_rect.h - 2.0F) {
                //collide_y(our, other);
                return;
            }
            add_position_x(-overlap_x);
            velocity_x(velocity().x * 0.95F);
        } else if (other_type == Type::BUG) {
            //if (our_rect.h + 2.0F < other_rect.y) return;

            if (our_velocity.y > 3.0F) {
                console::log("\nhmm\n");
                collide_y(our, other);
                return;
            }
            add_position_x(-overlap_x);

            //velocity_x(-0.9F;

            /*if (std::abs(our_velocity.x) < std::abs(other_velocity.x)) {
                velocity_x(other_velocity.x * 4.0F;
                velocity().y -= 2.0F;
                add_position_x( -overlap_x );
            }*/
            /*if (our_rect.h < other_rect.y + 2.0F ||
                our_rect.y > other_rect.h - 2.0F) {
                return;
            }
            add_position_x( -overlap_x );*/
        } else if (other_type == Type::CLIP) {
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        } else if (other_type == Type::CLIP_U || other_type == Type::SLOPE_U) {
        } else if (other_type == Type::CLIP_D) {
            if ((/*other_rect.h < our_rect.y &&*/ m_is_on_ground) || m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                if (!input::is_pressed(m_input_id, key_down)) {
                    if (velocity().x < 0.0F && velocity().x < -m_slide_ground_velocity_limit.x ||
                        velocity().x > 0.0F && velocity().x > m_slide_ground_velocity_limit.x) {
                        velocity_x(velocity().x * 0.5F);
                    }
                }
                return;
            }
        } else if (other_type == Type::CLIP_LEDGE) {
            if (m_is_on_ledge) return;
            if (our_rect.y >= other_rect.h - 2.0F) {
                return;
            }
            if (our_rect.h <= other_rect.y + 2.0F) {
                return;
            }
            if (velocity().x < 0.0F && our_rect.w < other_rect.x + velocity().x ||
                velocity().x > 0.0F && our_rect.x > other_rect.w + velocity().x) {
                return;
            }

            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            if (m_state == State::SWIM) {
                if (m_num_jumps == 1 && velocity().y > 0.0F) {
                    next_state(State::RUN);
                }
                return;
            }
            m_is_on_slope = false;
            if (m_is_hovering || input::is_locked(m_input_id, key_jump) || m_is_on_ground) return;
            //if (m_is_on_ground && !is_pressed(key_up)) return;

            m_is_wall_to_left = other_rect.x < our_rect.x;
            if ((m_is_wall_to_left && velocity().x + moved_velocity().x > 0.0F) ||
                (!m_is_wall_to_left && velocity().x + moved_velocity().x < 0.0F)) return;

            if (m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering && velocity().y > 0.0F &&
                (m_is_wall_to_left && input::is_pressed(m_input_id, key_left) || !m_is_wall_to_left && input::is_pressed(m_input_id, key_right))) {
                m_is_sliding_wall = true;
                if (velocity().y > 1.0F) {
                    velocity_y(1.0F);
                }
                sprite::is_leftward(m_sprite_id, !m_is_wall_to_left);
            }
            if (input::is_pressed(m_input_id, key_jump) && !input::is_locked(m_input_id, key_jump) && m_time_left_jumping_wall <= 0.0F && !m_is_carrying && !m_is_hovering) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }
            if (velocity().y > 0.0F && !m_is_carrying && !m_is_hovering) {
                if (m_is_wall_to_left && input::is_pressed(m_input_id, key_left) ||
                    !m_is_wall_to_left && input::is_pressed(m_input_id, key_right)) {
                    if (!m_is_on_ledge && our_rect.y <= other_rect.y - 0 && our_rect.y >= other_rect.y - 16) {
                        m_is_on_ledge = true;
                        m_is_sliding_wall = false;
                        position_y(other_rect.y - 8);
                        velocity_y(0.0F);

                        reset_anim(m_ledge_grab_anim_id);

                        sound::position(m_ledge_grab_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                        sound::play(m_ledge_grab_sound_id);
                    }
                }
            }
        } else if (other_type == Type::CLIP_L) {
            if (velocity().x + moved_velocity().x <= 0.0F || m_is_on_ledge) return;
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            if (m_state == State::SWIM && m_num_jumps == 1 && velocity().y > 0.0F) {
                m_next_state = State::RUN;
                return;
            }
            m_is_wall_to_left = false;
            m_is_on_slope = false;
            if (m_is_on_ground) return;

            if (velocity().y > 0.0F && input::is_pressed(m_input_id, key_right) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
                m_is_sliding_wall = true;
                sprite::is_leftward(m_sprite_id, true);
                if (velocity().y > 1.0F) {
                    velocity_y(1.0F);
                }
                //set_anim(m_slide_wall_anim_id);
            }
            if (input::is_pressed(m_input_id, key_jump) && !input::is_locked(m_input_id, key_jump) && m_time_left_jumping_wall <= 0.0F) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }
        } else if (other_type == Type::CLIP_LD) {
            if ((/*other_rect.h < our_rect.y &&*/ m_is_on_ground) || m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                if (!input::is_pressed(m_input_id, key_down)) {
                    if (velocity().x < 0.0F && velocity().x < -m_slide_ground_velocity_limit.x ||
                        velocity().x > 0.0F && velocity().x > m_slide_ground_velocity_limit.x) {
                        velocity_x(velocity().x * 0.5F);
                    }
                }
                return;
            }

            if (velocity().x + moved_velocity().x <= 0.0F || m_is_on_ledge) return;
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            if (m_state == State::SWIM && m_num_jumps == 1 && velocity().y > 0.0F) {
                m_next_state = State::RUN;
                return;
            }
            m_is_wall_to_left = false;
            m_is_on_slope = false;
            if (m_is_on_ground) return;

            if (velocity().y > 0.0F && input::is_pressed(m_input_id, key_right) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
                m_is_sliding_wall = true;
                sprite::is_leftward(m_sprite_id, true);
                if (velocity().y > 1.0F) {
                    velocity_y(1.0F);
                }
                //set_anim(m_slide_wall_anim_id);
            }
            if (input::is_pressed(m_input_id, key_jump) && !input::is_locked(m_input_id, key_jump) && m_time_left_jumping_wall <= 0.0F) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }

        } else if (other_type == Type::CLIP_R) {
            if (velocity().x + moved_velocity().x >= 0.0F || m_is_on_ledge) return;
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            if (m_state == State::SWIM && m_num_jumps == 1 && velocity().y > 0.0F) {
                m_next_state = State::RUN;
                return;
            }
            m_is_wall_to_left = true;
            m_is_on_slope = false;
            if (m_is_on_ground) return;
            if (velocity().y > 0.0F && input::is_pressed(m_input_id, key_left) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
                m_is_sliding_wall = true;
                sprite::is_leftward(m_sprite_id, false);
                if (velocity().y > 1.0F) {
                    velocity_y(1.0F);
                }
                //set_anim(m_slide_wall_anim_id);
            }
            if (input::is_pressed(m_input_id, key_jump) && !input::is_locked(m_input_id, key_jump) && m_time_left_jumping_wall <= 0.0F && !m_is_carrying && !m_is_hovering) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }
        } else if (other_type == Type::CLIP_RD) {
            if ((/*other_rect.h < our_rect.y &&*/ m_is_on_ground) || m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                if (!input::is_pressed(m_input_id, key_down)) {
                    if (velocity().x < 0.0F && velocity().x < -m_slide_ground_velocity_limit.x ||
                        velocity().x > 0.0F && velocity().x > m_slide_ground_velocity_limit.x) {
                        velocity_x(velocity().x * 0.5F);
                    }
                }
                return;
            }

            if (velocity().x + moved_velocity().x >= 0.0F || m_is_on_ledge) return;
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            if (m_state == State::SWIM && m_num_jumps == 1 && velocity().y > 0.0F) {
                m_next_state = State::RUN;
                return;
            }
            m_is_wall_to_left = true;
            m_is_on_slope = false;
            if (m_is_on_ground) return;
            if (velocity().y > 0.0F && input::is_pressed(m_input_id, key_left) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
                m_is_sliding_wall = true;
                sprite::is_leftward(m_sprite_id, false);
                if (velocity().y > 1.0F) {
                    velocity_y(1.0F);
                }
                //set_anim(m_slide_wall_anim_id);
            }
            if (input::is_pressed(m_input_id, key_jump) && !input::is_locked(m_input_id, key_jump) && m_time_left_jumping_wall <= 0.0F && !m_is_carrying && !m_is_hovering) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }
        } else if (is_conduit(other_type)) {
            return;
            if (other.owner->is_dead()) return;
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        } else if (other_type == Type::FROG) {
            if (m_state == State::SWING) {
                console::log("Player::collide_x swing, nah\n");
                return;
            }

            //if (our_rect.h + 1.0F <= other_rect.y) return;
            //velocity_x(moved_velocity_x(0.0F);

            add_position_x(-overlap_x);

            if (std::abs(velocity().x) > std::abs(other.owner->velocity().x)) {
                return;
            }

            cF32 vel_x = velocity().x;
            velocity_x(other.owner->velocity().x * 0.5F);
            other.owner->velocity_x(vel_x * 0.25f);
            if (our_rect.x < other_rect.x) {
                add_velocity({ -1.0F, 0.0F });
                other.owner->add_velocity({ 1.0F, 0.0F });
            } else {
                add_velocity({ 1.0F, 0.0F });
                other.owner->add_velocity({ -1.0F, 0.0F });
            }
        } else if (other_type == Type::LEVEL_L_0) {
            m_next_level = start::Type::L_0;
            m_next_start = { start::Type::R_0, 0 };
        } else if (other_type == Type::LEVEL_R_0) {
            m_next_level = start::Type::R_0;
            m_next_start = { start::Type::L_0, 0 };
        } else if (other_type == Type::LEVEL_CENTER) {
            if (velocity().x != 0.0F) {
                m_is_to_write_save = true;
            }
        } else if (other_type == Type::MOLE) {
            collide_y(our, other);
        } else if (other_type == Type::PARTICLE_BRICK) {
            //add_position_x( -overlap_x );
            //velocity().x *= 0.95F;
        } else if (other_type == Type::PARTICLE_HEALTH) {

        } else if (other_type == Type::PARTICLE_MELEE) {

        } else if (other_type == Type::PARTICLE_SHOT) {
            console::log("\n\nentity::Player::collide_x() ", to_string(other_type), "\n\n");
            hurt(other.owner);
        } else if (other_type == Type::SLOPE_L_1X1) {
            return;
            if (our_rect.h > other_rect.h) return;

            //if (velocity().x < 0.0F) return;
            if (m_state == State::SWIM) {
                if (our_velocity.x > 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(-our_velocity.x * 0.9F);
                    velocity_x(velocity().x * 0.95f);
                    add_position({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            if (velocity().x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F);
            }
            //velocity_y(-velocity().x;
            //position().y -= 1.0F;
            add_position({ 0.0F, -velocity().x });

            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_1X1) {
            return;
            if (our_rect.h > other_rect.h) return;

            //if (velocity().x > 0.0F) return;
            if (m_state == State::SWIM) {
                if (our_velocity.x < 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(our_velocity.x * 0.9F);
                    velocity_x(velocity().x * 0.95f);
                    add_position({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            //add_position_x( -overlap_x );
            //velocity_y(0.0F);
            if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F);
            }
            //velocity_y(velocity().x);
            //position().y -= 1.0F;
            add_position({ 0.0F, velocity().x });
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
            return;
            if (our_rect.h > other_rect.h) return;

            //if (velocity().x < 0.0F) return;
            if (m_state == State::SWIM) {
                if (our_velocity.x > 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(-our_velocity.x * 0.5F);
                    velocity_x(velocity().x * 0.95f);
                    add_position({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            if (velocity().x > max_velocity().x * 0.9F) {
                velocity_x(max_velocity().x * 0.9F);
            }
            //velocity_y(-velocity().x / 2.0F;
            //velocity().y -= 0.5F;
            add_position({ 0.0F, -velocity().x / 2.0F });

            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
            return;
            if (our_rect.h > other_rect.h) return;

            //if (velocity().x > 0.0F) return;
            if (m_state == State::SWIM) {
                if (our_velocity.x < 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(our_velocity.x * 0.5F);
                    velocity_x(velocity().x * 0.95f);
                    add_position({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            //add_position_x( -overlap_x );
            if (velocity().x < -max_velocity().x * 0.9F) {
                velocity_x(-max_velocity().x * 0.9F);
            }
            //velocity_y(velocity().x / 2.0F;
            //velocity().y -= 0.5F;
            add_position({ 0.0F, velocity().x / 2.0F });

            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SPRING_U) {
            collide_y(our, other);
        } else if (other_type == Type::TRAIN) {
            if (other.owner->time_left_alive() > 0) {
                m_is_on_ledge = false;
            }
        } else if (other_type == Type::PLAYER) {
            return;
            add_position_x(-overlap_x);
            //velocity_x(0.0F);

            //velocity_x(velocity().x * 0.5F + velocity.x * 0.5F;

            velocity_x(velocity().x * 0.75F + other_velocity.x * 0.25f);
            //velocity_y(velocity().y * 0.5F + velocity.y * 0.5F;
        } else if (is_water_line(other_type)) {
            collide_y(our, other);
        }


        if (m_is_sliding_wall && m_state == State::RUN) {
            //console::log("sliding wall: ", (int)m_time_sliding_wall, "\n");
            if (m_time_sliding_wall > 5) {
                m_time_sliding_wall = 0;
                cF32 wall_x = our_rect.x < other_rect.x ? other_rect.x : other_rect.w;

                particle::cType type = our_rect.x < other_rect.x ? particle::Type::DUST_L : particle::Type::DUST_R;

                particle::spawn({ .parent = this,
                                  .type = type,
                                  .position = Vec2F{ wall_x, position().y + 8.0F },
                                  .velocity = Vec2F{ 0.0F, 0.0F } });
            }
        }
    }
}