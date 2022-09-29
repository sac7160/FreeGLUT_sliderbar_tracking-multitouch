
#include <iostream>


#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/freeglut.h>

#include "SerialClass.h"

#include <HD/hd.h>
#include <HL/hl.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

# include <conio.h>
HLuint gEffect;
HDCallbackCode HLCALLBACK PantomCallback();

HDCallbackCode HDCALLBACK PhantomCallback()
{
    hlBeginFrame();
    hlEffectd(HL_EFFECT_PROPERTY_DURATION, 100);
    hlEffectdv(HL_EFFECT_PROPERTY_DIRECTION, hduVector3Dd(0, 0, 1));

    hlStartEffect(HL_EFFECT_CONSTANT, gEffect);

    hlEndFrame();

    return HD_CALLBACK_CONTINUE;
}

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

    
    glClear(GL_COLOR_BUFFER_BIT); 

    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.2, 0.47, 0.0);
    glVertex3f(0.8, 0.47, 0.0);
    glVertex3f(0.8, 0.53, 0.0);
    glVertex3f(0.2, 0.53, 0.0);
    glEnd();
    
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

/// <summary>
/// ///////////////////////////////////////////////////////////
/// </summary>

HHD ghHD = HD_INVALID_HANDLE;

//handle to haptic rendering context
HHLRC ghHLRC;

//effect ID


//effect properties
float gGain = .2f;
float gMagnitude = .5f;
HLenum newEffectType;   //필요하나? 


/******************************************************************************
 Initializes haptics.
******************************************************************************/
void initHL(void)
{
    HDErrorInfo error;

    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getchar();
        exit(-1);
    }

    // Create a haptic context for the device.  The haptic context maintains 
    // the state that persists between frame intervals and is used for
    // haptic rendering.
    ghHLRC = hlCreateContext(ghHD);
    hlMakeCurrent(ghHLRC);

    hlDisable(HL_USE_GL_MODELVIEW);

    gEffect = hlGenEffects(1);

}


/******************************************************************************
 Updates the effect with new properties.
******************************************************************************/
void updateEffect(HLenum newEffectType)
{
    // Update the gain and magnitude of the current effect.
    // The effect must be active.  
    // Note that not all effects will use all of these 
    // properties.
    hlBeginFrame();
    HLboolean bActive = false;
    hlGetEffectbv(gEffect, HL_EFFECT_PROPERTY_ACTIVE, &bActive);
    if (bActive)
    {
        hlEffectd(HL_EFFECT_PROPERTY_GAIN, gGain);
        hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, gMagnitude);
        hlUpdateEffect(gEffect);
        // To get the effect type, use the appropriate effect 
        // query to get the integer representation of the effect,
        // then cast that as an HLenum to get the string name.
        printf("Effect properties for ");
        HLint nType;
        hlGetEffectiv(gEffect, HL_EFFECT_PROPERTY_TYPE, &nType);
        printf("%s\n", newEffectType);
    }
    else
    {
        printf("No effect active\n");
    }
    printf("  Magnitude: %.1f\n", gMagnitude);
    printf("  Gain: %.1f\n", gGain);
    printf("\n");
    hlEndFrame();
}

/******************************************************************************
 Stops the effect.
******************************************************************************/
void stopEffect()
{
    hlBeginFrame();
    // Only necessary to stop the effect if it's active (i.e. on).
    HLboolean bActive = false;
    hlGetEffectbv(gEffect, HL_EFFECT_PROPERTY_ACTIVE, &bActive);
    if (bActive)
    {
        hlStopEffect(gEffect);
    }
    hlEndFrame();

}

/******************************************************************************
 Starts an effect type.
******************************************************************************/
void startEffectType(HLenum newEffectType)
{
    // First stop the current effect.
    stopEffect();

    // Start the new effect, set whatever appropriate unique 
    // properties are necessary for each effect type.
    hlBeginFrame();
    if (newEffectType == HL_EFFECT_SPRING)
    {
        hduVector3Dd position;
        hlGetDoublev(HL_DEVICE_POSITION, position);
        hlEffectdv(HL_EFFECT_PROPERTY_POSITION, position);
    }
    else if (newEffectType == HL_EFFECT_CONSTANT)
    {
        hlEffectd(HL_EFFECT_PROPERTY_DURATION, 100);
        hlEffectdv(HL_EFFECT_PROPERTY_DIRECTION, hduVector3Dd(0, 0, 1));
    }

    hlStartEffect(newEffectType, gEffect);

    hlEndFrame();

    updateEffect(newEffectType);
}

/******************************************************************************
 Prints help.
******************************************************************************/
void printHelp()
{
    static const char help[] = { "\
-----------------------------------------------\n\
      Effect Attributes Example Menu Options\n\
===============================================\n\
Effect Types;\n\
[F] : Switch to friction effect\n\
[S] : Switch to spring effect\n\
[C] : Switch to constant effect\n\
[V] : Switch to viscous effect\n\
[N] : Switch to no effect\n\
\n\
Effect Properties:\n\
[-] : Decrease effect magnitude\n\
[+] : Increase effect magnitude\n\
[[] : Decrease effect gain\n\
[]] : Increase effect gain\n\
\n\
Miscellaneous:\n\
[H] : Print help menu\n\
[Q] : Quit\n\
\n\
(note: Setting gain/magnitude at high values may cause\n\
instability in some force effects and devices.)\n" };

    printf("%s\n\n", help);

}



void Initialize() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    //openHaptics initialize
    initHL();
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

   // glutAddMenuEntry("Quit", 0);
   // glutAttachMenu(GLUT_RIGHT_BUTTON);

  
   
    ////////////////
    Initialize();
    glutDisplayFunc(onDisplay);
    glutMouseFunc(MyMouseClick);
    glutMultiMotionFunc(onMultiMotion);

    printHelp();

    // Loop until the user quits or an error occurs.
   /* bool bDone = false;
    while (!bDone)
    {
        if (_kbhit())
        {
            int key = toupper(_getch());
            bool bNeedsUpdate = false;

            switch (key)
            {
            case 'F':
                newEffectType = HL_EFFECT_FRICTION;
                startEffectType(newEffectType);
                break;
            case 'S':
                newEffectType = HL_EFFECT_SPRING;
                startEffectType(newEffectType);
                break;
            case 'C':
                newEffectType = HL_EFFECT_CONSTANT;
                startEffectType(newEffectType);
                break;
            case 'V':
                newEffectType = HL_EFFECT_VISCOUS;
                startEffectType(newEffectType);
                break;
            case 'N':
                stopEffect();
                break;

            case ']':
                gGain += 0.1f;
                if (gGain > 1.0f)
                    gGain = 1.0f;
                bNeedsUpdate = true;
                break;
            case '[':
                gGain -= 0.1f;
                if (gGain < 0.0f)
                    gGain = 0.0f;
                bNeedsUpdate = true;
                break;
            case '+':
            case '=':
                gMagnitude += 0.1f;
                if (gMagnitude > 1.0f)
                    gMagnitude = 1.0f;
                bNeedsUpdate = true;
                break;
            case '-':
            case '_':
                gMagnitude -= 0.1f;
                if (gMagnitude < 0.0f)
                    gMagnitude = 0.0f;
                bNeedsUpdate = true;
                break;

            case 'Q':
                bDone = true;
                break;
            case 'H':
            default:
                printHelp();
                break;
            }

            if (bNeedsUpdate)
            {
                updateEffect(newEffectType);
            }

            // Check for any errors.
            HLerror error;
            while (HL_ERROR(error = hlGetError()))
            {
                fprintf(stderr, "HL Error: %s\n", error.errorCode);

                if (error.errorCode == HL_DEVICE_ERROR)
                {
                    hduPrintError(stderr, &error.errorInfo,
                        "Error during haptic rendering\n");
                }
            }
        }
    }
    */
    // Release the effect id.
    hlDeleteEffects(gEffect, 1);
    
    // Cleanup.
    hlMakeCurrent(NULL);
    hlDeleteContext(ghHLRC);
    hdDisableDevice(ghHD);
    
    glutMainLoop();

    return EXIT_SUCCESS;
}

