module;
#include <cmath>
#include <sstream>
module entity.brick;
import console;
import collider.aabb;

namespace entity {
    void Brick::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (other.owner == m_parent || is_dead() || other.owner->is_dead()) return;

        cType other_type = other.owner->type();

        aabb::cName other_name = aabb::name(other.id);

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        Vec2F our_dimensions = { aabb::point(our.id, 1).x - aabb::point(our.id, 0).x,
                                 aabb::point(our.id, 3).y - aabb::point(our.id, 1).y };

        aabb::cName our_name = aabb::name(our.id);

        cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();

        if (other.owner->parent() && other_type == Type::BRICK) {
            return;
        }

        if (m_parent) {
            if (is_clip(other_type) ||
                other_type == Type::BRICK) {
                //console::log("entity::Brick::collide_x near: ", to_string(other_type), "\n");
                if (our_rect.w > other_rect.w) {
                    m_is_near_wall_L = true;
                }
                if (our_rect.x < other_rect.x) {
                    m_is_near_wall_R = true;
                }
            }
            return;
        }
        if (is_arch(other_type)) {
            if (m_parent || other.owner->parent() || is_hurting()) return;
            console::log("entity::Brick::collide_x ", to_string(other_type), "\n");
            add_position_x(-overlap_x);
            if (std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                velocity_x(velocity().x * -0.9F);
                hurt(other.owner);
            }
        } else if (other_type == Type::BRICK) {
            add_position_x(-overlap_x);
            moved_velocity_x(0.0F);

            if (m_state == State::TOSSED) {
                //if (!m_is_on_ground && std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                velocity_x(our_velocity.x * -0.5F);
                other.owner->velocity_x(our_velocity.x * 0.5F);

                hurt(other.owner);
                other.owner->hurt(this);
                return;
            }
            if (std::abs(other_velocity.x) > std::abs(our_velocity.x)) {
                velocity_x(other_velocity.x);
            }
        } else if (other_type == Type::BUG) {
            if (m_parent || other.owner->parent()) return;
            console::log("entity::Brick::collide_x bug\n");
            //add_position_x( -overlap_x );
            moved_velocity_x(0.0F);

            if (m_state == State::TOSSED || other.owner->state() == State::TOSSED) {
                //if (std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                velocity_x(other_velocity.x * -0.5F);
                other.owner->velocity_x(our_velocity.x * 0.5F);

                hurt(other.owner);
                other.owner->hurt(this);
                return;
            }
            other.owner->velocity_x(velocity().x * -0.9F);
            other.owner->moved_velocity_x(0.0F);

            sprite::is_leftward(other.owner->sprite_id(), !sprite::is_leftward(other.owner->sprite_id()));

            //velocity_x(velocity().x * -0.9F);
        } else if (other_type == Type::CLIP ||
            other_type == Type::CLIP_LEDGE ||
            ((other_type == Type::CLIP_L || other_type == Type::CLIP_LD) && our_velocity.x > 0.0F) ||
            ((other_type == Type::CLIP_R || other_type == Type::CLIP_RD) && our_velocity.x < 0.0F)
            ) {
            add_position_x(-overlap_x);
            moved_velocity_x(0.0F);
            velocity_x(velocity().x * -0.9F);
            //console::log("velocity.x ", velocity().x, "\n");
            if (std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {

                hurt(other.owner);
            }
        } else if (other_type == Type::FROG) {
            if (our_rect.h < other_rect.y + 4.0F) return;

            add_position_x(-overlap_x);
            velocity_x(velocity().x * -0.5F);
            moved_velocity_x(0.0F);
            if (m_state == State::TOSSED) {
                //if (std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                hurt(other.owner);
                other.owner->hurt(this);
                return;
            }
        } else if (other_type == Type::MOLE) {
            if (m_state == State::TOSSED && other.owner->state() != State::IDLE) {
                hurt(other.owner);                
            }
        } else if (other_type == Type::PARTICLE_SHOT) {
            hurt(other.owner);
        } else if (other_type == Type::PLAYER) {
            if (other_velocity.y >= 6.0F) {
                console::log("yo\n");
                hurt(other.owner);
                return;
            }
            velocity_x(other_velocity.x);
            add_position_x(-overlap_x);
            if (other.owner->state() == State::SWING) {
                velocity_x(-other.owner->rotation_speed());
                hurt(other.owner);
            }
        } else if (other_type == Type::SLOPE_L_1X1) {
            if (std::abs(velocity().x) >= m_break_velocity.x) {
                velocity_x(velocity().x * -0.9F);
                hurt(other.owner);
                return;
            }
            if (velocity().x > 0.0F) {
                velocity_y(-velocity().x);
            }
            moved_velocity({});
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_1X1) {
            if (std::abs(velocity().x) >= m_break_velocity.x) {
                velocity_x(velocity().x * -0.9F);
                hurt(other.owner);
                return;
            }
            if (velocity().x < 0.0F) {
                velocity_y(velocity().x);
            }
            moved_velocity({});
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
            if (std::abs(velocity().x) >= m_break_velocity.x) {
                velocity_x(velocity().x * -0.9F);
                hurt(other.owner);
                return;
            }
            moved_velocity({});
            if (velocity().x > 0.0F) {
                velocity_y(-velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
            if (std::abs(velocity().x) >= m_break_velocity.x) {
                velocity_x(velocity().x * -0.9F);
                hurt(other.owner);
                return;
            }
            moved_velocity({});
            if (velocity().x < 0.0F) {
                velocity_y(velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SPRING_U) {
            collide_y(our, other);
        } else if (is_water_line(other_type)) {
            collide_y(our, other);
        }
    }
}