#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <bcm2835.h>

#include "config.h"

extern pin_setting_t pin_configuration[];
extern uint8_t number_of_pins;
extern bool pin_state []; //array to store last state of pin

//x11 varibales
Display*  display; //the one and only display to use

//Other variables
volatile bool terminate = false; //abort condition

/*
Used Resources:
 http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
 http://www.raspberrypi.org/wp-content/uploads/2012/10/Raspberry-Pi-R2.0-Schematics-Issue2.2_027.pdf
 http://www.communitysqueeze.org/jivelite_about.jsp
 http://www.doctort.org/adam/nerd-notes/x11-fake-keypress-event.html
*/



//handler for sigterm event
void sigterm_handler(int signum) {
	terminate = true;
}


//Setups the chip, configures the pins  and returns true on sucess
bool setup() {
	if (!bcm2835_init()) { //when chip init fails
        	fprintf(stderr,"Failed to init bcm2835\n");
		return false;
	}

	
	for(int i=0; i<number_of_pins; i++) { //once for each pin
		pin_setting_t pin_setting = pin_configuration[i]; //get the current pin
		pin_state[i] = 1;	
	
    		// Set RPI pin to be an input
    		bcm2835_gpio_fsel(pin_setting.pin, BCM2835_GPIO_FSEL_INPT);
    		//  with a pullup
    		bcm2835_gpio_set_pud(pin_setting.pin, BCM2835_GPIO_PUD_UP);
	}


	// Obtain the X11 display.
   	display = XOpenDisplay(":1"); //pass NULL without "" to read from environment variable DISPLAY
   	if(display == NULL) {
		fprintf(stderr,"Failed open display\n");
      		return false;
	}



	//register sigterm handler
	signal(SIGINT, sigterm_handler);
	return true;
}

void send_event (unsigned int keycode, bool pressed) {
	//Code from: https://shanetully.com/2012/07/simulating-mediakey-presses-in-x11/
	XTestFakeKeyEvent(display, XKeysymToKeycode(display, keycode), pressed, 0);
	XFlush(display);	
}



void do_poll() {
	for(int i=0; i<number_of_pins; i++) { //once for each pin
		pin_setting_t pin_setting = pin_configuration[i]; //get the current pin
		bool current_state = (bcm2835_gpio_lev(pin_setting.pin)==HIGH);
		bool old_state = pin_state[i];
	
		if(current_state != old_state) { //edge dectected
    			if(current_state) {
				#ifdef DEBUG
					printf("Pin Released: %u\n",pin_setting.pin);
			 	#endif
				send_event(pin_setting.keycode,false);
			} else {
			 	send_event(pin_setting.keycode,true);
				#ifdef DEBUG
					printf("Pin Pressed: %u\nSending Keycode: %u\n",pin_setting.pin, pin_setting.keycode);
				#endif
			}
		}

		pin_state[i] = current_state;		
	}
}


int main() {
	
	if(!setup()) { //if setup fails
		fprintf(stderr, "Setup failed\n");
		return -1;
	}
	fprintf(stdout,"Setup ok\n");

	while(!terminate) {
		do_poll();
		bcm2835_delay(WAIT_CYCLE); // sleep for WAIT_CYCLE miliseconds
	}
	
	bcm2835_close();
	XCloseDisplay(display);
	fprintf(stdout,"\nTerminated\n");
}
