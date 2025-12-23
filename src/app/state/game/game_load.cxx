module;
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <map>

module state.game;
import camera;
import console;
import entity.brick;
import entity.bug;
import entity.frog;
import entity.grass;
import entity.conduit;
import entity.flag;
import entity.lever;
import entity.logic_and;
import entity.logic_not;
import entity.logic_or;
import entity.mole;
import entity.spring;
import entity.train;
import entity.track;
import entity.water_line;
import level_config;
import random;
import sprite;

namespace state {
    std::map<U16, std::string> Game::load_types_from_bin() {
        std::map<U16, std::string> types_map{};

        const std::filesystem::path path = std::filesystem::current_path() / "res" / "types.bin";

        std::ifstream in_file(path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) return {};

        U16 type_count = 0;
        in_file.read((I8*)&type_count, sizeof(U16));

        console::log("state::Game::load_types_from_bin() count: ", type_count, "\n");

        for (U16 i = 0; i < type_count; ++i) {
            U16 number = 0;
            char type[32] = {};
            in_file.read((I8*)&number, sizeof(U16));
            in_file.read((I8*)type, sizeof(char) * 32);

            //console::log("type: ", number, " ", type, "\n");

            types_map.emplace(number, type);

        }
        in_file.close();
        return types_map;
    }

    void Game::load_level(const std::filesystem::path& path) {
        console::log("Game::load_level\n");

        std::map<U16, std::string> types_map = load_types_from_bin();

        std::vector<sprite::Data> sprite_data = sprite::load_level(path);

        Vec2I highest_quad_tree_node = { 0, 0 };

        std::vector<Vec2I> quad_tree_node_coords{};

        U8 num_water_entity_per_tile = 4;

        for (auto& i : sprite_data) {
            cVec2F tile_offset = { (F32)i.x, (F32)i.y };

            m_visible_layers.insert(i.layer);

            if (i.tile_set != 255) {
                cI32 sprite_id = sprite::make(tile_set_texture_path(i.tile_set));

                m_level_sprite_ids.emplace_back(sprite_id);

                //sprite::id(sprite_id, sprite_id);
                sprite::tile_set(sprite_id, i.tile_set);
                sprite::layer(sprite_id, i.layer);
                sprite::offset(sprite_id, tile_offset);
                sprite::source_rect(sprite_id, Rect<I32>{ i.source_x, i.source_y, 16, 16 });
                sprite::transform_id(sprite_id, m_level_transform_id);
            } else if (i.tile_set == 255) {
                cU16 tile_x = i.source_x / 16;
                cU16 tile_y = i.source_y / 16;
                cU16 tile_number = tile_x + tile_y * 32;

                if (types_map.count(tile_number)) {
                    entity::Type entity_type = entity::string_to_type(types_map.at(tile_number));

                    //console::log("entity_type: ", types_map.at(tile_number), "\n");

                    if (start_info().type == start::Type::CENTER && entity_type == entity::Type::LEVEL_CENTER) {
                        console::log("state::Game::load_level() start position: ", tile_offset.x, " ", tile_offset.y, "\n");
                        m_start_position = tile_offset; //+ Vec2F{ 0.0F, -4.0F };
                        m_player.start_layer(i.layer + 1);
                        sprite::layer(m_player.sprite_id(), i.layer + 1);
                    } else if (start_info().type == start::Type::L_0 && entity_type == entity::Type::LEVEL_L_0) {
                        if (tile_offset.y > m_start_position.y) {
                            m_start_position = tile_offset;
                        }
                        m_player.start_layer(i.layer + 1);
                        sprite::layer(m_player.sprite_id(), i.layer + 1);
                        sprite::is_leftward(m_player.sprite_id(), false);
                    } else if (start_info().type == start::Type::R_0 && entity_type == entity::Type::LEVEL_R_0) {
                        if (tile_offset.y > m_start_position.y) {
                            m_start_position = tile_offset;
                        }
                        m_player.start_layer(i.layer + 1);
                        sprite::layer(m_player.sprite_id(), i.layer + 1);
                        sprite::is_leftward(m_player.sprite_id(), true);
                    }
                    if (entity_type != entity::Type::NONE) {
                        std::filesystem::path config_path = "res/entity/" + types_map.at(tile_number) + ".cfg";
                        //console::log("entity config path: ", config_path, "\n");

                        U8 num_to_create    = 1;
                        Vec2F added_offset  = { 0.0F, 0.0F },
                              add_to_offset = { 0.0F, 0.0F };

                        bool is_random_leftward = false;

                        if (entity_type == entity::Type::BRICK) {
                            m_entity_objects.emplace_back(new entity::Brick);
                        } else if (entity_type == entity::Type::BUG) {
                            m_entity_objects.emplace_back(new entity::Bug);
                            is_random_leftward = true;
                        } else if (entity_type == entity::Type::FROG) {
                            m_entity_objects.emplace_back(new entity::Frog);
                            is_random_leftward = true;
                        } else if (entity::is_grass(entity_type)) {
                            num_to_create = 6;
                            is_random_leftward = true;

                            if (entity::is_grass_2(entity_type)) {
                                config_path = "res/entity/GRASS_2.cfg";
                            } else if (entity::is_grass_1(entity_type)) {
                                num_to_create = 5;
                                config_path = "res/entity/GRASS_1.cfg";
                            } else {
                                num_to_create = 3;
                                config_path = "res/entity/GRASS_0.cfg";
                            }
                            add_to_offset.x = 16.0F / num_to_create;

                            if (entity_type == entity::Type::GRASS_0_L_1X1 || entity_type == entity::Type::GRASS_1_L_1X1 || entity_type == entity::Type::GRASS_2_L_1X1) {
                                added_offset.y = 15.0F;
                                add_to_offset.y = -add_to_offset.x;
                            } else if (entity_type == entity::Type::GRASS_0_R_1X1 || entity_type == entity::Type::GRASS_1_R_1X1 || entity_type == entity::Type::GRASS_2_R_1X1) {
                                added_offset.y = 2.0F;
                                add_to_offset.y = add_to_offset.x;
                            } else if (entity_type == entity::Type::GRASS_0_L_2X1_0 || entity_type == entity::Type::GRASS_1_L_2X1_0 || entity_type == entity::Type::GRASS_2_L_2X1_0) {
                                added_offset.y = 15.0F;
                                add_to_offset.y = -add_to_offset.x / 2.0F;
                            } else if (entity_type == entity::Type::GRASS_0_L_2X1_1 || entity_type == entity::Type::GRASS_1_L_2X1_1 || entity_type == entity::Type::GRASS_2_L_2X1_1) {
                                added_offset.y = 7.0F;
                                add_to_offset.y = -add_to_offset.x / 2.0F;
                            } else if (entity_type == entity::Type::GRASS_0_R_2X1_0 || entity_type == entity::Type::GRASS_1_R_2X1_0 || entity_type == entity::Type::GRASS_2_R_2X1_0) {
                                added_offset.y = 2.0F;
                                add_to_offset.y = add_to_offset.x / 2.0F;
                            } else if (entity_type == entity::Type::GRASS_0_R_2X1_1 || entity_type == entity::Type::GRASS_1_R_2X1_1 || entity_type == entity::Type::GRASS_2_R_2X1_1) {
                                added_offset.y = 10.0F;
                                add_to_offset.y = add_to_offset.x / 2.0F;
                            }

                            for (U8 i_create = 0; i_create < num_to_create; ++i_create) {
                                //m_entity_objects.emplace_back(std::make_unique<entity::Grass>());
                                m_entity_objects.emplace_back(new entity::Grass);
                                m_entity_objects.back()->number(i_create);
                                m_entity_objects.back()->transform_id(m_level_transform_id);                                
                            }
                             
                        } else if (entity::is_conduit(entity_type)) {
                            m_entity_objects.emplace_back(new entity::Conduit);
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                            m_unlocked_entity_objects.emplace_back(m_entity_objects.back());

                            //config_path = "res/entity/conduit.cfg";
                        } else if (entity::is_track(entity_type)) {
                            //console::log("Game::load_level() found track ", entity::to_string(entity_type), "\n");
                            m_entity_objects.emplace_back(new entity::Track);
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                            m_unlocked_entity_objects.emplace_back(m_entity_objects.back());
                        } else if (entity_type == entity::Type::LEVEL_CENTER) {                            
                            m_entity_objects.emplace_back(new entity::Flag);
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                        } else if (entity::is_logic_and(entity_type)) {                            
                            m_entity_objects.emplace_back(new entity::LogicAnd);
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                            m_unlocked_entity_objects.emplace_back(m_entity_objects.back());
                        } else if (entity::is_logic_not(entity_type)) {
                            m_entity_objects.emplace_back(new entity::LogicNot);
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                            m_unlocked_entity_objects.emplace_back(m_entity_objects.back());
                        } else if (entity::is_logic_or(entity_type)) {
                            m_entity_objects.emplace_back(new entity::LogicOr);
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                            m_unlocked_entity_objects.emplace_back(m_entity_objects.back());
                        } else if (entity_type == entity::Type::MOLE) {
                            m_entity_objects.emplace_back(new entity::Mole);
                        } else if (entity_type == entity::Type::SPRING_U) {
                            m_entity_objects.emplace_back(new entity::Spring);
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                        } else if (entity_type == entity::Type::TRAIN) {
                            m_entity_objects.emplace_back(new entity::Train);
                            //m_entity_objects.back()->position() = tile_offset;
                        } else if (entity_type == entity::Type::TRIGGER) {
                            //cU8 entity_number = entity::number_in_type(entity_type);
                            //console::log("trigger number: ", (int)entity_number, "\n");
                            //console::log("trigger_", (int)entity_number, " = ", LevelConfig::trigger_path(entity_number), "\n");

                            //m_entity_objects.emplace_back(std::make_unique<entity::Lever>());
                            m_entity_objects.emplace_back(new entity::Lever);
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                                                 
                            config_path = "res/entity/TRIGGER.cfg";
                            //entity_type = entity::Type::LEVER;

                            m_trigger_entity_objects.emplace_back(m_entity_objects.back());
                        } else if (entity::is_water_line(entity_type)) {
                            num_to_create = num_water_entity_per_tile;
                            add_to_offset.x = 16.0F / num_water_entity_per_tile;

                            for (U8 i_create = 0; i_create < num_to_create; ++i_create) {
                                m_entity_objects.emplace_back(new entity::WaterLine);
                                m_entity_objects.back()->number(i_create);
                                m_water_entity_objects.emplace_back(m_entity_objects.back());

                                //m_water_entity_objects.emplace_back(std::make_unique<entity::WaterLine>());
                                //m_water_entity_objects.back()->number(i_create);

                                //m_unlocked_entity_objects.emplace_back(m_entity_objects.back());
                            }

                            //console::log("water entity objects size: ", m_water_entity_objects.size(), "\n");
                            config_path = "res/entity/WATER_LINE.cfg";
                        } else {
                            //m_entity_objects.emplace_back(std::make_unique<entity::Object>());
                            m_entity_objects.emplace_back(new entity::Object);
                            m_entity_objects.back()->transform_id(m_level_transform_id);
                        }


                        for (U8 i_create = 0; i_create < num_to_create; ++i_create) {
                            const size_t entity_i = m_entity_objects.size() - num_to_create + i_create;                            

                            if (!m_entity_objects.at(entity_i)) continue;

                            m_entity_objects.at(entity_i)->type(entity_type);
                            m_entity_objects.at(entity_i)->start_layer(i.layer);

                            if (transform::is_level(m_entity_objects.at(entity_i)->transform_id())) {
                                m_entity_objects.at(entity_i)->start_offset({ tile_offset.x + added_offset.x,
                                                                              tile_offset.y + added_offset.y });
                                //console::log("start offset: ", m_entity_objects.at(entity_i)->start_offset().x, " ", m_entity_objects.at(entity_i)->start_offset().y, "\n");
                            }

                            m_entity_objects.at(entity_i)->load_config(config_path);

                            if (is_random_leftward) {
                                sprite::is_leftward(m_entity_objects.at(entity_i)->sprite_id(), random::number(0, 1));
                            }

                            if (!transform::is_level(m_entity_objects.at(entity_i)->transform_id())) {
                                m_entity_objects.at(entity_i)->position({ tile_offset.x + added_offset.x,
                                                                          tile_offset.y + added_offset.y });
                                m_entity_objects.at(entity_i)->start_position(m_entity_objects.at(entity_i)->position());
                                //camera::add_transform_id(m_entity_objects.back()->transform()->id);

                                /*if (entity::is_water(entity_type)) {
                                    console::log("water start_position.y: ", m_entity_objects.at(entity_i)->transform()->start_position.y, "\n");
                                }*/
                            }

                            added_offset += add_to_offset;
                        }
                    }
                }
            }
            cVec2I quad_tree_node_coord = { (i.x - i.x % 256 + 256) / 256, (i.y - i.y % 256 + 256) / 256 };
            if (std::find(quad_tree_node_coords.begin(), quad_tree_node_coords.end(), quad_tree_node_coord) == quad_tree_node_coords.end()) {
                quad_tree_node_coords.emplace_back(quad_tree_node_coord);
                m_level_quad_trees.emplace_back(
                    std::make_pair(quad_tree_node_coord, new QuadTreeNode(m_window_w, m_window_h, NUM_VISIBLE_LAYERS - 1, { 0, 0, 256, 256 })
                    )
                );
            }
            if (quad_tree_node_coord.x > highest_quad_tree_node.x)  highest_quad_tree_node.x = quad_tree_node_coord.x;
            if (quad_tree_node_coord.y > highest_quad_tree_node.y)  highest_quad_tree_node.y = quad_tree_node_coord.y;
        }

        console::log("highest quad tree node: ", highest_quad_tree_node.x, " ", highest_quad_tree_node.y, "\n");

        camera::scroll_limit = highest_quad_tree_node * 256 - Vec2I{ m_window_w, m_window_h };

        console::log("camera::scroll_limit: ", camera::scroll_limit.x, " ", camera::scroll_limit.y, "\n");

        //console::log("m_water_entity_objects size: ", m_water_entity_objects.size(), "\n");

        // connect water
        for (I16 water_i = 0; water_i < m_water_entity_objects.size(); ++water_i) {
            if (!m_water_entity_objects.at(water_i)) continue;

            if (m_water_entity_objects.at(water_i)->type() == entity::Type::WATER_LINE_L && water_i % num_water_entity_per_tile == 0) {
                m_water_entity_objects.at(water_i)->add_input_object(m_water_entity_objects.at(water_i + 1));
                //console::log("water_L 0\n");
            } else if (m_water_entity_objects.at(water_i)->type() == entity::Type::WATER_LINE_R && water_i % num_water_entity_per_tile == num_water_entity_per_tile - 1) {
                m_water_entity_objects.at(water_i)->add_input_object(m_water_entity_objects.at(water_i - 1));
                //console::log("water_R 15\n");
            } else {

                if (water_i - 1 >= 0 && m_water_entity_objects.at(water_i - 1)) {
                    m_water_entity_objects.at(water_i)->add_input_object(m_water_entity_objects.at(water_i - 1));
                }
                if (water_i + 1 < m_water_entity_objects.size() && m_water_entity_objects.at(water_i + 1)) {
                    m_water_entity_objects.at(water_i)->add_input_object(m_water_entity_objects.at(water_i + 1));
                }
                //console::log("water inputs: ", m_water_entity_objects.at(water_i)->input_objects.size(), "\n");
            }
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(m_water_entity_objects.begin(), m_water_entity_objects.end(), gen);

        //for (auto& entity : m_entity_objects)   check_entities_to_add_input_from(entity);

        for (auto& trigger_entity : m_trigger_entity_objects)   check_to_add_input_from(trigger_entity);
    }
}