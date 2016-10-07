# Tiny2313 Intervalometer
Tiny2313 AVR Intervalometer for Canon T3i Digital Rebel camera

Created by Davis Whittaker on 12/3/2014, last modified 12/5/2014.
 
Uses output ports on Tiny2313 AVR to simulate pressing the focus and shutter buttons at set intervals for time lapse photography.  An 2 x 16 LCD is also included in the project where settings can be viewed.

# Operation
This provides an interface with a 4 x 16 LCD and 4 buttons to set things up.

Button 1 (Reset_sw) is a the reset button.  This simply resets the processor and provides the initial menu on the LCD.  Use this to reset the device or clear it if you make a mistake setting the timer.

Button 2 (SwitchPin1) is the "10s" button.  Each press of the button adds 10 seconds to the interval timer.

Button 3 (SwitchPin2) is the "1s" button.  Each press of the button adds 1 second to the interval timer.

Button 4 (SwitchPin3)is the Confirm/Execute button.  Pressing it confirms the timer setting and then allows you to press it again to start the interval timer.  It will then trigger the camera to snap a photo every time the timer interval is reached.  It will continue indefinitely until stopped by the user.  I did not add the ability (yet) to set a total number of photos or aggregate time interval to halt after.

The LCD displays the number of photos taken after each snap so you can keep track.  To stop the process, press the reset switch.
