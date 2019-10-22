/*****************************************************************************/
/*
 * By Jonathan Crossley
 * Version 0.1 - 20 Oct 2019
 *
 * jc_class.h - Header file for jc_main, includes declarations for functions,
 *      defines, and global variables.
 */

#include <string>
#include <iostream>

using namespace std;

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

    bool wiper_flag;
    
    string doors_status;
    string seatbelt_status;
};
*/

//// Way to loop thru all class IDs to create an array for the if statement
//// in main, that way when new IDs are found or new objects are added, don't
//// have to change the if loop as well.



class Converted_CAN_Data
{
private:
    string message_id = "";   // ???
    // or
    int message_id;
    // or
    __u8 message_id;
    // or
    int message_id[];  // can be multiple, but will always be one
                    // Except for transmission gear_calc maybe???
                    // gear_calc is just a function, Trans object still gets pedal and reverse message
    bool reprint_var;
public:
    virtual void Convert_message(string message_data) = 0;
};

//// Possibly add a display and/or a printing_update variable to determind if it needs printing or not
// this should get rid of the looping function that prints regardless (print_dash)

class Throttle : public Converted_CAN_Data
{
//private:
//    int message_id[] = 17c, 1dc;  // may have more than one id
public:
    void Convert_message();  // convert the raw data from one or two can ids
    int display(int row);  // print to console starting with the row 
    void Throttle();  // Constructor to give starting values-falses, true, 0, etc.
};

class Engine : public Converted_CAN_Data
{
private:
    __u8 message_id = 1dc;
public:
    void Convert_message();
    void display(int row);
    void Engine_Data();

    // Not sure what to do with this
    friend &ostream operation<<(&ostream os, EngineMessages em);
};

class Pedals : public Converted_CAN_Data
{

};

class Transmission : public Converted_CAN_Data
{

};

#if DEMO_MODE
class Demo : public Converted_CAN_Data
{

};
#endif

class Cruise_control : public Converted_CAN_Data
{

};

class Speed : public Converted_CAN_Data
{
	// can include speed and wheel speed sensors
};

class Temperature : public Converted_CAN_Data
{

};

class Fuel : public Converted_CAN_Data
{

};

class AC : public Converted_CAN_Data
{

};

