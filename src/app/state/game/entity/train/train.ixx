export module entity.train;
import collider.aabb;
import anim;
import camera;
import console;
import entity;

export namespace entity {
    class Train : public Object {
        F32 m_speed       = 0.0F,
            m_start_speed = 1.0F;

        Vec2F m_prev_velocity = { 0.0F, 0.0F };

        bool m_is_clockwise = true;
    public:
        //Train() {
            //m_speed = m_start_speed;
        //}
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
        
        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
                //velocity() = m_start_speed;
                m_prev_velocity.x = m_start_speed;
            }
            //console::log("train position: ", position().x, " ", position().y, "\n");
            //console::log("is_powered: ", m_is_powered, "\n");
            //console::log("time_left_alive: ", m_time_left_alive, "\n");

            //console::log("speed: ", m_speed, "\n");

            /*if (m_is_powered) {
                m_time_left_alive = U16_MAX;
            } else {
                m_time_left_alive = 0;
            }*/

            m_direction.x = 0.0F;
            if (velocity().x < 0.0F) {
                m_direction.x = -1.0F;
            } else if (velocity().x > 0.0F) {
                m_direction.x = 1.0F;
            }
            m_direction.y = 0.0F;
            if (velocity().y < 0.0F) {
                m_direction.y = -1.0F;
            } else if (velocity().y > 0.0F) {
                m_direction.y = 1.0F;
            }

            //console::log("entity::Train::update() direction: ", m_direction.x, " ", m_direction.y, "\n");
            
            if (velocity().x < -0.05F || velocity().x > 0.05F) {
                m_prev_velocity.x = velocity().x;
            }
            if (velocity().y < -0.05F || velocity().y > 0.05F) {
                m_prev_velocity.y = velocity().y;
            }

            if (m_time_left_alive > 0) {
                if (m_start_speed > 0.0F && m_speed < m_start_speed) {
                    m_speed += 0.2F;
                } else if (m_start_speed < 0.0F && m_speed > m_start_speed) {
                    m_speed -= 0.2F;
                }
                if (m_start_speed > 0.0F && m_speed > m_start_speed ||
                    m_start_speed < 0.0F && m_speed < m_start_speed) {
                    m_speed = m_start_speed;
                }
                if (velocity().x >= -0.05F && velocity().x <= 0.05F &&
                    velocity().y >= -0.05F && velocity().y <= 0.05F) {
                    //console::log("entity::Train at zero\n");
                    velocity(m_prev_velocity);
                }
            } else {
                m_speed *= 0.9f;
                if (m_speed > 0.0F && m_speed < 0.01f) {
                    m_speed = 0.0F;          
                } else if (m_speed < 0.0F && m_speed > -0.01f) {
                    m_speed = 0.0F;
                }
            }
            //console::log("speed: ", m_speed, "\n");
            
            /*if (!m_is_second_update) {
                m_is_second_update = true;
                for (auto& i : m_aabb_ids) {
                    console::log("aabb ", i, " name: ", aabb::name_as_string(aabb::get(i)->name), "\n");
                }
            }*/
            
            

            //velocity_x(m_speed.x;
            //velocity_y(m_speed.y;
        }
    };
}