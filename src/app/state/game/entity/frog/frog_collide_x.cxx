module;
#include <utility>
module entity.frog;
import collider.aabb;
import console;

namespace entity {
    void Frog::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (!our.owner || !other.owner) return;

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

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        if (other_type == Type::BRICK) {
            //console::log("Frog::collide_x brick\n");
        } else if (other_type == Type::BUG) {
            add_position_x(-overlap_x);
            if (other.owner->state() == State::TOSSED) {
                return;
            }

            velocity_x(other_velocity.x * 0.5F);
            other.owner->velocity_x(our_velocity.x * 0.25F);

            if (our_rect.x < other_rect.x) {
                //velocity().x -= 1.0F;
                other.owner->add_velocity({ 1.0F, 0.0F });
                sprite::is_leftward(other.owner->sprite_id(), false);
            } else {
                //velocity().x += 1.0F;
                other.owner->add_velocity({ -1.0F, 0.0F });
                sprite::is_leftward(other.owner->sprite_id(), true);
            }
        } else if (other_type == Type::CLIP || other_type == Type::CLIP_LEDGE) {
            add_position_x(-overlap_x);
            if (m_state == State::SWIM) {
                if (velocity().x < 0.0F) {
                    sprite::is_leftward(m_sprite_id, false);
                    velocity_x(velocity().x * -1.0F);

                    add_position({ 1.0F, 0.0F });
                } else if (velocity().x > 0.0F) {
                    sprite::is_leftward(m_sprite_id, true);
                    velocity_x(velocity().x * -1.0F);
                    add_position({ -1.0F, 0.0F });
                }
            }
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        } else if (other_type == Type::CLIP_L || other_type == Type::CLIP_LD) {
            if (velocity().x + moved_velocity().x <= 0.0F) return;
            if (m_state == State::SWIM) {
                sprite::is_leftward(m_sprite_id, true);
            }
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        } else if (other_type == Type::CLIP_R || other_type == Type::CLIP_RD) {
            if (velocity().x + moved_velocity().x >= 0.0F) return;
            if (m_state == State::SWIM) {
                sprite::is_leftward(m_sprite_id, false);
            }
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        } else if (other_type == Type::FROG) {
            add_position_x(-overlap_x);
            if (std::abs(our_velocity.x) > std::abs(other_velocity.x)) {
                return;
            }
            velocity_x(other_velocity.x * 0.5F);
            other.owner->velocity_x(our_velocity.x * 0.25f);
            if (our_rect.x < other_rect.x) {
                add_velocity({ -1.0F, 0.0F });
                other.owner->add_velocity({ 1.0F, 0.0F });
            } else {
                add_velocity({ 1.0F, 0.0F });
                other.owner->add_velocity({ -1.0F, 0.0F });
            }
        } else if (other_type == Type::PARTICLE_MELEE) {

        } else if (other_type == Type::PLAYER) {
            add_position_x(-overlap_x);
            if (other.owner->state() == State::SWING) {
                velocity_x(-other.owner->rotation_speed() * 0.75F);
                hurt(other.owner);
                //m_sensed_objects.clear();
                return;
            }
            /*if (std::abs(velocity().x) > std::abs(other.owner->velocity().x)) {
                return;
            }*/

            //if (m_state == State::SWIM) return;

            cF32 force_x = other.owner->is_ducking() ? 4.0F : 1.0F;

            velocity_x(other_velocity.x * 0.5F);
            other.owner->velocity_x(our_velocity.x * 0.25f);
            if (our_rect.x < other_rect.x) {
                add_velocity({ -1.0F, 0.0F });
                other.owner->add_velocity({ force_x, 0.0F });
            } else {
                add_velocity({ 1.0F, 0.0F });
                other.owner->add_velocity({ -force_x, 0.0F });
            }
            moved_velocity_x(0.0F);

        } else if (other_type == Type::SLOPE_L_1X1) {
            velocity_y(-velocity().x);
            if (velocity().x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
            velocity_y(-velocity().x / 2.0F);
            if (velocity().x > max_velocity().x * 0.9F) {
                velocity_x(max_velocity().x * 0.9F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_1X1) {
            velocity_y(velocity().x);
            if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
            velocity_y(velocity().x / 2.0F);
            if (velocity().x < -max_velocity().x * 0.9F) {
                velocity_x(-max_velocity().x * 0.9F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
    }
}