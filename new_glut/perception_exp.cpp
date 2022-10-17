#include "perception_exp.h"
#include "phantom_helper.h"
#include <iostream>

#include "SerialClass.h"

Serial* SP = new Serial("\\\\.\\COM3");
int dataLength = 255;

#define NUM_DEVICES 1
#define NUM_CURSORS 2

struct Points
{
    float x, y;  // initializor
    Points() { x = 0.0; y = 0.0; } // constructor

    Points(float _x, float _y) : x(_x), y(_y) {}
} *points;

typedef struct cursor {
    char on;
    float x;
    float y;
} *Cursor;
struct cursor cursors[NUM_DEVICES][NUM_CURSORS];

class Square
{
public:
    Points pts[4]; // square structure
    Square(); // initialize constructor

    void draw(Square* sqr); // draw square
    Points mouse(int x, int y); // get mouse coordintaes
    Square* drag(Square* sqr, Points* mouse); // change points of sqr
};

Square::Square()
{
    pts[0] = Points(0.2, 0.4);
    pts[1] = Points(0.4, 0.4);
    pts[2] = Points(0.4, 0.6);
    pts[3] = Points(0.2, 0.6);
};

// draw function
void Square::draw(Square* sqr)
{

    glColor3f(0.827451, 0.827451, 0.827451);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glVertex2f(sqr->pts[i].x, sqr->pts[i].y);
    }
    glEnd();

    glColor3f(0.745098, 0.745098, 0.745098);
    glBegin(GL_LINES);
    glVertex2f((sqr->pts[0].x + sqr->pts[1].x) / 2, sqr->pts[0].y);
    glVertex2f((sqr->pts[0].x + sqr->pts[1].x) / 2, sqr->pts[2].y);
    glEnd();
}

Points Square::mouse(int x, int y)
{
    int windowWidth = 1200, windowHeight = 500;
    return Points(float(x) / windowWidth, 1.0 - float(y) / windowHeight);
}

float tmpX = 0.2;

Square* Square::drag(Square* sqr, Points* mouse)
{
    sqr->pts[0].x = mouse->x - 0.1;
    sqr->pts[1].x = mouse->x + 0.1;
    sqr->pts[2].x = mouse->x + 0.1;
    sqr->pts[3].x = mouse->x - 0.1;

    tmpX = sqr->pts[0].x;
    return sqr;
}

Square* sqr = new Square;



/******************************************************************/

perception_exp::perception_exp() 
{
    cnt = 0;
    tmp = 0;
    memset(cursors, 0, sizeof(cursors));
};

perception_exp::~perception_exp() {};

void perception_exp::init()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    PHANTOM_TOOLS::initHD();
    //initHL();
    
}

void perception_exp::onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.2, 0.47, 0.0);
    glVertex3f(0.8, 0.47, 0.0);
    glVertex3f(0.8, 0.53, 0.0);
    glVertex3f(0.2, 0.53, 0.0);
    glEnd();

    glColor3f(0, 0.74902, 1);
    glBegin(GL_POLYGON);
    glVertex3f(0.2, 0.47, 0.0);
    glVertex3f(tmpX, 0.47, 0.0);
    glVertex3f(tmpX, 0.53, 0.0);
    glVertex3f(0.2, 0.53, 0.0);
    glEnd();

    sqr->draw(sqr);

    glFlush();
}

void perception_exp::mouseClick(GLint Button, GLint State, GLint X, GLint Y)
{
    if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
        SP->WriteData("0", dataLength);
    }
    else if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP)
    {
        SP->WriteData("1", dataLength);
    }
}

void perception_exp::onMultiMotion(int cursor_id, int x, int y)
{

    cursors[0][cursor_id].x = (float)x;
    cursors[0][cursor_id].y = (float)y;

    std::cout << " Cursor_ID : " << cursor_id << " =>  x : " << cursors[0][cursor_id].x << '\n';

    if (tmp != cursor_id)
    {
        tmp = cursor_id;
        cnt++;
    }
    if (cnt != 0 && cnt % 2 == 0)
    {
        Points mousePt = sqr->mouse(cursors[0][tmp].x, cursors[0][tmp].y);

        Points* mouse = &mousePt;


        if (mouse->x > sqr->pts[0].x && mouse->y > sqr->pts[0].y && mouse->x > 0.25 && mouse->x < 0.75 && mouse->y >0.4 && mouse->y < 0.6)
        {
            if (mouse->x < sqr->pts[2].x && mouse->y < sqr->pts[2].y)
            {
                sqr->drag(sqr, mouse);
                glutPostRedisplay();
            }
        }
    }
}

