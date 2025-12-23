module;
#include <string>
#include <sstream>
export module state.overlay;
import state;
import console;
import bitmap_text;
import input;
import line;
import transform;
import sprite;

export namespace state {
    class Overlay : public Object {
        I32 //m_line_transform_id = -1,
            m_transform_id = -1;

        //I32 m_line_id = -1;

        BitmapText m_fps_text;

        //U8 m_timer = 0;
    public:
        Overlay(cU16 window_w, cU16 window_h) {
            m_current = m_next = Type::OVERLAY;

            m_input_id = input::make();

            m_transform_id = transform::make();

            m_fps_text.transform_id = m_transform_id;
            m_fps_text.layer = NUM_VISIBLE_LAYERS - 1;

            m_fps_text.texture_path = "res/texture/font_5_white.png";
            m_fps_text.font_size = 5;

            m_visible_layers.insert(m_fps_text.layer);
        }
        ~Overlay() {
            console::log("~Overlay()\n");
            input::erase(m_input_id);
            transform::erase(m_transform_id);
            m_fps_text.clear_text();
        }
        virtual void update(cF32 ts) override {
            /*++m_timer;
            if (m_timer < 2) {
                return;
            }
            m_timer = 0;*/            

            cF32 current_fps = 1.0F / ts;

            m_fps_text.transform_id = m_transform_id;
            m_fps_text.set_text(std::to_string((int)current_fps));

            if (input::is_pressed(m_input_id, input::Key::L)) {
                if (!input::is_locked(m_input_id, input::Key::L)) {
                    input::lock(m_input_id, input::Key::L);
                    console::log("Overlay::update ", current_fps, "\n");
                }
            } else {
                input::unlock(m_input_id, input::Key::L);
            }

            transform::position(m_transform_id, { (F32)(view().w - m_fps_text.get_text().size() * m_fps_text.font_size),
                                                  (F32)(view().h - m_fps_text.font_size) });
        }
        void draw(std::unique_ptr<Window>& window, cU8 layer) override {
            //console::log("fps text layer: ", (int)m_fps_text.layer, " ", (int)layer, "\n");
            if (m_fps_text.layer == layer) {
                m_fps_text.draw(window);
            }
        }
    };
}