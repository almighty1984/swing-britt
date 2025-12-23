export module entity.particle.health;
import console;
import entity.particle;
import collider.aabb;
import sound;
import health;

export namespace entity {
    class ParticleHealth : public Particle {
    public:
        ParticleHealth() {
            m_type = Type::PARTICLE_HEALTH;
            m_state = State::IDLE;
        }        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (is_dead() || m_is_to_erase || other.owner->is_dead() /* || other.owner == m_parent*/) return;

            cType other_type = other.owner->type();

            if (other_type == Type::FROG || other_type == Type::PLAYER) {
                if (health::is_full(other.owner->health_id())) return;
                
                m_time_left_alive = 1;
                m_time_left_dead = m_time_to_be_dead;

                health::add_amount(other.owner->health_id(), 1.0F);
                
                sound::position(m_dead_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_dead_sound_id);
            }
            else if (other_type == Type::PARTICLE_MELEE) {
                if (!other.owner->parent()) return;
                if (other.owner->parent()->type() == Type::FROG) {
                    health::add_amount(other.owner->parent()->health_id(), 1.0F);
                    m_time_left_alive = 7;
                    m_time_left_dead = m_time_to_be_dead;
                }
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}