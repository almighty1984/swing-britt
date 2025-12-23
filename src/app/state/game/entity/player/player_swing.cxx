module;
#include <cmath>
module entity.player;
import console;
import sprite;

void entity::Player::swing() {
    if (!m_parent) {
        m_next_state = entity::State::RUN;
        return;
    }
    
    Vec2F swing_position = m_parent->position() + m_parent->start_offset();  // level transform + start offset
    Vec2F offset_from_swing = position() - swing_position;

    cVec2F prev_pos = position();
    cVec2F level_pos = transform::position(transform::level_id());
    
    cVec2F pos_on_level = transform::position(m_transform_id) - transform::position(transform::level_id());
    if (m_is_first_state_update) {
        m_is_first_state_update = false;
        //m_next_state = entity::Type::SWING;

        for (auto& i : m_aabb_ids) {
            aabb::is_active(i, false);
            if (!aabb::is_active(i) && aabb::name(i) == aabb::Name::BODY) {
                aabb::is_active(i, true);
            }
        }

        m_rotation_speed = -velocity().y * velocity().x * 0.75F;
        //m_rotation_speed = -(velocity().x * velocity().y);
        //m_rotation_speed = offset_from_swing.x / 4.0F + offset_from_swing.y / 4.0F/* + velocity().y / 2.0F*/;

        cF32 lower_limit = 3.0F,
             upper_limit = 8.0F;

        if (!sprite::is_leftward(m_sprite_id)) {
            if (m_rotation_speed > -lower_limit) {
                m_rotation_speed = -lower_limit;
            } else if (m_rotation_speed < -upper_limit) {
                m_rotation_speed = -upper_limit;
            }
        } else {
            if (m_rotation_speed < lower_limit) {
                m_rotation_speed = lower_limit;
            } else if (m_rotation_speed > upper_limit) {
                m_rotation_speed = upper_limit;
            }
        }
        console::log("rotation speed: ", m_rotation_speed, "\n");

        //m_rotation_speed = velocity().y;

        //m_rotation_speed = 0.0F;

        console::log("m_rotation_speed: ", m_rotation_speed, "\n");

        cF32 radians = std::atan2f(offset_from_swing.y, offset_from_swing.x);
        cF32 angle = radians * 180.0F / 3.1415926535F;


        console::log("entity::Player::swing angle: ", angle, "\n");

        sprite::origin(m_sprite_id, { 32.0F, 20.0F });
        sprite::rotation(m_sprite_id, angle - 90.0F);

        //console::log("entity::Player::swing offset from swing: ", offset_from_swing.x, " ", " ", offset_from_swing.y, "\n");

        velocity( { 0.0F, 0.0F } );
        acceleration({ 0.1F, 0.1F });
        deceleration({ 0.0F, 0.0F });

        reset_anim(m_swing_anim_id);

        transform::max_velocity(m_transform_id, m_swing_velocity_limit);
    }
    m_is_on_ground = false;
    anim::speed(m_current_anim_id, 0.0F);
    

    cF32 rotate_dec = offset_from_swing.x / 80.0F;
    cF32 rotate_acc = 0.25f;
    m_rotation_speed += rotate_dec;
    
    sprite::rotate(m_sprite_id, m_rotation_speed);

    if (input::is_pressed(m_input_id, key_left)) {
        m_rotation_speed += rotate_acc;
    }
    if (input::is_pressed(m_input_id, key_right)) {
        m_rotation_speed -= rotate_acc;        
    }    

    position({ std::cosf(sprite::radians(m_sprite_id) + 3.1415956535F / 2.0F) * 28.0F + level_pos.x + m_parent->start_offset().x,
               std::sinf(sprite::radians(m_sprite_id) + 3.1415956535F / 2.0F) * 28.0F + level_pos.y + m_parent->start_offset().y });

    offset_from_swing = position() - swing_position;
    sprite::offset(m_sprite_id, {-24.0F - offset_from_swing.x, -10.0F - offset_from_swing.y});

    if (!input::is_pressed(m_input_id, key_melee)) {
        //release(key_melee);        
        m_next_state = entity::State::RUN;

        
        m_time_left_jump_again = m_time_to_jump_again;
        m_num_jumps = 0;
        reset_anim(m_jump_spin_anim_id);
        
        sound::pitch(m_jump_sound_id, 1.122454545454545f);

        velocity((position() - prev_pos) * 1.5F);        

        console::log("player_swing() velocity: ", velocity().x, " ", velocity().y, "\n");

        sprite::offset(m_sprite_id, m_start_sprite_offset);
        sprite::rotation(m_sprite_id, 0.0F);
        
        sprite::is_leftward(m_sprite_id, velocity().x < 0.0F);

        sound::position(m_swing_detach_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
        sound::play(m_swing_detach_sound_id);
    }

    m_rotation_speed *= 0.99f;

    if (m_rotation_speed < -13.0F) {
        m_rotation_speed = -13.0F;
    }
    if (m_rotation_speed > 13.0F) {
        m_rotation_speed = 13.0F;
    }

    //console::log("entity::Player::swing() rotation_speed: ", rotation_speed, "\n");
}