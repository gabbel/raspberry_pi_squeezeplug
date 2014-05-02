

//-----------------defines------------------------------
#define TIMER_INTERVAL  15*1000   //in microseconds
#define LONG_PRESS_TIME 1000 * 1000 //in microseconds


#define DEBUG          //uncomment to disable debugging output


#define ROTARY_ENC_PIN_A    RPI_V2_GPIO_P1_16
#define ROTARY_ENC_PIN_B    RPI_V2_GPIO_P1_18

#define ROTARY_ENC_KEYCODE_LEFT XK_Up
#define ROTARY_ENC_KEYCODE_RIGHT XK_Down



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



//-------- Assign Keyboard events to pins ---------------------
const pin_setting_t pin_configuration[] = {

                                                                                                                                //PIN   Command Short                                   Command Long
                                                                                                                                //01    Pin #1 = +3.3V (Primary
                                                                                                                                //02    Pin #2 = +5V (Primary)
//      { RPI_V2_GPIO_P1_03, 0 , 0 },                                                           //03    Not in use                                              Not in use
                                                                                                                                //04    Pin #4 = +5V
//      { RPI_V2_GPIO_P1_05, 0 , 0 },                                                           //05    Not in use                                              Not in use
                                                                                                                                //06    Pin #6 = Ground
        { RPI_V2_GPIO_P1_07, XK_minus , XK_minus },                                     //07    Vol -                                                   Same as short
//      { RPI_V2_GPIO_P1_08, 0 , 0 },                                                           //08    UART
                                                                                                                                //09    Pin #9 = Ground
//      { RPI_V2_GPIO_P1_10, 0 , 0 },                                                           //10    UART
        { RPI_V2_GPIO_P1_11, XK_plus, XK_plus },                                        //11    Vol +                                                   Same as short
        { RPI_V2_GPIO_P1_12, XK_Return, 0 },                                            //12    Enter (Go)                                              Not in use
//      { RPI_V2_GPIO_P1_13, 0 , 0 },                                                           //13    Not in use                                              Not in use
                                                                                                                                //14    Pin #14 = Ground
        { RPI_V2_GPIO_P1_15, XK_c, 0 },                                                         //15    Pause                                                   -
//      { RPI_V2_GPIO_P1_16, 0 , 0 },                                                           //16    Scroll Down (Rotary Encoder)
                                                                                                                                //17    Pin #17 = +3.3V
//      { RPI_V2_GPIO_P1_18, 0 , 0 },                                                           //18    Scroll Up (Rotary Encoder)
        { RPI_V2_GPIO_P1_19, XK_a, XK_A },                                                      //19    Add                                                             Add (hold)
                                                                                                                                //20    Pin #20 = Ground
        { RPI_V2_GPIO_P1_21, XK_p, XK_P },                                                      //21    Play
        { RPI_V2_GPIO_P1_22, XK_b, XK_B },                                                      //22    Skip Track (Forward)                    Fwd (hold)
        { RPI_V2_GPIO_P1_23, XK_Left, XK_h },                                           //23    Back                                                    Home
        { RPI_V2_GPIO_P1_24, XK_z, XK_Z },                                                      //24    Skip Track (Back)                               Rew (hold)
                                                                                                                                //25    Pin #25 = Ground
        { RPI_V2_GPIO_P1_26, XK_bracketleft, XK_bracketright }          //26    Now Playing                                             Show Playlist
};




//--------- Helper Variables for Configuration ----------------

#define NUM_PINS (sizeof(pin_configuration)/ sizeof(pin_setting_t))

const uint8_t number_of_pins = NUM_PINS; //number of configured pins

bool pin_state[NUM_PINS]; //storage to save last state of pin
uint16_t pin_pressed_time [NUM_PINS];  //storage to save the number cycles the pin was presse



