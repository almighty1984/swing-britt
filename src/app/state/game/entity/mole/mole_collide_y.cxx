module;
#include <cmath>
#include <sstream>
module entity.mole;
import console;
import collider.aabb;

namespace entity {
    void Mole::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() || other.owner->is_dead()) return;

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        entity::cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();

        if (other_type == Type::BRICK) {
            if (m_state != State::IDLE && other.owner->state() == State::TOSSED) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::CLIP_U || other_type == Type::CLIP_LEDGE || other_type == Type::SLOPE_U) {
            if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::SLOPE_U;

            if (other_type == Type::CLIP_LEDGE) {
                if (other_rect.x < our_rect.x) {
                    sprite::is_leftward(m_sprite_id, false);
                } else if (other_rect.w > our_rect.w) {
                    sprite::is_leftward(m_sprite_id, true);
                }
            }
            sprite::rotation(m_sprite_id, 0.0F);
        }
        else if (other_type == Type::PARTICLE_BRICK) {
            if (m_state != State::IDLE && !(other_velocity.y > -2.0F && other_velocity.y < 2.0F)) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::PLAYER) {
            if (m_state == State::EXIT && !sound::is_playing(m_bounce_sound_id)) {
                sound::position(m_bounce_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_bounce_sound_id);
            }
        } else if (other_type == Type::SLOPE_L_1X1) {
            add_position_y(-overlap_y);
            velocity_y(-velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite::rotation(m_sprite_id, 45.0F - 90.0F);
        } else if (other_type == Type::SLOPE_R_1X1) {
            add_position_y(-overlap_y);
            velocity_y(velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite::rotation(m_sprite_id, 135.0F - 90.0F);
        } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
            add_position_y(-overlap_y);
            moved_velocity_y(0.0F);
            velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite::rotation(m_sprite_id, 67.5F - 90.0F);
        } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
            add_position_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity_y(0.0F);

            if (velocity().y > 0.0F) {
                velocity_x(velocity().y / 2.0F);
            }
            m_is_on_ground = m_is_on_slope = true;
            sprite::rotation(m_sprite_id, 112.5F - 90.0F);
        }
    }
}