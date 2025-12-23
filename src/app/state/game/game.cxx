module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>

module state.game;
import camera;
import collider.aabb;
import console;
import input;
import line;
import quad_tree;
import sound;
import sprite;
import transform;
import types;

std::mutex quad_tree_node_mutex;

namespace state {
    Game::Game(cU16 window_w, cU16 window_h, std::filesystem::path level_path, start::Info start) {
        //console::log("Game::Game level: ", level_path, " start: ", start_position.x, " ", start_position.y, "\n");
        m_current = m_next = Type::GAME;
        m_window_w = window_w, m_window_h = window_h;

        start_info(start);

        console::log("transform::size: ", transform::size(), "\n");

        console::log("transform::unused_size: ", transform::unused_size(), "\n");

        m_bg_planes.create(3, 1);

        for (auto& i : m_bg_planes.get_sprite_ids()) {
            m_visible_layers.insert(sprite::layer(i));
        }        

        m_level_transform_id = transform::make();

        transform::level_id(m_level_transform_id);

        console::log("m_level_transform_id: ", m_level_transform_id, "\n");

        //console::log("level transform_id: ", m_level_transform_id, "\n");

        //m_entity_objects.emplace_back(std::make_unique<entity::Player>());
        //m_entity_objects.back()->level_transform_id = m_level_transform_id;
        //m_entity_objects.back()->position(m_start_position);

        load_level(level_path);
        m_level_path = level_path;

        /*for (U8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }*/

        m_visible_layers.insert(NUM_VISIBLE_LAYERS - 1);  // for particles like blood
                

        //m_player.start_offset(m_start_position);
        //m_player.load_config("res/entity/player/player.cfg");
        
        for (auto& i : m_player.aabb_ids()) {
            if (aabb::name(i) == aabb::Name::BODY) {
                m_player.position(m_start_position + Vec2F{ 0.0F, 16.0F - aabb::h(i) });
            }
        }

        

        console::log("start position: ", m_start_position.x, " ", m_start_position.y, "\n");

        //m_player.position() = Vec2F{ 320.0F, 0.0F };
        
        //m_player.id = 0;

        //m_player2.position(m_start_position - Vec2F{ 16.0F, 0.0F });


        //m_player2.id = 1;

        /*m_player2.key_up = input::Key::W;
        m_player2.key_down = input::Key::S;
        m_player2.key_left = input::Key::A;
        m_player2.key_right = input::Key::D;
        m_player2.key_jump = input::Key::G;
        m_player2.key_sprint = input::Key::H;
        m_player2.key_melee = input::Key::F;*/

        /*m_player3.set_layer(NUM_VISIBLE_LAYERS - 1);
        m_player3.position(m_start_position + Vec2F{ 16.0F, 0.0F });

        m_player3.key_up = input::Key::I;
        m_player3.key_down = input::Key::K;
        m_player3.key_left = input::Key::J;
        m_player3.key_right = input::Key::L;
        m_player3.key_jump = input::Key::R;
        m_player3.key_sprint = input::Key::E;
        m_player3.key_melee = input::Key::T;*/

        //m_player3.id = 2;

        //transform::get(m_transform_id)->velocity = { 2.0F, 1.0F };
        //transform::get(m_transform_id)->deceleration = { 0.001f, 0.001f };
        //console::log("Game transform_id: ", m_transform_id, "\n");

        m_input_id = input::make();

        //console::log("sprite::Objects size: ", sprite::size(), "\n");



       /* for (auto& i : m_bg_planes.get_transform_ids()) {
            m_camera.add_transform_id(i);
        }*/

        //camera::clear();
        camera::add_transform_id(m_level_transform_id);
        camera::add_transform_id(m_player.get_transform_id());

        camera::focus_transform = m_player.get_transform_id();
        //m_camera.add_transform_id(m_player2.get_transform_id());
        //m_camera.add_transform_id(m_player3.get_transform_id());

        //Vec2F camera_position = m_player.position() - Vec2F{ window_w / 2.0F, window_h / 2.0F };
        cVec2F camera_position = m_start_position - Vec2F{ window_w / 2.0F, window_h / 2.0F + 8.0F };


        //console::log("\nstate::Game::Game camera position: ", camera_position.x, " ", camera_position.y, "\n\n");

        camera::set_position(camera_position);

        //m_camera.add_transform_id(m_transform_id);

    }

    void Game::check_to_add_input_from(entity::Object* trigger_entity) {
        //if (trigger_entity->type() != entity::Type::TRACK_TRIGGER_UL) {
        //    //console::log("")
        //    return;
        //}
        entity::Object* prev_entity = trigger_entity;
        entity::Object* start_entity = nullptr;

        Vec2F offset_to_next, offset_to_next2;
        for (auto& entity : m_unlocked_entity_objects) {
            if (entity->start_offset() == trigger_entity->start_offset() + Vec2F{ 0.0F,-16.0F }) {
                console::log("check_to_add_input_from() entity above trigger: ", entity::to_string(entity->type()), "\n");
                //current_offset = trigger_entity->start_offset() + Vec2F{ 0.0F,-16.0F };

                entity::cType type = entity->type();
                if (type == entity::Type::CONDUIT_CORNER_UR || type == entity::Type::CONDUIT_TRIGGER_UL || type == entity::Type::TRACK_TRIGGER_UL) {
                    offset_to_next = { -16.0F, 0.0F };
                } else if (type == entity::Type::CONDUIT_CORNER_UL || type == entity::Type::CONDUIT_TRIGGER_UR || type == entity::Type::TRACK_TRIGGER_UR) {
                    offset_to_next = { 16.0F, 0.0F };
                } else {
                    continue;
                }

                entity->add_input_object(trigger_entity);
                prev_entity = entity;
                start_entity = entity;
                break;
            }
        }

    restart_loop:

        for (auto& entity : m_unlocked_entity_objects) {
            if (!entity::is_track(entity->type()) && !entity::is_conduit(entity->type())) continue;
            if (entity == prev_entity) continue;

            if (entity->start_offset() == prev_entity->start_offset() + offset_to_next ||
                (offset_to_next2 != Vec2F{ 0.0F, 0.0F } && entity->start_offset() == prev_entity->start_offset() + offset_to_next2)) {
                if (entity == start_entity) break;

                if (entity->type() == entity::Type::CONDUIT_TRIGGER_UL || entity->type() == entity::Type::CONDUIT_TRIGGER_UR ||
                    entity->type() == entity::Type::TRACK_TRIGGER_UL || entity->type() == entity::Type::TRACK_TRIGGER_UR) {
                    break;
                }

                console::log("check_to_add_input_from() entity: ", entity::to_string(entity->type()), "\n");
                entity->add_input_object(prev_entity);
                
                cVec2F pos = entity->start_offset(),
                       prev_pos = prev_entity->start_offset();

                entity::cType type = entity->type();

                if (offset_to_next2 != Vec2F{ 0.0F, 0.0F }) {
                    console::log("offset_to_next2 ", offset_to_next2.x, " ", offset_to_next2.y, "\n");
                }

                offset_to_next2 = {};

                if (type == entity::Type::CONDUIT_UD || type == entity::Type::TRACK_UD) {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{   0.0F,  16.0F } : Vec2F{   0.0F, -16.0F };
                } else if (type == entity::Type::CONDUIT_LR || type == entity::Type::TRACK_LR) {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{  16.0F,   0.0F } : Vec2F{ -16.0F,   0.0F };
                } else if (type == entity::Type::CONDUIT_CORNER_DL || type == entity::Type::TRACK_CORNER_DL) {
                    offset_to_next = prev_pos.x > pos.x ? Vec2F{   0.0F, -16.0F } : Vec2F{  16.0F,   0.0F };
                } else if (type == entity::Type::CONDUIT_CORNER_UL || type == entity::Type::TRACK_CORNER_UL) {
                    offset_to_next = prev_pos.x > pos.x ? Vec2F{   0.0F,  16.0F } : Vec2F{  16.0F,   0.0F };
                } else if (type == entity::Type::CONDUIT_CORNER_UR || type == entity::Type::TRACK_CORNER_UR) {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{   0.0F,  16.0F } : Vec2F{ -16.0F,   0.0F };
                } else if (type == entity::Type::CONDUIT_CORNER_DR || type == entity::Type::TRACK_CORNER_DR) {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{   0.0F, -16.0F } : Vec2F{ -16.0F,   0.0F };
                } else if (type == entity::Type::CONDUIT_L_1X1_0 || type == entity::Type::TRACK_L_1X1_0) {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{   0.0F,  16.0F } : Vec2F{ -16.0F,   0.0F };
                } else if (type == entity::Type::CONDUIT_L_1X1_1 || type == entity::Type::TRACK_L_1X1_1) {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{  16.0F,   0.0F } : Vec2F{   0.0F, -16.0F };
                } else if (type == entity::Type::CONDUIT_R_1X1_0 || type == entity::Type::TRACK_R_1X1_0) {
                    offset_to_next = prev_pos.y > pos.y ? Vec2F{  16.0F,   0.0F } : Vec2F{   0.0F,  16.0F };
                } else if (type == entity::Type::CONDUIT_R_1X1_1 || type == entity::Type::TRACK_R_1X1_1) {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{ -16.0F,   0.0F } : Vec2F{   0.0F, -16.0F };
                } else if (type == entity::Type::CONDUIT_L_1X2_0 || type == entity::Type::TRACK_L_1X2_0) {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{   0.0F,  16.0F } : Vec2F{ -16.0F, -16.0F };
                } else if (type == entity::Type::CONDUIT_L_1X2_1 || type == entity::Type::TRACK_L_1X2_1) {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{  16.0F,  16.0F } : Vec2F{   0.0F, -16.0F };
                } else if (type == entity::Type::CONDUIT_R_1X2_0 || type == entity::Type::TRACK_R_1X2_0) {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{  0.0F,   16.0F } : Vec2F{  16.0F, -16.0F };
                } else if (type == entity::Type::CONDUIT_R_1X2_1 || type == entity::Type::TRACK_R_1X2_1) {
                    if (prev_pos.y < pos.y) {
                        offset_to_next = { -16.0F,  16.0F };
                        offset_to_next2 = {  0.0F,  16.0F };
                    } else {
                        offset_to_next  = {  0.0F, -16.0F };
                    }
                } else if (type == entity::Type::CONDUIT_L_2X1_0 || type == entity::Type::TRACK_L_2X1_0) {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{  16.0F,  16.0F } : Vec2F{ -16.0F,   0.0F };                    
                } else if (type == entity::Type::CONDUIT_L_2X1_1 || type == entity::Type::TRACK_L_2X1_1) {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{  16.0F,   0.0F } : Vec2F{ -16.0F, -16.0F };
                } else if (type == entity::Type::CONDUIT_R_2X1_0 || type == entity::Type::TRACK_R_2X1_0) {
                    offset_to_next = prev_pos.y > pos.y ? Vec2F{  16.0F,   0.0F } : Vec2F{ -16.0F,  16.0F };
                } else if (type == entity::Type::CONDUIT_R_2X1_1 || type == entity::Type::TRACK_R_2X1_1) {
                    if (prev_pos.x < pos.x) {
                        offset_to_next  = {  16.0F, -16.0F };
                        offset_to_next2 = {  16.0F,   0.0F };
                    } else {
                        offset_to_next  = { -16.0F,   0.0F };
                    }                    
                }

                prev_entity = entity;
                goto restart_loop;
            }
        }

    }

    void Game::quad_trees_check_collision() {
        std::vector<std::thread> threads;
        for (std::pair<Vec2I, QuadTreeNode*>& quad_tree_node : m_level_quad_trees) {
            if (!quad_tree_node.second) continue;

            auto quad_trees_check_collision = [&]() {
                std::unique_lock<std::mutex> quad_tree_node_lock(quad_tree_node_mutex);

                quad_tree_node.second->clear();
                quad_tree_node.second->init(m_window_w, m_window_h,
                    NUM_VISIBLE_LAYERS - 1,
                    { (quad_tree_node.first.x - 1) * 256.0F + transform::position(m_level_transform_id).x,
                      (quad_tree_node.first.y - 1) * 256.0F + transform::position(m_level_transform_id).y,
                    256.0F, 256.0F });

                for (I32 aabb_id = 0; aabb_id < aabb::size(); ++aabb_id) {
                    if (!aabb::is_active(aabb_id)) continue;

                    //aabb->quad_tree_node.clear();

                    //for (auto& point : aabb->points()) {
                        //if (!aabb->parent) continue;
                        //quad_tree_node.second->insert_point(aabb_id, point);
                    //}

                    quad_tree_node.second->insert_point(aabb_id, aabb::point(aabb_id, 0));
                    quad_tree_node.second->insert_point(aabb_id, aabb::point(aabb_id, 1));
                    quad_tree_node.second->insert_point(aabb_id, aabb::point(aabb_id, 2));
                    quad_tree_node.second->insert_point(aabb_id, aabb::point(aabb_id, 3));
                }
                quad_tree_node_lock.unlock();

                quad_tree_node.second->check_collision();
                };
            threads.emplace_back(std::thread(quad_trees_check_collision));
        }
        //console::log("threads size: ", threads.size(), "\n");
        for (auto& i : threads) {
            i.join();
        }
    }
}