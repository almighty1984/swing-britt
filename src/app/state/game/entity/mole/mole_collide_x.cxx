module;
#include <cmath>
#include <sstream>
module entity.mole;
import console;
import collider.aabb;

namespace entity {
    void Mole::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() || other.owner->is_dead()) return;

        entity::cType other_type = other.owner->type();

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

        if (other_type == Type::BRICK) {
            if (m_state != State::IDLE && other.owner->state() == State::TOSSED) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::PARTICLE_BRICK) {
            if (m_state != State::IDLE && !(other_velocity.x >= -1.0F && other_velocity.x <= 1.0F)) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::LEVEL_L_0) {
            sprite::is_leftward(m_sprite_id, false);
        }
        else if (other_type == Type::LEVEL_R_0) {
            sprite::is_leftward(m_sprite_id, true);
        }
    }
}