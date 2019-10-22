/*****************************************************************************/
/*
 * By Jonathan Crossley
 * Version 0.1 - 20 Oct 2019
 *
 * jc_class_headlights.h - ???.
 */

class Headlights : public Converted_CAN_Data
{
private:
    //int message_id[] = 1a6, 294;
    bool right_turn_flag;
    bool left_turn_flag;
    string headlight_status;
public:
    void Convert_message();
    int display(int);
    int Headlights(int);   //// if it has to be void then need a variable that all the constructors have access to and can change
};

int Headlights::Headlights()
{
	message_id[] = 0x1a6, 0x294;  //// ???
	right_turn_flag = false;
	left_turn_flag = false;
	headlight_status = "Not sure...";

	// Print initial dash text
	mvprintw();
}

void Convert_message(struct can_frame &frame)
{

	if ((frame.data[0] >> 4) == 0x04)
		right_turn_flag = true;
	else
		right_turn_flag = false;

	if ((frame.data[0] >> 4) == 0x02)
		left_turn_flag = true;
	else
		left_turn_flag = false;


	if ((frame.data[0] & 0x0F) == 0x00 || (frame.data[0] & 0x0F) == 0x04)
		headlight_status = "Off";
	else if ((frame.data[0] & 0x0F) == 0x01 || (frame.data[0] & 0x0F) == 0x05)
		headlight_status = "Parking";
	else if ((frame.data[0] & 0x0F) == 0x02 || (frame.data[0] & 0x0F) == 0x06)
		headlight_status = "LOW Beams";
	else if ((frame.data[0] & 0x0F) == 0x03 || (frame.data[0] & 0x0F) == 0x07)
		headlight_status = "HIGH Beams";
	else
		headlight_status = "Not sure...";
}




int display(row)
{
	mvprintw(row++, DATA_COLM, "%s", data->headlight_status);
    clrtoeol();
    mvprintw(row++, DATA_COLM, "%s", data->right_turn_flag ? "On" : "Off");
    clrtoeol();
    mvprintw(row++, DATA_COLM, "%s", data->left_turn_flag ? "On" : "Off");
    clrtoeol();

    return row;
}