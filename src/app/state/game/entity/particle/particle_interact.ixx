//module;
//#include <sstream>
export module entity.particle.interact;
import console;
import entity;
import entity.particle;
import collider.aabb;

export namespace entity {
    class ParticleInteract : public Particle {
    public:
        ParticleInteract() {
            m_type = Type::PARTICLE_INTERACT;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!m_parent || m_parent == other.owner) return;

            cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cType other_type = other.owner->type();
            aabb::cName   other_name = aabb::name(other.id);
            
            cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

            if (other_type == Type::CLIP_LEDGE) {
                if (!m_parent->is_on_ground() || our_rect.h < other_rect.h || our_rect.y > other_rect.y) return;
                m_parent->interact(other.owner);
            }
            else if (other_type == Type::TRIGGER) {
                if (!m_parent->is_on_ground()) return;
                //m_is_to_erase = true;
                //console::log("ParticleInteract::collide_x: ", to_string(other_type), "\n");
                m_parent->interact(other.owner);
            }
            else if (other_type == Type::BRICK || other_type == Type::BUG) {
                m_is_to_erase = true;                
                m_parent->interact(other.owner);
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}