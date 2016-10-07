/*
 ATtiny2313 cheapo intervalometer digital camera
 Author: Davis Whittaker
 Created: 12/3/2014
 Last Modified: 12/5/2014
 
 Uses output ports on Tiny2313 to simulate pressing the focus and shutter buttons
 at set intervals for time lapse photography.  Settings can be changed and viewed on LCD.
 
 PD0 is connected via transistor to set the camera focus before triggering shutter
 PD1 is connected via transistor to close the shutter contacts
 PD2 is connected to an LED to indicate when a photo is being taken.
 
 PORTB is used to drive the LCD.
 
*/
/* ============================ Tiny2313 Pin Config ===================
 
                          +--\/--+
    RESET Switch <-- PA2 -|1   20|- VCC --> 5vdc Power
        focusPin <-- PD0 -|2   19|- PB7 
      shutterPin <-- PD1 -|3   18|- PB6 --> LCD DB4
                     PA1 -|4   17|- PB5 --> LCD DB5
                     PA0 -|5   16|- PB4 --> LCD DB6
      shutterLED <-- PD2 -|6   15|- PB3 --> LCD DB7
      switchPin1 <-- PD3 -|7   14|- PB2 --> LCD E
      switchPin2 <-- PD4 -|8   13|- PB1 --> LCD RS
      switchPin3 <-- PD5 -|9   12|- PB0 --> LCD R/W
                <|-- GND -|10  11|- PD6 
                          +------+
*/

/* ==================== Includes and Defines ========================= */
#define F_CPU 1000000
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "lcd.c"
/* =========================== Activate for intervalometer ==================== */
#define focusPin PD0        // Focus trigger pin (focusPin) connected to PD1
#define shutterPin PD1      // Shutter trigger pin (shutterPin) connected to PD1
#define shutterLED PD2      // Shutter indicator LED (shutterLED) connected to PD2
#define switchPin1 PD3      // Tens Switch (switchPin1) connected to PD3 
#define switchPin2 PD4      // Ones Switch (switchPin2) connected to PD4
#define switchPin3 PD5      // Execute Switch (switchPin3) connected to PD5
#define DEBOUNCE_TIME 250   // Time to wait while "de-bouncing" button
#define LOCK_INPUT_TIME 250 // Time to wait after a button press

/* Counter variables for storing button presses */
int tensctr = 0;
int onesctr = 0;
int interval = 0;

//======== Function prototypes ====================
int sw_is_pressed(int);		/* Check and debounce tens button */
void setup(void);		/* Setup ports and pins for input, output, etc. */
void confirm(int);		/* Confirm interval based on buttons pressed */
void get_interval(void);	/* Set interval from button presses */
void focus(void);		/* Set initial focus */
void snapshots(int);   		/* take the photos */

/* ===================- setup() function -=============== */
void setup(void) {
  // ----------- Initialize input and camera ports and pins ----------
  PORTD |= ((1 << switchPin1) | (1 << switchPin2) | (1 << switchPin3));  //Enable pull-ups for input buttons
  DDRD |= ((1 << shutterLED)|(1 << shutterPin)|(1 << focusPin));  // Set LED, focus and shutter pins as outputs
}// End setup() Function

/* ==================== Main Function ========================= */
int main(void){
  setup();
  get_interval();

  return 0;  // Main function return - never happens
} // End main()

// This function is used to collect input from the various switches to set the photo interval.
void get_interval(){
  char buffer1[7]; // buffer to hold converted integer
  
  /* initialize display, cursor off */
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();     /* clear display home cursor */
  lcd_command(LCD_DISP_ON); // turn off cursor
  lcd_puts( "Set Interval:\n" );
 
  while(1){
    // Test for button1 (tens) press here
    if (sw_is_pressed(switchPin1)) {		//Check if "tens" button has been pressed
        tensctr++;				// Count number of times pressed
        _delay_ms(LOCK_INPUT_TIME);
    }
    // Test for button2 (ones) press here
    if (sw_is_pressed(switchPin2)) {    	// Check if "ones" button has been pressed
        onesctr++;				// Count number of times pressed
        _delay_ms(LOCK_INPUT_TIME);      
    }
    interval = (10*tensctr)+onesctr; 		// calculate interval from button presses registered
    // now convert to char and display interval on LCD
    itoa(interval, buffer1, 10);
    lcd_gotoxy(0,1);
    lcd_puts(buffer1); lcd_puts(" seconds");
    // Test for button3 (execute) press here
    if(sw_is_pressed(switchPin3)){  		// Check if Execute button has been pressed
        confirm(interval);            		// Go to Confirm routine
    }  
  }
} // End get_interval()

/* Confirm settings before starting to snap photos */
void confirm(interval){ 
 
  char buffer[7]; // buffer to hold converted interval integer for output to LCD
     
  /* initialize display, cursor off */
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();     /* clear display home cursor */
  lcd_command(LCD_DISP_ON); // turn off cursor
  // convert int to ascii for LCD display and then display message
  itoa(interval, buffer, 10);
  lcd_puts( "Interval: " ); lcd_puts(buffer); lcd_puts( "s" );
  lcd_gotoxy(0,1);
  lcd_puts( "Press to start" );
// Check for execute button to start snapping photos
  while(1){
    if(sw_is_pressed(switchPin3)){  // Check if Execute button has been pressed
      snapshots(interval);	// Go to the photo taking function
    }
  }
} // End confirm()

/* ===================== Switch debounce function 1 =========== */
int sw_is_pressed(int switchPin) {
  /* the button is pressed when BUTTON_BIT is clear */
  if (bit_is_clear(PIND, switchPin)){
      _delay_ms(DEBOUNCE_TIME);
      if (bit_is_clear(PIND, switchPin)) return 1;
      }
  return 0;
} // End sw_is_pressed()

/* =========================== focus() ========================== */
void focus(void){
  _delay_ms(2000); 		// Delay 2 secs before starting
  PORTD |= (1<<focusPin); 	// Turn on - set focus
  _delay_ms(2000); 		// delay to make sure focus is set
  PORTD &= ~(1<<focusPin); 	// Resets PORTB (focus) pins to OFF		
} // End focus()

/* =========================== Take the snapshots ========================== */
void snapshots(int interval){
    
  int i;    // Counting variable for interval delay loop	
  long frames=0;
  char fbuffer[12];
  char ibuffer[7];

  itoa(interval, ibuffer, 10);  // convert interval to ascii for LCD
  // setup LCD to display frame count
  lcd_clrscr();     /* clear display home cursor */
  lcd_command(LCD_DISP_ON); // turn off cursor
  /* put string from program memory to display */
  lcd_puts("Interval: "); lcd_puts(ibuffer); lcd_puts(" secs\n");
  lcd_gotoxy(0,1);
  lcd_puts("Snaps: " );  
  focus();
	
  while(1) { // This is the photo snapping loop
    PORTD |= (1<<shutterPin)|(1<<shutterLED); // Turn on - snap photo and show LED indication
    _delay_ms(1000); // Delay to hold shutter pin high
    PORTD &= ~((1 << shutterPin)|(1<<shutterLED)); // Resets PORTD shutter pins to OFF
    frames++; // Increment frame number
    // Display frame count
    itoa(frames, fbuffer, 10);
    lcd_gotoxy(7,1);
    lcd_puts(fbuffer);	
		
    for(i=0;i<interval;i++) // Count out the interval in seconds delay
      {
      _delay_ms(1000); // One second delay for each i in interval
      }//for		
  }//while
} // End snapshots()
