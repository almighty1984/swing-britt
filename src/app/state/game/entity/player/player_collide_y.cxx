module;
#include <sstream>
module entity.player;
import console;
import particle;

namespace entity {
    void Player::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (other.owner->parent() == this) return;

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };
        /*if (velocity().y > 0.0F && our_rect.h - 4.0F - velocity().y > other_rect.y) {
            return;
        }*/
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();
        cVec2F our_velocity = velocity() + moved_velocity();

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);
        aabb::cName   our_name = aabb::name(our.id);

        //if (other_rect.y < our_rect.y && velocity().y >= 0.0F ||
            //other_rect.y > our_rect.y && velocity().y <= 0.0F) return;

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        //console::log("entity::Player::collide_y() type: ", to_string(other_type), " bounce time: ", (I32)m_time_left_bouncing, "\n");

        if (m_state == State::SWIM) {
            if (/*position_on_level().y <= m_water_line_y &&*/ m_num_jumps == 1 && velocity().y > 0.0F) {
                if (other_type == Type::CLIP || other_type == Type::CLIP_U || other_type == Type::CLIP_LEDGE ||
                    is_slope(other_type)) {
                    console::log("Player::collide_y ground above water line\n");
                    m_next_state = State::RUN;
                } else if (other_type == Type::BUG || other_type == Type::FROG) {
                    m_next_state = State::RUN;
                    other.owner->velocity_y(our_velocity.y);
                    velocity_y(0.0F);
                    moved_velocity_y(0.0F);
                    m_is_on_ground = true;
                }
            }
        }

        if (m_state == State::RUN && our_rect.y < other_rect.y) {
            if (!m_is_on_ground && !m_is_on_slope && velocity().y > 1.0F &&
                (is_clip(other_type) ||
                    is_slope(other_type))
                ) {
                particle::spawn({ .parent = this,
                                  .type = particle::Type::DUST,
                                  .position = Vec2F{ our_rect.x + 8.0F, other_rect.y },
                                  .velocity = Vec2F{ 0.0F, 0.0F } });
            }
        }

        if (m_state == State::SWING && (is_clip(other_type) || is_arch(other_type))) {
            if (!sound::is_playing(m_bump_head_sound_id)) {
                sound::stop(m_bump_head_sound_id);
                sound::position(m_bump_head_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_bump_head_sound_id);

                m_rotation_speed *= -1.0F;
                //add_position_y( -overlap_y );
                //velocity_y(-velocity().y;
            }
            return;
        }
        if (is_arch(other_type)) {
            if (velocity().y > 0.0F) return;

            if (m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                return;
            }
            if (m_state == State::SWIM && m_num_jumps == 0) {
                //add_position_y( -overlap_y );
                moved_velocity_y(0.0F);

                //velocity_y(0.0F);
                //return;

                //m_prev_state = State::RUN;
                //m_next_state = State::SWIM;
                //console::log("yoyo\n");

                if (std::abs(velocity().x) > std::abs(velocity().y)) {
                    if (other_type == Type::ARCH_L_1X1 && velocity().x > 0.0F) {
                        velocity_y(velocity().x * 0.8F);
                        //position().y += velocity().x;
                    } else if (other_type == Type::ARCH_R_1X1 && velocity().x < 0.0F) {
                        velocity_y(-velocity().x * 0.8F);
                        //position().y -= velocity().x;
                    } else if (other_type == Type::ARCH_L_2X1_0 || other_type == Type::ARCH_L_2X1_1 && velocity().x > 0.0F) {
                        velocity_y(velocity().x * 0.75F * 0.8F);
                        //position().y += velocity().x * 0.75F;
                    } else if (other_type == Type::ARCH_R_2X1_0 || other_type == Type::ARCH_R_2X1_1 && velocity().x < 0.0F) {
                        velocity_y(-velocity().x * 0.75F * 0.8F);
                        //position().y -= velocity().x * 0.75F;
                    }
                    add_position({ 0.0F, 0.5F });
                    velocity_x(velocity().x * 0.95f);
                    if (m_time_in_state < 10) {
                        velocity_x(0.0F);
                    }
                } else {
                    //if (std::abs(velocity().y) > 0.2F) {
                        //velocity().y *= 0.9F;
                    //}
                    if (other_type == Type::ARCH_L_1X1 && velocity().y < 0.0F) {
                        velocity_x(velocity().y * 0.8F);
                        add_position({ -1.0F, 0.0F });
                    } else if (other_type == Type::ARCH_R_1X1 && velocity().y < 0.0F) {
                        velocity_x(-velocity().y * 0.8F);
                        add_position({ 1.0F, 0.0F });
                    } else if (other_type == Type::ARCH_L_2X1_0 || other_type == Type::ARCH_L_2X1_1 && velocity().y < 0.0F) {
                        velocity_x(velocity().y * 1.2F * 0.8F);
                    } else if (other_type == Type::ARCH_R_2X1_0 || other_type == Type::ARCH_R_2X1_1 && velocity().y < 0.0F) {
                        velocity_x(-velocity().y * 1.2F * 0.8F);
                    }
                    velocity_y(velocity().y * 0.9f);
                    if (m_time_in_state < 10) {
                        velocity_y(0.0F);
                    }

                    //console::log("time in state: ", m_time_in_state, "\n");

                    /*if (velocity().x > 0.0F) {
                        position().y += velocity().x;
                    } else {
                        position().y -= velocity().x;
                    }*/
                }
            }
            if (m_is_on_ground) {
                m_time_left_ducking = m_time_to_duck;
            }
            if (our_rect.y < other_rect.y || m_time_left_ducking > 0) {
                return;
            }

            if (!m_is_on_ground && m_time_left_ducking == 0 && velocity().y < 0.0F) {
                sound::position(m_bump_head_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_bump_head_sound_id);
            }
            add_position_y(-overlap_y);
            velocity_y(std::abs(velocity().y) * 0.75F);

            if (other_type == Type::ARCH_L_1X1)
                add_velocity({ -velocity().y, 0.0F });
            else if (other_type == Type::ARCH_R_1X1)
                add_velocity({ velocity().y, 0.0F });
            else if (other_type == Type::ARCH_L_2X1_0 || other_type == Type::ARCH_L_2X1_1)
                add_velocity({ velocity().y * -0.5F, 0.0F });
            else if (other_type == Type::ARCH_R_2X1_0 || other_type == Type::ARCH_R_2X1_1)
                add_velocity({ velocity().y * 0.5F, 0.0F });
        }

        if (is_slope(other_type) && other_type != Type::SLOPE_U && m_state == State::SWIM) {
            //add_position_y(-overlap_y);
            moved_velocity_y(0.0F);

            F32 multiplier = other_type == Type::SLOPE_L_1X1 || other_type == Type::SLOPE_R_1X1 ? 0.5F : 1.0F;
            add_position_y(-0.5F / multiplier);

            if (std::abs(velocity().x) * multiplier > std::abs(velocity().y)) {
                if (other_type == Type::SLOPE_L_1X1 && velocity().x > 0.0F) {
                    velocity_y(-velocity().x * 0.8F);
                } else if (other_type == Type::SLOPE_R_1X1 && velocity().x < 0.0F) {
                    velocity_y(velocity().x * 0.8F);
                } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1 && velocity().x > 0.0F) {
                    velocity_y(-velocity().x * 0.75F * 0.8F);
                } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1 && velocity().x < 0.0F) {
                    velocity_y(velocity().x * 0.75F * 0.8F);
                }
                velocity_x(velocity().x * 0.8F);
            } else {
                if (other_type == Type::SLOPE_L_1X1 && velocity().y > 0.0F) {
                    velocity_x(-velocity().y * 0.8F);
                    //position().x -= 1.0F;
                } else if (other_type == Type::SLOPE_R_1X1 && velocity().y > 0.0F) {
                    velocity_x(velocity().y * 0.8F);
                    //position().x += 1.0F;
                } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1 && velocity().y > 0.0F) {
                    velocity_x(-velocity().y * 1.2F * 0.8F);
                    //position().x -= 1.0F;
                } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1 && velocity().y > 0.0F) {
                    velocity_x(velocity().y * 1.2F * 0.8F);
                    //position().x += 1.0F;
                }
                velocity_y(velocity().y * 0.8F);
            }
            return;
        } else if (other_type == Type::BRICK) {
            /*if (velocity().y < 0.0F && our_rect.y < other_rect.h - 2.0F || velocity().y > 0.0F && our_rect.h > other_rect.y + 2.0F) {
                collide_x(our, other);
                return;
            }*/

            if (our_rect.h < other_rect.y + 4.0F && our_velocity.y > 0.0F) {
                if (our_velocity.y >= 6.0F) {
                    other.owner->hurt(this);
                    return;
                }
                m_is_on_ground = true;
                moved_velocity(other_velocity);
                velocity_y(0.0F);
                add_position_y(-overlap_y);
            }


            /*if (our_rect.h < other_rect.y + 2.0F && velocity().y > 0.0F) {
                velocity_y(0.0F);
                m_is_on_ground = true;
            } else if (our_rect.y > other_rect.y - 2.0F && velocity().y < 0.0F) {
                velocity_y(velocity().y * -0.9F);
            }*/
        } else if (other_type == Type::BUG) {
            if (other.owner->parent() == this || other.owner->is_dead() || other.owner->is_interacting()) return;
            //if (m_state != State::RUN) return;

            if (other.owner->state() == State::SWIM) {
                if (velocity().y > 0.0F && our_rect.y < other_rect.y) {
                    other.owner->velocity_y(our_velocity.y);
                    //other.owner->moved_velocity_y(our_velocity.y;
                    velocity_y(0.0F);
                    add_position_y(-overlap_y);
                    m_is_on_ground = true;
                    console::log("entity::Player::collide_y() swim bug ", our_velocity.y, "\n");
                    /*if (velocity().y < 0.0F) {
                        velocity_y(velocity().y * -1.0F);
                    }*/
                }
                return;
            }

            /*if (our_rect.y > other_rect.h) {
                return;
            }*/

            if (our_rect.y >= other_rect.y) {   // BUG above
                Vec2F other_vel = other.owner->velocity() + other.owner->moved_velocity();
                other.owner->velocity_y(velocity().y / 4.0F - 3.0F);
                other.owner->add_position_y(-8.0F);

                cF32 our_mid_x = our_rect.w - our_rect.x / 2.0F;
                cF32 other_mid_x = other_rect.w - other_rect.x / 2.0F;

                cF32 mid_diff_x = other_mid_x - our_mid_x;
                other.owner->velocity_x((velocity().x + moved_velocity().x) + mid_diff_x / 4.0F);

                //console::log("mid diff: ", , "\n");
                // 
                //velocity() = vel / 2.0F;

                cF32 v = other.owner->velocity().y;

                //console::log("Bug::collide_y vel y: ", v, "\n");

                //hurt();
                console::log("entity::Player::collide_y() BUG above\n");

                return;
            }
            /*if (m_is_ducking) {
                m_is_on_ground = true;
                add_position_y( -overlap_y );
                return;
            }*/

            
            if ((other.owner->state() == State::UPENDED || other.owner->state() == State::BOUNCE)) {
                add_position_y(-overlap_y);
                console::log("Player::collide_y() BUG vel: ", velocity().y, "\n");
                max_velocity_y(10.0F);
                velocity_y(-5.0F - velocity().y / 4.0F);
                moved_velocity_y(0.0F);
                m_is_down_thrusting ? reset_anim(m_jump_spin_anim_id) : reset_anim(m_jump_anim_id);
                m_is_down_thrusting = false;
                m_is_on_ground = false;
                m_is_on_slope = false;
                m_num_jumps = 1;
                m_time_left_bouncing = m_time_to_bounce;

                other.owner->next_state(State::BOUNCE);
                return;
            }
            add_position_y(-overlap_y + 1.0F);
            moved_velocity(other_velocity);
            //velocity_x(velocity().x * 0.5F);
            velocity_y(0.0F);
            m_is_on_ground = true;
            console::log("entity::Player::collide_y() BUG below\n");
            
        } else if (other_type == Type::CLIP) {
            add_position_y(-overlap_y);

            if (m_state == State::SWIM && m_num_jumps == 0) {
                velocity_y(0.0F);
                moved_velocity_y(0.0F);
                return;
            }

            if (velocity().y > 0.0F && our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
                moved_velocity_x(0.0F);
                velocity_y(0.0F);
                moved_velocity_y(0.0F);
            } else {
                velocity_y(std::abs(velocity().y) * 0.75F);
                sound::position(m_bump_head_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_bump_head_sound_id);
            }
        } else if (other_type == Type::CLIP_D || other_type == Type::CLIP_LD || other_type == Type::CLIP_RD) {
            if (m_state == State::SWIM && m_num_jumps == 0) {
                velocity_y(0.0F);
                moved_velocity_y(0.0F);
                add_position_y(-overlap_y);
                //console::log("hmm\n");
                return;
            }
            if ((other_rect.y < our_rect.y && m_is_on_ground) || m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                return;
            }
            if (m_time_left_rising > 0) {
                return;
            }
            if (velocity().y > 0.0F) return;
            add_position_y(-overlap_y);

            if (our_rect.y > other_rect.y) {
                m_is_on_ground = false;
                m_is_on_slope = false;
                                
                velocity_y(std::abs(velocity().y) * 0.75F);
                
                sound::position(m_bump_head_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_bump_head_sound_id);
                reset_anim(m_fall_anim_id);
            }
        } else if (other_type == Type::CLIP_L) {
        } else if (other_type == Type::CLIP_R) {
        } else if (other_type == Type::CLIP_LEDGE) {
            if (velocity().y < 0.0F && velocity().y > -10.0F && our_rect.y < other_rect.h - 4.0F ||
                velocity().y > 0.0F && velocity().y <  10.0F && our_rect.h > other_rect.y + 4.0F) {
                collide_x(our, other);
                return;
            }

            /*console::log(" ", our_rect.y, " ", our_rect.h, "  ", other_rect.y, " ", other_rect.h, "\n");*/
            add_position_y(-overlap_y);
            if (m_state == State::SWIM) {
                if (m_num_jumps == 0) {
                    velocity_y(0.0F);
                    moved_velocity_y(0.0F);
                    return;
                }
            }

            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
                moved_velocity_x(0.0F);
                velocity_y(0.0F);
                moved_velocity_y(0.0F);
            } else if (our_rect.h > other_rect.h) {
                velocity_y(std::abs(velocity().y) * 0.75F);
                sound::position(m_bump_head_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_bump_head_sound_id);
            }
        } else if (other_type == Type::CLIP_U || other_type == Type::SLOPE_U) {
            if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::SLOPE_U;
        } else if (is_conduit(other_type)) {
        } else if (other_type == Type::FROG) {
            if (m_state == State::SWING) {
                collide_x(our, other);
                return;
            }
            if (our_rect.y < other_rect.y + 4.0F && our_velocity.y > 0.0F) {
                add_position_y(-overlap_y);

                console::log("Player::collide_y frog\n");
                if (our_velocity.y >= 6.0F) {
                    other.owner->hurt(this);

                    reset_anim(m_jump_spin_anim_id);
                    m_is_down_thrusting = false;
                    m_is_on_ground = false;
                    m_is_on_slope = false;
                    m_num_jumps = 1;
                    //velocity_y(velocity().y * -0.9F);
                    velocity_y(velocity().y * -0.7F);
                } else {
                    m_is_on_ground = true;
                    m_is_on_slope = false;
                    velocity_y(0.2F);
                    moved_velocity(other_velocity);
                }
            } else {
                console::log("Player::collide_y frog above\n");
            }

            //console::log("other vx: ", other_velocity.x, " ", our.owner->moved_velocity().x, "\n");        

            //if (our_rect.y < other_rect.y) {
            //    add_position_y( -overlap_y );// / 4.0F;

            //    if (velocity().y >= 9.0F) {
            //        //bounce
            //        velocity_y(velocity().y * -0.9F);
            //        moved_velocity_y(0.0F);
            //        return;
            //    }
            //    velocity_y(0.2F;
            //    

            //    moved_velocity(other_velocity);
            //    m_is_on_ground = true;
            //    m_is_on_slope = false;
            //}
            //else {
            //    //add_position_y( -overlap_y );
            //}
        } else if (other_type == Type::LEVEL_L_0 || other_type == Type::LEVEL_R_0) {
            collide_x(our, other);
        } else if (other_type == Type::LEVEL_CENTER) {
            collide_x(our, other);
        } else if (other_type == Type::MOLE) {
            if (other.owner->state() == State::EXIT) {
                max_velocity_y(10.0F);
                velocity_y(-7.0F);
                reset_anim(m_jump_spin_anim_id);
                m_is_down_thrusting = false;
                m_is_on_ground = false;
                m_is_on_slope = false;
                m_num_jumps = 1;
                m_time_left_bouncing = m_time_to_bounce;

                if (!sound::is_playing(m_jump_sound_id)) {
                    sound::position(m_jump_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound::play(m_jump_sound_id);
                }
            }
        } else if (other_type == Type::PARTICLE_BRICK) {
            if (our_rect.y < other_rect.y) {
                //add_position_y( -overlap_y );
                //velocity_y(0.0F);
                //moved_velocity(other_velocity);
                //m_is_on_ground = true;
            }
        } else if (other_type == Type::PARTICLE_SHOT) {
            console::log("\n\nentity::Player::collide_y() ", to_string(other_type), "\n\n");
            hurt(other.owner);
        } else if (other_type == Type::PLAYER) {
            return;
            add_position_y(-overlap_y);

            //console::log("overlap_y: ", overlap_y, "\n");

            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
                //time_left_holding_jump = time_to_hold_jump;

                moved_velocity(other_velocity);
                velocity_y(0.0F);
                //position().y -= 1.0F;

                //console::log("velocity: ", velocity.x, " ", velocity.y, "\n");
                //velocity() = { 0.0F, 0.0F };
                //velocity().x -= (velocity().x - velocity.x);
            } else {
                //object->is_on_ground = true;

                //object->moved_velocity() = velocity();
                //if (!is_on_ground && velocity().y < 0.0F) {
                //    velocity_y(velocity().y * -1.0F);
                //}
                //moved_velocity(other_velocity);
                //velocity_y(0.0F);
            }


            //velocity() = { 0.0F, 0.0F };


            //velocity_y(velocity.y;
            //velocity_y(0.0F);
        } else if (other_type == Type::SLOPE_L_1X1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            if (m_state == State::SWIM) {
                if (velocity().y > 0.0F && std::abs(velocity().x) < velocity().y) {
                    velocity_x(-velocity().y * 0.5F);
                    velocity_y(velocity().y * 0.95f);
                }
                return;
            }
            if (velocity().x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F);
            }
            //if (velocity().x < 0.0F) {
            //velocity_y(-velocity().x);
            //}

            velocity_y(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_1X1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            if (m_state == State::SWIM) {
                if (velocity().y > 0.0F && std::abs(velocity().x) < velocity().y) {
                    velocity_x(velocity().y * 0.5F);
                    velocity_y(velocity().y * 0.95f);
                }
                return;
            }
            if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F);
            }
            //if (velocity().x > 0.0F) {
            //velocity_y(velocity().x);
            //}
            velocity_y(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            if (m_state == State::SWIM) {
                if (velocity().y > 0.0F && std::abs(velocity().x) < velocity().y) {
                    velocity_x(velocity().y * -0.5F);
                    velocity_y(velocity().y * 0.95f);
                }
                return;
            }
            if (velocity().x > max_velocity().x * 0.9F) {
                velocity_x(max_velocity().x * 0.9F);
            }
            //if (velocity().x < 0.0F) {
            //velocity_y(velocity().x / -2.0F);
            //}
            velocity_y(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            if (m_state == State::SWIM) {
                if (velocity().y > 0.0F && std::abs(velocity().x) < velocity().y) {
                    velocity_x(velocity().y * 0.5F);
                    velocity_y(velocity().y * 0.95F);
                }
                return;
            }
            if (velocity().x < -max_velocity().x * 0.9F) {
                velocity_x(-max_velocity().x * 0.9F);
            }
            //if (velocity().x > 0.0F) {
            //velocity_y(velocity().x / 2.0F);
            //}
            velocity_y(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SPRING_U) {
            if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);
            max_velocity_y(10.0F);
            velocity_y(-5.0F - velocity().y / 4.0F);
            m_is_down_thrusting ? reset_anim(m_jump_spin_anim_id) : reset_anim(m_jump_anim_id);
            m_is_down_thrusting = false;
            m_is_on_ground = false;
            m_is_on_slope = false;
            m_num_jumps = 1;
            m_time_left_bouncing = m_time_to_bounce;
        } else if (other_type == Type::TRAIN) {
            if (other.owner->time_left_alive() > 0) {
                m_is_on_ledge = false;
            }
            //console::log("hello train ", aabb::name_as_string(other_name), "\n");
            if (velocity().y < 0.0F || other_name == aabb::Name::BONE ||
                //our_rect.h > other_rect.h
                //||
                position().y + 20 > other_rect.y
                ) return;

            moved_velocity(other_velocity);            
            velocity_y(1.0F);
            add_position({ 0.0F, -overlap_y / 6.0F }); // so we don't fly off at turns
            //} else {
                //velocity_y(0.0F);
                //add_position_y( -overlap_y );
            //}
            m_is_on_ground = true;

        } else if (is_water_line(other_type)) {
            //next_state(State::SWIM);

            m_is_down_thrusting = false;

            bool is_to_splash = false,
                is_upwards = false;

            if (velocity().y > 0.0F && our_rect.h > other_rect.y + 4 && !m_is_on_ground) {
                if (!sound::is_playing(m_water_enter_sound_id) && !(m_state == State::SWIM && m_num_jumps == 0)) {
                    sound::position(m_water_enter_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound::play(m_water_enter_sound_id);
                    is_to_splash = true;
                }
                velocity_y(velocity().y + 1.0F);
                m_next_state = State::SWIM;
                //m_prev_state = m_state;
                //m_next_state = State::SWIM;
                m_num_jumps = 0;
                m_water_line_y = other.owner->position_on_level().y;

            } else if (velocity().y < 0.0F && m_state == State::SWIM) {
                //console::log("is_pressed(key_jump): ", is_pressed(key_jump), "\n");
                if (velocity().y > -1.5F || input::is_pressed(m_input_id, key_jump)) {
                    velocity_y(velocity().y * -0.5F);
                    m_time_in_state = 0;
                } else if (velocity().y <= -1.5F) {
                    is_upwards = true;
                    if (!sound::is_playing(m_water_exit_sound_id)) {
                        sound::position(m_water_exit_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                        sound::play(m_water_exit_sound_id);
                        is_to_splash = true;
                    }
                    if (m_num_jumps == 0) {
                        velocity_y(velocity().y * 1.5F);
                        velocity_y(velocity().y + other_velocity.y * 0.5F);
                    }
                    if (velocity().y > -3.0F) {
                        velocity_y(-3.0F);
                    }
                    m_num_jumps = 1;
                    m_time_in_state = 0;
                    m_is_on_ground = false;

                    /*if (velocity().y <= -4.0F) {
                        velocity_y(-4.0F);
                    }*/
                    //next_state(State::RUN);

                    //m_prev_state = m_state;
                    //m_next_state = State::RUN;
                }
            }
            if (is_to_splash) {
                Vec2F pos = Vec2F{ our_rect.x, other_rect.y };
                spawn_fan(225.0F, 315.0F, 16,
                    particle::Type::DROP, pos + Vec2F{ 0.0F, is_upwards ? -12.0F : -6.0F },
                    Vec2F{ velocity().x * 0.9F, -std::abs(velocity().y * 0.1F) },
                    is_upwards ? 2.0F : 1.5F);
            }

        }
    }
}