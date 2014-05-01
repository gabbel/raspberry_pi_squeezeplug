

//-----------------defines------------------------------
#define WAIT_CYCLE 50   //wait time for the polling loop in miliseconds

#define DEBUG          //uncomment to disable debugging output






//----------------structures --------------------------
#include <stdbool.h>
#include <stdint.h>
#include <bcm2835.h>
#include <X11/keysym.h>

typedef struct pin_setting_s {
	uint8_t pin;   //the pin to press. one of RPI_V2_GPIO_P1_XX
	unsigned int keycode; //the keycode to send

} pin_setting_t;


