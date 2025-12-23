module;
#include <cmath>
#include <sstream>

export module entity.water_line;
import anim;
import console;
import entity;
import collider.aabb;
import line;
import random;

export namespace entity {
    class WaterLine : public Object {
        F32 m_start_y,
            m_line_offset_y;
        I32 m_top_line_id,
            m_bottom_line_id;
        U8 m_timer,
           m_top_line_size,
           m_bottom_line_size;

    public:
        WaterLine() : m_start_y(0.0F),
                      m_line_offset_y(4.0F),
                      m_top_line_id(-1),
                      m_bottom_line_id(-1),
                      m_timer(0),
                      m_top_line_size(2),
                      m_bottom_line_size(3) {
            m_input_limit = 2;
        }
        ~WaterLine() {            
            line::erase(m_top_line_id);
            line::erase(m_bottom_line_id);
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            //if (other.owner && other.owner->type() == Type::PLAYER) {
                collide_y(our, other);
            //}
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {            
            cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cType other_type = other.owner->type();

            if (other_type != Type::PLAYER && other_type != Type::BUG) {
                return;
            }
            
            cVec2F other_velocity = other.owner->velocity();
            cVec2F our_velocity = velocity();

                                    
            cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);
            if (other_type == Type::BUG) {
                if (other.owner->state() == State::SWIM) {               
                    if (std::abs(other_velocity.y) < 1.0F) {
                        return;
                    }
                    velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
                    //position_y(other.owner->position().y + 4.0F);
                }
            }
            else if (other_type == Type::PLAYER) {

                cF32 start_diff_y = m_start_y - position_on_level().y;
                if (std::abs(start_diff_y) > 8.0F || std::abs(other_velocity.y) < 1.0F) {
                    return;
                }
                //console::log("start_diff_y: ", start_diff_y, "\n");

                //if (velocity().y < 0.0F)
                    //velocity_y(other.owner->velocity().y * 1.0F;
                //else
                    velocity_y(other.owner->velocity().y * 0.3F);

                    //console::log("WaterLine::collide_y velocity y: ", velocity().y, "\n");

                //if (std::abs(start_diff_y) < 8.0F/* && std::abs(other.owner->velocity().y) > 0.5F*/) {
                    //velocity_y(other.owner->velocity().y * 0.9F;
                    //moved_velocity_y(other.owner->velocity().y * 1.0F;
                //}
                    //velocity_y(other.owner->velocity().y / std::abs(start_diff_y);
                    //velocity_y(other.owner->velocity().y * 1.0F;

                    

                    /*if (input_objects.front()) {
                        input_objects.front()->velocity_y(velocity().y * 0.3F + input_objects.front()->velocity().y * 0.5F;
                        if (input_objects.front()->input_objects.front()) {
                            input_objects.front()->input_objects.front()->velocity_y(input_objects.front()->velocity().y;
                        }
                    }
                    if (input_objects.back()) {
                        input_objects.back()->velocity_y(velocity().y * 0.3F + input_objects.back()->velocity().y * 0.5F;
                        if (input_objects.back()->input_objects.back()) {
                            input_objects.back()->input_objects.back()->velocity_y(input_objects.back()->velocity().y;
                        }
                    }*/

                    
                //}
            }
        }

        virtual void draw(std::unique_ptr<Window>& window) override {
            line::draw(window, m_top_line_id);
            line::draw(window, m_top_line_id);
        }

        void update() override {
            if (input_objects.empty()) return;

            if (m_is_first_update) {
                m_is_first_update = false;

                velocity_y(random::real_number(-0.1, 0.1));

                //console::log("water input: ", input_objects.size(), "\n");

                m_start_y = position_on_level().y;

                m_top_line_id = line::make(position() + Vec2F{ 0.0F, 2.0F },
                                           input_objects.back()->position() + Vec2F{ 0.0F, m_line_offset_y });                    
                line::size(m_top_line_id, m_top_line_size);
                line::color(m_top_line_id, { 52, 255, 255 });                
                line::layer(m_top_line_id, m_start_layer);

                m_bottom_line_id = line::make(position() + Vec2F{ 0.0F, 4.0F},
                                              input_objects.back()->position()  + Vec2F{  0.0F, m_line_offset_y });                    
                line::size(m_bottom_line_id, m_bottom_line_size);
                line::color(m_bottom_line_id, { 52, 206, 206 });
                
                line::layer(m_bottom_line_id, m_start_layer);
            }

            cF32 diff_y = position_on_level().y - m_start_y;
            add_velocity({ 0.0F, -diff_y * 0.025f });

            if (std::abs(velocity().y) > 0.1F) {
                velocity_y(velocity().y * 0.93f);
            } else {
                /*if (diff_y < 0.0F && diff_y > -0.04f) {
                    diff_y = -0.04f;
                } else if (diff_y >= 0.0F && diff_y < 0.04f) {
                    diff_y = 0.04f;
                }*/
            }

            if (m_timer < 2) {
                ++m_timer;
            }
            else {
                m_timer = 0;

                if (input_objects.size() == 2) {
                    if (std::abs(velocity().y) > std::abs(input_objects.front()->velocity().y)) {
                        //input_objects.front()->velocity_y(velocity().y * 0.95f + input_objects.front()->velocity().y * 0.1F;
                        input_objects.front()->velocity_y(velocity().y * 0.99f);
                    }
                    if (std::abs(velocity().y) > std::abs(input_objects.back()->velocity().y)) {
                        //input_objects.back()->velocity_y(velocity().y * 0.95f + input_objects.back()->velocity().y * 0.1F;
                        input_objects.back()->velocity_y(velocity().y * 0.99f);
                    }
                }
            }
            //moved_velocity().y -= diff_y * 0.1F;            
            //velocity().y *= 0.9F;
            /*if (diff_y < 0.0F && diff_y < -16.0F) {
                position().y += ((m_start_y - 8.0F) - position_on_level().y);
            }
            if (diff_y > 0.0F && diff_y > 16.0F) {
                position().y += ((m_start_y + 8.0F) - position_on_level().y);
            }*/

            cF32 half_w = 1.0F;
            cF32 size_offset = m_top_line_size / 4.0F;                

            Vec2F start = position(), 
                  end   = position() + Vec2F{ 0.0F, -8.0F };
            if (input_objects.size() == 1) {
                if (position().x > input_objects.back()->position().x) {
                    // R
                    start = input_objects.back()->position() + Vec2F{ half_w - size_offset, m_line_offset_y };
                    end   = position() + Vec2F{ half_w * 2 + size_offset + 1.5F, m_line_offset_y };                        
                } else {
                    // L
                    start = position() + Vec2F{ -0.1F, m_line_offset_y };
                    end   = input_objects.back()->position() + Vec2F{ half_w + size_offset, m_line_offset_y };                        
                }
            } else if (input_objects.size() == 2) {
                start = input_objects.front()->position() + Vec2F{ half_w - size_offset, m_line_offset_y };
                end   = input_objects.back()->position()  + Vec2F{ half_w + size_offset, m_line_offset_y };
            }

            line::set(m_top_line_id, start, end);
            line::set(m_bottom_line_id, start + Vec2F{ 0.0F, size_offset * 1.0F },
                                        end   + Vec2F{ 0.0F, size_offset * 1.0F });
        }
    };
}