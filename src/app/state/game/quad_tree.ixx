module;
#include <vector>
#include <sstream>
//#include <set>

export module quad_tree;
import collider.aabb;
import console;
import types;
import line;
import sprite;
import transform;
import window;
//import entity;

cF32 size_at_depth(cU8 d) {
    F32 size = 256.0F;
    for (U8 i = 0; i < d; ++i) {
        size /= 2.0F;
    }
    return size;
}

export class QuadTreeNode {
    static inline U16 s_max_depth   = 4, s_max_objects = 4,
                      s_window_w    = 0, s_window_h    = 0;
    static inline U8  s_layer       = 0;
    
    bool m_is_parent = false;
    
    QuadTreeNode* m_child[4] = {nullptr};

    RectF m_rect{ 0, 0, 0, 0 };
    std::vector<I32> m_aabb_ids;

    I32 m_up_line_id    = -1,
        m_down_line_id  = -1,
        m_left_line_id  = -1,
        m_right_line_id = -1;

    void split() {
        m_is_parent = true;
        cF32 half_w = m_rect.w / 2.0F, half_h = m_rect.h / 2.0F;

        m_child[0] = new QuadTreeNode(s_window_w, s_window_h, s_layer,  { m_rect.x,          m_rect.y,          half_w, half_h });
        m_child[0]->id = id;        

        m_child[1] = new QuadTreeNode(s_window_w, s_window_h, s_layer,  { m_rect.x + half_w, m_rect.y,          half_w, half_h });
        m_child[1]->id = id + 1;        

        m_child[2] = new QuadTreeNode(s_window_w, s_window_h, s_layer,  { m_rect.x,          m_rect.y + half_h, half_w, half_h});
        m_child[2]->id = id + 2;        
                
        m_child[3] = new QuadTreeNode(s_window_w, s_window_h, s_layer,  { m_rect.x + half_w, m_rect.y + half_h, half_w, half_h });
        m_child[3]->id = id + 3;
        
        m_child[0]->depth = m_child[1]->depth = m_child[2]->depth = m_child[3]->depth = depth + 1;
        //m_child[0]->transform_id = m_child[1]->transform_id = m_child[2]->transform_id = m_child[3]->transform_id = transform_id;

        for (auto& aabb_id : m_aabb_ids) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 0));
                m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 1));
                m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 2));
                m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 3));
            }

            // aabb too large? insert more in-between points
            if (aabb::w(aabb_id) > size_at_depth(depth + 1) || aabb::h(aabb_id) > size_at_depth(depth + 1)) {
                for (size_t i = 0; i < 4; ++i) {

                    for (F32 scalar = 0.25F; scalar < 1.0F; scalar += 0.25F) {
                        m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 0) + Vec2F{ aabb::w(aabb_id) * scalar, 0.0F });  // U
                        m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 0) + Vec2F{ 0.0F, aabb::h(aabb_id) * scalar }); // L
                        m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 2) + Vec2F{ aabb::w(aabb_id) * scalar, 0.0F});  // D
                        m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 1) + Vec2F{ 0.0F, aabb::h(aabb_id) * scalar }); // R

                        //m_child[i]->insert_point(object, object->point_0() + Vec2F{ object->width() * scalar, object->height() * scalar });
                        //m_child[i]->insert_point(object, object->point_1() + Vec2F{ object->width() * -scalar, object->height() * scalar });
                    }
                    m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 0) + Vec2F{ aabb::w(aabb_id) * 0.25F, aabb::h(aabb_id) * 0.25F });
                    m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 0) + Vec2F{ aabb::w(aabb_id) * 0.5F, aabb::h(aabb_id) * 0.5F });
                    m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 0) + Vec2F{ aabb::w(aabb_id) * 0.75F, aabb::h(aabb_id) * 0.75F });

                    m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 1) + Vec2F{ aabb::w(aabb_id) * -0.25F, aabb::h(aabb_id) * 0.25F });
                    m_child[i]->insert_point(aabb_id, aabb::point(aabb_id, 1) + Vec2F{ aabb::w(aabb_id) * -0.75F, aabb::h(aabb_id) * 0.75F });





                }
            }
        }
        m_aabb_ids.clear();
    }
public:
    I32 id = 0;
    U16 depth = 0;
    //I32 transform_id = -1;

    //QuadTreeNode() = delete;
    //QuadTreeNode(cI32 t_id) { }
    //QuadTreeNode() { }
    QuadTreeNode(cU16 window_w, cU16 window_h, cU8 layer, cRectF rect) {
        //console::log("size at depth ", (int)s_max_depth, ": ", size_at_depth(s_max_depth), "\n");        
        init(window_w, window_h, layer, rect);
    }
    ~QuadTreeNode() {
        clear();
    }
    void draw(std::unique_ptr<Window>& window) {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->draw(window);
            }
        }
        line::draw(window, m_up_line_id);
        line::draw(window, m_down_line_id);
        line::draw(window, m_left_line_id);
        line::draw(window, m_right_line_id);
    }

    cF32 x() const { return m_rect.x; }
    cF32 y() const { return m_rect.y; }
    cF32 w() const { return m_rect.w; }
    cF32 h() const { return m_rect.h; }

    size_t depth_at(cVec2F position) {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                if (m_child[i] &&
                    position.x >= m_child[i]->x() && position.x <= m_child[i]->x() + m_child[i]->w() &&
                    position.y >= m_child[i]->y() && position.y <= m_child[i]->y() + m_child[i]->h()) {
                    return m_child[i]->depth_at(position);
                }
            }
        }
        return depth;
    }
    size_t id_at(cVec2F position) {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                if (m_child[i] &&
                    position.x >= m_child[i]->x() && position.x <= m_child[i]->x() + m_child[i]->w() &&
                    position.y >= m_child[i]->y() && position.y <= m_child[i]->y() + m_child[i]->h()) {
                    return m_child[i]->id_at(position);
                }
            }
        }
        return id;
    }
    std::vector<I32>& aabb_ids_at(cVec2F position) {
        for (size_t i = 0; i < 4; ++i) {
            if (m_child[i] &&
                position.x >= m_child[i]->x() && position.x <= m_child[i]->x() + m_child[i]->w() &&
                position.y >= m_child[i]->y() && position.y <= m_child[i]->y() + m_child[i]->h()) {
                return m_child[i]->aabb_ids_at(position);
            }
        }
        return m_aabb_ids;
    }
    void clear() {
        line::erase(m_up_line_id);
        line::erase(m_down_line_id);
        line::erase(m_left_line_id);
        line::erase(m_right_line_id);

        m_aabb_ids.clear();
        id = 0;
        depth = 0;
        m_is_parent = false;
        for (int i = 0; i < 4; ++i) {
            if (m_child[i]) {
                delete m_child[i];
                m_child[i] = nullptr;
            }
        }        
        //m_up_line_id = m_down_line_id = m_left_line_id = m_right_line_id = -1;
    }    
    void init(cU16 window_w, cU16 window_h, cU8 layer, cRectF rect) {
        //if (m_up_line_id != -1) return;
        s_window_w = window_w, s_window_h = window_h;
        s_layer = layer;

        m_is_parent = false;
        m_rect = rect;

        /*if (transform::get(transform_id)) {
            m_rect.x = rect.x + transform::get(transform_id)->position.x;
            m_rect.y = rect.y + transform::get(transform_id)->position.y;
        }*/

        
        /*if (m_rect.x < -m_rect.w || m_rect.x > window_w ||  // FIXME: sometimes though in view??
            m_rect.y < -m_rect.h || m_rect.y > window_h) {
            return;
        }*/

        m_up_line_id    = line::make( {m_rect.x, m_rect.y           }, {m_rect.x + m_rect.w, m_rect.y           } );
        m_down_line_id  = line::make( {m_rect.x, m_rect.y + m_rect.h}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );
        m_left_line_id  = line::make( {m_rect.x, m_rect.y           }, {m_rect.x,            m_rect.y + m_rect.h} );
        m_right_line_id = line::make( {m_rect.x + m_rect.w, m_rect.y}, {m_rect.x + m_rect.w, m_rect.y + m_rect.h} );

        //console::log("x: ", m_rect.x, "\n");

        line::layer(m_up_line_id, s_layer);
        line::layer(m_down_line_id, s_layer);
        line::layer(m_left_line_id, s_layer);
        line::layer(m_right_line_id, s_layer);
        line::is_debug(m_up_line_id, true);
        line::is_debug(m_down_line_id, true);
        line::is_debug(m_left_line_id, true);
        line::is_debug(m_right_line_id, true);
        
        //console::log("layer: ", (int)layer, "\n");
    }    

    bool insert_point(cI32 id, cVec2F point) {
        if (!aabb::is_active(id)) return false;

        if (point.x < m_rect.x || point.x > m_rect.x + m_rect.w ||
            point.y < m_rect.y || point.y > m_rect.y + m_rect.h) {
            return false;
        }
                        

        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->insert_point(id, point);
            }
            return false;
        } else {
            if (m_aabb_ids.size() >= s_max_objects && depth < s_max_depth) {
                m_is_parent = true;
                split();
                m_aabb_ids.clear();
            }
            else {
                //if (point.x >= m_rect.x && point.x <= m_rect.x + m_rect.w &&
                //    point.y >= m_rect.y && point.y <= m_rect.y + m_rect.h) {
                    if (std::find(m_aabb_ids.begin(), m_aabb_ids.end(), id) == m_aabb_ids.end()) {
                        m_aabb_ids.emplace_back(id);
                        return true;
                    }
                //}
            }
        }
        return false;
    }

    void check_collision() {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->check_collision();                
            }
            return;
        }
        for (auto& a : m_aabb_ids) {
            if (!aabb::owner(a) || !aabb::is_active(a)) continue;
            if (aabb::time_left_colliding(a) > 0) {
                //console::log("time left colliding: ", a->time_left_colliding, "\n");

                aabb::time_left_colliding(aabb::time_left_colliding(a) - 1);
                
                continue;
            }
                
            for (auto& b : m_aabb_ids) {
                if (a == b || !aabb::owner(b) || aabb::owner(a) == aabb::owner(b) || !aabb::is_active(b)) continue;
                //a->owner->update();

                if (aabb::time_left_colliding(b) > 0) {
                    continue;
                }
                //console::log(a->point_0(c).x, " ", b->point_0().x, "\n");
                if (is_aabb_overlap_y(a, b)) {
                    aabb::cInfo a_info = { aabb::owner(a), a };
                    aabb::cInfo b_info = { aabb::owner(b), b };
                                        
                    aabb::owner(a)->collide_y(a_info, b_info);
                    aabb::owner(b)->collide_y(b_info, a_info);
                }
                if (is_aabb_overlap_x(a, b)) {
                    aabb::cInfo a_info = { aabb::owner(a), a };
                    aabb::cInfo b_info = { aabb::owner(b), b };

                    aabb::owner(a)->collide_x(a_info, b_info);
                    aabb::owner(b)->collide_x(b_info, a_info);
                }
            }
        }
    }
    bool is_aabb_overlap_x(cI32 a, cI32 b) {
        F32 y_dec = (aabb::point(a, 2).y - aabb::point(a, 0).y) / 16.0F;
        if      (y_dec < 1.0F) y_dec = 1.0F;
        else if (y_dec > 4.0F) y_dec = 4.0F;
        //cF32 h_dec = 3.0F;
                
        cVec2F a_vel = aabb::owner(a)->velocity() + aabb::owner(a)->moved_velocity();
        cVec2F b_vel = aabb::owner(b)->velocity() + aabb::owner(b)->moved_velocity();
        
        return aabb::point(a, 1).x + a_vel.x > aabb::point(b, 0).x + b_vel.x &&
               aabb::point(a, 0).x + a_vel.x < aabb::point(b, 1).x + b_vel.x &&
               aabb::point(a, 2).y + a_vel.y - y_dec > aabb::point(b, 0).y + b_vel.y &&
               aabb::point(a, 0).y + a_vel.y + y_dec < aabb::point(b, 2).y + b_vel.y;
    }
    bool is_aabb_overlap_y(cI32 a, cI32 b) {
        F32 x_dec = (aabb::point(a, 1).x - aabb::point(a, 0).x) / 16.0F;
        if      (x_dec < 1.0F) x_dec = 1.0F;
        else if (x_dec > 4.0F) x_dec = 4.0F;

        cVec2F a_vel = aabb::owner(a)->velocity() + aabb::owner(a)->moved_velocity();
        cVec2F b_vel = aabb::owner(b)->velocity() + aabb::owner(b)->moved_velocity();

        return aabb::point(a, 2).y + a_vel.y > aabb::point(b, 0).y + b_vel.y &&
               aabb::point(a, 0).y + a_vel.y < aabb::point(b, 2).y + b_vel.y &&
               aabb::point(a, 1).x + a_vel.x - x_dec > aabb::point(b, 0).x + b_vel.x &&
               aabb::point(a, 0).x + a_vel.x + x_dec < aabb::point(b, 1).x + b_vel.x;
    }
};