module;
#include <sstream>
export module entity.particle.sense;
import camera;
import console;
import entity;
import entity.particle;
import collider.aabb;

export namespace entity {
    class ParticleSense : public Particle {
    public:
        ParticleSense() {
            m_type = Type::PARTICLE_SENSE;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!m_parent || other.owner->is_dead() || m_parent == other.owner || m_parent->is_blocked()) return;
            cVec2F other_velocity = other.owner->velocity();

            if (other_velocity.y >= 5.0F) {
                //console::log("too fast!\n");
                return;
            }
            cType other_type = other.owner->type();

           /* if (m_parent->type() == Type::MOLE) {
                console::log("type: ", to_string(m_parent->type()), "\n");
            }*/

            if (is_arch(other_type)  ||
                is_clip(other_type)  ||
                is_slope(other_type)) {
                m_is_to_erase = true;
                //console::log("ParticleSense::collide_x type: ", to_string(other.owner->type()), "\n");
            }
            else if (other_type == Type::BRICK           ||
                     other_type == Type::BUG             ||
                     other_type == Type::FROG            ||
                     other_type == Type::WATER_LINE
                ) {
                //console::log("vel: ", other.owner->velocity().x, " ", other.owner->velocity().y, "\n");
                m_parent->add_sensed_object(other.owner);
                m_is_to_erase = true;
            }
            else if (other_type == Type::PARTICLE_BRICK) {
                if (!(other_velocity.x > -0.2F && other_velocity.x < 0.2F && other_velocity.y > -2.0F && other_velocity.y < 2.0F)) {
                    m_parent->add_sensed_object(other.owner);
                }
            }
            else if (other_type == Type::PARTICLE_HEALTH) {
                m_parent->add_sensed_object(other.owner);
            }
            else if (other_type == Type::PLAYER) {
                m_parent->add_sensed_object(other.owner);
                m_is_to_erase = true;
            }
            else if (other_type == Type::PARTICLE_DOWN_THRUST) {
                m_is_to_erase = true;
            } else if (other_type == Type::PARTICLE_SENSE) {

            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}