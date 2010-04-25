/**
   JWacom 1.0, Jan 2004.

   JWacom functions for reading Wacom pointing device properties.
   2004 Copyright (C) Tue Haste Andersen

   @author Tue Haste Andersen <haste@diku.dk>
*/

package jwacom;

import javax.swing.event.EventListenerList;

public class Wacom extends Thread
{
    private EventListenerList wacomListeners = new EventListenerList();
    private boolean deviceOpen;
    
    public Wacom(String name)
    {
        deviceOpen = false;
        if (openDevice(name))
        {
            start();
            deviceOpen = true;
        }
        else
            System.out.println("Could not open wacom device.");
    }

    public boolean isDeviceOpen()
    {
        return deviceOpen;
    }

    /** Main thread loop */
    public void run()
    {
        boolean bInProximity = inProximity();
        boolean bInFocus = false;
        int iPosX = getPosX();
        int iPosY = getPosY();
        int iTiltX = getTiltX();
        int iTiltY = getTiltY();
        int iPressure = getPressure();


        while (true)
        {
            int event = nextEvent();
            switch (event)
            {
            case 0:
                bInProximity = inProximity();
                sendProximityEvent(new WacomEvent(bInProximity, bInFocus, iPosX, iPosY, iTiltX, iTiltY, iPressure, 0));
                break;
            case 1:
                bInFocus = true;
                sendFocusEvent(new WacomEvent(bInProximity, bInFocus, iPosX, iPosY, iTiltX, iTiltY, iPressure, 0));
                break;
            case 2:
                bInFocus = false;
                sendFocusEvent(new WacomEvent(bInProximity, bInFocus, iPosX, iPosY, iTiltX, iTiltY, iPressure, 0));
                break;
            case 3:
                iPosX = getPosX();
                iPosY = getPosY();
                iTiltX = getTiltX();
                iTiltY = getTiltY();
                iPressure = getPressure();
                sendMotionEvent(new WacomEvent(bInProximity, bInFocus, iPosX, iPosY, iTiltX, iTiltY, iPressure, 0));
                break;
            default:
                // Button press
                int iButton = event-3;
                if (iButton>6)
                {
                    iButton -= 6;
                    sendButtonDownEvent(new WacomEvent(bInProximity, bInFocus, iPosX, iPosY, iTiltX, iTiltY, iPressure, iButton));
                }
                else
                    sendButtonUpEvent(new WacomEvent(bInProximity, bInFocus, iPosX, iPosY, iTiltX, iTiltY, iPressure, iButton));

            }
        }
    }

    public void addWacomListener(WacomListener listener)
    {
        wacomListeners.add(WacomListener.class, listener);
    }

    public void removeWacomListener(WacomListener listener)
    {
        wacomListeners.remove(WacomListener.class, listener);
    }

    protected void sendProximityEvent(WacomEvent wacomEvent)
    {
        Object[] listeners = wacomListeners.getListenerList();

        // loop through each listener and pass on the event if needed
        int numListeners = listeners.length;
        for (int i=0; i<numListeners; ++i)
        {
            if (listeners[i]==WacomListener.class)
            {
                // pass the event to the listeners event dispatch method
                ((WacomListener)listeners[i+1]).proximity(wacomEvent);
            }
        }
    }

    protected void sendFocusEvent(WacomEvent wacomEvent)
    {
        Object[] listeners = wacomListeners.getListenerList();

        // loop through each listener and pass on the event if needed
        int numListeners = listeners.length;
        for (int i=0; i<numListeners; ++i)
        {
            if (listeners[i]==WacomListener.class)
            {
                // pass the event to the listeners event dispatch method
                ((WacomListener)listeners[i+1]).focus(wacomEvent);
            }
        }
    }

    protected void sendMotionEvent(WacomEvent wacomEvent)
    {
        Object[] listeners = wacomListeners.getListenerList();

        // loop through each listener and pass on the event if needed
        int numListeners = listeners.length;
        for (int i=0; i<numListeners; ++i)
        {
            if (listeners[i]==WacomListener.class)
            {
                // pass the event to the listeners event dispatch method
                ((WacomListener)listeners[i+1]).motion(wacomEvent);
            }
        }
    }

    protected void sendButtonDownEvent(WacomEvent wacomEvent)
    {
        Object[] listeners = wacomListeners.getListenerList();

        // loop through each listener and pass on the event if needed
        int numListeners = listeners.length;
        for (int i=0; i<numListeners; ++i)
        {
            if (listeners[i]==WacomListener.class)
            {
                // pass the event to the listeners event dispatch method
                ((WacomListener)listeners[i+1]).buttonDown(wacomEvent);
            }
        }
    }

    protected void sendButtonUpEvent(WacomEvent wacomEvent)
    {
        Object[] listeners = wacomListeners.getListenerList();

        // loop through each listener and pass on the event if needed
        int numListeners = listeners.length;
        for (int i=0; i<numListeners; ++i)
        {
            if (listeners[i]==WacomListener.class)
            {
                // pass the event to the listeners event dispatch method
                ((WacomListener)listeners[i+1]).buttonUp(wacomEvent);
            }
        }
    }

    /** Open a XInput device
        @param name name of device, eg. "stylus"
        @return true if successful
    */
    private native boolean openDevice(String name);
    /** Close device
    */
    private native void closeDevice();
    /** Waits for an event to occur and return the event number.
      * @return event number
    */
    private native int nextEvent();
    /** Queries the pointing device to see if it is in proximity
        @return true if the device is in proximity
    */
    public native boolean inProximity();
    /** Get pressure.
        @return pressure
    */
    public native int getPressure();
    /** Get X tilt.
        @return x tilt
    */
    public native int getTiltX();
    /** Get Y tilt.
        @return y tilt
    */
    public native int getTiltY();
    /** Get x pos.
        @return x pos
    */
    public native int getPosX();
    /** Get y pos.
        @return y pos
    */
    public native int getPosY();

    static
    {
        System.loadLibrary("jwacom");
    }
}
