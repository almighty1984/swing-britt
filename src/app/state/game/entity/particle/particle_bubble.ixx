module;
#include <cmath>

export module entity.particle.bubble;
import console;
import entity.particle;
import collider.aabb;

export namespace entity {
    class ParticleBubble : public Particle {
    public:
        ParticleBubble() {
            m_type = Type::PARTICLE_BUBBLE;
            m_next_state = State::IDLE;
        }
        void idle() override {
            //console::log("ParticleBubble::idle direction: ", m_direction.x, "\n");
            ++m_time_in_state;
            if (m_time_in_state < 10) return;

            if (m_direction.x == 0.0F) {
                add_velocity({ -acceleration().x, 0.0F });
            } else {
                add_velocity({ acceleration().x, 0.0F });
            }
            if (velocity().x < -0.2F) {
                m_direction.x = 1.0F;
            }
            else if (velocity().x > 0.2F) {
                m_direction.x = 0.0F;
            }
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            //collide_y(our, other);
            cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cType other_type = other.owner->type();
            aabb::cName   other_name = aabb::name(other.id);
           

            cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

            if (is_clip(other_type)) {
                m_time_in_state = 0;
                add_position_x(-overlap_x);
                velocity_x(0.0F);
            } else if (other_type == Type::PARTICLE_BUBBLE) {
                add_position_x(-overlap_x);
                //cF32 vel_x = velocity().x;
                //velocity_x((other.owner->velocity().x + vel_x) / 2.0F;
                //other.owner->velocity_x((other.owner->velocity().x + vel_x) / 2.0F;
                if (std::abs(velocity().x) > std::abs(other.owner->velocity().x)) {
                    other.owner->velocity_x(velocity().x);
                } else {
                    velocity_x(other.owner->velocity().x);
                }
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cType other_type = other.owner->type();
            aabb::cName   other_name = aabb::name(other.id);            

            cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);
            
            if (is_clip(other.owner->type())) {
                add_position_y( -overlap_y );

                m_time_in_state = 0;

                //if (our_rect.x < other_rect.x + (other_rect.w - other_rect.x) / 2) {

                if (velocity().x == 0.0F) {
                    if (m_direction.x == 0.0F) {
                        velocity_x(-0.1F);
                        //position().x -= overlap_y;
                    } else {
                        velocity_x(0.1F);
                        //position().x += overlap_y;
                    }
                }

                /*if (velocity().y < 0.0F) {
                    velocity_x(velocity().y);
                    velocity_y(0.0F);
                }*/
            }
            else
            if (is_water_line(other.owner->type())) {
                if (!is_dead()) {
                    m_time_left_alive = 0;
                    m_time_left_dead = m_time_to_be_dead;
                    other.owner->add_velocity_y(our.owner->velocity().y / 4.0F);
                }
            }
        }
    };
}