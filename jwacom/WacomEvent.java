/**
   JWacom 1.0, Jan 2004.

   JWacom event class.
   2004 Copyright (C) Tue Haste Andersen

   @author Tue Haste Andersen <haste@diku.dk>
*/

package jwacom;

public class WacomEvent
{
    private boolean bInProximity;
    private boolean bInFocus;
    private int iPosX, iPosY, iTiltX, iTiltY, iPressure, iButton;

    /** Creates a Wacom proximity event.
      *
      * @param bInProximity is true if the pointer is in proximity
      */
    public WacomEvent(boolean bInProximity, boolean bInFocus,
                      int iPosX, int iPosY, int iTiltX, int iTiltY, int iPressure,
                      int iButton)
    {
        this.bInProximity = bInProximity;
        this.bInFocus = bInFocus;
        this.iPosX = iPosX;
        this.iPosY = iPosY;
        this.iTiltX = iTiltX;
        this.iTiltY = iTiltY;
        this.iPressure = iPressure;
        this.iButton = iButton;
    }

    /** Returns true if the device is in proximity.
     *
     * @return true if the device is in proximity
     */
    public boolean inProximity()
    {
        return bInProximity;
    }

    public boolean inFocus()
    {
        return bInFocus;
    }

    public int posX()
    {
        return iPosX;
    }

    public int posY()
    {
        return iPosY;
    }

    public int tiltX()
    {
        return iTiltX;
    }

    public int tiltY()
    {
        return iTiltY;
    }

    public int pressure()
    {
        return iPressure;
    }

    public int button()
    {
        return iButton;
    }
}
