
#include <iostream>


#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/freeglut.h>

#include "SerialClass.h"

////////////////////////////////
//OpenHaptics

#include <HL/hl.h>
#include <HDU/hduError.h>

HLuint gMyShapeId;
HHLRC hHLRC;
HHD hHD;

void exitHandler()
{
    // Deallocate the sphere shape id we reserved in initHL.
    hlDeleteShapes(gMyShapeId, 1);

    // Free up the haptic rendering context.
    hlMakeCurrent(NULL);
    if (hHLRC != NULL)
    {
        hlDeleteContext(hHLRC);
    }

    // Free up the haptic device.
    if (hHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(hHD);
    }
}

void glutMenu(int ID)
{
    switch (ID) {
    case 0:
        exit(0);
        break;
    }
}



////////////////////////////////
using namespace std;

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

    glColor3f(0.827451,	0.827451,0.827451);
    glBegin(GL_QUADS);
    for (int i = 0; i < 4; ++i)
    {
        glVertex2f(sqr->pts[i].x, sqr->pts[i].y);
    }
    glEnd();

    glColor3f(0.745098, 0.745098 , 0.745098);
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

GLint TopLeftX, TopLeftY, BottomRightX, BottomRightY;
void onDisplay(void) {

    hlBeginFrame(); //start haptic frame
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //depthbufferbit 추가

    glColor3f(0.0, 0.0, 0.0);
      // Start the haptic shape.
    hlBeginShape(HL_SHAPE_DEPTH_BUFFER, gMyShapeId);
    glBegin(GL_POLYGON);
    glVertex3f(0.25, 0.25, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, 0.75, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
    glEnd();

    // End the haptic shape.
    hlEndShape();

    // End the haptic frame.
    hlEndFrame();


    /*glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.2, 0.47, 0.0);
    glVertex3f(0.8, 0.47, 0.0);
    glVertex3f(0.8, 0.53, 0.0);
    glVertex3f(0.2, 0.53, 0.0);
    glEnd();
    */
    

    glColor3f(0	,0.74902	,1);
    glBegin(GL_POLYGON);
    glVertex3f(0.2, 0.47, 0.0);
    glVertex3f(tmpX, 0.47, 0.0);
    glVertex3f(tmpX, 0.53, 0.0);
    glVertex3f(0.2, 0.53, 0.0);
    glEnd();

    sqr->draw(sqr);

    glFlush();

    
}

int cnt = 0, tmp = 0;

void onMultiMotion(int cursor_id, int x, int y) {

    cursors[0][cursor_id].x = (float)x;
    cursors[0][cursor_id].y = (float)y;

    cout << " Cursor_ID : " << cursor_id << " =>  x : " << cursors[0][cursor_id].x <<  '\n';

    if (tmp != cursor_id)
    {
        tmp = cursor_id;
        cnt++;
    }
    if (cnt != 0 && cnt % 2 == 0)
    {
        Points mousePt = sqr->mouse(cursors[0][tmp].x , cursors[0][tmp].y );

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

void Initialize() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    /*glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);*/


    //OpenHaptics init

    
    hlLoadIdentity();
    hlOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    HDErrorInfo error;
    hHD = hdInitDevice(HD_DEFAULT_DEVICE);

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(-1);
    }

    if (HD_SUCCESS != hdGetError().errorCode)
    {
        fprintf(stderr, "Erorr initializing haptic device.\nPress any key to exit");
        getchar();
        exit(-1);
    }

    hHLRC = hlCreateContext(hHD);
    hlMakeCurrent(hHLRC);

    gMyShapeId = hlGenShapes(1);
    hlWorkspace(-80, -80, -70, 80, 80, 20);

    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlLoadIdentity();
    hlOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

   
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
   // glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("LRA_FreeGLUT");

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {

        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }


    /////////////////
    glutCreateMenu(glutMenu);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    atexit(exitHandler);
    ////////////////
    Initialize();
    glutDisplayFunc(onDisplay);
    glutMouseFunc(MyMouseClick);
    glutMultiMotionFunc(onMultiMotion);

    glutMainLoop();

    return EXIT_SUCCESS;
}

