module state.game;
import health;

namespace state {
    void Game::draw(std::unique_ptr<Window>& window, cU8 layer) {        
        for (auto& i : m_level_sprite_ids) {
            if (sprite::layer(i) == layer) {
                sprite::draw(window, i);
            }
        }
        if (sprite::layer(m_player.sprite_id()) == layer) {            
            is_drawing_debug ? m_player.draw_debug(window) : m_player.draw(window);            
        }
        for (auto& i : m_entity_objects) {
            if (i && sprite::layer(i->sprite_id()) == layer) {                
                is_drawing_debug ? i->draw_debug(window) : i->draw(window);
            }
        }
        for (auto& i : m_water_entity_objects) {
            if (i && i->start_layer() == layer) {                
                is_drawing_debug ? i->draw_debug(window) : i->draw(window);
            }
        }        
        is_drawing_debug ? particle::draw_debug(window, layer) : particle::draw(window, layer);

        if (is_drawing_debug && layer == 0) {
            for (auto& i : m_level_quad_trees) {
                i.second->draw(window);
            }
        }
        health::draw(window, layer);
    }
}