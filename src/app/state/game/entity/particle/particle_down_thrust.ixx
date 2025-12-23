module;
#include <sstream>

export module entity.particle.down_thrust;
import camera;
import console;
import entity;
import entity.particle;
import collider.aabb;
//import particle;
import sound;

export namespace entity {
    class ParticleDownThrust : public Particle {
    public:
        ParticleDownThrust() {
            m_type = Type::PARTICLE_DOWN_THRUST;
            m_time_left_dead = 0;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (m_time_left_alive == 0 || is_dead() || !m_parent || m_parent == other.owner || other.owner->is_dead()) return;

            cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cType other_type = other.owner->type();

            if (other_type == Type::PARTICLE_DOWN_THRUST) return;

            //console::log("type: ", to_string(other_type), "\n");

            if (is_arch(other_type) || is_clip(other_type) || is_slope(other_type)) {
                //m_is_to_erase = true;
            } else if (other.owner->type() == Type::PARTICLE_SENSE) {
                
            } else if (other_type == Type::BUG  ||
                       other_type == Type::FROG ||
                       other_type == Type::MOLE) {

                if (other.owner->is_hurting()) return;
                
                cF32 our_w = our_rect.w - our_rect.x;
                cF32 our_h = our_rect.h - our_rect.y;

                cF32 other_w = other_rect.w - other_rect.x;
                cF32 other_h = other_rect.h - other_rect.y;

                cVec2F our_mid_point = position() + Vec2F{ our_w / 2.0F, our_h / 2.0F };
                cVec2F other_mid_point = other.owner->position() + Vec2F{ other_w / 2.0F, other_h / 2.0F };

                cVec2F hit_pos = Vec2F(our_mid_point + other_mid_point) / 2.0F - Vec2F{ our_w / 2.0F, our_h / 2.0F };

                other.owner->hurt(this);
                velocity( {} );
                m_time_left_alive = 0;
                m_time_left_dead = m_time_to_be_dead;
                
                sound::position(m_melee_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_melee_sound_id);
                
                spawn(particle::Type::HIT, hit_pos, {});
                //particle::spawn({ this, particle::Type::HIT, position(), {} });
            } else if (other_type == Type::SWING) {                
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}