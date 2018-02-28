#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>

#include "gl_viewer.h"
#include "quaternion.h"
#include "obj.h"
#include "timer.h"

class line_segment
{
public:
    line_segment(int x0, int y0, int x1, int y1)
        : x0(x0), y0(y0), x1(x1), y1(y1)
    {}

    int x0, y0, x1, y1;
};

class application : public gl_viewer
{
public:
    application();
    ~application();
    void init_event();
    void draw_event();
    void reset_animation();
    void apply_multiview_transform(int view_index,const std::string& viewname,const std::pair<float,vec3>& start_rotation,const std::pair<float,vec3>& end_rotation);
    void mouse_click_event(int button, int button_state, int x, int y);
    void mouse_move_event(int x, int y);
    void keyboard_event(unsigned char key, int x, int y);
    void get_test_parameters(std::pair<float,vec3>& start_rotation,std::pair<float,vec3>& end_rotation);

private:
    bool solid;
    timer t;
    obj o;
    bool paused;
    float pause_time;
    int test_number;
    vec3 init_pos,end_pos,pos;
    vec<vec3,3> colors;
};

#endif
