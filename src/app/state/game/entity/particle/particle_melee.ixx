module;
#include <sstream>

export module entity.particle.melee;
import camera;
import console;
import entity;
import entity.particle;
import collider.aabb;
//import particle;
import sound;

export namespace entity {
    class ParticleMelee : public Particle {
    public:
        ParticleMelee() {
            m_type = Type::PARTICLE_MELEE;
        }
        bool hurt(Object* culprit) override {
            if (!culprit || is_dead()) return false;

            console::log("ParticleMelee::hurt culprit: ", to_string(culprit->type()), "\n");

            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;

            if (culprit->type() == Type::SWING) {
                sound::position(m_bounce_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_bounce_sound_id);
            }
            else {
                sound::position(m_melee_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound::play(m_melee_sound_id);
            }
            return true;
        }

        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (is_dead() || m_is_to_erase || !m_parent || m_parent == other.owner || m_parent->is_blocked()) return;

            cType other_type = other.owner->type();

            cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cVec2F our_velocity = velocity();

            if (other_type == Type::CLIP || other_type == Type::CLIP_LEDGE) {
                aabb::is_active(our.id, false);
            }
            else if (other_type == Type::CLIP_L || other_type == Type::CLIP_LD) {
                if (our_velocity.x > 0.0F) {
                    aabb::is_active(our.id, false);
                }
            }
            else if (other_type == Type::CLIP_R || other_type == Type::CLIP_RD) {
                if (our_velocity.x < 0.0F) {
                    aabb::is_active(our.id, false);
                }
            }
            else if (is_arch(other_type) || is_slope(other_type)) {                
                if (other.owner->parent() && other.owner->parent()->type() == Type::FROG) {
                    //m_is_to_erase = true;
                }
            }
            else if (other.owner->type() == Type::BRICK) {
                //m_is_to_erase = true;
                //hurt(other.owner);
                other.owner->hurt(this);
                aabb::is_active(our.id, false);
            }
            else if (other_type == Type::BUG  ||
                     other_type == Type::FROG ||                     
                     other_type == Type::PLAYER) {
                aabb::is_active(our.id, false);
                hurt(other.owner);
                other.owner->hurt(this);
            }
            else if (other_type == Type::MOLE) {
                if (other.owner->state() == State::IDLE) return;
                aabb::is_active(our.id, false);
                hurt(other.owner);
                other.owner->hurt(this);
            }
            else if (other_type == Type::PARTICLE_HEALTH) {
                if (m_parent->type() == Type::FROG) {
                    other.owner->velocity(-our_velocity / 4.0F);
                    m_is_to_erase = true;
                }
            }
            else if (other_type == Type::PARTICLE_MELEE) {
                if (other.owner->parent()->type() == Type::FROG) {
                    other.owner->parent()->next_state(State::BLOCKED);
                    m_is_to_erase = true;
                    //other.owner->time_left_alive(0);
                    //other.owner->time_left_dead(0);
                }
            }
            else if (other_type == Type::SWING) {
                if (m_parent->type() == Type::PLAYER) {
                    
                    //if (m_parent->is_on_ground()) return;
                    m_parent->next_state(State::SWING);
                    m_parent->parent(other.owner);

                    //cVec2F rect_size = { other_rect.w - other_rect.x, other_rect.h - other_rect.y };
                    //m_parent->swing_level_offset = Vec2F{ other_rect.x, other_rect.y } + camera::position - rect_size / 2;

                    //velocity({});

                    hurt(other.owner);
                }
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            if (is_dead() || m_is_to_erase || !m_parent || m_parent == other.owner || m_parent->is_blocked()) return;

            cType other_type = other.owner->type();

            cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cVec2F our_velocity = velocity();

            if (other_type == Type::CLIP_D || other_type == Type::CLIP_LD || other_type == Type::CLIP_RD) {
                if (our_velocity.y < 0.0F) {
                    aabb::is_active(our.id, false);
                }
            }
            else if (other_type == Type::CLIP_U || is_slope(other_type)) {
                if (our_velocity.y > 0.0F) {
                    aabb::is_active(our.id, false);
                }
            }
            else if (other_type == Type::PARTICLE_HEALTH) {
                collide_x(our, other);
            }
            else if (other_type == Type::PARTICLE_MELEE) {
                collide_x(our, other);
            }
        }
    };
}