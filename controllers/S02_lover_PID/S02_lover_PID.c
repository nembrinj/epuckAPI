
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif

#define PID_LOVER_TARGET_A 150
#define PID_LOVER_TARGET_B 200
#define PID_COUNTER 10

#define K 10
#define T_I 300
#define T_D 0.15

#define TIME_STEP 64
#define CYCLE 500


double error = 0;
double deriv = 0;
double integ = 0;

int counter = 0;


int target() {
  return (counter % CYCLE < CYCLE/2 ? PID_LOVER_TARGET_A: PID_LOVER_TARGET_B);
}

double pid(int tof, int counter) {

  double prev_err = error;
  error = tof - target();
  
  //deriv = (error - prev_err);
  deriv = (error - prev_err)*1000/TIME_STEP;
  //integ += error;
  integ += error*TIME_STEP/1000;
    
  return K * ( error + 1.0 / T_I * integ + T_D * deriv);
}

void robot_setup() {
    init_robot();
    init_sensors();
}

void robot_loop() {
	short int tof[TOF_SENSOR_COUNT];
	
	// open files for writing
    FILE *log = fopen("logPID.csv", "w");

    if (log == NULL) {
        printf("Error opening file!\n");
        return ;
    } 

    // write header in CSV file
    fprintf(log, "time,target,sensor,P,I,D,ds\n");


    while (robot_go_on()) {
    
        counter++;

        get_tof(tof);
                
        //compute PID response according to tof sensor value
        double ds = pid(tof[0],counter);      
      
        // adjust speed values to distance from obstacle
        double speed_right =  ds;
        double speed_left  =  ds;          
                               
        // write a line of data in log file
        fprintf(log, "%d,%d,%d,%f,%f,%f,%f\n", counter,target(),tof[0], K*error, K*integ/T_I, K*deriv*T_D, ds);
        
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




