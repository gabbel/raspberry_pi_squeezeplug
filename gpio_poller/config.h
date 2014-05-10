
//#define DEBUG          //uncomment to disable debugging output

//-----------------defines------------------------------
// Timings
#define WAIT_CYCLE  1   //in miliseconds (must be a common divisor of the following 3 defines)
#define ENC_READ_TIME 1 //in miliseconds
#define ENC_SEND_TIME 20 //in miliseconds
#define KEY_READ_TIME 50 //in miliseconds
 
#define LONG_PRESS_TIME 500 //in miliseconds

// Display
#define DISPLAY  ":0"   //NULL = user environment variable DISPLAY. Otherwise pass a string for the display host:displaynumber e.g. :0

// Rotary Encoder
#define ROTARY_ENC_PIN_A    RPI_V2_GPIO_P1_18 // Pin A
#define ROTARY_ENC_PIN_B    RPI_V2_GPIO_P1_16 // Pin B

// Keys, Rotary Encoder Keyboard events
#define ROTARY_ENC_KEYCODE_LEFT XK_Up // Scroll Up (Rotary Encoder)
#define ROTARY_ENC_KEYCODE_RIGHT XK_Down // Scroll Down (Rotary Encoder)


//----------------calculations------------------------

#define KEY_READ_CYCLES ((KEY_READ_TIME)/(WAIT_CYCLE)) //how many cycles to wait to read the keys
#define LONG_PRESS_CYCLES ((LONG_PRESS_TIME)/(WAIT_CYCLE)/(KEY_READ_CYCLES)) //how many times do we need to read the keys to define a longpress
#define ENC_READ_CYCLES ((ENC_READ_TIME)/(WAIT_CYCLE)) //how many cycles to wait to read the encoder
#define ENC_SEND_CYCLES ((ENC_SEND_TIME)/(WAIT_CYCLE)) //how many cycles to wait to send the encoder state
#define NUM_PINS (sizeof(pin_configuration)/ sizeof(pin_setting_t)) //how many pins are configured

//----------------structures --------------------------
#include <stdbool.h> // for bool datatype
#include <stdint.h>  //for uint_*t and int_*t datatypes
#include <bcm2835.h> // Library for of the BCM2835 SOC
#include <X11/keysym.h> // Headerfile for the X11 keycode defines

typedef struct pin_setting_s {
	uint8_t pin;   //the pin to press. one of RPI_V2_GPIO_P1_XX
	unsigned int keycode_short; //the keycode to send for a short press event
	unsigned int keycode_long; //the keycode to send for a long press event

} pin_setting_t;




//-------- Assign Keyboard events to pins ---------------------
//see \usr\include\X11\keysymdef.h for the keycode defines
const pin_setting_t pin_configuration[] = {

							//PIN# Command Short, Command Long
							//01 Pin #1 = +3.3V (Primary
							//02 Pin #2 = +5V (Primary)
// { RPI_V2_GPIO_P1_03, 0 , 0 }, 			//03 Not in use, Not in use
							//04 Pin #4 = +5V
// { RPI_V2_GPIO_P1_05, 0 , 0 }, 			//05 Not in use, Not in use
							//06 Pin #6 = Ground
 { RPI_V2_GPIO_P1_07, XK_minus , XK_minus }, 		//07 Vol -, Same as short
// { RPI_V2_GPIO_P1_08, 0 , 0 }, 			//08 UART
							//09 Pin #9 = Ground
// { RPI_V2_GPIO_P1_10, 0 , 0 }, 			//10 UART
 { RPI_V2_GPIO_P1_11, XK_plus, XK_plus }, 		//11 Vol +, Same as short
// { RPI_V2_GPIO_P1_12, 0 , 0 }, 			//12 Not in use, Not in use
 { RPI_V2_GPIO_P1_13, XK_c, 0 }, 			//13 Pause, -
							//14 Pin #14 = Ground
 { RPI_V2_GPIO_P1_15, XK_Return, 0 },			//15 Enter (Go), Not in use
// { RPI_V2_GPIO_P1_16, 0 , 0 }, 			//16 Scroll Down (Rotary Encoder)
							//17 Pin #17 = +3.3V
// { RPI_V2_GPIO_P1_18, 0 , 0 }, 			//18 Scroll Up (Rotary Encoder)
 { RPI_V2_GPIO_P1_19, XK_a, XK_A }, 			//19 Add, Add (hold)
							//20 Pin #20 = Ground
 { RPI_V2_GPIO_P1_21, XK_p, XK_P }, 			//21 Play, -
 { RPI_V2_GPIO_P1_22, XK_b, XK_B }, 			//22 Skip Track (Forward), Fwd (hold)
 { RPI_V2_GPIO_P1_23, XK_Escape, XK_h }, 		//23 Back, Home
 { RPI_V2_GPIO_P1_24, XK_z, XK_Z }, 			//24 Skip Track (Back), Rew (hold)
							//25 Pin #25 = Ground
 { RPI_V2_GPIO_P1_26, XK_bracketleft, XK_bracketright } //26 Now Playing, Show Playlist
};



