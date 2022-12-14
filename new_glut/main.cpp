#include <iostream>
#include "phantom_helper.h"
#include "perception_exp.h"

void initialize();
void onDisplay();
void MyMouseClick(GLint, GLint, GLint, GLint);
void MyMultiMotion(int, int, int);
void MyKeyboard(unsigned char, int, int);


perception_exp myExp;

int main(int argc, char* argv[]) {
    //memset(cursors, 0, sizeof(cursors));


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

    initialize();
    glutKeyboardFunc(MyKeyboard);
    glutDisplayFunc(onDisplay);
    glutMouseFunc(MyMouseClick);
    glutMultiMotionFunc(MyMultiMotion);


    glutMainLoop();
    
    atexit(PHANTOM_TOOLS::exitHandler);

    return EXIT_SUCCESS;
}

void initialize()
{
    myExp.init();
}

void MyKeyboard(unsigned char KeyPressed, int x, int y)
{
    myExp.MyKeyboard(KeyPressed,x,y);
}

void onDisplay()
{
    myExp.onDisplay();
}

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y)
{
    myExp.mouseClick(Button, State, X, Y);
}

void MyMultiMotion(int cursor_id, int x, int y)
{
    myExp.onMultiMotion(cursor_id, x, y);
}

