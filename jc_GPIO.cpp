/*****************************************************************************/
/* 
 * jc_GPIO.c
 * By Jonathan Crossley
 * Version 0.1 - 20 Oct 2019
 *
 */

/*****************************************************************************/
void setup_GPIO(void)
{
    // Initialize wiringPi program
    wiringPiSetup();
    
    // Initilize pins and set modes
    pinMode(PICAN_LED_RED, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);

    //// FOR WHEN PNUMATICS ARE ADDED
    //pinMode(EXHAUST_CUTOUT_PIN, OUTPUT);
}