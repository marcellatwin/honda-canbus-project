/*****************************************************************************/
/*
 * By Jonathan Crossley
 * Version 0.1 - 29 Jun 2019
 *
 * jc_main.h - Header file for jc_main, includes declarations for functions,
 *      defines, and global variables.
 */

#include <string>
#include <iostream>

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

// Set up the WiringPi pin numbers for the GPIO pins
#define PICAN_LED_RED   3   // GPIO 22 - PiCan2 Red LED
#define LED_RED         4   // GPIO 23 - Red LED on Breadboard
#define LED_GREEN       29  // GPIO 21 - Green LED on Breadboard
#define LED_YELLOW      25  // GPIO 26 - Yellow LED on Breadboard
//// FOR WHEN PNUMATICS ARE ADDED
//#define EXHAUST_CUTOUT_PIN      ??  // GPIO ?? - ?????

// Set Demo & OBD mode variable whether running in demo or OBD mode or not
#define DEMO_MODE   1   // True
#define OBD_MODE    0   // False

// Set up class for converted CAN bus values
/*
class can_data_conv
{
public:
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
    string headlight_status;
    
    string doors_status;
    string seatbelt_status;
};
*/

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

static void setup_GPIO(void);
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