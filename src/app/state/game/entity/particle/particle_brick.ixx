module;
#include <cmath>

export module entity.particle.brick;
import console;
import entity.particle;
import collider.aabb;
import random;
import sound;
import sprite;

export namespace entity {
    class ParticleBrick : public Particle {
    public:
        ParticleBrick() {
            m_type = Type::PARTICLE_BRICK;
            m_state = m_next_state = State::IDLE;
        }
        void idle() override {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_left_alive = m_time_to_be_alive + random::number(0, 50);

                //console::log("m_time_left_alive: ", m_time_left_alive, "\n");
                
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, false);
                }
            }
            m_parent = nullptr;

            if (m_time_in_state <= 5) {
                ++m_time_in_state;
            }
            if (m_time_in_state == 5) {
                for (auto& i : m_aabb_ids) {                    
                    aabb::is_active(i, true);
                }
            }

            if (velocity().x > acceleration().x || velocity().x < -acceleration().x ||
                velocity().y > acceleration().y || velocity().y < -acceleration().y) {
                F32 radians = std::atan2(velocity().y, velocity().x);
                if (radians < 0.0F) radians += (3.1415926535F * 2.0F);

                cF32 degrees = radians * 180.0F / 3.1415926535F;

                sprite::rotation(m_sprite_id, degrees);
            }
        }
        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cType other_type = other.owner->type();
            aabb::cName   other_name = aabb::name(other.id);

            cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

            cVec2F other_size = { other_rect.w - other_rect.x, other_rect.h - other_rect.y };

            cVec2F other_velocity = other.owner->velocity();
            cVec2F our_velocity   = velocity();
                        
            if (other_type == Type::ARCH_L_1X1 || other_type == Type::ARCH_R_1X1) {
                velocity_x(velocity().x * 0.5F);
            }
            else if (other_type == Type::ARCH_L_2X1_0 || other_type == Type::ARCH_L_2X1_1 ||
                     other_type == Type::ARCH_R_2X1_0 || other_type == Type::ARCH_R_2X1_1) {
                velocity_x(velocity().x * -0.75F);
            }
            else if (other_type == Type::BRICK ||
                other_type == Type::CLIP  ||
               (other_type == Type::CLIP_L && velocity().x > 0.0F) ||
               (other_type == Type::CLIP_R && velocity().x < 0.0F) ||
                other_type == Type::CLIP_LEDGE
                ) {

                if (other_rect.x < our_rect.x && velocity().x > 0.0F ||
                    other_rect.w > our_rect.w && velocity().x < 0.0F) return;

                add_position_x(-overlap_x);
                velocity_x(velocity().x * -0.75F);
            }
            else if (other_type == Type::FROG) {
                if (std::abs(velocity().x) >= 2.0F) {
                    other.owner->hurt(this);
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };

                    sound::position(m_melee_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound::play(m_melee_sound_id);
                    
                    spawn(particle::Type::HIT, hit_pos, {});
                }
                add_position_x( -overlap_x );
                velocity_x(velocity().x * -1.0F);
            }
            else if (other_type == Type::MOLE) {
                if (is_hurting() || (velocity().x >= -1.0F && velocity().x <= 1.0F)) return;

                if (other.owner->state() == State::IDLE) {
                    m_time_left_hurt = m_time_to_hurt;
                    velocity_y(other_velocity.y - 1.0F);
                    velocity_x(other_velocity.x * 0.5F);
                    return;
                }

                add_position_x(-overlap_x);
                velocity_x(velocity().x * -1.0F);

                cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
                sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::play(m_melee_sound_id);
                spawn(particle::Type::HIT, hit_pos, {});

            }
            else if (other_type == Type::SLOPE_L_1X1) {
                //add_position_x( -overlap_x );
                collide_y(our, other);
            } else if (other_type == Type::SLOPE_R_1X1) {
                //add_position_x( -overlap_x );
                collide_y(our, other);
            } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
                //add_position_x( -overlap_x );
                collide_y(our, other);
            } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
                //add_position_x( -overlap_x );
                collide_y(our, other);
            } else if (other_type == Type::PARTICLE_BRICK) {
                //if (is_hurting() || std::abs(other_velocity.x) < 1.0F) return;

                //add_position_x( -overlap_x );
                //velocity_x(our_velocity.x * 0.4F + other_velocity.x * 0.4F;
                //other.owner->velocity_x(our_velocity.x * 0.5F + other_velocity.x * 0.5F;

                //velocity_x(other_velocity.x * 0.8F;
                //other.owner->velocity_x(our_velocity.x * 0.8F;

                /*if (std::abs(velocity().x) > std::abs(other.owner->velocity().x)) {
                    other.owner->velocity_x(velocity().x;
                } else {
                    velocity_x(other.owner->velocity().x;
                }*/               
            } else if (other_type == Type::PLAYER) {
                if (is_hurting() || std::abs(other_velocity.x) < 1.0F) return;

                m_time_left_hurt = m_time_to_hurt;

                //add_position_x( -overlap_x );
                //position().y -= 2.0F;
                velocity_y(other_velocity.y - 2.0F);

                velocity_x(other_velocity.x * 1.2F);
                //other.owner->velocity_x(other_velocity.x * 0.8F + our_velocity.x * 0.2F;

                //other.owner->velocity().x *= 0.95F;
            } else if (is_water_line(other.owner->type())) {
                collide_y(our, other);
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

            cVec2F other_velocity = other.owner->velocity();
            cVec2F our_velocity = velocity();
                               
            if (is_arch(other_type)) {                
                if (velocity().y < 0.0F) {
                    add_position_y( -overlap_y );
                    velocity_y(velocity().y * -1.0F);
                }
            }
            else if (other_type == Type::BRICK || other_type == Type::CLIP || other_type == Type::CLIP_LEDGE) {
                if (velocity().y < 0.0F && our_rect.h < other_rect.h) return;
                if (velocity().y > 0.0F && our_rect.y > other_rect.y) return;

                add_position_y( -overlap_y );
                velocity_y(velocity().y * -0.5F);
                if (velocity().y >= -acceleration().y && velocity().y <= acceleration().y) {
                    velocity_y(0.0F);
                }
                moved_velocity({});

                velocity_x(velocity().x * 0.5F);
            } else if (other_type == Type::CLIP_U || other_type == Type::SLOPE_U) {
                if (velocity().y < 0.0F) return;
                //if (velocity().y > 0.0F && our_rect.y > other_rect.h) return;

                add_position_y( -overlap_y );
                
                if (velocity().y > 1.0F) {
                    velocity_y(velocity().y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }
                
                moved_velocity({});

                velocity_x(velocity().x * 0.5F);
            }
            else if (other_type == Type::CLIP_D || other_type == Type::CLIP_LD || other_type == Type::CLIP_RD) {
                if (velocity().y > 0.0F) return;
                //if (velocity().y < 0.0F && our_rect.h < other_rect.h) return;

                add_position_y( -overlap_y );
                velocity_y(velocity().y * -0.5F);
                moved_velocity({});

                velocity_x(velocity().x * 0.5F);
            }
            else if (other_type == Type::FROG) {
                if (std::abs(velocity().y) >= 2.0F) {
                    other.owner->hurt(this);
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };

                    sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound::play(m_melee_sound_id);

                    spawn(particle::Type::HIT, hit_pos, {});
                }
                if (our_rect.y < other_rect.y) {
                    add_position_y( -overlap_y );
                    velocity_y(velocity().y * -1.0F);
                }
            }
            else if (other_type == Type::MOLE) {
                if (other.owner->state() == State::IDLE || (velocity().y >= -2.0F && velocity().y <= 2.0F)) return;
                if (our_rect.y < other_rect.y) {
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
                    sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound::play(m_melee_sound_id);
                    spawn(particle::Type::HIT, hit_pos, {});

                    add_position_y(-overlap_y);
                    velocity_y(velocity().y * -1.0F);
                }
            }
            else if (other_type == Type::SLOPE_L_1X1) {
                if (velocity().y < 0.0F) return;
                add_position_y( -overlap_y );
                if (velocity().y > 1.0F) {
                    velocity_y(velocity().y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }
                sprite::rotation(m_sprite_id, 45.0f);

                velocity_x(velocity().x * 0.5F);
            } else if (other_type == Type::SLOPE_R_1X1) {
                if (velocity().y < 0.0F) return;
                add_position_y( -overlap_y );
                if (velocity().y > 1.0F) {
                    velocity_y(velocity().y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }
                sprite::rotation(m_sprite_id, 135.0f);

                velocity_x(velocity().x * 0.5F);
            } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
                if (velocity().y < 0.0F) return;
                add_position_y( -overlap_y );
                if (velocity().y > 1.0F) {
                    velocity_y(velocity().y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }
                sprite::rotation(m_sprite_id, 67.5f);

                velocity_x(velocity().x * 0.5F);
            } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
                if (velocity().y < 0.0F) return;
                add_position_y( -overlap_y );
                if (velocity().y > 1.0F) {
                    velocity_y(velocity().y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }
                sprite::rotation(m_sprite_id, 112.5f);

                velocity_x(velocity().x * 0.5F);
            }
            else if (other_type == Type::PLAYER) {
                if (is_hurting() || std::abs(other_velocity.x) < 1.0F) return;
                m_time_left_hurt = m_time_to_hurt;

                add_position({ 0.0F, -2.0F });
                velocity_x(other_velocity.x * 1.2F);
                velocity_y(other_velocity.y - 1.0F);
            } else if (other_type == Type::PARTICLE_BRICK) {
                //if (is_hurting() || std::abs(other_velocity.x) < 1.0F) return;
                ////if (other_rect.y > our_rect.y) return;

                ////add_position_y( -overlap_y );
                ////velocity_x(other_velocity.y * 0.5F;
                ////other.owner->velocity_x(our_velocity.y * 0.5F;

                ////velocity_y(our_velocity.y * 0.4F + other_velocity.y * 0.4F;
                ////other.owner->velocity_y(our_velocity.y * 0.4F + other_velocity.y * 0.4F;
                //velocity_y(0.0F);


                //velocity_x(velocity().x * -0.9F);
            } else if (is_water_line(other_type)) {
                if (acceleration() == Vec2F{0.05F, 0.05F}) return;

                acceleration({ 0.05F, 0.05F });
                max_velocity( {0.4F, 0.7F} );

                if (velocity().y > 0.0F) {                                 
                    if (!sound::is_playing(m_water_enter_sound_id)) {
                        sound::position(m_water_enter_sound_id, { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                        sound::play(m_water_enter_sound_id);
                        spawn_fan(235.0F, 305.0F, 3, particle::Type::DROP, position() + Vec2F{ -4.0F, 0.0F }, Vec2F{ velocity().x * 0.1F, -velocity().y * 0.1F }, 1.5F);
                    }
                }

            }
        }
    };
}