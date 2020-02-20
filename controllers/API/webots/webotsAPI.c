
#include "webotsAPI.h"




WbDeviceTag left_motor, right_motor;

void init_motor() {

  // get a handler to the motors 
  left_motor = wb_robot_get_device("left wheel motor");
  right_motor = wb_robot_get_device("right wheel motor");
  
  // set target position to infinity (speed control)
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  
  // set speed to 0
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);

}


// constrain speed to ± MAX_SPEED
double bounded_speed(double speed){
	return (speed > MAX_SPEED) ? MAX_SPEED : (speed < -MAX_SPEED) ? -MAX_SPEED : speed;
}


void set_speed(double left, double right) {
  wb_motor_set_velocity(left_motor, left);
  wb_motor_set_velocity(right_motor, right);
}


// /////////////
// LEDs

const char *led_names[LED_COUNT] =   {"led1", "led3", "led5", "led7"};
//  {"led0", "led1", "led2", "led3","led4", "led5", "led6", "led7"};
WbDeviceTag led_tags[LED_COUNT];

void init_led(){  
  int i;
  // init leds
  for (i=0;  i<LED_COUNT; i++) {
    led_tags[i] = wb_robot_get_device(led_names[i]);
  }
}


void toggle_led(int led_position) {
  wb_led_set(led_tags[led_position],wb_led_get(led_tags[led_position]) ? 1 : 0);
}

void enable_led(int led_position){
  wb_led_set(led_tags[led_position], 1);
}

void disable_led(int led_position){
  wb_led_set(led_tags[led_position], 0);
}


// /////////////
// IR sensors


const char *prox_sensors_names[PROX_SENSORS_COUNT] =
  {"ps0", "ps1", "ps2", "ps3", "ps4", "ps5", "ps6", "ps7"};
WbDeviceTag prox_sensor_tags[PROX_SENSORS_COUNT];
double prox_corr_vals[PROX_SENSORS_COUNT];


void init_prox() {
  int i;
  // init prox sensors
  for (i=0;  i<PROX_SENSORS_COUNT; i++) {
    prox_sensor_tags[i] = wb_robot_get_device(prox_sensors_names[i]);
    wb_distance_sensor_enable(prox_sensor_tags[i], TIME_STEP);
  }
}

void get_prox(short int *prox_values) {
  int i;
  for (i=0; i<PROX_SENSORS_COUNT; i++) {
    prox_values[i] = wb_distance_sensor_get_value(prox_sensor_tags[i]);
  }
}

void get_prox_calibrated(short int *prox_values) {
    get_prox(prox_values);
}

void calibrate_prox() {
  printf("no calibration in simulation: get_prox_calibrated() is equivalent to get_prox()\n");
}



const char *ground_sensors_names[GROUND_SENSORS_COUNT] =
  {"gs0", "gs1", "gs2"};
WbDeviceTag gs[GROUND_SENSORS_COUNT]; /* ground sensors */
unsigned short gs_value[GROUND_SENSORS_COUNT]={0,0,0};


void init_ground() {
  for (int i = 0; i < GROUND_SENSORS_COUNT; i++) {
      gs[i] = wb_robot_get_device(ground_sensors_names[i]); /* ground sensors */
      wb_distance_sensor_enable(gs[i],TIME_STEP);
  }
}


void get_ground(short int *ground_values) {
  // fetch ground sensor values
  for(int i=0;i<GROUND_SENSORS_COUNT;i++) {
    ground_values[i] = wb_distance_sensor_get_value(gs[i]);
  }
}



// /////////////
// Camera


WbDeviceTag cam;

void init_camera() {
  cam = wb_robot_get_device("camera");
  wb_camera_enable(cam,CAM_RATE*TIME_STEP);
}

void disable_camera() {
  wb_camera_disable(cam);
}


void get_camera(unsigned char *red, unsigned char *green, unsigned char *blue) {
  const unsigned char* im = wb_camera_get_image(cam);
  int height = CAMERA_HEIGHT;
  int width = CAMERA_WIDTH;

  int m,n;

  for (n=0; n<height; n++) {                  
    for (m=0; m<width; m++) {
      //gr[n*width+m] = wb_camera_image_get_grey(im, width, m, n);
      red[n*width+m] = wb_camera_image_get_red(im, width, m, n);
      green[n*width+m] = wb_camera_image_get_green(im, width, m, n);
      blue[n*width+m] = wb_camera_image_get_blue(im, width, m, n);
    }
  } 
}   

// ///////////////////
// Communication

WbDeviceTag emitter,receiver;


void init_communication() {
  emitter = wb_robot_get_device("emitter");
  receiver = wb_robot_get_device("receiver");
  
  // setup communication
  wb_emitter_set_channel(emitter, COM_CHANNEL);
  wb_receiver_enable(receiver, TIME_STEP);
  wb_receiver_set_channel(receiver, COM_CHANNEL);
}


void disable_communication() {
  // does nothing up to now....
}

// send the message
void send_msg(const char *msg) {
  wb_emitter_send(emitter, msg, strlen(msg) + 1);
}


// receive message
void receive_msg(char* buffer) {  
  strcpy(buffer,MSG_NONE);
  
  // check if there is a packet in the packet queue
  if (wb_receiver_get_queue_length(receiver) > 0) {
    // read current packet's data
    strcpy(buffer,(char *) wb_receiver_get_data(receiver));
    // flush the current packet and get the next one
    wb_receiver_next_packet(receiver);
  }

}


int get_robot_ID() {
    int a;
    // parse webots name
    const char* name = wb_robot_get_name();
    if (sscanf(name, "%i", &a) < 1) {
      printf("please provide an integer for the robot name in webots\n");
      return -1;
    }
    else return a;
}




// ///////////////
// Initialisation


void init_robot() {
  wb_robot_init();
  init_motor();
  init_led();
}

void init_sensors() {
  init_prox();
  init_ground();
}

int robot_go_on() {
  return wb_robot_step(TIME_STEP)!=-1;
}

void cleanup_robot() {
  wb_robot_cleanup();
}




