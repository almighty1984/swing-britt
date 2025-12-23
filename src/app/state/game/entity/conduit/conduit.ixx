export module entity.conduit;
import anim;
import console;
import entity;
import collider.aabb;
import sprite;

export namespace entity {
    class Conduit : public Object {
    public:
        void update() override {
            m_input_limit = 1;
            m_time_left_alive = 0;

            

            m_is_powered = false;
            m_time_left_alive = 0;
            for (auto& i : input_objects) {                
                if (i->is_powered()) {
                    m_is_powered = true;

                    if (i->time_left_alive() > 0) {
                        m_time_left_alive = U16_MAX;

                        if (m_type == Type::CONDUIT_LR) {
                            if (i->start_offset().x > m_start_offset.x) {
                                cI16 num_frames = anim::texture_w(m_current_anim_id) / sprite::source_rect(m_sprite_id).w;
                                anim::first_frame(m_idle_anim_id, num_frames - 1);
                                anim::last_frame(m_idle_anim_id, 0);
                                anim::is_reverse(m_idle_anim_id, true);
                                //console::log("entity::Conduit::update() is reverse: ", anim::is_reverse(m_idle_anim_id), "\n");
                            }
                        }
                        break;
                    }
                }
            }
            if (!m_is_powered) {
                m_time_left_alive = 0;
            }

            sprite::is_hidden(m_sprite_id, !m_is_powered);
            if (m_time_left_alive > 0) {
                m_time_left_dead = U16_MAX;
                m_current_anim_id = m_idle_anim_id;
                
                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
            } else {
                m_current_anim_id = m_dead_anim_id;
            }

            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));
        }
    };
}