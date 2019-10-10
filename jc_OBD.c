/*****************************************************************************/
/* 
 * jc_OBD.c
 * By Jonathan Crossley
 * Version 0.1 - 22 May 2019
 *          0.2 - 31 July 2019
 *
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
#include "lib.h"

// Library for GPIO control
#include "wiringPi.h"

// Library for controlling the terminal curser
#include <curses.h>

// Global variable used for stopping program and both threads
bool g_quit = false;

// Structure for function return of multiple types
struct return_struct
{
    int s;
    struct can_frame frame;
};

// Function prototypes
void *can_send_thread(void *setup_CAN_return);
int setup_CAN_communication(struct return_struct *setup_CAN_return);


int parse_canframe(char *cs, struct can_frame *cf);
/*
 * Transfers a valid ASCII string decribing a CAN frame into struct can_frame.
 *
 * <can_id>#{R|data}
 *
 * can_id can have 3 (standard frame format) or 8 (extended frame format)
 *        hexadecimal chars
 *
 * data has 0 to 8 hex-values that can (optionally) be seperated by '.'
 *
 * Examples:
 *
 * 123# -> standard CAN-Id = 0x123, dlc = 0
 * 12345678# -> extended CAN-Id = 0x12345678, dlc = 0
 * 123#R -> standard CAN-Id = 0x123, dlc = 0, RTR-frame
 * 7A1#r -> standard CAN-Id = 0x7A1, dlc = 0, RTR-frame
 *
 * 123#00 -> standard CAN-Id = 0x123, dlc = 1, data[0] = 0x00
 * 123#1122334455667788 -> standard CAN-Id = 0x123, dlc = 8
 * 123#11.22.33.44.55.66.77.88 -> standard CAN-Id = 0x123, dlc = 8
 * 123#11.2233.44556677.88 -> standard CAN-Id = 0x123, dlc = 8
 * 32345678#112233 -> error frame with CAN_ERR_FLAG (0x2000000) set
 *
 * Simple facts on this compact ASCII CAN frame representation:
 *
 * - 3 digits: standard frame format
 * - 8 digits: extendend frame format OR error frame
 * - 8 digits with CAN_ERR_FLAG (0x2000000) set: error frame
 * - an error frame is never a RTR frame
 * 
 */

/*****************************************************************************/
int main(void)
{	
    // Set up variable to create new thread.
    pthread_t can_send_thread_id;

    // Wake up counter
    int wakeup_count = 0;

    // Set up CAN communication
    struct return_struct setup_CAN_return;
    if (setup_CAN_communication(&setup_CAN_return) == -1) {
        printf("Error setting up CAN communication. Try again.\n");
        
        // TEST //
        printf("TEST - main exiting\n");
        // TEST //
        
        exit(0);
    }

    // Start the other thread to send the wake up message
    int thread_check = pthread_create(&can_send_thread_id, NULL, can_send_thread, (void *)&setup_CAN_return);
    if (thread_check != 0) {
        printf("ERROR, At line %d, %s\n", __LINE__, strerror(thread_check));
                
        // TEST //
        printf("TEST - main exiting\n");
        // TEST //

        exit(0);
    }
    
    // TEST //
    char arr[100];
    printf("TEST - %d \n", setup_CAN_return.s);
    // TEST //

    // Loop until "q" is pressed
	while (!g_quit) {

		if(read(setup_CAN_return.s, (void *)(&arr), sizeof(struct can_frame)) == -1) {
        //if(read(setup_CAN_return.s, (void *)(&setup_CAN_return.frame), sizeof(struct can_frame)) < 0) {    
            perror("Error reading CAN bus address");
            g_quit = true;
            pthread_join(can_send_thread_id, NULL);
            break;
        }

        // TEST //
        printf("TEST SIZE - %d \n", sizeof(struct can_frame));
        printf("Hi\n");
        // TEST //

		if (setup_CAN_return.frame.can_id == 0x2e1) {
            // Print out each time recieve a wakeup message
            printf("Wake up recieved: %d\n", wakeup_count++);
		}
        
        // Determind wheather to keep running or not
		if (getch() == 'q') {
			g_quit = true;
            pthread_join(can_send_thread_id, NULL);
            break;
		}
	}

    // Make sure the other thread is terminated
    pthread_join(can_send_thread_id, NULL);
    
    // TEST //
    printf("TEST - main exiting\n");
    // TEST //

    return 0;
}

/*****************************************************************************/
void *can_send_thread(void *setup_CAN_return)
{
    // Define OBD messages to be sent
    struct can_frame wakeup_frame;
    char *OBD_eng_coolant_temp =	"7df#02.01.05.00.00.00.00.00";
    char *OBD_eng_RPM = 			"7df#02.01.0C.00.00.00.00.00";
    char *OBD_speed = 				"7df#02.01.0D.00.00.00.00.00";
    char *OBD_ambient_temp = 		"7df#02.01.46.00.00.00.00.00";
	char *OBD_pedal_position_D = 	"7df#02.01.49.00.00.00.00.00";
	char *OBD_pedal_position_E = 	"7df#02.01.4A.00.00.00.00.00";
	char *OBD_commanded_throttle = 	"7df#02.01.4C.00.00.00.00.00";
    parse_canframe(wakeup_id, &wakeup_frame);

    // Cast CAN address variable now so loop doesn't have to everytime
    int ss = ((struct return_struct *)setup_CAN_return)->s;

    // Continue to send wakeup message till main quits
    while (!g_quit)
    {
        // Send wake up message over the CAN bus
        if (write(ss, &wakeup_frame, sizeof(wakeup_frame)) != sizeof(wakeup_frame)) {
            perror("Error sending CAN frame");
            g_quit = true;
            break;
        }

        // Determind wheather to keep running or not
        if (getch() == 'q') {
            g_quit = true;
            break;
        }

        // Delay for 1 second.
        delay(1000);
    }

    // TEST //
    printf("TEST - thread exiting\n");
    // TEST //

    return NULL;
}

/*****************************************************************************/
int setup_CAN_communication(struct return_struct *setup_CAN_return)
{
    // Set up variables to connect to CAN bus
    struct sockaddr_can addr;
    struct ifreq ifr;

    // Get socket address
    setup_CAN_return->s = socket(PF_CAN, SOCK_DGRAM, CAN_BCM);
    if (setup_CAN_return->s == -1) {
        perror("Error creating endpoint for socket communication");
        return -1;
    }

    // Copy some addresses and names into the CAN structs
    strcpy(ifr.ifr_name, "can0");
    addr.can_family = AF_CAN;
    
    // Do something with control device
    if (ioctl(setup_CAN_return->s, SIOCGIFINDEX, &ifr) == -1) {
        perror("Error with control device (SIOCGIFINDEX)");
        return -1;
    }
    
    // Do some more with structs and something with the CPU socket
    addr.can_ifindex = ifr.ifr_ifindex;
    setsockopt(setup_CAN_return->s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    // Combine it all together into "s" for the final socket address
    if (bind(setup_CAN_return->s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        //printf("%d\n",errno);
        perror("Error with binding socket address");
        return -1;
    }

    // Return sucess
    return 0;
}

int parse_canframe(char *cs, struct can_frame *cf) {
	/* documentation see lib.h */

	int i, idx, dlc, len;
	unsigned char tmp;

	len = strlen(cs);
	//printf("'%s' len %d\n", cs, len);

	memset(cf, 0, sizeof(*cf)); /* init CAN frame, e.g. DLC = 0 */

	if (len < 4)
		return 1;

	#define CANID_DELIM '#'

	if (cs[3] == CANID_DELIM) { /* 3 digits */

		idx = 4;
		for (i=0; i<3; i++){
			if ((tmp = asc2nibble(cs[i])) > 0x0F)
				return 1;
			cf->can_id |= (tmp << (2-i)*4);
		}

	} else if (cs[8] == CANID_DELIM) { /* 8 digits */

		idx = 9;
		for (i=0; i<8; i++){
			if ((tmp = asc2nibble(cs[i])) > 0x0F)
				return 1;
			cf->can_id |= (tmp << (7-i)*4);
		}
		if (!(cf->can_id & CAN_ERR_FLAG)) /* 8 digits but no errorframe?  */
			cf->can_id |= CAN_EFF_FLAG;   /* then it is an extended frame */

	} else
		return 1;

	if((cs[idx] == 'R') || (cs[idx] == 'r')){ /* RTR frame */
		cf->can_id |= CAN_RTR_FLAG;
		return 0;
	}

	for (i=0, dlc=0; i<8; i++){

		if(cs[idx] == DATA_SEPERATOR) /* skip (optional) seperator */
			idx++;

		if(idx >= len) /* end of string => end of data */
			break;

		if ((tmp = asc2nibble(cs[idx++])) > 0x0F)
			return 1;
		cf->data[i] = (tmp << 4);
		if ((tmp = asc2nibble(cs[idx++])) > 0x0F)
			return 1;
		cf->data[i] |= tmp;
		dlc++;
	}

	cf->can_dlc = dlc;

	return 0;
}

unsigned char asc2nibble(char c) {

	if ((c >= '0') && (c <= '9'))
		return c - '0';

	if ((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;

	if ((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;

	return 16; /* error */
}
