
#define SIMULATION 1

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

        double prox_left = (0 * prox_values[7] + 4 * prox_values[6] + 1 * prox_values[5]) / 5.;
        double prox_right = (0 * prox_values[0] + 4 * prox_values[1] + 1 * prox_values[2]) / 5.;

        double ds_left = (NORM_SPEED * prox_left) / MAX_PROX;
        double ds_right = (NORM_SPEED * prox_right) / MAX_PROX;
        
        double speed_left = bounded_speed(NORM_SPEED - ds_right);
        double speed_right = bounded_speed(NORM_SPEED - ds_left);

        set_speed(speed_left, speed_right);
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




