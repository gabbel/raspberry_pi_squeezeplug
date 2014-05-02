#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>


#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <bcm2835.h>
#include <sys/time.h>


#include "config.h"


//x11 varibales
Display*  display; //the one and only display to use

//Other variables
volatile bool terminate = false; //abort condition
volatile int8_t enc_delta;          // -128 ... 127
static int8_t enc_last;


/*
Used Resources:
 http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
 http://www.raspberrypi.org/wp-content/uploads/2012/10/Raspberry-Pi-R2.0-Schematics-Issue2.2_027.pdf
 http://www.communitysqueeze.org/jivelite_about.jsp
 http://www.doctort.org/adam/nerd-notes/x11-fake-keypress-event.html
*/

void do_poll();



//handler for sigterm event
void sigterm_handler(int signum) {
	terminate = true;
}


//timer interrupt
void sigtimer_handler(int signum) {
	do_poll();
	signal(SIGALRM, sigtimer_handler);
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
   	display = XOpenDisplay(":0"); //pass NULL without "" to read from environment variable DISPLAY
   	if(display == NULL) {
		fprintf(stderr,"Failed open display\n");
      		return false;
	}



	//register sigterm handler
	signal(SIGINT, sigterm_handler);
	signal(SIGALRM, sigtimer_handler);
	
	struct itimerval timer;
	timer.it_value.tv_sec = 0 ;
   	timer.it_value.tv_usec = TIMER_INTERVAL;
   	timer.it_interval.tv_sec = 0;
   	timer.it_interval.tv_usec = TIMER_INTERVAL;
	setitimer ( ITIMER_REAL, &timer, NULL ) ;

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
	

		if(!current_state) { //button is LOW
			if(pin_pressed_time[i] == LONG_PRESS_CYCLES && pin_setting.keycode_long!=0 ) {
				printf("send long press down\n");
				send_event(pin_setting.keycode_long,true);
			}

			pin_pressed_time[i]++;
		}

		if(current_state != old_state && current_state) { //if butten released = positive edge detected
			if(pin_pressed_time[i] > LONG_PRESS_CYCLES) {
				if(pin_setting.keycode_long!=0) {
					printf("send long press up\n");
					send_event(pin_setting.keycode_long,false);
				}
			} else {
				printf("send short press down\n");
				send_event(pin_setting.keycode_short,true);
				printf("send short press up\n");
				send_event(pin_setting.keycode_short,false);
			}
			pin_pressed_time[i] = 0;
		} 		
		
		pin_state[i] = current_state;		
	}

	//http://www.mikrocontroller.net/articles/Drehgeber#Solide_L.C3.B6sung:_Beispielcode_in_C
	  int8_t new=0, diff;
 
	  if(bcm2835_gpio_lev(ROTARY_ENC_PIN_A)==HIGH)
	    new = 3;
	  if(bcm2835_gpio_lev(ROTARY_ENC_PIN_B)==HIGH)
	    new ^= 1;                   // convert gray to binary
	  diff = enc_last - new;                // difference last - new
	  if( diff & 1 ){               // bit 0 = value (1)
	    enc_last = new;                 // store new as next last
	    enc_delta += (diff & 2) - 1;        // bit 1 = direction (+/-)
	  }
		


}

 
int8_t encode_read1( void )         // read single step encoders
{
  int8_t val;
  val = enc_delta;
enc_delta = val & 3;; 
 //enc_delta = 0;
  return val>>2;                   // counts since last call
}
 


int main() {
	
	if(!setup()) { //if setup fails
		fprintf(stderr, "Setup failed\n");
		return -1;
	}
	fprintf(stdout,"Setup ok\n");

	while(!terminate) {
	/*	do_poll();
		bcm2835_delay(WAIT_CYCLE); // sleep for WAIT_CYCLE miliseconds*/
		sleep(1); //wait for one second
		int8_t delta = encode_read1();
		if(delta!=0) 
			printf("encoder %d\n",delta);
	}
	
	bcm2835_close();
	XCloseDisplay(display);
	fprintf(stdout,"\nTerminated\n");
}
