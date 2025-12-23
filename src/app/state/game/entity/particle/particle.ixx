export module entity.particle;
import anim;
import console;
import entity;
import collider.aabb;
import sprite;

export namespace entity {
    class Particle : public Object {
    protected:
        bool m_is_to_erase = false;
    public:        
        bool is_to_erase() const { return m_is_to_erase; }

        virtual void idle() {}
        virtual void dead() {}

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
                m_time_left_alive = m_time_to_be_alive;
                m_time_left_dead = m_time_to_be_dead;
                //m_start_offset = position_on_level();
            }
            /*if (m_time_left_alive == 0 && m_time_left_dead == 0) {
                m_is_to_erase = true;
                return;
            }*/
            if (m_type == entity::Type::PARTICLE_SENSE && m_parent && m_parent->state() == entity::State::MELEE) {
                m_is_to_erase = true;
            }
            if (m_time_left_hurt > 0) {
                --m_time_left_hurt;
            }
            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            add_velocity_y(acceleration().y);

            switch (m_state) {
                case entity::State::IDLE: idle();  break;
                case entity::State::DEAD: dead();  break;
                default: break;
            }
            if (m_time_left_alive > 0) {
                --m_time_left_alive;
                if (m_time_left_alive == 0) {
                    m_time_left_dead = m_time_to_be_dead;
                    if (m_time_to_be_dead == 0) {
                        m_is_to_erase = true;
                    }
                }
                if (m_current_anim_id != m_idle_anim_id) {
                    m_current_anim_id = m_idle_anim_id;
                    anim::source_x(m_idle_anim_id, anim::first_frame(m_idle_anim_id));                    
                }
            } else if (m_time_left_dead > 0) {
                velocity({});

                if (m_time_left_dead != U16_MAX) {
                    --m_time_left_dead;
                    if (m_time_left_dead == 0) {
                        m_is_to_erase = true;
                    }
                }
                if (m_current_anim_id != m_dead_anim_id) {
                    m_current_anim_id = m_dead_anim_id;
                    anim::source_x(m_dead_anim_id, anim::first_frame(m_dead_anim_id));
                }
            }
            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));
        }
    };
}