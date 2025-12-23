module;
#include <sstream>

module state.game;
import camera;
import collider.aabb;
import console;
import transform;
import random;
import entity.particle.bubble;
import entity.particle.drop;
import entity.particle.sense;
import entity.water_line;
import sound;
import particle;

namespace state {
    void Game::update_unlocked() {
        for (auto& i : m_unlocked_entity_objects) {
            if (i) i->update();
        }
    }
    void Game::update(cF32 ts) {
        //console::log("visible layers: ", m_visible_layers.size(), "\n");
        if (input::is_pressed(m_input_id, input::Key::T) && !is_to_transition) {
            input::release(m_input_id, input::Key::T);
            console::log("state::Game pressed t\n");
            is_to_transition = true;
            
            m_start_info = { .type = start::Type::CENTER,
                             .number = 0 };

            
            return;
        }
        if (input::is_pressed(m_input_id, input::Key::P)) {
            input::release(m_input_id, input::Key::P);
            camera::init_shake(320, { 24, 32 }, { 24, 64 }, 0.01F);
        }
        if (input::is_pressed(m_input_id, input::Key::CTRL)) {
            if (input::is_pressed(m_input_id, input::Key::LEFT)) {
                camera::focal_point.x += 4.0F;
            }
            if (input::is_pressed(m_input_id, input::Key::RIGHT)) {
                camera::focal_point.x -= 4.0F;
            }
            if (input::is_pressed(m_input_id, input::Key::UP)) {
                camera::focal_point.y += 4.0F;
            }
            if (input::is_pressed(m_input_id, input::Key::DOWN)) {
                camera::focal_point.y -= 4.0F;
            }
            console::log("state::Game::update camera focal point: ", camera::focal_point.x, " ", camera::focal_point.y, "\n");
        } else {
            camera::focal_point = { WINDOW_W / 2.0F, WINDOW_H / 2.0F };
        }

        //console::log("level transform: ", m_level_transform_id, " position: ", transform::get(m_level_transform_id)->position.x, "\n");
        //console::log("transforms: ", transform::size(), " unused: ", transform::unused_size(), "\n");

        particle::check_to_erase();
        particle::check_to_spawn();
        particle::update();

        camera::update();
        m_bg_planes.set_velocity(-camera::difference / 2.0F);

        m_player.update();

        m_bg_planes.update();
        //console::log("focus transform: ", camera::focus_transform, " ", m_player.get_transform_id(), "\n");

        for (auto& i : m_entity_objects) {
            if (i && !entity::is_water_line(i->type()) && !entity::is_track(i->type())) {

                if (!i->is_start_in_view() && i->state() == entity::State::DEAD && i->time_left_dead() == 0 && i->time_to_be_dead() != U16_MAX) {
                    console::log("revived ", entity::to_string(i->type()), "\n");
                    i->next_state(i->start_state());
                    i->position(i->start_position());
                }

                i->update();
            }
        }
        for (auto& i : m_water_entity_objects) {
            if (i) i->update();
        }


        if (camera::focus_transform == m_player.get_transform_id() &&
            (m_player.position().x < -8.0F || m_player.position().x > m_window_w - 8.0F)) {
            m_start_info = m_player.next_start();
            is_to_transition = true;
            m_next_level = m_player.next_level();
            console::log("state::Game next level: ", start::to_string(m_next_level), "\n");
            return;
            //restart();
        }
        
        
        if (input::is_pressed(m_input_id, input::Key::F1)) {
            input::release(m_input_id, input::Key::F1);
            //console::log("level path: ", m_level_path, "\n");
            
            //m_next_start = start::Type::NONE;
            
            is_to_transition = true;

            m_start_info = { .type = start::Type::NONE,
                             .number = 0 };            

            m_next = Type::EDIT;

            return;
        }

        //next_start(m_player.next_start().type);


        if (input::is_pressed(m_input_id, input::Key::G)) {
            input::release(m_input_id, input::Key::G);
            is_drawing_debug = !is_drawing_debug;
        }
        /*if (input::is_pressed(m_input_id, input::Key::ENTER)) {
            input::release(m_input_id, input::Key::ENTER);
            if (m_camera.focus_transform == m_player.get_transform_id()) {
                m_camera.focus_transform = m_player2.get_transform_id();
            } else if (m_camera.focus_transform == m_player2.get_transform_id()) {
                m_camera.focus_transform = m_player3.get_transform_id();
            } else if (m_camera.focus_transform == m_player3.get_transform_id()) {
                m_camera.focus_transform = m_player.get_transform_id();
            }
        }*/

        if (input::is_pressed(m_input_id, input::Key::SPACE)) {
            input::release(m_input_id, input::Key::SPACE);
            //m_sound.play();

        }

        //console::log("state::Game::update mouse: ", input::mouse.x, " ", input::mouse.y, "\n");

        /*if (input::is_pressed(m_input_id, input::Key::UP)) {
            input::release(m_input_id, input::Key::UP);
            console::log("Game::update pressed up\n");
        }*/
                

        if (m_player.is_to_write_save()) {
            is_to_write_save = true;
            m_player.is_to_write_save(false);
        }

        
        //m_player2.update();
        //m_player3.update();

        
       /* for (auto& i : m_entity_objects) {
            if (i && i->time_left_alive() > 0) i->time_left_alive(i->time_left_alive() - 1);
        }*/



        //console::log("entity objects size: ", m_entity_objects.size(), "\n");

        //m_sound.position({ (m_player.position().x + sprite::get(m_player.aabb_ids().front())->source_rect.w / 2.0F) / m_window_w,
        //                   (m_player.position().y + sprite::get(m_player.aabb_ids().front())->source_rect.h / 2.0F) / m_window_h });

        //console::log("state::Game::update sound position: ", m_sound.position().x, " ", m_sound.position().y, "\n");

        //console::log("quadnode x:", m_level_quad_trees.front().second->x(), "\n");




        //console::log("position: ", m_player.position().x, " ", m_player.position().y, "\n");
        //console::log("position: ", m_player.position().x + m_camera.position.x, " ", m_player.position().y + m_camera.position.y, "\n");

        //console::log("level position: ", transform::get(m_level_transform_id)->position.x, " ", transform::get(m_level_transform_id)->position.y, "\n");


        //console::log("camera position: ", m_camera.position.x, " ", m_camera.position.y, "\n");

        //if (m_player.position().x < -collider::aabb::get(m_player.aabb_ids().front())->get_rect().w || m_player.position().x > m_window_w) {

        
        /*else if (m_camera.focus_transform == m_player2.get_transform_id() &&
            (m_player2.position().x < -8.0F || m_player2.position().x > m_window_w)) {
            start_info(m_player2.next_start());
            restart();
        } else if (m_camera.focus_transform == m_player3.get_transform_id() &&
            (m_player3.position().x < -8.0F || m_player3.position().x > m_window_w)) {
            start_info(m_player3.next_start());
            restart();
        }*/


        if (input::is_pressed(m_input_id, input::Key::GRAVE)) {
            input::release(m_input_id, input::Key::GRAVE);
            is_to_change_view = true;
            if (view() == RectF{0.0F, 0.0F, 480.0F, 270.0F}) {
                view(RectF{ 0.0F, 0.0F, 320.0F, 180.0F });
            } else {
                view(RectF{ 0.0F, 0.0F, 480.0F, 270.0F });
            }
        }


        quad_trees_check_collision();
    }
}