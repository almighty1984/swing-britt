module entity.player;
import collider.aabb;
import camera;
import console;
import health;
import particle;

namespace entity {
    void Player::run() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            sprite::is_hidden(m_sprite_id, false);
            m_water_line_y = 0.0F;
            if (m_prev_state != State::RUN) {
                m_prev_state = State::RUN;
                m_next_state = State::RUN;
                console::log("entity::Player::run\n");
            }
            if (sprite::rotation(m_sprite_id) != 0.0F) {
                sprite::is_leftward(m_sprite_id, velocity().x < 0.0F);
                sprite::is_upended(m_sprite_id, false);
                sprite::rotation(m_sprite_id, 0.0F);
            }
        }
        for (auto& i : m_aabb_ids) {
            if (aabb::name(i) == aabb::Name::BODY) {
                aabb::is_active(i, true);
            } else {
                aabb::is_active(i, false);
            }
        }

        if (is_pressed(input::Key::CTRL)) {
            input::lock(m_input_id, key_up);
            input::lock(m_input_id, key_down);
            input::lock(m_input_id, key_left);
            input::lock(m_input_id, key_right);
        } else {
            if (!is_pressed(key_up)) unlock(key_up);
            if (!is_pressed(key_down)) unlock(key_down);
            if (!is_pressed(key_left)) unlock(key_left);
            if (!is_pressed(key_right)) unlock(key_right);
        }

        if (is_pressed(input::Key::H)) {
            release(input::Key::H);
            health::add_amount(m_health_id, -8.0f);
            console::log("heath: ", health::amount(m_health_id), "\n");
        }
        if (is_pressed(input::Key::J)) {
            release(input::Key::J);
            health::add_amount(m_health_id, 8.0f);
            //console::log("heath: ", health::get(m_health_id)->amount, "\n");
        }
        if (is_pressed(input::Key::B)) {
            release(input::Key::B);
            spawn_fan(180.0F, 0.0F, 4, particle::Type::DROP_BLOOD,
                position() + Vec2F{ 6.0F, -4.0F },
                velocity() + moved_velocity(),
                3.0F);
        }

        //console::log("is carrying: ", m_is_carrying, "\n");

        if (is_pressed(key_melee) && !m_is_carrying && !m_is_ducking && !m_is_on_ledge && !is_locked(key_jump)) {
            if (!is_locked(key_melee)) {
                //console::log("entity::Player::run() attack melee\n");
                lock(key_melee);
                m_time_left_melee = m_time_to_melee;
                reset_anim(m_melee_anim_id);

                sound::position(m_melee_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_melee_sound_id);
            }
        } else {
            unlock(key_melee);
        }

        if (m_time_left_melee > 0) {
            if (m_time_left_melee == m_time_to_melee) {
                particle::spawn({ .parent = this,
                                           .type = particle::Type::MELEE,
                                           .position = position() + Vec2F{ sprite::is_leftward(m_sprite_id) ? -16.0F : 16.0F, 0.0F},
                                           .velocity = velocity() + Vec2F{ sprite::is_leftward(m_sprite_id) ? -2.0F : 2.0F, 0.0F}
                    });
            }
            --m_time_left_melee;

            if (m_is_on_ground) {
                velocity_x(velocity().x * 0.9F);
            }
        }

        //console::log("player position: ", position().x, " ", position().y, "\n");

        if (m_time_left_bouncing > 0) {
            //console::log("time left bouncing: ", (int)m_time_left_bouncing, "\n");
            --m_time_left_bouncing;
            if (m_time_left_bouncing == 0 && m_is_on_ground) {
                max_velocity(m_ground_velocity_limit);
            }
        }


        if (m_time_left_until_down_thrust > 0) {
            --m_time_left_until_down_thrust;
            velocity({ 0.0F, 0.0F });
            if (m_time_left_until_down_thrust > 0) {
                return;
            } else if (!m_is_down_thrusting) {
                m_is_down_thrusting = true;
                max_velocity_y(10.0F);
                velocity_y(10.0F);
            }
        }

        if (m_is_down_thrusting) {
            if (m_is_on_ground) {
                m_is_down_thrusting = false;
                m_time_left_hitting_ground = m_time_to_hit_ground;
                reset_anim(m_hit_ground_anim_id);

                sound::position(m_hit_ground_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_hit_ground_sound_id);
            } else {
                set_anim(m_down_thrust_anim_id);
                return;
            }
        }

        if (m_time_left_hitting_ground > 0) {
            if (m_time_left_hitting_ground == m_time_to_hit_ground) {
                spawn_fan(180.0F, 0.0F, 8, particle::Type::DROP_BLOOD, position() + Vec2F{ 0.0F, 4.0F }, {}, 5.0F);
                spawn_fan(180.0F, 0.0F, 8, particle::Type::DROP_BLOOD, position() + Vec2F{ 0.0F, 4.0F }, {}, 4.0F);

                spawn_down_thrust(position() + Vec2F{ 8.0f, 8.0f });

                camera::init_shake(12, { 16, 24 }, { 24, 32 }, 0.05F);
            }
            --m_time_left_hitting_ground;
            velocity_x(0.0F);
            //velocity_y(0.0F);
            return;
        }


        if (m_is_on_ground && !m_is_ducking && m_time_left_rising == 0 &&
            !is_pressed(key_left) && !is_pressed(key_right) && m_time_left_melee == 0
            //velocity().x >= -acceleration().x && velocity().x <= acceleration().x
            ) {
            set_anim(m_idle_anim_id);
        }

        m_is_sliding_ground = false;
        if (m_is_on_ground && m_time_left_melee == 0 && m_time_left_skidding == 0) {
            if (velocity().x >= -acceleration().x && velocity().x <= acceleration().x) {
                if (!m_is_ducking && m_time_left_rising == 0) {
                    set_anim(m_idle_anim_id);
                }
                if (m_is_ducking) {
                    set_anim(m_duck_anim_id);
                }
            } else {
                if (m_is_ducking) {
                    if (m_time_left_rising == m_time_to_rise) {
                        if (!m_is_carrying &&
                            velocity().x < -m_slide_ground_velocity_limit.x ||
                            velocity().x > m_slide_ground_velocity_limit.x) {
                            m_is_sliding_ground = true;
                        }

                        if (m_is_sliding_ground) {
                            set_anim(m_slide_ground_anim_id);
                        } else {
                            anim::source_x(m_duck_anim_id, anim::source(m_duck_anim_id).w * 4);
                            set_anim(m_crawl_anim_id);
                            if (velocity().x < max_velocity().x * -0.5F) {
                                velocity({ max_velocity().x * -0.5F, velocity().y });
                            } else if (velocity().x > max_velocity().x * 0.5F) {
                                velocity({ max_velocity().x * 0.5F, velocity().y });
                            }
                        }
                    }
                } else {
                    anim::source_x(m_slide_ground_anim_id, 0);
                    //console::log("run walk\n");
                    is_pressed(key_sprint) ? set_anim(m_run_anim_id) : set_anim(m_walk_anim_id);
                }
            }
        }

        if (is_pressed(key_left) && !is_locked(key_left)) {
            if (!m_is_sliding_ground && (velocity().x > -m_ground_velocity_limit.x * is_pressed(key_sprint) ? 1.2F : 1.0F)) {
                //console::log("max_velocity().x: ", max_velocity().x, "\n");
                //console::log("acceleration().x: ", acceleration().x, "\n");
                //console::log("m_ground_velocity_limit.x: ", m_ground_velocity_limit.x, "\n");
                add_velocity_x(-acceleration().x);
            }

            if (m_is_on_ground && !m_is_ducking) {
                if (velocity().x >= 0.0F) {
                    m_time_left_skidding = m_time_to_skid;
                    if (!sprite::is_leftward(m_sprite_id)) {
                        anim::source_x(m_skid_anim_id, 0);
                    }
                }
                if (velocity().x < -2.0F) {
                    m_time_left_skidding = 0.0F;
                }
            }
            if (!m_is_sliding_wall && !m_is_on_ledge && m_time_left_jumping_wall == 0) {
                sprite::is_leftward(m_sprite_id, true);
            }
        }
        if (is_pressed(key_right) && !is_locked(key_right)) {
            if (!m_is_sliding_ground && (velocity().x < m_ground_velocity_limit.x * is_pressed(key_sprint) ? 1.2F : 1.0F)) {
                add_velocity({ acceleration().x, 0.0F });
            }
            if (m_is_on_ground && !m_is_ducking) {
                if (velocity().x <= 0.0F) {
                    m_time_left_skidding = m_time_to_skid;
                    if (sprite::is_leftward(m_sprite_id)) {
                        anim::source_x(m_skid_anim_id, 0);
                    }
                }
                if (velocity().x > 2.0F) {
                    m_time_left_skidding = 0.0F;
                }
            }
            if (!m_is_sliding_wall && !m_is_on_ledge && m_time_left_jumping_wall == 0) {
                sprite::is_leftward(m_sprite_id, false);
            }
        }
        if (m_time_left_skidding > 0 && m_time_left_melee == 0 && !m_is_sliding_wall) {
            if (m_time_left_skidding == m_time_to_skid) {
                sound::position(m_skid_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_skid_sound_id);
            }
            --m_time_left_skidding;
            set_anim(m_skid_anim_id);
            //console::log("entity::Player::run m_time_left_skidding: ", m_time_left_skidding, "\n");
        } else {
            anim::source_x(m_skid_anim_id, 0);
        }

        acceleration({ 0.2F,  0.2F });
        deceleration({ 0.04F, 0.04F });

        if (m_is_on_ground) {
            max_velocity(m_ground_velocity_limit);
        }
        //else {
            //transform::max_velocity(m_transform_id, m_air_velocity_limit);
        //}




        add_velocity({ 0.0F, acceleration().y });
        //moved_velocity().y += acceleration().y;

        if (moved_velocity().y < 0.0F) {
            add_moved_velocity_y(acceleration().y);
        }


        //console::log("moved y: ", moved_velocity().y, "\n");

        if (m_is_on_ground) {
            if (m_is_sliding_ground) {
                deceleration_x(0.025f);
            } else {
                deceleration_x(0.1F);
            }
            acceleration_x(0.2F);
        } else {
            deceleration_x(0.0F);
            acceleration_x(0.1F);
        }

        if (m_is_on_ground || m_is_on_ledge) {
            //m_velocity_at_jump = 0.0F;
            m_is_hovering = false;
            if (m_time_left_jump_again > 0) {
                m_time_left_jump_again -= 1;
            } else {
                m_num_jumps = 0;
            }

            if (is_pressed(key_sprint)) {
                max_velocity_x(m_ground_velocity_limit.x * 1.2F);
            } else {
                max_velocity_x(m_ground_velocity_limit.x);
            }
            if (m_is_carrying) {
                max_velocity_x(max_velocity().x * 0.9F);
            }
        }


        if (velocity().y > acceleration().y + (m_is_on_slope ? max_velocity().y : 1.0F)) {
            //if (velocity().y >= 1.0F && !m_is_on_slope) {
            m_is_on_ground = false;
            m_is_on_slope = false;
            m_is_ducking = false;
            //m_is_on_ledge = false;
        }
        if (m_is_on_slope && velocity().y >= max_velocity().y) {
            m_is_on_slope = false;
        }
        if (m_is_on_ground || velocity().y > 2.0F) {
            m_is_climbing_ledge = false;
        }


        if (!m_is_on_ground && !m_is_ducking && !m_is_sliding_wall && !m_is_on_ledge && !m_is_climbing_ledge && m_time_left_melee == 0) {
            if (velocity().y > acceleration().y) {
                if (is_pressed(key_jump) && !is_locked(key_jump) && m_time_left_jumping_wall == 0.0F) {
                    lock(key_jump);
                    if (moved_velocity().y != 0.0F) {
                        add_velocity({ 0.0F, moved_velocity().y });
                        moved_velocity_y(0.0F);
                    }
                    m_is_hovering = true;

                    m_time_left_ducking = 0;
                    m_time_left_rising = 0;
                } else if (m_current_anim_id != m_jump_spin_anim_id) {
                    reset_anim(m_fall_anim_id);
                    anim::speed(m_fall_anim_id, velocity().y / 10.0F);
                }
            }
        }

        if (is_pressed(key_down) && !is_locked(key_down) && !m_is_on_ledge) {
            if (m_is_carrying) {
                console::log("drop carried\n");
                m_is_carrying = false;
                lock(key_down);
            } else if (m_is_on_ground) {
                m_time_left_ducking = m_time_to_duck;
                release(key_sprint);
            } else {
                if (m_time_left_ducking == 0 && m_time_left_rising == 0) {
                    release(key_down);
                    release(key_jump);
                    m_time_left_until_down_thrust = m_time_until_down_thrust;
                    reset_anim(m_down_thrust_anim_id);
                    if (sound::is_playing(m_hover_sound_id)) {
                        sound::stop(m_hover_sound_id);
                    }
                    sound::position(m_down_thrust_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound::play(m_down_thrust_sound_id);
                }
            }
        }

        if (!m_is_ducking && !m_is_on_ledge) {
            if (is_pressed(key_up) && !is_locked(key_up)) {
                release(key_up);
                spawn(particle::Type::INTERACT, position() + Vec2F{ -8.0f, 0.0f }, velocity() + moved_velocity());
            }
        }

        if (m_time_left_lever > 0) {
            --m_time_left_lever;
            set_anim(m_lever_anim_id);
            velocity({ 0.0F,0.0F });
        }


        if (m_is_on_ledge) {
            m_is_on_ground = false;
            m_num_jumps = 0;

            set_anim(m_ledge_grab_anim_id);

            sprite::is_leftward(m_sprite_id, m_is_wall_to_left);

            deceleration_x(0.0F);
            acceleration_x(0.0F);
            velocity_x(0.0F);
            velocity_y(0.0F);

            if (is_pressed(key_up) && !is_locked(key_up)) {
                release(key_up);
                sound::position(m_ledge_climb_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_ledge_climb_sound_id);

                m_is_on_ledge = false;
                m_is_climbing_ledge = true;
                velocity_y(-2.8F);
                reset_anim(m_ledge_climb_anim_id);
                ++m_num_jumps;
                if (m_num_jumps > 2) {
                    m_num_jumps = 2;
                }
                m_time_left_jump_again = m_time_to_jump_again;
                //m_time_left_ducking = m_time_to_duck;
            }
            if (is_pressed(key_down) && !is_locked(key_down)) {
                release(key_down);
                m_is_on_ledge = false;
            }
            if (is_pressed(key_jump)) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_is_on_ledge = false;
            }
        }

        /*if (id == 0) {
            console::log("position: ", position().x, " ", position().y, "\n");
        }*/


        if (m_time_left_jumping_wall > 0) {
            //set_anim(m_jump_anim_id);

            velocity({ 0.0F, 0.0F });
            --m_time_left_jumping_wall;
            if (is_pressed(key_left)) {
                m_is_to_wall_jump_left = true;
            }
            if (is_pressed(key_right)) {
                m_is_to_wall_jump_left = false;
            }
            if (m_time_left_jumping_wall == 0) {
                if (m_is_to_wall_jump_left) {
                    //console::log("m_is_wall_to_left: ", m_is_wall_to_left, "\n");
                    velocity_x(m_is_wall_to_left ? 1.0F : -1.5F);
                } else {
                    //console::log("m_is_wall_to_left: ", m_is_wall_to_left, "\n");
                    velocity_x(m_is_wall_to_left ? 1.5F : -1.0F);
                }
                //console::log("velocity.y: ", velocity().y, "\n");
                m_is_on_ground = true;
                m_is_on_ledge = false;

                unlock(key_jump);
            }
        }


        if (is_pressed(key_jump)) {
            if (m_is_on_ground && !is_locked(key_jump) && !m_is_ducking) {
                lock(key_jump);
                m_time_left_jump_again = m_time_to_jump_again;
                m_is_on_ground = false;
                m_is_on_slope = false;
                m_is_hovering = false;
                m_is_sliding_ground = false;
                m_time_left_holding_jump = m_time_to_hold_jump;
                m_time_left_rising = 0;
                //velocity() += moved_velocity();
                //moved_velocity({});

                /*if (velocity().x <= -m_ground_velocity_limit.x || velocity().x >= m_ground_velocity_limit.x) {
                    m_velocity_at_jump = velocity().x;
                    console::log("velocity at jump: ", m_velocity_at_jump, "\n");
                }*/

                add_position({ 0.0F,-2.0F });

                ++m_num_jumps;
                if (m_is_carrying) {
                    m_num_jumps = 1;
                }
                if (m_num_jumps == 3) {
                    m_time_left_skidding = 0;
                    m_num_jumps = 0;
                    velocity_y(-3.0F);
                    reset_anim(m_jump_spin_anim_id);
                    sound::pitch(m_jump_sound_id, 1.122454545454545F);
                } else {
                    if (m_num_jumps == 2) {
                        velocity_y(-2.2F);
                        reset_anim(m_jump_anim_id);
                        sound::pitch(m_jump_sound_id, 1.059454545454545F);
                    } else {
                        velocity_y(-2.0F);
                        reset_anim(m_jump_anim_id);
                        sound::pitch(m_jump_sound_id, 1.0F);
                    }
                    if (m_time_left_skidding > 0) {
                        m_time_left_skidding = 0;
                        reset_anim(m_jump_skid_anim_id);
                    }
                }
                sound::position(m_jump_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_jump_sound_id);
            } else {
                if (m_time_left_holding_jump > 0 && velocity().y < 0.0F) {
                    --m_time_left_holding_jump;
                    add_velocity({ 0.0F,-0.3F });
                    //console::log("Player holding jump: ", m_time_left_holding_jump, "\n");
                } else {
                    //release(key_jump);
                    m_time_left_holding_jump = 0;
                }
            }
        } else {
            unlock(key_jump);
            m_is_hovering = false;
        }

        if (m_current_anim_id == m_run_anim_id || m_current_anim_id == m_walk_anim_id) {
            if (velocity().x < 0.0F) {
                anim::speed(m_current_anim_id, velocity().x / -5.0F);
            } else {
                anim::speed(m_current_anim_id, velocity().x / 5.0F);
            }
            if (anim::speed(m_current_anim_id) < 0.2F) {
                anim::speed(m_current_anim_id, 0.2F);
            }
        }


        if (m_is_hovering) {
            cF32 limit_y = m_is_carrying ? 1.5F : 1.0F;

            if (velocity().y >= limit_y) {
                velocity_y(limit_y);
                set_anim(m_hover_anim_id);
            }
            if (sound::is_stopped(m_hover_sound_id)) {
                sound::position(m_hover_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_hover_sound_id);
            }
        } else {
            sound::stop(m_hover_sound_id);
        }


        if (!m_is_sliding_wall && sound::is_playing(m_slide_wall_sound_id)) {
            sound::stop(m_slide_wall_sound_id);
        }
        if ((!is_pressed(key_left) && !is_pressed(key_right)) ||
            m_time_left_jumping_wall > 0 || m_is_on_ground || m_is_on_slope ||
            velocity().x < -acceleration().x || velocity().x > acceleration().x) {
            m_is_sliding_wall = false;
        } else {
            if (m_is_sliding_wall && !m_is_on_ledge) {
                set_anim(m_slide_wall_anim_id);
                m_time_left_ducking = 0;
                m_time_left_rising = 0;
                if (sound::is_stopped(m_slide_wall_sound_id)) {
                    //console::log("sliding wall\n");
                    sound::play(m_slide_wall_sound_id);
                }
                m_is_on_ground = false;
            }
        }


        if (anim::source(m_jump_spin_anim_id).x >= anim::source(m_jump_spin_anim_id).w * 4) {
            anim::first_frame(m_jump_spin_anim_id, 4);
        } else {
            anim::first_frame(m_jump_spin_anim_id, 0);
        }


        if (m_current_anim_id == m_run_anim_id || m_current_anim_id == m_walk_anim_id) {
            if (anim::source(m_current_anim_id).x == 128 || anim::source(m_current_anim_id).x == 384) {
                sound::position(m_step_grass_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_step_grass_sound_id);
            }
        }


        //console::log(start::to_string(m_next_level), "\n");

        if (is_pressed(input::Key::NUM_5)) {
            release(input::Key::NUM_5);
            m_prev_state = m_state;
            m_next_state = State::SWIM;
            m_time_left_in_state = 20;
        }


        /*if (m_velocity_at_jump < 0.0F && velocity().x < m_velocity_at_jump) {
            velocity_x(m_velocity_at_jump;
        }
        if (m_velocity_at_jump > 0.0F && velocity().x > m_velocity_at_jump) {
            velocity_x(m_velocity_at_jump;
        }*/

        //console::log("swing_position: ", (position().x - m_swing_position.x), "\n");

        if (m_time_left_ducking > 0) {
            //console::log("Player::run() time_left_ducking: ", (int)m_time_left_ducking, "\n");
            --m_time_left_ducking;

            if (!m_is_sliding_ground) {
                if (!is_pressed(key_left) && !is_pressed(key_right)) {
                    m_is_ducking ? set_anim(m_duck_anim_id) : reset_anim(m_duck_anim_id);
                } else {
                    set_anim(m_crawl_anim_id);
                }
            }

            m_is_ducking = m_time_left_ducking > 0;

            anim::source_x(m_rise_anim_id, 0);
            m_time_left_rising = m_time_to_rise;
        }

        if (!m_is_ducking) {
            anim::source_x(m_duck_anim_id, 0);
            if (m_time_left_rising > 0) {
                --m_time_left_rising;
                set_anim(m_rise_anim_id);
            }
        }

        if (m_next_state == State::SWIM) {
            if (m_time_left_in_state > 0) --m_time_left_in_state;
            if (m_time_left_in_state == 0) {
                m_state = State::SWIM;
                m_is_first_state_update = true;
                sprite::is_upended(m_sprite_id, sprite::is_leftward(m_sprite_id));
                sprite::is_leftward(m_sprite_id) ? sprite::rotation(m_sprite_id, 180.0F) : sprite::rotation(m_sprite_id, 0.0F);
            }
        }
        if (m_next_state == State::SWING && m_time_left_in_state == 0) {
            m_state = State::SWING;
            m_is_first_state_update = true;
        }
    }
}