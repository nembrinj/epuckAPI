
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

        double prox_left = (4 * prox_values[7] + 2 * prox_values[6] + prox_values[5]) / 7.;
        double prox_right = (4 * prox_values[0] + 2 * prox_values[1] + prox_values[2]) / 7.;

        double ds_left = (NORM_SPEED * prox_left) / MAX_PROX;
        double ds_right = (NORM_SPEED * prox_right) / MAX_PROX;
        
        double speed_left = bounded_speed(NORM_SPEED - ds_left);
        double speed_right = bounded_speed(NORM_SPEED - ds_right);

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




