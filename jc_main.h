/*****************************************************************************/
/*
 * By Jonathan Crossley
 * Version 0.1 - 29 Jun 2019
 *          1.0 - 20 Oct 2019
                - Converted to C++
 *
 * jc_main.h - Header file for jc_main, includes declarations for functions,
 *      defines, and global variables.
 */


// Includes for main
#include <string>
#include <iostream>

//// ARE ANY OF THESE NEEDED FOR C++
// Standard libraries
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
//#include <pthread.h>

//#include <string>
//#include <iostream>

//// ARE ANY OF THESE NEEDED FOR C++
// Libraries for connecting to CAN socket
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>

//// ARE ANY OF THESE NEEDED FOR C++
#include <net/if.h>

// Libraries for linux-can-utils
#include <linux/can.h>
#include <linux/can/raw.h>

// Library for sending CAN messages
//#include "lib.h"

// Library for GPIO control
#include "wiringPi.h"

// Library for controlling the terminal curser
#include <ncurses.h>

// Header files to be included
#include "jc_GPIO.h"


using namespace std;

// Set up escape sequences for cursor and screen control
#if 0
#define CLR_SCREEN		"\33[2J"
#define CLR_LINE		"\33[2K"
#define CSR_HIDE  		"\33[?25l"
#define CSR_SHOW  		"\33[?25h"
#define CSR_HOME  		"\33[H"
#define CSR_NEW_HOME	"\33"
#endif

// Define the column where the updating text starts
#define DATA_COLM   23

// Set Demo & OBD mode variable whether running in demo or OBD mode or not
#define DEMO_MODE   0   // True
#define OBD_MODE    0   // False





int main()
{
    s = socketHandler;
    d = displayHandler;
    Message messageHandlers[1];
    messageHandlers[0] = new EngineMessages();
    while(True){
        string message = s.getMessage();
        string type = s.getType();
        for(int i = 0; i < count(messageHandlers); i++){
            if(messageHandlers[i].handlesType(type)){
                messageHandlers[i].Process(message);
                break;
            }
        }
        
        d.update(messageHandlers);
    }



// Set up data structure for the converted CANbus values
struct can_data_conv
{
    // TEST //
    int n;
    bool increase;
    // TEST //

    float throttle_conv;
    bool throttle_flag;
    float throttle_comp_conv;
    
    int rpm_conv;
    bool rpm_flag;
    
    bool vtec_flag;
    bool exhaust_flag;
    
    int speedo_conv;
    float speed_conv;
    //char *gear_estimation;
    string gear_estimation;
    float gear_ratio_est;
    bool reverse_status;
    
    bool clutch_status;
    bool brake_status;
    
    /////////////////////////////////////////////////////////////////////
    bool cruise_cont_active_flag;
    bool cruise_main_flag;
    float cruise_target_speed;
    float cruise_target_diff;

    bool cruise_cancel_flag;
    ///////////////////////////////////////////////////////////////////
    
    int intake_temp_conv;
    int water_temp_conv;
    
    float fuel_level_conv;
    float fuel_consump_conv;

    bool ac_comp_flag;

    bool right_turn_flag;
    bool left_turn_flag;
    bool wiper_flag;
    //char *headlight_status;
    string headlight_status;
    
    //char *doors_status;
    string doors_status;
    //char *seatbelt_status;
    string seatbelt_status;
};

//// TILL THREADS ARE ADDED
// Global variable used for stopping program and both threads
//bool g_quit = false;

// Function prototypes
#if DEMO_MODE
    static void demo_test(struct can_data_conv *data_conv);
#endif

#if OBD_MODE
    static void OBD_send();
#endif


static int setup_CAN_communication(int *s);
static void jc_LED_enable(int wiringPi_pin, bool flag);
//// FOR WHEN PNUMATICS ARE ADDED
//static void jc_exhaust_cutout_enable(int wiringPi_pin, bool flag);
void text_dash_setup(void);
//// TILL THREADS ARE ADDED
//static void *print_text_dash_thread(void *data_conv);
static void print_dash(struct can_data_conv *data_conv);
static void gear_calc(struct can_data_conv *data_conv);
static void can_convert_17C(struct can_data_conv *data_conv, struct can_frame *frame);
static void can_convert_1A6(struct can_data_conv *data_conv, struct can_frame *frame);
static void can_convert_13C(struct can_data_conv *data_conv, struct can_frame *frame);
static void can_convert_294(struct can_data_conv *data_conv, struct can_frame *frame);
static void can_convert_405(struct can_data_conv *data_conv, struct can_frame *frame);
//static void cruise_control_buttons(struct can_data_conv *data_conv, struct can_frame *frame);
//static void cruise_control(struct can_data_conv *data_conv);
//static float cruise_control(struct can_data_conv *data_conv, struct can_frame *frame);