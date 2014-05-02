#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>


#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <bcm2835.h>

#include "config.h"


//x11 varibales
Display*  display; //the one and only display to use


//Pin variables 
const uint8_t number_of_pins = NUM_PINS; //number of configured pins
bool pin_state[NUM_PINS]; //storage to save last state of pin
uint16_t pin_pressed_time [NUM_PINS];  //storage to save the number cycles the pin was presse

//Timing variables
uint16_t key_read_time =0;
uint16_t enc_read_time =0;
uint16_t enc_send_time =0;


//Other variables
volatile bool terminate = false; //abort condition
volatile int8_t enc_delta;          // -128 ... 127
static int8_t enc_last;


//Encoder variables
const int8_t table[16]  = {0,0,-1,0,0,0,0,1,1,0,0,0,0,-1,0,0}; //half resolution table - better accuracy   
//const int8_t table[16] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0}; //full resolution table



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

	//configure rotary encoder
	bcm2835_gpio_fsel(ROTARY_ENC_PIN_A, BCM2835_GPIO_FSEL_INPT);
    	bcm2835_gpio_set_pud(ROTARY_ENC_PIN_A, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_fsel(ROTARY_ENC_PIN_B, BCM2835_GPIO_FSEL_INPT);
    	bcm2835_gpio_set_pud(ROTARY_ENC_PIN_B, BCM2835_GPIO_PUD_UP);

 	int8_t new = 0;
	if(bcm2835_gpio_lev(ROTARY_ENC_PIN_A)==HIGH)
		new = 3;
	if(bcm2835_gpio_lev(ROTARY_ENC_PIN_B)==HIGH)
	    	new ^= 1;                   // convert gray to binary
	  
	enc_last = new;                   // power on state
	enc_delta = 0;	


	// Obtain the X11 display.
   	display = XOpenDisplay(DISPLAY);
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

 
int8_t encode_read1(void)         // read single step encoders
{
  int8_t val= enc_delta;
  enc_delta = 0; 
  return val;                   // counts since last call
}

void do_cycle(void) {

	if(++key_read_time == KEY_READ_CYCLES) { //increment counter and compare
		key_read_time=0;

		for(int i=0; i<number_of_pins; i++) { //once for each pin
			pin_setting_t pin_setting = pin_configuration[i]; //get the current pin
			bool current_state = (bcm2835_gpio_lev(pin_setting.pin)==HIGH);
			bool old_state = pin_state[i];
		

			if(!current_state) { //button is LOW
				if(pin_pressed_time[i] == LONG_PRESS_CYCLES && pin_setting.keycode_long!=0 ) {
					#ifdef DEBUG
						printf("send long press down\n");
					#endif
					send_event(pin_setting.keycode_long,true);
				}

				pin_pressed_time[i]++;
			}

			if(current_state != old_state && current_state) { //if butten released = positive edge detected
				if(pin_pressed_time[i] > LONG_PRESS_CYCLES) {
					if(pin_setting.keycode_long!=0) {
					#ifdef DEBUG
						printf("send long press up\n");
					#endif
						send_event(pin_setting.keycode_long,false);
					}
				} else {
					#ifdef DEBUG
						printf("send short press down\n");
					#endif
					send_event(pin_setting.keycode_short,true);
					
					#ifdef DEBUG
						printf("send short press up\n");
					#endif
					send_event(pin_setting.keycode_short,false);
				}
				pin_pressed_time[i] = 0;
			} 		
			
			pin_state[i] = current_state;		
		}
	}

	if(++enc_read_time == ENC_READ_CYCLES) { //increment counter and compare
		enc_read_time=0;
		//http://www.mikrocontroller.net/articles/Drehgeber#Dekoder_f.C3.BCr_Drehgeber_mit_wackeligen_Rastpunkten
		enc_last = (enc_last << 2)  & 0x0F;

		if(bcm2835_gpio_lev(ROTARY_ENC_PIN_A)==HIGH)
			enc_last |= 2;
		if(bcm2835_gpio_lev(ROTARY_ENC_PIN_B)==HIGH)
			enc_last |=1;
		enc_delta += table[enc_last];
	}
		

	if(++enc_send_time == ENC_SEND_CYCLES) { //increment counter and compare
		enc_send_time=0;
		int8_t delta = encode_read1();
		
		if(delta!=0) {
			#ifdef DEBUG
				printf("Rotary Encoder %d\n",delta);
			#endif

			int8_t amount = abs(delta);
			unsigned int keycode = ROTARY_ENC_KEYCODE_RIGHT;

			if(delta<0) {
				keycode=ROTARY_ENC_KEYCODE_LEFT;
			} 		
			for(int i=0; i<amount; i++) {
				send_event(keycode,true); //pressed event
				send_event(keycode,false); //released event
			}
		} 
	}

}




int main() {
	
	if(!setup()) { //if setup fails
		fprintf(stderr, "Setup failed\n");
		return -1;
	}
	fprintf(stdout,"Setup ok\n");

	while(!terminate) {
		bcm2835_delay(WAIT_CYCLE); // sleep for WAIT_CYCLE miliseconds*/
		do_cycle();
	}
	
	bcm2835_close();
	XCloseDisplay(display);
	fprintf(stdout,"\nTerminated\n");
}
