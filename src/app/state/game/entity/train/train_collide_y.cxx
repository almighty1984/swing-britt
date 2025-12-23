module;
#include <cmath>
#include <sstream>

module entity.train;

namespace entity {
    void Train::collide_y(aabb::cInfo our, aabb::cInfo other) {
        aabb::cName our_name = aabb::name(our.id);

        if (our_name != aabb::Name::BONE) {
            return;
        }

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();

        aabb::cName other_name = aabb::name(other.id);

        if (!is_track(other_type) &&
            //other_type != Type::PLAYER &&
            other_type != Type::TRIGGER &&
            other_type != Type::TRACK_TRIGGER_UL) return;


        /*if (std::abs(velocity().x) > std::abs(velocity().y)) {
            collide_x(our, other);
            return;
        }*/


        cF32 diff_x = other_rect.x - our_rect.x;
        cF32 diff_y = other_rect.y - our_rect.y;

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        bool is_larger_vel_y = std::abs(velocity().y) > std::abs(velocity().x);

        //m_speed = 0.0F;
        if (is_track(other_type) || other_type == Type::TRIGGER) {
            m_is_powered = other.owner->is_powered();
            m_time_left_alive = other.owner->time_left_alive();
            //m_speed = m_start_speed;
            //console::log("time_left_alive: ", other.owner->time_left_alive(), "\n");
            //console::log("is_powered: ", m_is_powered, "\n");
        }

        if (other_type == Type::TRACK_UD) {
            if (other_name == aabb::Name::TRACK) {
                add_position_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
                m_direction.x = 0.0F;
                if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                } else if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
            }
        } else if (other_type == Type::TRACK_LR || other_type == Type::TRACK_TRIGGER_UL || other_type == Type::TRACK_TRIGGER_UR) {
            if (other_name == aabb::Name::TRACK) {
                add_position_y(other_rect.y - our_rect.y);
                velocity_y(0.0F);
                m_direction.y = 0.0F;
                if (m_direction.x < 0.0F) {
                    velocity_x(-std::abs(m_speed));
                } else if (m_direction.x > 0.0F) {
                    velocity_x(std::abs(m_speed));
                }
            }
        } else if (other_type == Type::TRACK_TRIGGER_UL || other_type == Type::TRACK_LR) {
            if (other_name == aabb::Name::UP) {
                if (our_rect.y < other_rect.h) {
                    velocity_y(0.0F);
                    add_position_y(-overlap_y);
                }
            } else if (other_name == aabb::Name::DOWN) {
                if (our_rect.h > other_rect.y) {
                    velocity_y(0.0F);
                    add_position_y(-overlap_y);
                }
            } else if (other_name == aabb::Name::TRACK) {
                if (m_direction.x < 0.0F) {
                    velocity_x(-std::abs(m_speed));
                } else if (m_direction.x > 0.0F) {
                    velocity_x(std::abs(m_speed));
                }
                velocity_y(diff_y / 16.0F * std::abs(m_speed));
            }
        } else if (other_type == Type::TRACK_CORNER_UL) {
            if (other_name == aabb::Name::UP) {
                if (m_direction.y < 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity_y(0.0F);
                        velocity_x(std::abs(m_speed));
                        add_position_y(other_rect.y - our_rect.y);
                    }
                } else if (m_direction.y > 0.0F && m_direction.x < 0.0F) {
                    add_position_y(other_rect.y - our_rect.y);
                    velocity_y(0.0F);                    
                    velocity_x(-std::abs(m_speed));
                }
            } else if (other_name == aabb::Name::DOWN) {
                if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                } else if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                add_position_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
            }
        } else if (other_type == Type::TRACK_CORNER_UR) {
            if (other_name == aabb::Name::UP) {
                if (m_direction.y < 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity_y(0.0F);
                        velocity_x(-std::abs(m_speed));
                        add_position_y(other_rect.y - our_rect.y);
                    }
                }
            } else if (other_name == aabb::Name::DOWN) {
                if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));                    
                } else if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                add_position_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
            }
        } else if (other_type == Type::TRACK_CORNER_DL) {
            if (other_name == aabb::Name::UP) {
                if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                } else if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                add_position_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
            } else if (other_name == aabb::Name::DOWN) {
                if (m_direction.y > 0.0F && m_direction.x >= 0.0F) {
                    if (our_rect.y > other_rect.y) {
                        velocity_y(0.0F);
                        velocity_x(std::abs(m_speed));
                        add_position_y(other_rect.y - our_rect.y);
                    }
                } else if (m_direction.y < 0.0F && m_direction.x < 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity_y(0.0F);
                        velocity_x(-std::abs(m_speed));
                        add_position_y(other_rect.y - our_rect.y);
                    }
                }
            }
        } else if (other_type == Type::TRACK_CORNER_DR) {
            if (other_name == aabb::Name::UP) {
                if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                } else if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                add_position_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
            } else if (other_name == aabb::Name::DOWN) {
                if (m_direction.y > 0.0F) {
                    if (our_rect.y > other_rect.y) {
                        add_position_y(other_rect.y - our_rect.y);
                        velocity_y(0.0F);
                        velocity_x(-std::abs(m_speed));
                    }
                } else if (m_direction.x > 0.0F) {
                    if (our_rect.y < other_rect.y) {                        
                        add_position_y(other_rect.y - our_rect.y); 
                        velocity_x(std::abs(m_speed));
                    }
                }
            }
        } else if (other_type == Type::TRACK_L_1X1_0 || other_type == Type::TRACK_L_1X1_1) {
            if (other_name == aabb::Name::TRACK) {
                if (m_direction.y > 0.0F && m_direction.x == 0.0F) {
                    if (our_rect.y > other_rect.y) {
                        velocity({ std::abs(m_speed) * 0.71F, std::abs(m_speed) * 0.71F });
                    }
                }
                else if (m_direction.x > 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity({ std::abs(m_speed) * 0.71F, std::abs(m_speed) * 0.71F });

                        //add_position_y(other_rect.y - our_rect.y);
                    }
                } else if (m_direction.y < 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity({ -std::abs(m_speed) * 0.71F, -std::abs(m_speed) * 0.71F });
                    }
                }
            }
        } else if (other_type == Type::TRACK_R_1X1_0 || other_type == Type::TRACK_R_1X1_1) {
            if (other_name == aabb::Name::TRACK) {
                if (m_direction.y > 0.0F && m_direction.x <= 0.0F) {
                    if (our_rect.y > other_rect.y) {
                        velocity_x(-std::abs(m_speed) * 0.71F);
                        velocity_y( std::abs(m_speed) * 0.71F);
                    }
                }
                else if (m_direction.y < 0.0F && m_direction.x >= 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity_x( std::abs(m_speed) * 0.71F);
                        velocity_y(-std::abs(m_speed) * 0.71F);

                        //add_position_y(other_rect.y - our_rect.y);
                    }
                } else if (m_direction.y < 0.0F && m_direction.x <= 0.0F) {

                    add_position_y(other_rect.y - our_rect.y);
                    if (our_rect.y < other_rect.y) {
                        velocity_x(-std::abs(m_speed) * 0.71F);
                        velocity_y( std::abs(m_speed) * 0.71F);
                    }
                }
            }
        } else if (other_type == Type::TRACK_L_1X2_0 || other_type == Type::TRACK_L_1X2_1) {
            if (m_direction.y < 0.0F) {
                if (our_rect.y < other_rect.y) {
                    velocity_y(-std::abs(m_speed) * 0.75F);
                    velocity_x(-std::abs(m_speed) * 0.5F);
                }
            } else if (m_direction.y > 0.0F) {
                if (our_rect.y > other_rect.y) {
                    velocity_y(std::abs(m_speed) * 0.9F);
                    velocity_x(std::abs(m_speed) * 0.5F);
                }
            }
        }
        else if (other_type == Type::TRACK_R_1X2_0 || other_type == Type::TRACK_R_1X2_1) {
            if (m_direction.y < 0.0F/* && m_direction.x >= 0.0F*/) {
                if (our_rect.y < other_rect.y) {
                    velocity_y(-std::abs(m_speed) * 0.9F);
                    velocity_x(std::abs(m_speed) * 0.5F);
                }
            } else if (m_direction.y > 0.0F/* && m_direction.x <= 0.0F*/) {
                if (our_rect.y > other_rect.y) {
                    velocity_y(std::abs(m_speed) * 0.9F);
                    velocity_x(-std::abs(m_speed) * 0.5F);
                }
            }
        } else if (other_type == Type::TRACK_L_2X1_0 || other_type == Type::TRACK_L_2X1_1) {
            if (m_direction.y < 0.0F && m_direction.x <= 0.0F) {
                if (our_rect.y < other_rect.y) {
                    velocity_x(-std::abs(m_speed) * 0.9F);
                    velocity_y(-std::abs(m_speed) * 0.5F);
                }
            } else if (m_direction.y > 0.0F && m_direction.x >= 0.0F) {
                if (our_rect.y > other_rect.y) {
                    velocity_x(std::abs(m_speed) * 0.9F);
                    velocity_y(std::abs(m_speed) * 0.5F);
                }
            }
        } else if (other_type == Type::TRACK_R_2X1_0 || other_type == Type::TRACK_R_2X1_1) {
            if (m_direction.y < 0.0F && m_direction.x >= 0.0F) {
                if (our_rect.y < other_rect.y) {
                    velocity_x(std::abs(m_speed) * 0.9F);
                    velocity_y(-std::abs(m_speed) * 0.5F);
                }
            } else if (m_direction.y > 0.0F && m_direction.x <= 0.0F) {
                if (our_rect.y > other_rect.y) {
                    velocity_x(-std::abs(m_speed) * 0.9F);
                    velocity_y(std::abs(m_speed) * 0.5F);
                }
            }
        }
    }
}