/*****************************************************************************/
/* 
 * jc_main.c
 * By Jonathan Crossley
 * Version 0.1 - 7 Aug 2018
 *         0.2 - 29 Aug 2018
 *		   0.3 - 1 Sept 2018
 *         1.0 - 22 May 2019
 *              Started adding threads during my time at Volvo, program 
 *              became broken, couldn't find fix, step away from project for
 *              a while.
 *         2.0 - 9 Oct 2019
 *              Starting project back up again, also combining with C++
 *              school project, so will convert some code to C++
 */

// Standard libraries
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

// Libraries for connecting to CAN socket
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <net/if.h>

// Libraries for linux-can-utils
#include <linux/can.h>
#include <linux/can/raw.h>

// Library for sending CAN messages
//#include "lib.h"

// Library for GPIO control
#include "wiringPi.h"

// Library for controlling the terminal curser
#include <curses.h>

// Library with declarations for functions, defines, & global variables
#include "jc_main.h"

/*****************************************************************************/
int main(void)
{
	
	// Initilize flags and other variables
    //// DOES THIS NEED TO HAPPEN, ALL THE FALSES???
    struct can_data_conv data_conv = {
        .throttle_flag = false,
        .rpm_flag = false,
        .vtec_flag = false,
        .exhaust_flag = false,
        .reverse_status = false
    };
    
    // Set up GPIO pins
	setup_GPIO();

    // Set up CAN communication
    int s;
    struct can_frame frame;
    if (setup_CAN_communication(&s) == -1) {
        printf("Error setting up CAN communication. Try again.\n");
        
        // TEST //
        printf("TEST - main exiting\n");
        // TEST //
        
        exit(0);
    }

    // Initialize curses variables
    initscr();
    nodelay(stdscr, TRUE);
    clear();
    curs_set(0);


    // Setup text based dashboard's static text
    text_dash_setup();

    // Setup and start the other thread to print the dash
    pthread_t print_text_dash_thread_id;
    int thread_check = pthread_create(&print_text_dash_thread_id, NULL, print_text_dash_thread, (void *)&data_conv);
    
    if (thread_check != 0) {
        printf("Error creating other thread, Line %d, %s\n", __LINE__, strerror(thread_check));
                
        // TEST //
        printf("TEST - main exiting\n");
        // TEST //

        exit(0);
    }

    // TEST //
    data_conv.n = 0;
    data_conv.increase = true;
    // TEST //

    // Loop until "q" is pressed
	while (!g_quit) {
    
    // Determind if in Demo mode
#if DEMO_MODE
        demo_test(&data_conv);

#else
		if(read(s, &frame, sizeof(struct can_frame)) < 0) {
            printf(" Hello!\n");
            perror("Error reading CAN bus address");
            g_quit = true;
            pthread_join(print_text_dash_thread_id, NULL);
            break;
        }

		if (frame.can_id == 0x17c) {
			can_convert_17C(&data_conv, &frame);
		}

		if (frame.can_id == 0x1dc) {
			data_conv.rpm_conv = ((frame.data[1] << 8) + (frame.data[2]));
			if (data_conv.rpm_conv > 5000) {
				data_conv.rpm_flag = true;
			}
			else if (data_conv.rpm_conv < 4800) {
				data_conv.rpm_flag = false;
			}
		}

		if (frame.can_id == 0x320) {
			data_conv.vtec_flag = true;
		}
		else {
			data_conv.vtec_flag = false;
		}
#endif

		if (data_conv.throttle_flag && data_conv.rpm_flag && data_conv.vtec_flag) {
			data_conv.exhaust_flag = true;
		}
		else {
			data_conv.exhaust_flag = false;
		}

		jc_LED_enable(PICAN_LED_RED, data_conv.exhaust_flag);
		//jc_exhaust_cutout_enable(EXHAUST_CUTOUT_PIN, data_conv.exhaust_flag);

#if !DEMO_MODE
		if (frame.can_id == 0x158) {
			data_conv.speed_conv = ((frame.data[0] << 8) + (frame.data[1])) / 160;
		}

		if (frame.can_id == 0x1A6) {
			can_convert_1A6(&data_conv, &frame);
            //cruise_control_buttons(&data_conv, &frame);
		}

		if (frame.can_id == 0x13c) {
			can_convert_13C(&data_conv, &frame);
		}

		if (frame.can_id == 0x294) {
			can_convert_294(&data_conv, &frame);
		}

		if (frame.can_id == 0x309) {
			data_conv.speedo_conv = (frame.data[3]);
		}

		// Fix, find scaling, educated guess right now
		if (frame.can_id == 0x324) {
			data_conv.water_temp_conv = frame.data[0] - 40;		// Think it's in C
			data_conv.intake_temp_conv = frame.data[1] - 40;	    // Think it's in C
		}

		// Fix, find scaling, educated guess right now
		if (frame.can_id == 0x377) {
			data_conv.fuel_consump_conv = frame.data[0] / 1.7;	// Think it's in MPG
		}

		// Fix, find scaling, educated guess right now
		if (frame.can_id == 0x378) {
			data_conv.fuel_level_conv = ((frame.data[4] << 8) + (frame.data[5])) / 1000;	// Think it's in gals
		}
        
        if (frame.can_id == 0x405) {
            can_convert_405(&data_conv, &frame);
        }
        
        //cruise_control(&data_conv);
#endif

        // Determind wheather to keep running or not
		if (getch() == 'q') {
			g_quit = true;
            pthread_join(print_text_dash_thread_id, NULL);
            break;
		}
	}

    // Reset screen and show the curser
	endwin();
    curs_set(1);

    // TEST //
    printf("TEST - main exiting\n");
    // TEST //

    return 0;
}

#if DEMO_MODE
/*****************************************************************************/
void demo_test(struct can_data_conv *data_conv)
{
    // TEST //
    if (data_conv->increase) {
        data_conv->rpm_conv = data_conv->n++;
        data_conv->throttle_flag = true;
    }
    else {
        data_conv->rpm_conv = data_conv->n--;
        data_conv->throttle_flag = false;
    }
    if (data_conv->n > 8000) {
        data_conv->increase = false;
    }
    else if (data_conv->n < 0) {
        data_conv->increase = true;
    }
    if (data_conv->n > 5000) {
        data_conv->vtec_flag = true;
        data_conv->rpm_flag = true;
    }
    else {
        data_conv->vtec_flag = false;
        data_conv->rpm_flag = false;    
    }
    // TEST //
}
#endif

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
    //pinMode(EXHAUST_CUTOUT_PIN, OUTPUT);
}

/*****************************************************************************/
int setup_CAN_communication(int *s)
{
    // Set up variables to connect to CAN bus
    struct sockaddr_can addr;
    struct ifreq ifr;

    /*
    //// For when PiCanDuo comes into play ////
    idx = idx2dindex(addr.can_ifindex, s[i]);
    devname[idx]
    strcpy(devname[i], ifr.ifr_name);
    static char devname[MAXIFNAMES][IFNAMSIZ+1];
    */

    // Get socket address
    *s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (*s == -1) {
        perror("Error creating endpoint for socket communication");
        return -1;
    }

    // Copy some addresses and names into the CAN structs
    strcpy(ifr.ifr_name, "can0");
    addr.can_family = AF_CAN;
    
    // Do something with control device
    if (ioctl(*s, SIOCGIFINDEX, &ifr) == -1) {
        perror("Error with control device (SIOCGIFINDEX)");
        return -1;
    }
    
    // Do some more with structs and something with the CPU socket
    addr.can_ifindex = ifr.ifr_ifindex;
    setsockopt(*s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    // Combine it all together into "s" for the final socket address
    if (bind(*s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        //printf("%d\n",errno);
        perror("Error with binding socket address");
        return -1;
    }

    // Return success
    return 0;
}

/*****************************************************************************/
void jc_LED_enable(int wiringPi_pin, bool flag)
{    
	// Turn on/off LED
	if (flag == true) {
		digitalWrite(wiringPi_pin, HIGH);
	}
	else {
		digitalWrite(wiringPi_pin, LOW);
	}
}

/*****************************************************************************/
/*
void jc_exhaust_cutout_enable(int wiringPi_pin, bool flag)
{
	// Open/Close Exhaust Cutout valve
    if (flag == true) {
        digitalWrite(wiringPi_pin, HIGH);
    }
    else {
        digitalWrite(wiringPi_pin, LOW);
    }
}
*/

/*****************************************************************************/
void text_dash_setup(void)
{
    int row = 1;

    mvprintw(row++, 1, "Thottle (%%):");
    mvprintw(row++, 1, "Thottle flag:");
    mvprintw(row++, 1, "Thottle Comp:");
    row++;

    mvprintw(row++, 1, "RPM:");
    mvprintw(row++, 1, "RPM flag:");
    row++;

    mvprintw(row++, 1, "VTEC:");
    mvprintw(row++, 1, "Exhaust Cutout:");
    row++;

    mvprintw(row++, 1, "Speedo (MPH):");
    mvprintw(row++, 1, "Speed (MPH):");
    mvprintw(row++, 1, "Gear (Est):");
    mvprintw(row++, 1, "Gear Ratio (Est):");
    row++;

    mvprintw(row++, 1, "Clutch:");
    mvprintw(row++, 1, "Brake:");
    row++;

    mvprintw(row++, 1, "Cruise Control:");
    mvprintw(row++, 1, "Cruise Control Main:");
    mvprintw(row++, 1, "Cruise Target:");
    mvprintw(row++, 1, "Cruise Target Diff:");
    row++;

    mvprintw(row++, 1, "Intake Temp (F):");
    mvprintw(row++, 1, "Water Temp (F):");
    row++;

    mvprintw(row++, 1, "MPG:");
    mvprintw(row++, 1, "Fuel Level (gals):");
    row++;

    mvprintw(row++, 1, "AC Compressor:");
    row++;

    mvprintw(row++, 1, "Headlights:");
    mvprintw(row++, 1, "Right Turn Signal:");
    mvprintw(row++, 1, "Left Turn Signal:");
    mvprintw(row++, 1, "Wipers:");
    row++;

    mvprintw(row++, 1, "Doors or Trunk:");
    mvprintw(row++, 1, "Seatbelts (Any):");
    //row++;
}

/*****************************************************************************/
void *print_text_dash_thread(void *data_conv)
//void print_dash(struct can_data_conv *data_conv)
{
    int row;

    struct can_data_conv *data = (struct can_data_conv *)data_conv;

    while (!g_quit)
    {
        row = 1;

        mvprintw(row++, DATA_COLM, "%.1f", data->throttle_conv);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->throttle_flag ? "True" : "False");
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%.2f", data->throttle_comp_conv);
        clrtoeol();
        row++;
        
        mvprintw(row++, DATA_COLM, "%d", data->rpm_conv);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->rpm_flag ? "True" : "False");
        clrtoeol();
        row++;
        
        mvprintw(row++, DATA_COLM, "%s", data->vtec_flag ? "Just Kicked In" : "Off");
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->exhaust_flag ? "Open" : "Off");
        clrtoeol();
        row++;
        
        mvprintw(row++, DATA_COLM, "%d", data->speedo_conv);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%.2f", data->speed_conv);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->gear_estimation);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%.5f", data->gear_estimation);
        clrtoeol();
        row++;
        
        mvprintw(row++, DATA_COLM, "%s", data->clutch_status ? "In" : "Out");
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->brake_status ? "On" : "Off");
        clrtoeol();
        row++;
        
        mvprintw(row++, DATA_COLM, "%s", data->cruise_cont_active_flag ? "On" : "Off");
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->cruise_main_flag ? "On" : "Off");
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%.2f", data->cruise_target_speed);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%.2f", data->cruise_target_diff);
        clrtoeol();
        row++;

        mvprintw(row++, DATA_COLM, "%d", data->intake_temp_conv);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%d", data->water_temp_conv);
        clrtoeol();
        row++;
        
        mvprintw(row++, DATA_COLM, "%.2f", data->fuel_consump_conv);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%.2f", data->fuel_level_conv);
        clrtoeol();
        row++;

        mvprintw(row++, DATA_COLM, "%s", data->ac_comp_flag ? "On" : "Off");
        clrtoeol();
        row++;

        mvprintw(row++, DATA_COLM, "%s", data->headlight_status);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->right_turn_flag ? "On" : "Off");
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->left_turn_flag ? "On" : "Off");
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->wiper_flag ? "On" : "Off");
        clrtoeol();
        row++;
        
        mvprintw(row++, DATA_COLM, "%s", data->doors_status);
        clrtoeol();
        mvprintw(row++, DATA_COLM, "%s", data->seatbelt_status);
        clrtoeol();
        //row++;   
    }

    // TEST //
    printf("TEST - thread exiting\n");
    // TEST //

    return NULL;
}

/*****************************************************************************/
void gear_calc(struct can_data_conv *data_conv)
{
	//  Gear Ratios			    Ratios
	// Final:	4.76		 Gear * Final
	// 1st:		3.267		1st:	15.5509
	// 2nd:		2.040		2nd:	9.7104
	// 3rd:		1.429		3rd:	6.80204
	// 4th:		1.073		4th:	5.1075
	// 5th:		0.830		5th:	3.9508
	// 6th:		0.647		6th:	3.07972

	float tire_diameter = 25.1;	// 25.1" or 637.182 mm
								// Based on 225/40R18 tires
	float tire_circum = 3.141592 * tire_diameter;
	float gear_ratio_calc = (tire_circum * data_conv->rpm_conv * 0.00094697) / data_conv->speed_conv;	// Convert in/min to mph
	float error_pos = 1.1;
	float error_neg = 0.9;
    data_conv->gear_ratio_est = gear_ratio_calc;

	if ((15.5509 * error_neg) < gear_ratio_calc && gear_ratio_calc < (15.5509 * error_pos)) {
		data_conv->gear_estimation = "1st - 15.5509";
	}
	else if ((9.7104 * error_neg) < gear_ratio_calc && gear_ratio_calc < (9.7104 * error_pos)) {
		data_conv->gear_estimation = "2nd - 9.7104";
	}
	else if ((6.80204 * error_neg) < gear_ratio_calc && gear_ratio_calc < (6.80204 * error_pos)) {
		data_conv->gear_estimation = "3rd - 6.80204";
	}
	else if ((5.1075 * error_neg) < gear_ratio_calc && gear_ratio_calc < (5.1075 * error_pos)) {
		data_conv->gear_estimation = "4th - 5.1075";
	}
	else if ((3.9508 * error_neg) < gear_ratio_calc && gear_ratio_calc < (3.9508 * error_pos)) {
		data_conv->gear_estimation = "5th - 3.9508";
	}
	else if ((3.07972 * error_neg) < gear_ratio_calc && gear_ratio_calc < (3.07972 * error_pos)) {
		data_conv->gear_estimation = "6th - 3.07972";
	}
	else {
		data_conv->gear_estimation = "Neutral";
	}
}

/*****************************************************************************/
void can_convert_17C(struct can_data_conv *data_conv, struct can_frame *frame)
{
	data_conv->throttle_conv = ((frame->data[0] << 8) + (frame->data[1])) / 654;

	if (data_conv->throttle_conv > 50) {
		data_conv->throttle_flag = true;
	}
	else if (data_conv->throttle_conv < 45) {
		data_conv->throttle_flag = false;
	}

	if (frame->data[4] == 0x48) {
		data_conv->cruise_cont_active_flag = true;
	}
	else {
		data_conv->cruise_cont_active_flag = false;
	}

	if (frame->data[6] == 0x20) {
		data_conv->brake_status = true;
	}
	else {
		data_conv->brake_status = false;
	}
    //// IN FUTURE
	//throttle = cruise_control(data_conv, frame);
}

/*****************************************************************************/
void can_convert_1A6(struct can_data_conv *data_conv, struct can_frame *frame)
{
	if ((frame->data[2] & 0x0F) == 0x04) {
		data_conv->reverse_status = true;
	}
	else {
		data_conv->reverse_status = false;
	}

	if ((frame->data[2] >> 4) == 0x04) {
		data_conv->ac_comp_flag = true;
	}
	else {
		data_conv->ac_comp_flag = false;
	}

	if ((frame->data[0] & 0x0F) == 0x00 || (frame->data[0] & 0x0F) == 0x04) {
		data_conv->headlight_status = "Off";
	}
	else if ((frame->data[0] & 0x0F) == 0x01 || (frame->data[0] & 0x0F) == 0x05) {
		data_conv->headlight_status = "Parking";
	}
	else if ((frame->data[0] & 0x0F) == 0x02 || (frame->data[0] & 0x0F) == 0x06) {
		data_conv->headlight_status = "LOW Beams";
	}
	else if ((frame->data[0] & 0x0F) == 0x03 || (frame->data[0] & 0x0F) == 0x07) {
		data_conv->headlight_status = "HIGH Beams";
	}
	else {
		data_conv->headlight_status = "Not sure...";
	}
}

/*****************************************************************************/
void can_convert_13C(struct can_data_conv *data_conv, struct can_frame *frame)
{
	if (data_conv->reverse_status == true) {
		data_conv->gear_estimation = "Reverse";
	}
	else {
		if (frame->data[6] == 0x04) {
			data_conv->clutch_status = false;
			if (data_conv->speed_conv > 0) {
				//data_conv.gear_estimation = gear_calc(speed_conv, rpm_conv);
                gear_calc(data_conv);
			}
			else {
				data_conv->gear_estimation = "Neutral";
			}
		}
		else {
			data_conv->clutch_status = true;
			data_conv->gear_estimation = "Clutch";
		}
	}

	data_conv->throttle_comp_conv = frame->data[1];	//// FIX, figure out scaling
}

/*****************************************************************************/
void can_convert_294(struct can_data_conv *data_conv, struct can_frame *frame)
{
	if ((frame->data[0] >> 4) == 0x04) {
		data_conv->right_turn_flag = true;
	}
	else {
		data_conv->right_turn_flag = false;
	}

	if ((frame->data[0] >> 4) == 0x02) {
		data_conv->left_turn_flag = true;
	}
	else {
		data_conv->left_turn_flag = false;
	}

	if ((frame->data[0] & 0x0F) == 0x0C) {
		data_conv->wiper_flag = true;
	}
	else {
		data_conv->wiper_flag = false;
	}
}

/*****************************************************************************/
void can_convert_405(struct can_data_conv *data_conv, struct can_frame *frame)
{
    if ((frame->data[1] & 0x0F) == 0x01) {
        data_conv->doors_status = "Closed";
    }
    else if ((frame->data[1] & 0x0F) == 0x02) {
        data_conv->doors_status = "Open";
    }
    else {
        data_conv->doors_status = "Don't Know";
    }
    
    if ((frame->data[2]) == 0x20) {
        data_conv->doors_status = "Buckled";
    }
    else if ((frame->data[2]) == 0x24) {
        data_conv->doors_status = "Unbuckled";
    }
    else {
        data_conv->doors_status = "Don't Know";
    }
}


#if 0
/*****************************************************************************/
void cruise_control_buttons(struct can_data_conv *data_conv, struct can_frame *frame)
{
    // Setting the cruise control speed
    if (data_conv->cruise_cont_active_flag == true && (frame->data[0] >> 4) == 0x06 && data_conv->cruise_main_flag == false) {
        data_conv->cruise_target_speed = data_conv->speed_conv;
    }
    // Deaccelerate Button
    if (data_conv->cruise_cont_active_flag == true && (frame->data[0] >> 4) == 0x06 && data_conv->cruise_main_flag == true) {
        ///////////////////////////////////////////  IS THIS CORRECT???
        data_conv->cruise_target_speed -= 1.0;
    }
    // Accelerate Button
    if (data_conv->cruise_cont_active_flag == true && (frame->data[0] >> 4) == 0x08 && data_conv->cruise_main_flag == true) {
        data_conv->cruise_target_speed += 1.0;
    }
    // Cancel Button
    if (data_conv->cruise_cont_active_flag == true && data_conv->cruise_target_speed > 1.0 && data_conv->cruise_main_flag == false) {
        data_conv->cruise_target_speed = 0.0;
    }
    // Reset Button
    if (data_conv->cruise_cont_active_flag == true && data_conv->cruise_target_speed > 1.0 && (frame->data[0] >> 4) == 0x08 && data_conv->cruise_main_flag == false) {
        data_conv->cruise_target_speed = 0.0;
    }
    
    // When cruise control turned off, set target and diff speeds to 0
    if (data_conv->cruise_cont_active_flag == false) {
        data_conv->cruise_target_diff = 0.0;
        data_conv->cruise_target_speed = 0.0;
    }
    // If Cruise control on and active, calculate diff speed
    else if (data_conv->cruise_cont_active_flag == true && data_conv->cruise_main_flag == true) {
        data_conv->cruise_target_diff = data_conv->cruise_target_speed - data_conv->speed_conv;
    }
    else if (data_conv->cruise_cont_active_flag == true && data_conv->cruise_main_flag == false) {
        data_conv->cruise_target_diff = 0.0;
    }




#if 0
    if (data_conv->cruise_cont_active_flag == true) {
        if ((frame->data[0] >> 4) == 0x02) {
            data_conv->cruise_main_flag = false;
            data_conv->cruise_cont_active_flag = false;
            data_conv->cruise_target_speed = 0;
            data_conv->cruise_target_diff = 0;
        }
        if ((frame->data[0] >> 4) == 0x04) {
            data_conv->cruise_cont_active_flag = false;
        }  
    }
    else {
        if ((frame->data[0] >> 4) == 0x02) {

        }
    }   
/*****************************************/
    if (data_conv->cruise_cont_active_flag == false && data_conv->cruise_main_flag == true && (frame->data[0] >> 4) == 0x06) {
        data_conv->cruise_target_speed = data_conv->speed_conv;
    }
    else if (data_conv->cruise_cont_active_flag == true && (frame->data[0] >> 4) == 0x06) {
        data_conv->cruise_target_speed -= 1.0;
    }
    else if (data_conv->cruise_cont_active_flag == true && (frame->data[0] >> 4) == 0x08) {
        data_conv->cruise_target_speed += 1.0;
    }
    
	if ((frame->data[0] >> 4) == 0x02) {
		if (data_conv->cruise_main_flag == true) {
			data_conv->cruise_main_flag = false;
		}
		else {
			data_conv->cruise_main_flag = true;
		}
	}
	else if ((frame->data[0] >> 4) == 0x04) {
		cruise_cancel_flag = true;
	}
#endif
}

/*****************************************************************************/
void cruise_control(struct can_data_conv *data_conv)
//float cruise_control(struct can_data_conv *data_conv, struct can_frame *frame)
{
    float data_conv->cruise_target_diff = data_conv->cruise_target_speed - data_conv->speed_conv;

#if 0
	float new_throttle;
	if (diff_speed < 0) {
		new_throttle = 0
	}
    
	return new_throttle;
#endif
}
#endif
