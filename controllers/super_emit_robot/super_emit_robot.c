
#define SIMULATION 1

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif


// messages
#define MSG_RCV "FFFF"



void robot_setup() {
    init_robot();
    init_sensors();
    init_communication();
    calibrate_prox();
}

void led_all_off() {
    for (int i=0;i<LED_COUNT;i++)
      disable_led(i);
}

void led_all_on() {
    for (int i=0;i<LED_COUNT;i++)
      enable_led(i);
}


void robot_loop() {
    

    while (robot_go_on()) {

        
        char rcv[MSG_LENGTH];

        receive_msg(rcv);

        // strcmp returns 0 when strings are equal
        if (strcmp(rcv,MSG_RCV)==0) {
          led_all_on();
        }
        else {
          led_all_off();
        }
          
    }
}

int main (int argc, char **argv) {
    #if SIMULATION
    #else
    ip = argv[1];
    #endif
    
    robot_setup();
    robot_loop();   
}




