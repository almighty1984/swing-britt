module entity.player;
import console;
import health;

namespace entity {
    void Player::dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_down_thrusting = false;
            m_is_carrying = false;
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
            }
            sprite::is_hidden(m_sprite_id, true);

            sprite::offset(m_sprite_id, m_start_sprite_offset);
            sprite::rotation(m_sprite_id, 0.0F);

            m_state = m_next_state = State::DEAD;
            //reset_anim(m_dead_anim_id);
            spawn_fan(0.0F, 360.0F, 8, particle::Type::DROP_BLOOD, position() + Vec2F{ 0.0F, -8.0F }, velocity(), 3.0F);
            spawn(particle::Type::HEALTH, position() + Vec2F{ 4.0F, 4.0F }, {});
            m_time_left_alive = 0;
            m_time_left_dead = 200;

            console::log("entity::Player::dead\n");

            if (sound::is_playing(m_hover_sound_id)) {
                sound::stop(m_hover_sound_id);
            }
        }

        velocity({});
        moved_velocity({});

        --m_time_left_dead;
        if (m_time_left_dead == 0) {
            m_next_state = State::RUN;
            health::reset(m_health_id);
        }
    }
}