#include "config.h"

const pin_setting_t pin_configuration[] = {
	{ RPI_V2_GPIO_P1_07, XK_Left, XK_Right },
	{ RPI_V2_GPIO_P1_11, XK_Up,  0 }




};




//--------- Helper Variables for Configuration ----------------

#define NUM_PINS (sizeof(pin_configuration)/ sizeof(pin_setting_t))

const uint8_t number_of_pins = NUM_PINS; //number of configured pins

bool pin_state[NUM_PINS]; //storage to save last state of pin
uint16_t pin_pressed_time [NUM_PINS];  //storage to save the number cycles the pin was presse


