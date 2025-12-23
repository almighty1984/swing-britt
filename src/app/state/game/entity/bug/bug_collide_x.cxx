module;
#include <cmath>
#include <sstream>
module entity.bug;
import collider.aabb;
import console;
import sound;
import particle;

namespace entity {
    void Bug::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent || is_dead() || other.owner->is_dead()) return;

        cType other_type = other.owner->type();

        aabb::Name other_name = aabb::name(other.id);

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };


        if (is_clip(other_type) ||
            (other_type == Type::BRICK && !other.owner->is_tossed())) {
            //console::log(to_string(other_type), " has parent\n");
            if (our_rect.w > other_rect.w) {
                m_is_near_wall_L = true;
            }
            if (our_rect.x < other_rect.x) {
                m_is_near_wall_R = true;
            }
        }
        if (m_parent) {
            return;
        }

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();


        cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        if (is_arch(other_type)) {
            if (m_parent || other.owner->parent() || is_hurting()) return;
            console::log("Bug::collide_x ", to_string(other_type), "\n");
            add_position_x(-overlap_x);
            if (std::abs(our_velocity.x - other_velocity.x) >= 2.0F) {

                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                velocity_x(velocity().x * -0.9F);
                spawn(particle::Type::HIT, hit_pos, velocity());
                hurt(other.owner);
                return;
            }
        } else if (other_type == Type::BUG) {
            if (other.owner->parent()) return;
            if (m_state == State::UPENDED || m_state == State::BOUNCE) {
                if (other_velocity.y > 1.0F) {
                    m_state = State::UPENDED;
                    m_next_state = State::BOUNCE;
                    return;
                }
            }

            add_position_x(-overlap_x);
            velocity_x(velocity().x * -1.0F);
            moved_velocity_x(0.0F);
            sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));
        } else if (other_type == Type::BRICK) {
            //if (m_parent || other.owner->parent()) return;
            //console::log("Bug::collide_x brick\n");

            add_position_x(-overlap_x);
            //if (std::abs(our_velocity.x - other_velocity.x) >= 2.0F) {

            //    cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
            //                       (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

            //    velocity_x(other_velocity.x);
            //    other.owner->velocity_x(our_velocity.x);
            //    spawn(particle::Type::HIT, hit_pos, velocity());
            //    hurt(other.owner);
            //    //other.owner->hurt(this);
            //    return;
            //}
            //
            //velocity_x(velocity().x * -0.9F);
            //moved_velocity_x(0.0F);
        } else if (other_type == Type::CLIP || other_type == Type::CLIP_LEDGE) {
            //if ((velocity().x < 0.0F && !m_is_near_wall_L) ||
            //    (velocity().x > 0.0F && !m_is_near_wall_R)) {
            add_position_x(-overlap_x);
            //}

            moved_velocity_x(0.0F);
            //if (m_time_left_turning > 0 || !m_is_on_ground) return;

            if (velocity().x <= -2.0F || velocity().x >= 2.0F) {
                velocity_x(velocity().x * -0.9F);
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                spawn(particle::Type::HIT, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::HURT;

                return;
            }
            if (m_state == State::UPENDED) {
                console::log("Bug::collide_x velocity ", velocity().x, "\n");
                if (velocity().x < 0.0F) {
                    m_is_near_wall_L = true;
                } else if (velocity().x > 0.0F) {
                    m_is_near_wall_R = true;
                }
                velocity_x(0.0F);
            }
            if (m_state == State::WALK) {
                if (velocity().x < 0.0F) {
                    if (our_rect.x > other_rect.x) {
                        velocity_x(1.0F);
                        //position().x += 2.0F;
                        //m_time_left_turning = 30;
                    }
                } else if (velocity().x > 0.0F) {

                    if (our_rect.x < other_rect.x) {
                        velocity_x(-1.0F);
                        //position().x -= 2.0F;
                        //m_time_left_turning = 30;
                    }
                }
                sprite::is_leftward(m_sprite_id, velocity().x < 0.0F);
            }
        } else if (other_type == Type::CLIP_L || other_type == Type::CLIP_LD) {
            if (velocity().x + moved_velocity().x <= 0.0F) return;
            add_position_x(-overlap_x);
            moved_velocity_x(0.0F);

            if (m_state == State::TOSSED ||
                m_state == State::UPENDED && (velocity().x > 1.0F)) {
                velocity_x(velocity().x * -0.9F);
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                        (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                spawn(particle::Type::HIT, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::HURT;
                sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::play(m_melee_sound_id);
                return;
            }
            velocity_x(velocity().x * -1.0F);
            sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));

        } else if (other_type == Type::CLIP_R || other_type == Type::CLIP_RD) {
            if (velocity().x + moved_velocity().x >= 0.0F) return;

            add_position_x(-overlap_x);
            moved_velocity_x(0.0F);

            if (m_state == State::TOSSED ||
                m_state == State::UPENDED && (velocity().x < -1.0F)) {
                velocity_x(velocity().x * -0.9F);
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };
                spawn(particle::Type::HIT, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::HURT;

                sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::play(m_melee_sound_id);
                return;
            }
            velocity_x(velocity().x * -1.0F);
            sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));

        } else if (other_type == Type::FROG) {
            add_position_x(-overlap_x);
            if (m_state == State::TOSSED) {
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                velocity_x(velocity().x * -0.9F);
                spawn(particle::Type::HIT, hit_pos, velocity());
                hurt(other.owner);
                other.owner->hurt(this);
                m_next_state = State::HURT;

                //velocity().y *= -1.0f;

                sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::play(m_melee_sound_id);
                return;
            }

        } else if (other_type == Type::PARTICLE_INTERACT) {
        } else if (other_type == Type::PLAYER) {
            //console::log("entity::Bug::collide_x player overlap_x: ", overlap_x, "\n");
            if (other_velocity.y > 3.0F) {
                collide_y(our, other);
                return;
            }
            
            if (m_state == State::TOSSED) return;
            //console::log("aabb name: ", aabb::to_string(other_name), "\n");

            if (other_rect.h < our_rect.y + 2.0F + other_velocity.y) return;

            if (!m_is_near_wall_L && !m_is_near_wall_R) {
                add_position_x(-overlap_x);
                velocity_x(other_velocity.x);
            } else {
                if (m_is_near_wall_L) {
                    other.owner->add_position({ 2.0F, 0.0F });
                    other.owner->velocity_x(2.0F);
                } else if (m_is_near_wall_R) {
                    other.owner->add_position({ -2.0F, 0.0F });
                    other.owner->velocity_x(-2.0F);
                }
            }

            if (other.owner->state() == State::SWING) {
                velocity_x(-other.owner->rotation_speed() * 0.5F);
                hurt(other.owner);
                return;
            }


            if (m_state == State::WALK) {
                add_position_x(-overlap_x);
                cVec2F vel_sum = our_velocity + other_velocity;

                if (vel_sum.x <= 0.1F && !sprite::is_leftward(m_sprite_id)) {
                    sprite::is_leftward(m_sprite_id, true);
                    velocity_x(-1.0F);
                    velocity_y(-1.0F);
                    other.owner->velocity_x(velocity().x * -0.9F);
                } else if (vel_sum.x >= -0.1F && sprite::is_leftward(m_sprite_id)) {
                    sprite::is_leftward(m_sprite_id, false);
                    velocity_x(1.0F);
                    velocity_y(-1.0F);
                    other.owner->velocity_x(velocity().x * -0.9F);
                } else {
                    other.owner->velocity_x(other.owner->velocity().x * -0.9F + our_velocity.x * 2.0F);
                    other.owner->velocity_y(-1.0F);
                }
            }
        } else if (other_type == Type::SLOPE_L_1X1) {
            //if (our_rect.h > other_rect.h) return;

            //velocity_y(-velocity().x;
            add_position({ 0.0F, -velocity().x });
            /*if (velocity().x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F;
            }*/
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_1X1) {
            //if (our_rect.h > other_rect.h) return;

            //velocity_y(velocity().x);
            add_position({ 0.0F, velocity().x });
            /*if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F;
            }*/
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
            moved_velocity({});
            if (velocity().x > 0.0F) {
                velocity_y(-velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
            moved_velocity({});
            if (velocity().x < 0.0F) {
                velocity_y(velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::WATER_LINE) {
            collide_y(our, other);
        }
    }
}