
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif

short int IR_prox[PROX_SENSORS_COUNT];
short int IR_prox_calib[PROX_SENSORS_COUNT];

void robot_setup() {

    init_robot();
    init_sensors();
    
    calibrate_prox();
}

#define MAXSTEPS 50

void robot_loop() {
  
    // open files for writing
  FILE *calib = fopen("calib.csv", "w");
  FILE *uncalib = fopen("uncalib.csv", "w");
  
  if (calib == NULL) {
    printf("Error opening calib file!\n");
    return;    
  } else if (uncalib == NULL) {
    printf("Error opening uncalib file!\n");
    return;
  }
  
  // write header in CSV file
  fprintf(calib, "time,");
  fprintf(uncalib, "time,");
  for (int i=0; i<PROX_SENSORS_COUNT; i++) {
    fprintf(calib, "ps%d,", i);
    fprintf(uncalib, "ps%d,", i);
  }
  fprintf(calib, "\n");
  fprintf(uncalib, "\n");
  
  // wait for a while 
  for (int step=0; step<MAXSTEPS; step++)  {
    robot_go_on();
  }

  for (int step=0; step<MAXSTEPS; step++)  {
      
    robot_go_on();
    
    get_prox(IR_prox);
    get_prox_calibrated(IR_prox_calib);
        
    // write a line of data in each file
    fprintf(calib, "%d,", step);
    fprintf(uncalib, "%d,", step); 
    for (int i=0; i<PROX_SENSORS_COUNT; i++) {
      fprintf(calib, "%d,", IR_prox_calib[i]);
      fprintf(uncalib, "%d,", IR_prox[i]);
    }
    fprintf(calib, "\n");
    fprintf(uncalib, "\n");    

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






