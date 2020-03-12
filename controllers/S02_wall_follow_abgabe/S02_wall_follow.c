
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif




#define TIME_STEP 64


#define K xxx
#define T_I xxx
#define T_D xxx
#define PID_WALL_FOLLOW_TARGET xxx
#define PID_MAX_DS 200

double error = 0;
double deriv = 0;
double integ = 0;

double pid(double proxLR) {

  double prev_err = error;
  error = proxLR - PID_WALL_FOLLOW_TARGET;
  
  //deriv = (error - prev_err);
  deriv = (error - prev_err)*1000/TIME_STEP;
  //integ += error;
  integ += error*TIME_STEP/1000;
    
  return K * ( error + 1.0 / T_I * integ + T_D * deriv);
}

void robot_setup() {
    init_robot();
    init_sensors();
    calibrate_prox();
}

void robot_loop() {
	short int prox_values[PROX_SENSORS_COUNT];
	
	// open files for writing
    FILE *log = fopen("logPID.csv", "w");

    if (log == NULL) {
        printf("Error opening file!\n");
        return ;
    } 

    // write header in CSV file
    fprintf(log, "time,P,I,D,ds,left speed,right speed\n");

    int loop_counter = 0;

    while (robot_go_on()) {
    
        loop_counter++;

        get_prox_calibrated(prox_values);

        double prox_right = (a * prox_values[0] + b * prox_values[1] + c * prox_values[2] + d * prox_values[3]) / (a+b+c+d);
                      
        //compute PID response according to IR sensor value
        double ds = pid(prox_right);      
      
        // adjust speed values to turn away from obstacle
        double speed_right = (abs(ds)>PID_MAX_DS ? 0 : NORM_SPEED) + ds;
        double speed_left  = (abs(ds)>PID_MAX_DS ? 0 : NORM_SPEED) - ds;          
            
        // write a line of data in log file
        fprintf(log, "%d,%f,%f,%f,%f,%f,%f\n", loop_counter, K*error, K*integ/T_I, K*deriv*T_D, ds, speed_left,speed_right);

        set_speed(bounded_speed(speed_left), bounded_speed(speed_right));
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




