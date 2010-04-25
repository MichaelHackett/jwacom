/**
   JWacom 1.0, Jan 2004.

   JWacom test program.
   2004 Copyright (C) Tue Haste Andersen

   @author Tue Haste Andersen <haste@diku.dk>
*/

import java.awt.*;
import java.lang.*;
import javax.swing.*;
import jwacom.*;

public class JWacomTest extends JPanel
{
    private Wacom wacom;

    public JWacomTest()
    {
        // Setup window
        this.setPreferredSize(new Dimension(300, 300));

        // Open wacom device
        wacom = new Wacom("stylus");

        // Add listeners
        wacom.addWacomListener(new WacomListener()
        {
            public void proximity(WacomEvent e)
            {
                if (e.inProximity())
                    System.out.println("in proximity");
                else
                    System.out.println("out of proximity");
            }

            public void focus(WacomEvent e)
            {
                if (e.inFocus())
                    System.out.println("in focus");
                else
                    System.out.println("out of focus");
            }

            public void motion(WacomEvent e)
            {
                System.out.println("x " + e.posX() + ", y " + e.posY() + ", pressure " + e.pressure() + ", tilt x " + e.tiltX() + ", tilt y " +e.tiltY());
            }

            public void buttonDown(WacomEvent e)
            {
                System.out.println("button " + e.button() + " pressed");
            }

            public void buttonUp(WacomEvent e)
            {
                System.out.println("button " + e.button() + " released");
            }
        });
    }

    public static void main(String[] args)
    {
        JFrame frame = new JFrame();
        JWacomTest test = new JWacomTest();
        Container content = frame.getContentPane();
        content.setLayout(new BoxLayout(content, BoxLayout.X_AXIS));
        content.add(test);

        frame.setTitle("Wacom test");
        frame.pack();

        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }
}
