
#define SIMULATION 1

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif

#define WAIT_STEPS 20 
#define MAX_SAMPLES 10

void robot_setup() {
    init_robot();
    init_camera();
}

void robot_loop() {

  unsigned char red[CAMERA_WIDTH * CAMERA_HEIGHT];
  unsigned char green[CAMERA_WIDTH * CAMERA_HEIGHT];
  unsigned char blue[CAMERA_WIDTH * CAMERA_HEIGHT];


  // wait for a while 
  for (int step=0; step<WAIT_STEPS; step++)  {
    robot_go_on();
  }

  for (int step=0; step<MAX_SAMPLES; step++)  {
      
    robot_go_on();
    
    // this will record images in the ./images folder
    get_camera(red, green, blue);
        
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




