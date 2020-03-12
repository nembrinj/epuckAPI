
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif


unsigned char temp[TEMP_SENSOR_COUNT];
short int tof[TOF_SENSOR_COUNT];

short gyro[GYRO_SENSOR_COUNT];

float orientation[ORIENT_SENSOR_COUNT];

float inclination[INCLIN_SENSOR_COUNT];

float acceleration[ACC_SENSOR_COUNT];
short int acceleration_raw[ACC_RAW_SENSOR_COUNT];

short int microphone[MICROPHONE_COUNT];


void robot_setup() {
    init_robot();
    init_sensors();
}

#define MAXSTEPS 200

void robot_loop() {
    
        // open files for writing
  FILE *uncalib = fopen("uncalib.csv", "w");
  
  if (uncalib == NULL) {
    printf("Error opening record file!\n");
    return;    
  }
  
  // write header in CSV file
  fprintf(uncalib, "time,temperature,tof,");
  fprintf(uncalib, "gyro X,gyro Y,gyro Z,acc X,acc Y,acc Z,");
  fprintf(uncalib, "orientation,inclination,acceleration,");
  fprintf(uncalib, "micro front,micro right,micro back,micro left,");
  fprintf(uncalib, "\n");
  
  // wait for a while 
  for (int step=0; step<10; step++)  {
    robot_go_on();
  }

  for (int step=0; step<MAXSTEPS; step++)  {
      
    robot_go_on();
    
	get_temp(temp);
	printf("temperature : %d\n", temp[0]);

	get_tof(tof);
    printf("tof : %d\n", tof[0]);

    get_gyro_axes(gyro);
    printf("gyro");
    for (int i=0; i<GYRO_SENSOR_COUNT; i++) {
      printf(" %d",gyro[i]);
    }
    printf("\n");

    get_acceleration_axes(acceleration_raw);
    printf("Acceleration X: %d\n", acceleration_raw[0]);
    printf("Acceleration Y: %d\n", acceleration_raw[1]);
    printf("Acceleration Z: %d\n", acceleration_raw[2]);

    get_orientation(orientation);
    printf("Orientation : %f\n", orientation[0]);

    get_inclination(inclination);
    printf("Inclination : %f\n", inclination[0]);

    get_acceleration(acceleration);
    printf("Acceleration : %f\n", acceleration[0]);

    get_microphones(microphone);
    printf("microphone");
    for (int i=0; i<MICROPHONE_COUNT; i++) {
      printf(" %d",microphone[i]);
    }
    printf("\n");
    
    // write a line of data in each file
    fprintf(uncalib, "%d,", step); 

    fprintf(uncalib, "%u,", temp[0]);

    fprintf(uncalib, "%d,", tof[0]);

    fprintf(uncalib, "%d,", gyro[AXES_X]);
    fprintf(uncalib, "%d,", gyro[AXES_Y]);
    fprintf(uncalib, "%d,", gyro[AXES_Z]);

    fprintf(uncalib, "%d,", acceleration_raw[AXES_X]);
    fprintf(uncalib, "%d,", acceleration_raw[AXES_Y]);
    fprintf(uncalib, "%d,", acceleration_raw[AXES_Z]);

    fprintf(uncalib, "%f,", (double) orientation[0]);
    fprintf(uncalib, "%f,", (double) inclination[0]);
    fprintf(uncalib, "%f,", (double) acceleration[0]);

    fprintf(uncalib, "%d,", microphone[MICROPHONE_FRONT]);
    fprintf(uncalib, "%d,", microphone[MICROPHONE_RIGHT]);
    fprintf(uncalib, "%d,", microphone[MICROPHONE_BACK]);
    fprintf(uncalib, "%d,", microphone[MICROPHONE_LEFT]);

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






