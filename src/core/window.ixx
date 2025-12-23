module;
#include <SFML/Graphics.hpp>

export module window;
import console;
//import line;
//import sprite;
import transform;
import types;
import shader;

export class Window {
    RectF m_view = { 0.0F, 0.0F, 320.0F, 180.0F };
    U8  m_scale = 1;
    U16 m_w = 0,
        m_h = 0;

public:
    sf::RenderWindow m_sf_window;
    Shader test_shader;

    Window() = delete;
    Window(cU16 w, cU16 h, cU8 scale, const char* title) {
        console::log("Window::init\n");
        m_w = w, m_h = h;
        m_scale = scale;
        m_view = { 0.0F, 0.0F, (F32)w, (F32)h };
        U32 video_w = (U32)(w * scale);
        U32 video_h = (U32)(h * scale);
        m_sf_window = sf::RenderWindow(sf::VideoMode({ video_w, video_h }), title);
        m_sf_window.setView(sf::View((sf::FloatRect(sf::Vector2f(0.0F, 0.0F), sf::Vector2f((F32)video_w, (F32)video_h)))));

        test_shader.load("res/shader/test.vert", "res/shader/test.frag");
    }
    cRectF view()  const { return m_view;  }
    cU8    scale() const { return m_scale; }
    cU16   w()     const { return m_w;     }
    cU16   h()     const { return m_h;     }

    void view(cRectF view) {
        console::log("Window::view: ", view.x, " ", view.y, " ", view.w, " ", view.h, "\n");
        m_view = view;
        m_sf_window.setView(sf::View((sf::FloatRect(sf::Vector2f(view.x * m_scale, view.y * m_scale), sf::Vector2f(view.w * m_scale, view.h * m_scale)))));
        //m_sf_window.setView(sf::View({ view.x * m_scale, view.y * m_scale }, { view.w * m_scale, view.h * m_scale }));        
    }
    void clear()   { m_sf_window.clear();   }
    void close()   { m_sf_window.close();   }
    void display() { m_sf_window.display(); }
    
    void draw_line(sf::Vertex* vertices) {
        m_sf_window.draw(vertices, 4, sf::PrimitiveType::TriangleFan);
    }
    void draw_sf_sprite(sf::Sprite& sf_sprite) {
        sf::RenderStates states;
        states.shader = test_shader.get_sfml_shader();
        m_sf_window.draw(sf_sprite, states);
    }
    //void draw(sprite::Object* sprite) {
    //    if (!sprite || sprite->is_hidden) return;
    //    if (transform::position(sprite->transform_id).x + sprite->offset.x < m_view.x - sprite->source_rect.w ||
    //        transform::position(sprite->transform_id).x + sprite->offset.x > m_view.w ||
    //        transform::position(sprite->transform_id).y + sprite->offset.y < m_view.y - sprite->source_rect.h ||
    //        transform::position(sprite->transform_id).y + sprite->offset.y > m_view.h) {
    //        return;
    //    }
    //    sf::RenderStates states;
    //    states.shader = test_shader.get_sfml_shader();
    //    m_sf_window.draw(sprite->sf_sprite, states);

    //    //m_sf_window.draw(sprite->sf_sprite);
    //}
    bool is_open() const { return m_sf_window.isOpen(); }
    std::optional<sf::Event> poll_event() { return m_sf_window.pollEvent(); }
};