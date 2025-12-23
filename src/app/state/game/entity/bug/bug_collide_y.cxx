module;
#include <cmath>
#include <sstream>
module entity.bug;
import collider.aabb;
import console;
import sound;
import particle;

namespace entity {
    void Bug::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent || is_dead() || other.owner->is_dead()) return;

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        /*if (velocity().y > 0.0F && our_rect.h - 4.0F - velocity().y > other_rect.y) {
            return;
        }*/

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        //console::log("type: ", to_string(other_type), "\n");

        if (is_arch(other_type)) {
            if (m_parent || other.owner->parent() || is_hurting()) return;
            console::log("Bug::collide_y ", to_string(other_type), "\n");
            add_position_y(-overlap_y);
            if (velocity().y <= -2.0F || velocity().y >= 2.0F) {

                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                velocity_y(velocity().y * -0.9F);
                spawn(particle::Type::HIT, hit_pos, velocity());
                hurt(other.owner);
                return;
            }
        } else if (other_type == Type::BUG) {
            if (other.owner->parent()) return;

        } else if (other_type == Type::BRICK) {
            if (m_parent || other.owner->parent()) return;
            if (our_rect.y > other_rect.y) return;
            //if (our_rect.y > other_rect.h - 4.0F) return;

            console::log("Bug::collide_y brick\n");
            if (our_rect.h < other_rect.y + 4.0F) {
                add_position_y(-overlap_y);
            }
            if (velocity().y <= -2.0F || velocity().y >= 2.0F) {

                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                velocity_y(velocity().y * -0.9F);
                spawn(particle::Type::HIT, hit_pos, velocity());
                hurt(other.owner);
                //other.owner->hurt(this);
                return;
            }
        } else if (other_type == Type::CLIP || other_type == Type::CLIP_LEDGE) {
            if (our_rect.y < other_rect.h) {
                if (velocity().y > 0.0F) {
                    add_position_y(-overlap_y);
                    //console::log("Bug::collide_y hmmmmm\n");                
                }
            }
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
            }

            //if (!m_is_on_ground) return;
            if (other_type == Type::CLIP_LEDGE) {
                if (velocity().y > 0.0F && our_rect.y > other_rect.y) return;

                if (velocity().y <= -2.0F || velocity().y >= 2.0F) {
                    velocity_y(velocity().y * -0.9F);
                    cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                       (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                    spawn(particle::Type::HIT, hit_pos, {});
                    hurt(other.owner);
                    m_next_state = State::HURT;
                    sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound::play(m_melee_sound_id);
                    return;
                }
                if (m_state == State::WALK) {
                    if (velocity().x > 0.0F && other_rect.w > our_rect.w) {
                        velocity_x(velocity().x * -1.0F);
                        sprite::is_leftward(m_sprite_id, true);
                    } else if (velocity().x < 0.0F && other_rect.x < our_rect.x) {
                        velocity_x(velocity().x * -1.0F);
                        sprite::is_leftward(m_sprite_id, false);
                    }
                }
            }
        } else if (other_type == Type::CLIP_U || other_type == Type::SLOPE_U) {
            if (velocity().y < 0.0F) return;
            if (our_rect.y > other_rect.y) return;

            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::SLOPE_U;
        } else if (other_type == Type::CLIP_D || other_type == Type::CLIP_LD || other_type == Type::CLIP_RD) {
            if (velocity().y > 0.0F) return;
            add_position_y(-overlap_y);
            if (velocity().y <= -2.0F || velocity().y >= 2.0F) {
                velocity_y(velocity().y * -0.9F);
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                spawn(particle::Type::HIT, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::HURT;
                sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::play(m_melee_sound_id);
                return;
            }
            m_is_on_ground = false;
            m_is_on_slope = false;
            velocity_y(std::abs(velocity().y) * 0.75F);

            m_next_state = State::HURT;
        } else if (other_type == Type::FROG) {
            console::log("Bug::collide_y frog\n");
            if (m_state == State::TOSSED || (m_state == State::UPENDED && our_rect.y < other_rect.y)) {
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };
                spawn(particle::Type::HIT, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::HURT;
                other.owner->hurt(this);
                //velocity().x *= -1.0f;
                velocity_y(velocity().y * -1.0f);
                sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::play(m_melee_sound_id);
                return;
            }

            if (our_rect.h < other_rect.y + 4.0F) {
                add_position_y(-overlap_y);
                if (m_state == State::UPENDED) {
                    m_next_state = State::BOUNCE;
                }
            }
            //if (other_rect.y > our_rect.y) {            
                //if (!is_hurting()) {
                //    velocity_y(other.owner->velocity().y + velocity().y - 10.0f;
                //}
            //} else {
                //add_position_y( -overlap_y );            
            //}
        } else if (other_type == Type::PARTICLE_INTERACT) {
            collide_x(our, other);
        } else if (other_type == Type::PLAYER) {
            if (m_state == State::UPENDED || m_state == State::BOUNCE) {                
                if (other_velocity.y > 1.0F) {
                    console::log("entity::BUG::collide_y() PLAYER\n");
                    m_state = State::UPENDED;
                    m_next_state = State::BOUNCE;
                    return;
                }
            }
            //console::log("Bug::collide_y player overlap_y: ", overlap_y, "\n");
            if (m_state == State::SWIM) {
                //velocity().y += other_velocity.y;
                //moved_velocity_y(other_velocity.y;
                return;
            }
            if (other.owner->state() == State::SWING) return;

            if (our_rect.y > other_rect.h - 2.0F/* - other_velocity.y*/) {
                
            } else if (our_rect.h <= other_rect.y + 4.0F) {
                cVec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
                cVec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

                cVec2F hit_pos = Vec2F{ our_center.x + other_center.x, our_center.y + other_center.y } / 2.0F - Vec2F{ 8.0F, 8.0F };

                spawn(particle::Type::HIT, hit_pos, {});

                sound::position(m_melee_sound_id, { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound::play(m_melee_sound_id);
            }
        } else if (other_type == Type::SLOPE_L_1X1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(-velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_1X1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_L_2X1_0 || other_type == Type::SLOPE_L_2X1_1) {
            add_position_y(-overlap_y);
            moved_velocity_y(0.0F);
            velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::SLOPE_R_2X1_0 || other_type == Type::SLOPE_R_2X1_1) {
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            //if (velocity().x > 0.0F) {
                //velocity_y(velocity().x / 2.0F;
            //}
            if (velocity().y > 0.0F) {
                velocity_x(velocity().y / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::WATER_LINE) {
            //m_water_line_y = other.owner->start_position().y - 0.0F;
            m_water_line_y = other.owner->position_on_level().y - 0.0F;

            //other.owner->position_y(position().y + 4.0F);

            //other.owner->velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F;

            if (velocity().y > 0.0F && m_state != State::SWIM) {
                m_next_state = State::SWIM;
            }
            if (m_state == State::SWIM) {
                //velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
                //other.owner->velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }
        }
    }
}