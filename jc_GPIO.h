/*****************************************************************************/
/* 
 * jc_GPIO.h
 * By Jonathan Crossley
 * Version 0.1 - 20 Oct 2019
 *
 */

//// GPIOs used by PiCAN2 and future PiCANDuo should be listed here for referance


// Set up the WiringPi pin numbers for the GPIO pins
#define PICAN_LED_RED   3   // GPIO 22 - PiCan2 Red LED
#define LED_RED         4   // GPIO 23 - Red LED on Breadboard
#define LED_GREEN       29  // GPIO 21 - Green LED on Breadboard
#define LED_YELLOW      25  // GPIO 26 - Yellow LED on Breadboard
//// FOR WHEN PNUMATICS ARE ADDED
//#define EXHAUST_CUTOUT_PIN      ??  // GPIO ?? - ?????

// Function prototypes
static void setup_GPIO(void);

