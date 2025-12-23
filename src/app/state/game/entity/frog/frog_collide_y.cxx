module;
#include <utility>
module entity.frog;
import collider.aabb;
import console;

namespace entity {
    void Frog::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (!our.owner || !other.owner) return;

        aabb::cName our_name = aabb::name(our.id);

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        if (velocity().y > 0.0F && our_rect.h - 4.0F - velocity().y > other_rect.y) {
            return;
        }

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();
        cVec2F our_velocity = velocity() + moved_velocity();

        if (m_state == State::SWIM) {
            if (position_on_level().y < m_water_line_y) {
                if (is_clip(other_type) || is_slope(other_type)) {
                    m_next_state = State::IDLE;
                    m_water_line_y = 0;
                }
            }
        }
        if (other_type == Type::BRICK) {

            /*console::log("Frog::collide_y brick\n");
            if (other_velocity.y >= other.transform()->max_velocity.y) {

            }*/
        } else if (other_type == Type::BUG) {
            //console::log("Frog::collide_y bug\n");
            if (other.owner->state() == State::TOSSED) {
                return;
            }
            if (other_rect.y > our_rect.y) return;

            add_position_y(-overlap_y);

            if (our_rect.y > other_rect.y) {
                hurt(other.owner);
                m_next_state = State::HURT;
                /*Vec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
                Vec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

                spawn(particle::Type::HIT, { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
                hurt(other_type);*/
                return;
            }
            if (other.owner->state() == State::UPENDED || other.owner->state() == State::BOUNCE) {
                if (velocity().y < 0.0F) return;
                add_position_y(-overlap_y);
                max_velocity_y(10.0F);
                velocity_y(-5.0F - velocity().y / 4.0F);
                m_is_on_ground = false;
                m_is_on_slope = false;
                //m_time_left_bouncing = m_time_to_bounce;
                return;
            }
            moved_velocity(other_velocity);
            //velocity_x(velocity().x * 0.5F);
            velocity_y(0.0F);
            m_is_on_ground = true;
        } else if (other_type == Type::CLIP || other_type == Type::CLIP_LEDGE) {
            add_position_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
                moved_velocity_x(0.0F);
            }
        } else if (other_type == Type::CLIP_U || other_type == Type::SLOPE_U) {
            if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            moved_velocity_x(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::SLOPE_U;
        } else if (other_type == Type::CLIP_D || other_type == Type::CLIP_LD || other_type == Type::CLIP_RD) {
            if (velocity().y > 0.0F) return;
            add_position_y(-overlap_y);
            m_is_on_ground = false;
            m_is_on_slope = false;
            velocity_y(std::abs(velocity().y) * 0.75F);

            spawn(particle::Type::HIT, { other_rect.x + 8.0F, other_rect.h }, {});
            //, m_melee_sound_id
        } else if (other_type == Type::FROG) {
            if (our_rect.h <= other_rect.y) {
                add_position_y(-overlap_y);
                velocity_y(-4.0F);

                velocity_x(other.owner->velocity().x + other.owner->moved_velocity().x);
                add_velocity({ 0.0F, other.owner->velocity().y + other.owner->moved_velocity().y });


                Vec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
                Vec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

                spawn(particle::Type::HIT, { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
                //, m_melee_sound_id
            }
        } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            //if (velocity().x < 0.0F) {
            velocity_y(velocity().x / -2.0F);
            //}        
            m_is_on_ground = true;
            m_is_on_slope = true;
            if (velocity().x > max_velocity().x * 0.9F) {
                velocity_x(max_velocity().x * 0.9F);
            }
            moved_velocity_x(0.0F);
        } else if (other_type == Type::SLOPE_R_1X1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            //if (velocity().x > 0.0F) {
            velocity_y(velocity().x);
            //}
            m_is_on_ground = true;
            m_is_on_slope = true;
            if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F);
            }
            moved_velocity_x(0.0F);
        } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            if (velocity().x > 0.0F) {
                velocity_y(velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
            if (velocity().x < -max_velocity().x * 0.9F) {
                velocity_x(-max_velocity().x * 0.9F);
            }
            moved_velocity_x(0.0F);
        } else if (other_type == Type::PLAYER) {
            if (other.owner->state() == State::SWING) {
                collide_x(our, other);
                return;
            }
            if (our_rect.y <= other_rect.y) {

                add_position_y(-overlap_y);

                cI32 rand_num = random::number(-1, 1);

                //console::log("rand_num: ", rand_num, "\n");

                velocity_x(other_velocity.x * 0.5F + our_velocity.x * 0.5F + (F32)rand_num);
                velocity_y(other_velocity.y * 0.5F - 3.0F + (F32)rand_num);

                /*if (other.owner->transform()) {
                    velocity_x(other.owner->velocity().x + other.owner->moved_velocity().x;
                    velocity().y += (other.owner->velocity().y + other.owner->moved_velocity().y);
                }*/

                if (other_velocity.y < 0.0F) {
                    hurt(other.owner);
                    m_next_state = State::HURT;
                }

                //cVec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
                cVec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

                cVec2F hit_pos = { other_center.x - 8.0F, other_rect.y - 12.0F };

                spawn(particle::Type::HIT, hit_pos, {});

                other.owner->velocity_y(-other.owner->velocity().y * 0.9F);

                sound::position(m_bump_head_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_bump_head_sound_id);

                //next_state(State::HURT);
                //m_time_left_in_state = 200;
            }
        } else if (other_type == Type::WATER_LINE) {
            if (velocity().y > 0.0F && m_state != State::SWIM) {
                m_next_state = State::SWIM;

                m_water_line_y = other.owner->start_position().y;

            }
            if (m_state == State::SWIM) {
                if (position().y > m_water_line_y) {
                    velocity_y(other_velocity.y);
                }
            }
        }
    }
}