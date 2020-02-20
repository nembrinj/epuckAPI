
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

        get_prox_calibrated(prox_values);

        double prox = (prox_values[7] + prox_values[0]) / 2.;
        double ds = (NORM_SPEED * prox) / MAX_PROX;
        double speed = bounded_speed(NORM_SPEED - ds);

        set_speed(speed, speed);
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




