/**
   JWacom 1.0, Jan 2004.

   JWacom native shared library.
   2004 Copyright (C) Tue Haste Andersen
   @authorTue Haste Andersen <haste@diku.dk>
*/

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <stdbool.h>
#include <jni.h>
#include "jwacom_Wacom.h"
#include <stdlib.h>

/** X error code */
int iLastXError;
/** Pointer to open device */
XDevice *pDevice;
/** Pointer to open display */
Display *pDisplay;
/** Proximity */
bool bProximity;
/** X, Y, tilt X, tilt Y, pressure */
int iPosX, iPosY, iTiltX, iTiltY, iPressure;

// These variables are used to scale the position values received from the X
// server to actually match screen pixel positions
int minX, maxX, minY, maxY, width, height;


// Event enumeration
enum
{
    INPUTEVENT_KEY_PRESS,
    INPUTEVENT_KEY_RELEASE,
    INPUTEVENT_FOCUS_IN,
    INPUTEVENT_FOCUS_OUT,
    INPUTEVENT_BTN_PRESS,
    INPUTEVENT_BTN_RELEASE,
    INPUTEVENT_PROXIMITY_IN,
    INPUTEVENT_PROXIMITY_OUT,
    INPUTEVENT_MOTION_NOTIFY,
    INPUTEVENT_DEVICE_STATE_NOTIFY,
    INPUTEVENT_DEVICE_MAPPING_NOTIFY,
    INPUTEVENT_CHANGE_DEVICE_NOTIFY,
    INPUTEVENT_DEVICE_POINTER_MOTION_HINT,
    INPUTEVENT_DEVICE_BUTTON_MOTION,
    INPUTEVENT_DEVICE_BUTTON1_MOTION,
    INPUTEVENT_DEVICE_BUTTON2_MOTION,
    INPUTEVENT_DEVICE_BUTTON3_MOTION,
    INPUTEVENT_DEVICE_BUTTON4_MOTION,
    INPUTEVENT_DEVICE_BUTTON5_MOTION,

    INPUTEVENT_MAX
};

int iInputEvent[INPUTEVENT_MAX] = { 0 };

//
// Library helper functions
//

int errorHandler(Display *pDisplay, XErrorEvent *pEvent)
{
    char chBuf[64];
    XGetErrorText(pDisplay, pEvent->error_code, chBuf, sizeof(chBuf));
    fprintf(stderr,"X Error: %d %s\n", pEvent->error_code, chBuf);
    iLastXError = pEvent->error_code;
    return 0;
}

int getLastXError()
{
    return iLastXError;
}

Display* initXInput()
{
    Display* pDisplay;
    int iMajor, iFEV, iFER;

    pDisplay = XOpenDisplay(NULL);
    if (!pDisplay)
    {
        fprintf(stderr,"Failed to connect to X server.\n");
        return NULL;
    }

    XSetErrorHandler(errorHandler);

    XSynchronize(pDisplay,1);

    if (!XQueryExtension(pDisplay,INAME,&iMajor,&iFEV,&iFER))
    {
        fprintf(stderr,"Server does not support XInput extension.\n");
        XCloseDisplay(pDisplay);
        return NULL;
    }

    return pDisplay;
}

XID getDeviceID(Display *pDisplay, const char *sDeviceName)
{
    // Get list of devices
    int no;
    XID id = 0;
    XDeviceInfo *pDevInfoList = XListInputDevices(pDisplay, &no);
    if (!pDevInfoList)
    {
        fprintf(stderr,"Failed to get input device list.\n");
        return 0;
    }

    // Find device in list
    int i;
    for (i=0; i<no; ++i)
    {
        if (strcasecmp(pDevInfoList[i].name,sDeviceName) == 0)
        {
            id = pDevInfoList[i].id;

            // Get axis ranges
            XAnyClassPtr pClass = pDevInfoList[i].inputclassinfo;
            int j;
            for (j=0; j<pDevInfoList[i].num_classes; ++j)
            {
                if (pClass->class == ValuatorClass)
                {
                    XValuatorInfoPtr pVal = (XValuatorInfoPtr)pClass;
                    minX = pVal->axes[0].min_value;
                    maxX = pVal->axes[0].max_value;
                    minY = pVal->axes[1].min_value;
                    maxY = pVal->axes[1].max_value;

                    break;
                }

                // skip to next record
                pClass = (XAnyClassPtr)((char*)pClass + pClass->length);
            }

            return id;
        }
    }

    return 0;
}

void queryDevice()
{
    XDeviceState *pState = XQueryDeviceState(pDisplay, pDevice);
    XInputClass *pInputClass = pState->data;

    int j;
    for (j=0; j<pState->num_classes; ++j)
    {
        if (pInputClass->class==ValuatorClass)
        {
            // Read parameters into global variables
            XValuatorState *pValuatorState = (XValuatorState*)pInputClass;

            if (pValuatorState->mode & 2)
                bProximity = false;
            else
                bProximity = true;

            if (pValuatorState->num_valuators>4)
            {
                iPressure = pValuatorState->valuators[2];
                iTiltX = pValuatorState->valuators[3];
                iTiltY = pValuatorState->valuators[4];
                iPosX = pValuatorState->valuators[0];
                iPosY = pValuatorState->valuators[1];

                // Scale positions
                iPosX = ((iPosX-minX)*width)/(maxX-minX);
                iPosY = ((iPosY-minY)*height)/(maxY-minY);
            }
            break;
        }
        pInputClass = (XInputClass*)(((char*)pInputClass)+pInputClass->length);
    }
    free(pState);
}

//
// Library API functions
//

jboolean JNICALL Java_jwacom_Wacom_openDevice(JNIEnv *env, jobject obj, jstring name)
{
    // Get the name string from Java
    char buf[128];
    const char *str = (*env)->GetStringUTFChars(env, name, 0);

    // Get pointer to display of XServer with XInput
    pDisplay = initXInput();
    if (!pDisplay)
        return false;

    // Get pointer to stylus device
    XID iDevice = getDeviceID(pDisplay, str);

    // Release string from Java
    (*env)->ReleaseStringUTFChars(env, name, str);

    // Open device
    pDevice = XOpenDevice(pDisplay, iDevice);
    if (!pDevice)
        return false;

    // Get scale factors
    XWindowAttributes rootAttributes;
    XGetWindowAttributes(pDisplay, DefaultRootWindow(pDisplay), &rootAttributes);
    width = rootAttributes.width;
    height = rootAttributes.height;

    // Get initial values
    queryDevice();

    //
    // Initialize events
    //
    XEventClass cls;
    XEventClass eventList[32];
    int iEventListCnt = 0;


    // key events
    DeviceKeyPress(pDevice,iInputEvent[INPUTEVENT_KEY_PRESS],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    DeviceKeyRelease(pDevice,iInputEvent[INPUTEVENT_KEY_RELEASE],cls);
    if (cls) eventList[iEventListCnt++] = cls;

    // focus events
    DeviceFocusIn(pDevice,iInputEvent[INPUTEVENT_FOCUS_IN],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    DeviceFocusOut(pDevice,iInputEvent[INPUTEVENT_FOCUS_OUT],cls);
    if (cls) eventList[iEventListCnt++] = cls;

    // button events
    DeviceButtonPress(pDevice,iInputEvent[INPUTEVENT_BTN_PRESS],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    DeviceButtonRelease(pDevice,iInputEvent[INPUTEVENT_BTN_RELEASE],cls);
    if (cls) eventList[iEventListCnt++] = cls;

    // Proximity events
    ProximityIn(pDevice, iInputEvent[INPUTEVENT_PROXIMITY_IN], cls);
    if (cls) eventList[iEventListCnt++] = cls;
    ProximityOut(pDevice,iInputEvent[INPUTEVENT_PROXIMITY_OUT],cls);
    if (cls) eventList[iEventListCnt++] = cls;

    // motion events
    DeviceMotionNotify(pDevice,iInputEvent[INPUTEVENT_MOTION_NOTIFY],cls);
    if (cls) eventList[iEventListCnt++] = cls;

    // device state
    DeviceStateNotify(pDevice,iInputEvent[INPUTEVENT_DEVICE_STATE_NOTIFY],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    DeviceMappingNotify(pDevice,iInputEvent[INPUTEVENT_DEVICE_MAPPING_NOTIFY],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    ChangeDeviceNotify(pDevice,iInputEvent[INPUTEVENT_CHANGE_DEVICE_NOTIFY],cls);
    if (cls) eventList[iEventListCnt++] = cls;

    // button motion
    DeviceButtonMotion(pDevice,iInputEvent[INPUTEVENT_DEVICE_BUTTON_MOTION],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    DeviceButton1Motion(pDevice,iInputEvent[INPUTEVENT_DEVICE_BUTTON1_MOTION],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    DeviceButton2Motion(pDevice,iInputEvent[INPUTEVENT_DEVICE_BUTTON2_MOTION],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    DeviceButton3Motion(pDevice,iInputEvent[INPUTEVENT_DEVICE_BUTTON3_MOTION],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    DeviceButton4Motion(pDevice,iInputEvent[INPUTEVENT_DEVICE_BUTTON4_MOTION],cls);
    if (cls) eventList[iEventListCnt++] = cls;
    DeviceButton5Motion(pDevice,iInputEvent[INPUTEVENT_DEVICE_BUTTON5_MOTION],cls);
    if (cls) eventList[iEventListCnt++] = cls;

    XGrabDevice(pDisplay, pDevice, DefaultRootWindow(pDisplay),
                0, // no owner events
                iEventListCnt, eventList, // events
                GrabModeAsync, // don't queue
                GrabModeAsync, // same
                CurrentTime);

    return true;
}

void JNICALL Java_jwacom_Wacom_closeDevice(JNIEnv *env, jobject obj)
{
    // Ungrab and close device
    XUngrabDevice(pDisplay,pDevice,CurrentTime);
    XCloseDevice(pDisplay, pDevice);

    // Close display
    XCloseDisplay(pDisplay);
}

jint JNICALL Java_jwacom_Wacom_nextEvent(JNIEnv *env, jobject obj)
{
    // Wait for an event in the X queue
    XEvent event;
    XAnyEvent *pAnyEvent;
    XNextEvent(pDisplay, &event);

    // Check if the event affects the stylus
    pAnyEvent = (XAnyEvent *)&event;

    int r = pAnyEvent->type;

    if (r==iInputEvent[INPUTEVENT_PROXIMITY_IN])
    {
        bProximity = true;
        return 0;
    }
    else if (r==iInputEvent[INPUTEVENT_PROXIMITY_OUT])
    {
        bProximity = false;
        return 0;
    }
    else if (r==iInputEvent[INPUTEVENT_FOCUS_IN])
        return 1;
    else if (r==iInputEvent[INPUTEVENT_FOCUS_OUT])
        return 2;
    else if (r==iInputEvent[INPUTEVENT_MOTION_NOTIFY])
    {
        queryDevice();
        return 3;
    }
    else if (r==iInputEvent[INPUTEVENT_BTN_PRESS] || r==iInputEvent[INPUTEVENT_BTN_RELEASE])
    {
        XDeviceButtonEvent* pButton = (XDeviceButtonEvent*)pAnyEvent;
        bool bDown = (pAnyEvent->type == iInputEvent[INPUTEVENT_BTN_PRESS]);
        int iButton = pButton->button;
        if ((iButton < 1) || (iButton > 5))
            iButton = 6;
        if (bDown)
            iButton += 6;

        return 3+iButton;
    }
    else
        return -1;
}

jboolean JNICALL Java_jwacom_Wacom_inProximity(JNIEnv *env, jobject obj)
{
    return bProximity;
}

jint JNICALL Java_jwacom_Wacom_getPressure(JNIEnv *env, jobject obj)
{
    return iPressure;
}

jint JNICALL Java_jwacom_Wacom_getTiltX(JNIEnv *env, jobject obj)
{
    return iTiltX;
}

jint JNICALL Java_jwacom_Wacom_getTiltY(JNIEnv *env, jobject obj)
{
    return iTiltY;
}

jint JNICALL Java_jwacom_Wacom_getPosX(JNIEnv *env, jobject obj)
{
    return iPosX;
}

int JNICALL Java_jwacom_Wacom_getPosY(JNIEnv *env, jobject obj)
{
    return iPosY;
}
