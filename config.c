#include "config.h"

const pin_setting_t pin_configuration[] = {
	{ RPI_V2_GPIO_P1_07, XK_Left },
	{ RPI_V2_GPIO_P1_11, XK_Up }




};




//--------- Helper Variables for Configuration ----------------
const uint8_t number_of_pins = sizeof(pin_configuration)/ sizeof(pin_setting_t); //number of configured pins

bool pin_state[sizeof(pin_configuration)/ sizeof(pin_setting_t)]; //storage to save last state of pin



