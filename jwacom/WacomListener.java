/**
   JWacom 1.0, Jan 2004.

   JWacom listener interface.
   2004 Copyright (C) Tue Haste Andersen

   @author Tue Haste Andersen <haste@diku.dk>
*/

package jwacom;

import java.util.EventListener;

public interface WacomListener extends EventListener
{
    // event dispatch methods
    void proximity(WacomEvent e);
    void focus(WacomEvent e);
    void motion(WacomEvent e);
    void buttonDown(WacomEvent e);
    void buttonUp(WacomEvent e);
}
