export module entity.track;
import anim;
import console;
import entity;
import collider.aabb;
import sprite;

export namespace entity {
    class Track : public Object {
    public:
        Track() {
            m_input_limit = 1;
        }
        void update() override {                        
            m_time_left_alive = 0;

            //sprite()->is_hidden = false;

            //console::log("num inputs: ", input_objects.size(), "\n");
            m_is_powered = false;
            m_time_left_alive = 0;
            for (auto& i : input_objects) {
                /*if (i->type() == entity::Type::trigger) {
                    console::log("hello trigger\n");
                    if (!i->is_powered()) {
                        m_is_powered = false;
                        break;
                    }
                }*/
                if (i->is_powered()) {
                    m_is_powered = true;
                    if (i->time_left_alive() > 0) {
                        m_time_left_alive = U16_MAX;
                        break;
                    }
                }
            }
            if (!m_is_powered) {
                m_time_left_alive = 0;
            }
            sprite::is_hidden(m_sprite_id, !m_is_powered);

            //console::log("is_powered: ", m_is_powered, "\n");

            if (m_time_left_alive > 0) {
                //sprite()->source_rect = { 8, 8, 16, 16 };
                m_time_left_dead = U16_MAX;

                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
            } else {
                //sprite()->source_rect = { 4, 4, 16, 16 };
            }
        }
    };
}