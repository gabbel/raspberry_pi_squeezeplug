

//-----------------defines------------------------------
#define TIMER_INTERVAL  15*1000   //in microseconds
#define LONG_PRESS_TIME 1000 * 1000 //in microseconds








#define DEBUG          //uncomment to disable debugging output






//----------------structures --------------------------
#include <stdbool.h>
#include <stdint.h>
#include <bcm2835.h>
#include <X11/keysym.h>

typedef struct pin_setting_s {
	uint8_t pin;   //the pin to press. one of RPI_V2_GPIO_P1_XX
	unsigned int keycode_short; //the keycode to send
	unsigned int keycode_long;

} pin_setting_t;

#define LONG_PRESS_CYCLES ((LONG_PRESS_TIME)/(TIMER_INTERVAL)) //how many timer interrupts to wait for a longpress
