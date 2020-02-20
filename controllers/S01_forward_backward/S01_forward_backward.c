
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif

void robot_setup() {
    init_robot();
    init_sensors();
}

void robot_loop() {

    int counter = 0;
    while (robot_go_on()) {
      int dir = (++counter % 100) > 50 ? -1 : 1; 
      set_speed(dir * NORM_SPEED,dir * NORM_SPEED);
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




