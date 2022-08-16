/**
 * Sample multi-touch program that displays a square where a cursor
 * clicks, with a different color for each cursor.
 *
 * Copyright (C) 2012  Sylvain Beucler
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/freeglut.h>

#include "SerialClass.h"

using namespace std;

Serial* SP = new Serial("\\\\.\\COM3");    // adjust as needed
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
    // draw square fill
    int i;
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
    for (i = 0; i < 4; ++i)
    {
        glVertex2f(sqr->pts[i].x, sqr->pts[i].y);
    }
    glEnd();
    // draw square points
    i = 0;

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
    for (i = 0; i < 4; ++i)
    {
        glVertex2f(sqr->pts[i].x, sqr->pts[i].y);
    }
    glEnd();
}

Points Square::mouse(int x, int y)
{
    int windowWidth = 1200, windowHeight = 500;
    return Points(float(x) / windowWidth, 1.0 - float(y) / windowHeight);
}

Square* Square::drag(Square* sqr, Points* mouse)
{
    sqr->pts[0].x = mouse->x - 0.1;
    //sqr->pts[0].y = mouse->y - 0.1;
    sqr->pts[1].x = mouse->x + 0.1;
    //sqr->pts[1].y = mouse->y - 0.1;

    sqr->pts[3].x = mouse->x - 0.1;
    // sqr->pts[3].y = mouse->y + 0.1;

    sqr->pts[2].x = mouse->x + 0.1;
    // sqr->pts[2].y = mouse->y + 0.1;

    return sqr;
}

Square* sqr = new Square;

/*static float square[] = {
        -.5, -.5,
         .5, -.5,
        -.5,  .5,
         .5,  .5,
};*/
GLint TopLeftX, TopLeftY, BottomRightX, BottomRightY;
void onDisplay(void) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.2, 0.45, 0.0);
    glVertex3f(0.8, 0.45, 0.0);
    glVertex3f(0.8, 0.55, 0.0);
    glVertex3f(0.2, 0.55, 0.0);
    glEnd();

    sqr->draw(sqr);


    glFlush();

}

int cnt = 0, tmp = 0;

void onMultiMotion(int cursor_id, int x, int y) {
    /*if (cursor_id >= NUM_CURSORS) {
        fprintf(stderr, "cursor_id (%d) >= NUM_CURSORS (%d), out of slots\n", cursor_id, NUM_CURSORS);
        return;
    }*/
    /*    정상 작동코드*************************************************************************************
    cursors[0][cursor_id].x = (float)x;
    cursors[0][cursor_id].y = (float)y;

    cout << " Cursor_ID : " << cursor_id << " =>  x : " << cursors[0][cursor_id].x << '\n';

    if (cursors[0][cursor_id].x == 300) { SP->WriteData("0", dataLength); Sleep(100); }
    else if (cursors[0][cursor_id].x == 700) { SP->WriteData("1", dataLength); Sleep(100); }
    //else { SP->WriteData("-1", dataLength); Sleep(100); }


    Points mousePt = sqr->mouse(cursors[0][cursor_id].x,cursors[0][cursor_id].y);
  
    Points* mouse = &mousePt;
    

    if (mouse->x > sqr->pts[0].x && mouse->y > sqr->pts[0].y && mouse->x > 0.25 && mouse->x < 0.75 && mouse->y >0.4 && mouse->y < 0.6)
    {
        if (mouse->x < sqr->pts[2].x && mouse->y < sqr->pts[2].y)
        {
            // then drag by chaning square coordinates relative to mouse
            sqr->drag(sqr, mouse);
            glutPostRedisplay();
        }
    }*/

    cursors[0][cursor_id].x = (float)x;
    cursors[0][cursor_id].y = (float)y;

    cout << " Cursor_ID : " << cursor_id << " =>  x : " << cursors[0][cursor_id].x << " tmp : " << tmp <<"  cnt : " << cnt << '\n';

    if (tmp != cursor_id)
    {
        tmp = cursor_id;
        cnt++;
    }
    if (cnt != 0 && cnt % 2 == 0)
    {
        //Points mousePt = sqr->mouse((cursors[0][tmp].x+cursors[0][tmp-2].x)/2, (cursors[0][tmp].y+cursors[0][tmp-2].y)/2);
        Points mousePt = sqr->mouse(cursors[0][tmp].x , cursors[0][tmp].y );

        Points* mouse = &mousePt;


        if (mouse->x > sqr->pts[0].x && mouse->y > sqr->pts[0].y && mouse->x > 0.25 && mouse->x < 0.75 && mouse->y >0.4 && mouse->y < 0.6)
        {
            if (mouse->x < sqr->pts[2].x && mouse->y < sqr->pts[2].y)
            {
                // then drag by chaning square coordinates relative to mouse
                sqr->drag(sqr, mouse);
                glutPostRedisplay();
            }
        }
    }
}

/*void onReshape(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glOrtho(0, width, height, 0, -1, 1);
}*/

/*void onIdle(void) {
    glutPostRedisplay();
}*/
void Initialize() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
    if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
        SP->WriteData("0", dataLength) ;
    }
    else if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP)
    {
        SP->WriteData("1", dataLength);
    }
}

int main(int argc, char* argv[]) {
    memset(cursors, 0, sizeof(cursors));


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1200, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("LRA_FreeGLUT");

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
   // glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {

        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }



    glutDisplayFunc(onDisplay);
    // glutReshapeFunc(onReshape);
    //glutIdleFunc(onIdle);
    // glutMouseFunc(onMouse);
     //glutMotionFunc(onMotion);


    // glutMultiButtonFunc(onMultiButton);
    glutMouseFunc(MyMouseClick);
    glutMultiMotionFunc(onMultiMotion);
    Initialize();

    glutMainLoop();

    return EXIT_SUCCESS;
}

