#include "phantom_helper.h"
# include <conio.h>

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


bool once = true;
HDdouble kStiffness = 0.5; /* N/mm 이 변수로 N 조절*/

namespace PHANTOM_TOOLS
{
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


        hUpdateDeviceCallback = hdScheduleAsynchronous(
            DeviceStateCallback, 0, HD_MAX_SCHEDULER_PRIORITY);


        hdEnable(HD_FORCE_OUTPUT);
        hdStartScheduler();

    }

    void adjust_force()
    {
        if (_kbhit())
        {
            int key = toupper(getchar());
            //char key = getchar();

            switch (key)
            {
            case '_':
            case '-':
                kStiffness -= 0.1;
                break;
            case '=':
            case '+':
                kStiffness += 0.1;
                break;
            }
            once = true;
        }
    }
}




HDCallbackCode HDCALLBACK DeviceStateCallback(void* data)
{


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

    hduVector3Dd sub = { 1,0,0 };
    hduVector3Dd previous_position;
    hduVecSubtract(previous_position, position, sub);    //현재 position x방향 -1 position


    hduVector3Dd tmp;
    hduVecSubtract(tmp, previous_position, position);


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


    PHANTOM_TOOLS::adjust_force();


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