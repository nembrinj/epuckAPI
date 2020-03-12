
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif

// states
#define LOVER 1
#define EXPLORER 2
#define AVOID 3


#define COUNT_EXP 5
#define COUNT_LOV 20
#define COUNT_OBST 1
#define THRESH_SPEED 100
#define THRESH_PROX_LO 50
#define THRESH_PROX_HI 200


void robot_setup() {
    init_robot();
    init_sensors();
    calibrate_prox();
}

void robot_loop() {
	short int prox_values[8];
	
	int counter_wall = 0;
	int counter_expl = 0;
	int counter_obstacle = 1;
    int state = EXPLORER;


    while (robot_go_on()) {

        get_prox_calibrated(prox_values);

        double prox_left = (4 * prox_values[7] + 2 * prox_values[6] + prox_values[5]) / 7.;
        double prox_right = (4 * prox_values[0] + 2 * prox_values[1] + prox_values[2]) / 7.;

        double ds_left = (NORM_SPEED * prox_left) / MAX_PROX;
        double ds_right = (NORM_SPEED * prox_right) / MAX_PROX;
        
        double speed_left = NORM_SPEED - ( (state == EXPLORER)||(state == AVOID) ? ds_right : ds_left);;
        double speed_right = NORM_SPEED - ( (state == EXPLORER)||(state == AVOID) ? ds_left : ds_right);;
        
        
        // increment counters
        if (state == LOVER) {
          if ((abs(speed_right) < THRESH_SPEED) && (abs(speed_left) < THRESH_SPEED))
              counter_wall++;
        } else if (state == EXPLORER){
            if ((abs(prox_right) > THRESH_PROX_HI) || (abs(prox_left) > THRESH_PROX_HI)) 
                counter_expl++;
        } else if (state == AVOID){            
            if ((abs(prox_right) < THRESH_PROX_LO) && (abs(prox_left) < THRESH_PROX_LO)) {
                counter_expl++;
            } 
        }
        
        // state machine
        if (state == EXPLORER) { 
            if (counter_obstacle > COUNT_OBST) {
              state = LOVER;
              counter_wall = 0;
              counter_obstacle = 0;
              for (int i=0; i<LED_COUNT; i++)
                toggle_led(i);
            } else if (counter_expl > COUNT_EXP) {
              state = AVOID;
              counter_expl = 0;
            }
        } else if ((state == AVOID) && (counter_expl > COUNT_EXP)) {
              counter_expl = 0;
              counter_obstacle++;
              state = EXPLORER; 
        } else if ((state == LOVER) && (counter_wall > COUNT_LOV)) {
              counter_expl = 0;
              counter_obstacle = 0;
              state = AVOID; 
              for (int i=0; i<LED_COUNT; i++)
                toggle_led(i);
        } 

        speed_left = bounded_speed(speed_left);
        speed_right = bounded_speed(speed_right);

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




