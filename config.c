#include "config.h"

const pin_setting_t pin_configuration[] = {
							//PIN	Command Short	Command Long
	{ RPI_V2_GPIO_P1_07, XK_minus, 0 },		//04	Vol -		-
        { RPI_V2_GPIO_P1_11, XK_plus, 0 },		//17	Vol +		-
        { RPI_V2_GPIO_P1_12, XK_Return, 0 },		//18	Enter (Go)	-
        { RPI_V2_GPIO_P1_24, XK_z, XK_Z },		//08
        { RPI_V2_GPIO_P1_22, XK_b, XK_B }

};




//--------- Helper Variables for Configuration ----------------

#define NUM_PINS (sizeof(pin_configuration)/ sizeof(pin_setting_t))

const uint8_t number_of_pins = NUM_PINS; //number of configured pins

bool pin_state[NUM_PINS]; //storage to save last state of pin
uint16_t pin_pressed_time [NUM_PINS];  //storage to save the number cycles the pin was presse


