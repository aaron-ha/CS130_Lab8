#include "application.h"

#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

using namespace std;
#include "obj.h"
#include "quaternion.h"
#include "axis_angle.h"

enum {AXIS_ANGLE_INTERPOLATION=0,SLERP,MATRIX_INTERPOLATION};
int split=5;
int view_count=3;
int mode=0;

void draw_grid(int dim);
void draw_obj(obj *o,const std::string& material_prefix="",float scale=1.25);

application::application()
    : solid(true),paused(false),pause_time(0),test_number(1),
    init_pos(-6,0,0),end_pos(6,0,0),pos(end_pos),colors(vec3(0,0,1),vec3(0,1,0),vec3(1,0,0))
{
}

application::~application()
{
}

// triggered once after the OpenGL context is initialized
void application::init_event()
{

    cout << "CAMERA CONTROLS: \n  LMB: Rotate \n  MMB: Move \n  RMB: Zoom" << endl;
    cout << "KEYBOARD CONTROLS: \n"<<std::endl;  
    cout<<"  'p'  : Pause/Play" << endl;
    cout<<"  '1-4': Select test case" << endl;
    cout<<"  'm'  : Toggle mode" << endl;

    const GLfloat ambient[] = { 0.50, 0.50, 0.50, 1.0 };
    const GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    const GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };

    // enable a light
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glEnable(GL_LIGHT1);

    // enable depth-testing, colored materials, and lighting
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    // normalize normals so lighting calculations are correct
    // when using GLUT primitives
    glEnable(GL_NORMALIZE);

    // enable smooth shading
    glShadeModel(GL_SMOOTH);

    glClearColor(0.9, 0.9, 1.0, 0.0);
    int m=5;

    set_camera_for_box(vec3(-m,-m,-m),vec3(m,m,m));

    reset_animation();

    o.load("fighter.obj");
}
void print_bitmap_string(float x,float y,const std::string& s)
{
    glRasterPos2f(x,y);
    for (unsigned int i =0; i < s.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15,s[i]);
        // glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,s[i]);
}

void application::get_test_parameters(std::pair<float,vec3>& start_rotation,std::pair<float,vec3>& end_rotation)
{
    switch(test_number){
        case 2:
            start_rotation.second=vec3(1,1,0); start_rotation.first=45;
            end_rotation.second=vec3(0,0,1); end_rotation.first=-145;
            break;
        case 3:
            start_rotation.second=vec3(1,1,1); start_rotation.first=287;
            end_rotation.second=vec3(1,0,0); end_rotation.first=0;
            break;
        case 4:
            start_rotation.second=vec3(0,1,1); start_rotation.first=10;
            end_rotation.second=vec3(1,1,1); end_rotation.first=180;
            break;
        default:
            start_rotation.second=vec3(1,0,0); start_rotation.first=0;
            end_rotation.second=vec3(0,1,1); end_rotation.first=60;
    }
}

vec3 Interpolate_Position(const vec3& pos0, const vec3& pos1, float u)
{
    //TODO: Correct the calculations below
    return pos0;
}

axis_angle Interpolate_Axis_Angle(const axis_angle& AA0, const axis_angle& AA1, float u)
{
    //TODO: Correct the calculations below
    return axis_angle(AA0.w*u);
}

mat4 Interpolate_Rotation_Matrix(const mat4& R0,const mat4& R1, float u)
{
    //TODO: Correct the calculations below
    return R0*u;
}

void application::apply_multiview_transform(int view_index,const std::string& viewname,const std::pair<float,vec3>& start_rotation,const std::pair<float,vec3>& end_rotation)
{
    if(view_count>1){
        float start_h=view_index*height/view_count;
        glViewport(0,start_h,width,height/view_count);}
    else glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(colors[view_index][0],colors[view_index][1],colors[view_index][2]);
    print_bitmap_string(-0.9,0.8,viewname.c_str());
    apply_gl_transform();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(field_of_view_y/view_count,(float)width/height*view_count,nearclip,farclip);
    glMatrixMode(GL_MODELVIEW);

    // draws the grid and frame at the origin
    glColor3f(.60, .60, .60);
    draw_grid(50);
    if(mode!=0) return;



    //draw starting state
    glPushMatrix();
    glTranslatef(init_pos[0],init_pos[1],init_pos[2]);
    glRotatef(start_rotation.first,start_rotation.second[0],start_rotation.second[1],start_rotation.second[2]);
    // glScalef(s,s,s);
    draw_obj(&o,"gray.");
    glPopMatrix();
        
    //draw end state
    glPushMatrix();
    glTranslatef(end_pos[0],end_pos[1],end_pos[2]);
    glRotatef(end_rotation.first,end_rotation.second[0],end_rotation.second[1],end_rotation.second[2]);
    draw_obj(&o,"gray.");
    glPopMatrix();

}

void application::reset_animation()
{
    t.reset();
    pause_time=0;
    pos=init_pos;
}


// triggered each time the application needs to redraw
void application::draw_event()
{
    float step=1.0/split;
    float mtime=pause_time+(paused?0:t.elapsed());
    if(mtime>12){reset_animation();}
    float alpha=min(mtime/8,(float)1);
    int s=std::min(split-1,int(alpha/step));
    // apply our camera transformation
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(0,0,0);
    print_bitmap_string(-0.3,0.8,"TRY OTHER TESTS USING KEYS: 1-4");
    print_bitmap_string(0.7,0.8,"u="+std::to_string(alpha));
    glPopMatrix();
    const GLfloat light_pos1[] = { 0.0, 10.0, 0.0, 1 };
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
    
    std::pair<float,vec3> start_rotation,end_rotation,cur_rotation;
    get_test_parameters(start_rotation,end_rotation);
    quaternion start_q,end_q;
    start_q.from_angle_and_axis(start_rotation.first,start_rotation.second);
    end_q.from_angle_and_axis(end_rotation.first,end_rotation.second);
    mat4 p_matrix=start_q.rotation_matrix4();
    mat4 q_matrix=end_q.rotation_matrix4();
    axis_angle start_aa,end_aa;

    start_aa.from_angle_and_axis(start_rotation.first,start_rotation.second);
    end_aa.from_angle_and_axis(end_rotation.first,end_rotation.second);

    if(mode==0){
        view_count=3;
        pos=Interpolate_Position(init_pos,end_pos,alpha);


        //1. AXIS-ANGLE View
        apply_multiview_transform(AXIS_ANGLE_INTERPOLATION,"AXIS-ANGLE",start_rotation,end_rotation);

        //draw mid states
        for(int i=1;i<=s;i++){ float u=i*step;
            glPushMatrix();
            vec3 mpos=Interpolate_Position(init_pos,end_pos,u);
            glTranslatef(mpos[0],mpos[1],mpos[2]);
            Interpolate_Axis_Angle(start_aa,end_aa,u).to_angle_and_axis(cur_rotation.first,cur_rotation.second);
            glRotatef(cur_rotation.first,cur_rotation.second[0],cur_rotation.second[1],cur_rotation.second[2]);
            draw_obj(&o,"gray.");
            glPopMatrix();
        }

        //draw current state
        glTranslatef(pos[0],pos[1],pos[2]);
        Interpolate_Axis_Angle(start_aa,end_aa,alpha).to_angle_and_axis(cur_rotation.first,cur_rotation.second);
        glRotatef(cur_rotation.first,cur_rotation.second[0],cur_rotation.second[1],cur_rotation.second[2]);
        draw_obj(&o,"");


        //2. SLERP View
        apply_multiview_transform(SLERP,"SLERP",start_rotation,end_rotation);
        //draw mid states
        for(int i=1;i<=s;i++){ float u=i*step;
            glPushMatrix();
            vec3 mpos=Interpolate_Position(init_pos,end_pos,u);
            glTranslatef(mpos[0],mpos[1],mpos[2]);
            slerp(u,start_q,end_q).to_angle_and_axis(cur_rotation.first,cur_rotation.second);
            glRotatef(cur_rotation.first,cur_rotation.second[0],cur_rotation.second[1],cur_rotation.second[2]);
            draw_obj(&o,"gray.");
            glPopMatrix();
        }
        //draw current state
        glTranslatef(pos[0],pos[1],pos[2]);
        slerp(alpha,start_q,end_q).to_angle_and_axis(cur_rotation.first,cur_rotation.second);
        glRotatef(cur_rotation.first,cur_rotation.second[0],cur_rotation.second[1],cur_rotation.second[2]);
        draw_obj(&o,"green.");



        //3. Rotation matrix view
        apply_multiview_transform(MATRIX_INTERPOLATION,"ROTATION MATRIX",start_rotation,end_rotation);
        //draw mid states
        for(int i=1;i<=s;i++){ float u=i*step;
            glPushMatrix();
            vec3 mpos=Interpolate_Position(init_pos,end_pos,u);
            glTranslatef(mpos[0],mpos[1],mpos[2]);
            mat4 rot4=Interpolate_Rotation_Matrix(p_matrix,q_matrix,u);
            glMultTransposeMatrixf(&rot4.x[0][0]);
            draw_obj(&o,"gray.");
            glPopMatrix();
        }
        //draw current state
        glTranslatef(pos[0],pos[1],pos[2]);
        mat4 rot4=Interpolate_Rotation_Matrix(p_matrix,q_matrix,alpha);
        glMultTransposeMatrixf(&rot4.x[0][0]);
        draw_obj(&o,"red.");
    }
    else
    {
        view_count=1;
        apply_multiview_transform(SLERP,"SLERP",start_rotation,end_rotation);
        //TODO: Here make the plane to go at its head direction by updating the plane position, pos, properly.
        //      Do not forget to update the cur_rotation using from_angle_and_axis(cur_rotation.first,cur_rotation.second);
        float speed=1;
        vec3 direction;
        pos+=speed*direction; 
        
        //Do not touch beneath this
        glTranslatef(pos[0],pos[1],pos[2]);
        glRotatef(cur_rotation.first,cur_rotation.second[0],cur_rotation.second[1],cur_rotation.second[2]);
        draw_obj(&o,"green.");
        target_position=pos;


    }
}

// triggered when mouse is clicked
void application::mouse_click_event(int button, int button_state, int x, int y)
{
}

// triggered when mouse button is held down and the mouse is
// moved
void application::mouse_move_event(int x, int y)
{
}

// triggered when a key is pressed on the keyboard
void application::keyboard_event(unsigned char key, int x, int y)
{
    if (key == '=')
    {
        if (solid) {
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_FILL);
        } else {
            glPolygonMode(GL_FRONT, GL_LINE);
            glPolygonMode(GL_BACK, GL_LINE);
        }
    }
    else if (key == 'r')
        reset_animation();
    else if (key == 'p')
    {
        paused=!paused;
        if(paused)
            pause_time=pause_time+t.elapsed();
        else
            t.reset();
    }
    else if (key == 'm')
    {
        mode=1-mode;
        reset_animation();
        if(mode==1)
            pos=init_pos;
        else target_position=vec3();
    }
    else if (key == '1'){
        test_number=1;
        reset_animation();}
    else if (key == '2'){
        test_number=2;
        reset_animation();}
    else if (key == '3'){
        test_number=3;
        reset_animation();}
    else if (key == '4'){
        test_number=4;
        reset_animation();}
}

void draw_grid(int dim)
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0);

    glPushMatrix();
    glTranslatef(0,-1,0);

    //
    // Draws a grid along the x-z plane
    //
    glLineWidth(1.0);
    glBegin(GL_LINES);

        int ncells = dim;
        int ncells2 = ncells/2;

        for (int i= 0; i <= ncells; i++)
        {
            int k = -ncells2;
            k +=i;
            glVertex3f(ncells2,0,k);
            glVertex3f(-ncells2,0,k);
            glVertex3f(k,0,ncells2);
            glVertex3f(k,0,-ncells2);
        }
    glEnd();
    glPopMatrix();

    //
    // Draws the coordinate frame at origin
    //
    glPushMatrix();
    // glScalef(1.0, 1.0, 1.0);
    glBegin(GL_LINES);

        // x-axis
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);

        // y-axis
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);

        // z-axis
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 1.0);
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void draw_obj(obj *o,const std::string& material_prefix,float scale)
{
    glScalef(scale,scale,scale);
    glDisable(GL_COLOR_MATERIAL);
    size_t nfaces = o->get_face_count();
    for (size_t i = 0; i < nfaces; ++i)
    {
        const obj::face& f = o->get_face(i);

        glPushMatrix();
        if (f.mat != "none") {
            const obj::material& mat = o->get_material(material_prefix+f.mat);

            GLfloat mat_amb[] = { mat.ka[0], mat.ka[1], mat.ka[2], 1 };
            GLfloat mat_dif[] = { mat.kd[0], mat.kd[1], mat.kd[2], 1 };
            GLfloat mat_spec[] = { mat.ks[0], mat.ks[1], mat.ks[2], 1 };
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_dif);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
            glMaterialf(GL_FRONT, GL_SHININESS, mat.ns);
        }


        glBegin(GL_POLYGON);
            for (size_t j = 0; j < f.vind.size(); ++j)
            {
                if (f.nind.size() == f.vind.size()) {
                    const float *norm = o->get_normal(f.nind[j]);
                    glNormal3fv(norm);
                }

                const float *vert = o->get_vertex(f.vind[j]);
                glVertex3fv(vert);
            }
        glEnd();
        glPopMatrix();
    }
    glEnable(GL_COLOR_MATERIAL);
}
