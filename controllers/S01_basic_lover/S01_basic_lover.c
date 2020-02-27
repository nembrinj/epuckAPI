
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif

void robot_setup() {
    init_robot();
    init_sensors();
    calibrate_prox();
}

void robot_loop() {
	short int prox_values[8];

    while (robot_go_on()) {

        // fetch proximity sensor values 
        get_prox_calibrated(prox_values);
        
        // basic love behavior
        double prox_front = (prox_values[PROX_RIGHT_FRONT] + prox_values[PROX_LEFT_FRONT]) / 2.;
        double ds = - (NORM_SPEED * prox_front) / MAX_PROX;
        double speed = NORM_SPEED + ds;

        // make sure speed values are legal
        set_speed(bounded_speed(speed),bounded_speed(speed));
    }
    cleanup_robot();
}

int main (int argc, char **argv) {
    #if SIMULATION
    #else
    ip = argv[1];
    #endif
    
    robot_setup();
    robot_loop();   
}




