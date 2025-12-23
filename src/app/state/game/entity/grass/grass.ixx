module;
#include <cmath>
#include <sstream>

export module entity.grass;
import anim;
import camera;
import collider.aabb;
import console;
import entity;
import random;
import sprite;

export namespace entity {
    class Grass : public Object {
        I32 m_prev_source_x     = 0;

        U8 m_time_limit         = 8,
           m_random_time_offset = 0;

        bool m_is_in_view = false;
    public:        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!our.owner || !other.owner) return;

            entity::cType other_type = other.owner->type();

            cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cF32 our_w = our_rect.w - our_rect.x;
            cF32 our_mid_x = our_rect.x + our_w / 2.0f;

            cF32 other_w = other_rect.w - other_rect.x;
            cF32 other_mid_x = other_rect.x + other_w / 2.0f;

            if (other_type == entity::Type::PLAYER &&
                (anim::type(other.owner->current_anim_id()) == anim::Type::RUN || anim::type(other.owner->current_anim_id()) == anim::Type::WALK) &&
                (sprite::source_rect(other.owner->sprite_id()).x == 64 || sprite::source_rect(other.owner->sprite_id()).x == 320)) {
                return;
            }

            if (other_type == entity::Type::BUG  ||
                other_type == entity::Type::FROG ||
                other_type == entity::Type::MOLE ||
                other_type == entity::Type::PLAYER) {
                //console::log("source x: ", other.owner->sprite()->source_rect.x, "\n");
                //console::log("anim type: ", anim::to_string(other.owner->anim()->type), "\n");                

                int num = 0;
                if (our_mid_x > other_mid_x) {
                    //sprite::is_leftward(m_sprite_id, false);
                    num = (int)std::abs(our_mid_x - other_mid_x);
                } else {
                    //sprite::is_leftward(m_sprite_id, true);
                    num = (int)std::abs(our_mid_x - other_mid_x);
                }
                int tile_x = 32 * 7 - ((num / 4) * 32);
                tile_x -= tile_x % 32;

                sprite::source_rect_x(m_sprite_id, tile_x);
                //m_time_left_dead = m_time_to_be_dead;
            }
        };
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        };

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
                m_random_time_offset = random::number(0, m_time_limit / 4);
                //console::log("m_random_time_offset: ", (I16)m_random_time_offset, "\n");
                //console::log("grass origin: ", sprite()->origin.x, " ", sprite()->origin.y, "\n");
            }
            //console::log("pos: ", position().x, " ", m_start_offset.x, "\n");            
            if (camera::position.x          > m_start_offset.x ||
                camera::position.x + 320.0f < m_start_offset.x
                ) {
                m_is_in_view = false;
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, false);
                }
                //return;
            } else {
                m_is_in_view = true;
                for (auto& i : m_aabb_ids) {                    
                    aabb::is_active(i, true);                    
                }
            }
            
            ++m_time_in_state; 
            if (m_time_in_state > m_time_limit + m_random_time_offset) {
                m_time_in_state = 0;
                                
                if (m_is_in_view) {
                    m_is_first_update = true;
                }

                if (sprite::source_rect(m_sprite_id).x == 0) {
                    if (m_prev_source_x == 32) {
                        sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));
                    }
                    sprite::source_rect_x(m_sprite_id, 32);
                    m_prev_source_x = 0;
                } else if (sprite::source_rect(m_sprite_id).x == 32) {
                    m_prev_source_x == 0 ? sprite::source_rect_x(m_sprite_id, 64) : sprite::source_rect_x(m_sprite_id, 0);
                    m_prev_source_x = 32;
                } else if (sprite::source_rect(m_sprite_id).x == 64) {
                    m_prev_source_x == 32 ? sprite::source_rect_x(m_sprite_id, 96) : sprite::source_rect_x(m_sprite_id, 32);
                    m_prev_source_x = 64;
                } else if (sprite::source_rect(m_sprite_id).x >= 96) {
                    sprite::source_rect_x(m_sprite_id, 64);
                    m_prev_source_x = 96;
                }
            }
        }        
    };
}