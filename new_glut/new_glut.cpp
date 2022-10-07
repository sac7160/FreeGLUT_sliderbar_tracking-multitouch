
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

/******************************************************************************
*******************************************************************************/
static HHD ghHD = HD_INVALID_HANDLE;

static HDSchedulerHandle hUpdateDeviceCallback = HD_INVALID_HANDLE;

static HDboolean isActive = HD_FALSE;

/* Handle to haptic rendering context. */
HHLRC ghHLRC;

//effect id
HLuint gEffect;

/* Effect properties */
float gGain = 1.0f;
float gMagnitude = 1.0f;


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
    hdEndFrame(ghHD);
}


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


/*******************************************************************************
 This is the main haptic rendering callback.  This callback will render an
 anchored spring force when the user presses the button.
*******************************************************************************/
HDCallbackCode HDCALLBACK mainCallback(void* pUserData)
{
    int currentButtons, lastButtons;

    HDErrorInfo error;
    hduVector3Dd force = { 0, 0, 0 };

    //hdBeginFrame(ghHD);

    hdGetIntegerv(HD_CURRENT_BUTTONS, &currentButtons);
    hdGetIntegerv(HD_LAST_BUTTONS, &lastButtons);

    /* Detect button state transitions. */
    if ((currentButtons & HD_DEVICE_BUTTON_1) != 0 &&
        (lastButtons & HD_DEVICE_BUTTON_1) == 0)
    {
        isActive = HD_TRUE;
        //hdGetDoublev(HD_CURRENT_POSITION, gAnchorPosition); position확인 주석처리
    }
    else if ((currentButtons & HD_DEVICE_BUTTON_1) == 0 &&
        (lastButtons & HD_DEVICE_BUTTON_1) != 0)
    {
        isActive = HD_FALSE;
    }

    if (isActive)   //haptic effect
    {
        //std::cout << "isActive" << '\n';
        // Start the new effect, set whatever appropriate unique 
        // properties are necessary for each effect type.
        hlBeginFrame();

        hlStartEffect(HL_EFFECT_FRICTION, gEffect);

        hlEndFrame();

    }

    hdSetDoublev(HD_CURRENT_FORCE, force);

    //hdEndFrame(ghHD);

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        if (hduIsForceError(&error))
        {
            /* Disable the anchor following the force error. */
            isActive = HD_FALSE;
        }
        else
        {
            /* This is likely a more serious error, so bail. */
            hduPrintError(stderr, &error, "Error during haptic rendering");
            exit(-1);
        }
    }

    return HD_CALLBACK_CONTINUE;
}

void startEffectType(HLenum newEffectType)
{
    // First stop the current effect.
    stopEffect();

    // Start the new effect, set whatever appropriate unique 
    // properties are necessary for each effect type.
  
    hlBeginFrame();

    hlEffectd(HL_EFFECT_PROPERTY_GAIN, gGain);
    hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, gMagnitude);
    hlStartEffect(newEffectType, gEffect);
    
    hlEndFrame();

    updateEffect(newEffectType);
}

 HDboolean first = HD_TRUE;

HDCallbackCode HDCALLBACK mainCallback2(void* pUserData)
{
    int currentButtons, lastButtons;

    HDErrorInfo error;

    hduVector3Dd position;
    hduVector3Dd force = { 0, 0, 0 };
    HDdouble forceClamp;

   
    hlBeginFrame();
    

    //hdGetIntegerv(HD_CURRENT_BUTTONS, &currentButtons);
    //hdGetIntegerv(HD_LAST_BUTTONS, &lastButtons);

    /* Detect button state transitions. */
    /*if ((currentButtons & HD_DEVICE_BUTTON_1) != 0 &&
        (lastButtons & HD_DEVICE_BUTTON_1) == 0)
    {
        std::cout << "isActive true로 바꿈";
        isActive = HD_TRUE;
        //hdGetDoublev(HD_CURRENT_POSITION, gAnchorPosition); position확인 주석처리
    }
    else if ((currentButtons & HD_DEVICE_BUTTON_1) == 0 &&
        (lastButtons & HD_DEVICE_BUTTON_1) != 0)
    {
        std::cout << "isActive false로 바꿈";
        isActive = HD_FALSE;
    }*/
    
    if (isActive && first)   //haptic effect
    {
        std::cout << "isActive" << '\n';
        // Start the new effect, set whatever appropriate unique 
        // properties are necessary for each effect type.
        //startEffectType(HL_EFFECT_FRICTION); 

      

        
       
        first = false;


    }

    if (first)
    {
        hlBeginFrame(); 
        hlEffectd(HL_EFFECT_PROPERTY_DURATION, 1000); 
        hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.4); 
        hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 0.4); 
        hlTriggerEffect(HL_EFFECT_FRICTION);   
        hlEndFrame();
        first = HL_FALSE;
    }

    hlEndFrame();
 

    return HD_CALLBACK_CONTINUE;
}

bool once = true;

HDCallbackCode HDCALLBACK test(void* data)
{
    const HDdouble kStiffness = 0.5; /* N/mm 이 변수로 N 조절*/
    const HDdouble kGravityWellInfluence = 40; /* mm */

    /* This is the position of the gravity well in cartesian
       (i.e. x,y,z) space. */
    static const hduVector3Dd wellPos = { -100,0,0 };

    HDErrorInfo error;
    hduVector3Dd position;
    hduVector3Dd force;
    hduVector3Dd positionTwell;

    HHD hHD = hdGetCurrentDevice();

    /* Begin haptics frame.  ( In general, all state-related haptics calls
       should be made within a frame. ) */
    hdBeginFrame(hHD);

    /* Get the current position of the device. */
    hdGetDoublev(HD_CURRENT_POSITION, position);

    memset(force, 0, sizeof(hduVector3Dd));

    hduVector3Dd sub = {1,0,0};
    hduVector3Dd previous_position;
    hduVecSubtract(previous_position, position,sub);    //현재 position x방향 -1 position

    /* >  positionTwell = wellPos-position  <
       Create a vector from the device position towards the gravity
       well's center. */
    //hduVecSubtract(positionTwell, wellPos, position);

    hduVector3Dd tmp;
    hduVecSubtract(tmp,  previous_position,position );

    /* If the device position is within some distance of the gravity well's
       center, apply a spring force towards gravity well's center.  The force
       calculation differs from a traditional gravitational body in that the
       closer the device is to the center, the less force the well exerts;
       the device behaves as if a spring were connected between itself and
       the well's center. */
    //if (hduVecMagnitude(positionTwell) < kGravityWellInfluence)
    //{
        /* >  F = k * x  <
           F: Force in Newtons (N)
           k: Stiffness of the well (N/mm)
           x: Vector from the device endpoint position to the center
           of the well. */
    //    hduVecScale(force, positionTwell, kStiffness);
   // }

    //현재 position과 이전 position 1차이
    /* > F = k * x <
        F: Force in Newtons(N)
        k : Stiffness of the well(N / mm)
        x : 오른쪽 방향 1mm
    */

    hduVecScale(force, tmp, kStiffness);

    /* Send the force to the device. */
    hdSetDoublev(HD_CURRENT_FORCE, force);
    if (once) std::cout << kStiffness << "N 크기의 force 생성";
    once = false;
    

    /* End haptics frame. */
    hdEndFrame(hHD);

    /* Check for errors and abort the callback if a scheduler error
       is detected. */
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error,
            "Error detected while rendering gravity well\n");

        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }

    /* Signify that the callback should continue running, i.e. that
       it will be called again the next scheduler tick. */
    return HD_CALLBACK_CONTINUE;
}


/*void initHD()
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

    hUpdateDeviceCallback = hdScheduleAsynchronous(
        mainCallback, 0, HD_MAX_SCHEDULER_PRIORITY);

    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start the scheduler");
        exit(-1);
    }

}
*/

void initHD()
{
    HDErrorInfo error;

    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(-1);
    }

    printf("Found device model: %s / serial number: %s.\n\n",
        hdGetString(HD_DEVICE_MODEL_TYPE), hdGetString(HD_DEVICE_SERIAL_NUMBER));

    /////////////////
    hUpdateDeviceCallback = hdScheduleAsynchronous(
        test, 0, HD_MAX_SCHEDULER_PRIORITY);

    hdEnable(HD_FORCE_OUTPUT);
    hdStartScheduler();

}

void initHL()
{
 

    ghHLRC = hlCreateContext(ghHD);
    hlMakeCurrent(ghHLRC);

   

    //hlDisable(HL_USE_GL_MODELVIEW);

    gEffect = hlGenEffects(1);
  

  

    hUpdateDeviceCallback = hdScheduleAsynchronous(
        test, 0, HD_MAX_SCHEDULER_PRIORITY);

    hdEnable(HD_FORCE_OUTPUT);
    hdStartScheduler();
    
    /*
    hdScheduleAsynchronous(
        mainCallback2, 0, HD_MAX_SCHEDULER_PRIORITY);
    ghHLRC = hlCreateContext(ghHD);
    hlMakeCurrent(ghHLRC);
    hdEnable(HD_FORCE_OUTPUT);
   */


}





/******************************************************************************
*******************************************************************************/


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

int cnt = 0, tmp = 0;

void onMultiMotion(int cursor_id, int x, int y) {

    cursors[0][cursor_id].x = (float)x;
    cursors[0][cursor_id].y = (float)y;

    cout << " Cursor_ID : " << cursor_id << " =>  x : " << cursors[0][cursor_id].x << '\n';

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




void Initialize() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    initHD();
    //initHL();
}

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
    if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
        SP->WriteData("0", dataLength);
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



    glutMainLoop();

    return EXIT_SUCCESS;
}

