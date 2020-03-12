
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif

short int IR_ground[GROUND_SENSORS_COUNT];

void robot_setup() {

    init_robot();
    init_sensors();    
}

#define MAXSTEPS 200

void robot_loop() {
  
    // open files for writing
  FILE *uncalib = fopen("uncalib.csv", "w");
  
  if (uncalib == NULL) {
    printf("Error opening calib file!\n");
    return;    
  }
  
  // write header in CSV file
  fprintf(uncalib, "time,");
  for (int i=0; i<GROUND_SENSORS_COUNT; i++) {
    fprintf(uncalib, "gs%d,", i);
  }
  fprintf(uncalib, "\n");
  
  // wait for a while 
  for (int step=0; step<10; step++)  {
    robot_go_on();
  }

  for (int step=0; step<MAXSTEPS; step++)  {
      
    robot_go_on();
    
    get_ground(IR_ground);
        
    // write a line of data in each file
    fprintf(uncalib, "%d,", step); 
    for (int i=0; i<GROUND_SENSORS_COUNT; i++) {
      fprintf(uncalib, "%d,", IR_ground[i]);
    }
    fprintf(uncalib, "\n");    
    
    set_speed(NORM_SPEED,NORM_SPEED);
  }  
  
  //stop robot at the end
  set_speed(0,0);
  robot_go_on();
  
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






