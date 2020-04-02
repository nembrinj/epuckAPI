
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

const char *led_names[LED_COUNT+2] =   {"led0", "led2", "led4", "led6","led8","led9"};
//  {"led0", "led1", "led2", "led3","led4", "led5", "led6", "led7"};
WbDeviceTag led_tags[LED_COUNT+2];

void init_led(){  
  int i;
  // init leds
  for (i=0;  i<LED_COUNT+2; i++) {
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

void enable_body_led() {
  wb_led_set(led_tags[4], 1);
}

void disable_body_led() {
  wb_led_set(led_tags[4], 0);
}

void enable_front_led() {
  wb_led_set(led_tags[5], 1);
}

void disable_front_led() {
  wb_led_set(led_tags[5], 0);
}


const char *rgbled_names[LED_COUNT] =   {"led1", "led3", "led5", "led7"};
//  {"led0", "led1", "led2", "led3","led4", "led5", "led6", "led7"};
WbDeviceTag rgbled_tags[LED_COUNT];

void init_rgbled(){  
  int i;
  // init leds
  for (i=0;  i<LED_COUNT; i++) {
    rgbled_tags[i] = wb_robot_get_device(rgbled_names[i]);
  }
}

void enable_rgbled(int led_position, int led_color){
  wb_led_set(rgbled_tags[led_position], led_color);
}

void disable_rgbled(int led_position){
  wb_led_set(rgbled_tags[led_position], 0);
}






// /////////////
// IR sensors


const char *prox_sensors_names[PROX_SENSORS_COUNT] =
  {"ps0", "ps1", "ps2", "ps3", "ps4", "ps5", "ps6", "ps7"};
WbDeviceTag prox_sensor_tags[PROX_SENSORS_COUNT];


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

const char *light_sensors_names[PROX_SENSORS_COUNT] =
  {"ls0", "ls1", "ls2", "ls3", "ls4", "ls5", "ls6", "ls7"};
WbDeviceTag light_sensor_tags[PROX_SENSORS_COUNT];




void init_light() {
  int i;
  // init light sensors
  for (i=0;  i<PROX_SENSORS_COUNT; i++) {
    light_sensor_tags[i] = wb_robot_get_device(light_sensors_names[i]);
    wb_light_sensor_enable(light_sensor_tags[i], TIME_STEP);
  }
}


void calibrate_light() {
  printf("no calibration in simulation: get_light_calibrated() is equivalent to get_light()\n");
}




void get_light(short int *light_values) {
  int i;
  
  //printf("%f\n",wb_light_sensor_get_value(light_sensor_tags[3]));
  for (i=0; i<PROX_SENSORS_COUNT; i++) {
    light_values[i] = wb_light_sensor_get_value(light_sensor_tags[i]);
  }
}

void get_light_calibrated(short int *light_values) {
    get_light(light_values);
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
  
  // create dir images
  struct stat st = {0};

  if (stat("./images", &st) == -1) {
    #if defined(_WIN32)
      mkdir("./images");
    #else
      mkdir("./images", 0700);
    #endif
  }

}

void disable_camera() {
  wb_camera_disable(cam);
}


// https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
// https://en.wikipedia.org/wiki/BMP_file_format#File_structure

void save_bmp_image(const unsigned char *image) {
	static int image_counter = 0;
	char filename[32];
	
	
	sprintf(filename, "images/image%03d.bmp", image_counter++);

	int width = CAMERA_WIDTH;
	int height = CAMERA_HEIGHT;

	int filesize = 54 + 3 * width * height;
	unsigned char bmpfileheader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
	
	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);
	
	unsigned char bmpinfoheader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};
	
	bmpinfoheader[4] = (unsigned char)(width);
	bmpinfoheader[5] = (unsigned char)(width >> 8);
	bmpinfoheader[6] = (unsigned char)(width >> 16);
	bmpinfoheader[7] = (unsigned char)(width >> 24);
	bmpinfoheader[8] = (unsigned char)(height);
	bmpinfoheader[9] = (unsigned char)(height >> 8);
	bmpinfoheader[10] = (unsigned char)(height >> 16);
	bmpinfoheader[11] = (unsigned char)(height >> 24);
	
	unsigned char bmppad[3] = {0, 0, 0};
	
	FILE *f = fopen(filename, "wb");
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);
	for (int i = 0; i < height; i++) {
		// https://stackoverflow.com/questions/7886196/c-pointer-1-meaning/7886232
		fwrite(image + (width * (height - i - 1) * 3), 3, width, f);
		fwrite(bmppad, 1, (4 - (width * 3) % 4) % 4, f);
	}
	fclose(f);
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
  
  unsigned char bgr888[CAMERA_WIDTH * CAMERA_HEIGHT * 3];

  for (int i = 0; i < CAMERA_HEIGHT * CAMERA_WIDTH; i++) {
		bgr888[3 * i + 2] = red[i];
		bgr888[3 * i + 1] = green[i];
		bgr888[3 * i] = blue[i];
  }
  
  save_bmp_image(bgr888);
}   





/*
// get inclination values
void get_inclination(unsigned char *inclination_values) { //8-bits value
	inclination_values[INCLINATION_FRONT] = *(unsigned char *)(&sensor[14]);
	inclination_values[INCLINATION_RIGHT] = *(unsigned char *)(&sensor[15]);
	inclination_values[INCLINATION_BACK] = *(unsigned char *)(&sensor[16]);
	inclination_values[INCLINATION_LEFT] = *(unsigned char *)(&sensor[17]);
}
*/

// //////////////
// temperature sensor (credits: Jonas Fontana)


// get temperature
void get_temp(unsigned char *temp) {
    printf("no temperature in Webots\n");
	temp[0] = -1;
}


// //////////////
// time-of-flight sensor (credits: Jonas Fontana)


const char *tof_sensors_names[1] = {"tof"};
WbDeviceTag tof_sensor_tags[1];




void init_tof() {
  // init time of flight sensor
  tof_sensor_tags[0] = wb_robot_get_device(tof_sensors_names[0]);
  wb_distance_sensor_enable(tof_sensor_tags[0], TIME_STEP);
}


// get distance given in millimeters
void get_tof(short int *tof_distance) {
  //printf("%f\n",wb_distance_sensor_get_value(tof_sensor_tags[0]));
  tof_distance[0] =  wb_distance_sensor_get_value(tof_sensor_tags[0]);
}


// //////////////
// accelerometer values (credits: Jonas Fontana)

// get gyro
void get_gyro_axes( short *gyro) {
    printf("no gyroscope in Webots\n");
	gyro[AXES_X] = -1;
	gyro[AXES_Y] = -1;
	gyro[AXES_Z] = -1;
}



const char *acc_sensors_names[1] =
  {"accelerometer"};
WbDeviceTag accelerometer_tags[1];

void init_accelerometer() {
  accelerometer_tags[0] = wb_robot_get_device(acc_sensors_names[0]);
  wb_accelerometer_enable(accelerometer_tags[0], TIME_STEP);
}

// get orientation
void get_orientation(float *orientation) {
    const double* acc = wb_accelerometer_get_values(accelerometer_tags[0]);
	orientation[0] = atan2(acc[0],acc[1]);
}

// get inclination
void get_inclination(float *inclination) {
    const double* acc = wb_accelerometer_get_values(accelerometer_tags[0]);
	inclination[0] =  atan2(sqrt( acc[0]*acc[0] + acc[1]*acc[1]),acc[2]);
}

// get acceleration
void get_acceleration(float *acceleration) {
    const double* acc = wb_accelerometer_get_values(accelerometer_tags[0]);
	acceleration[0] = sqrt( acc[0]*acc[0] + acc[1]*acc[1] + acc[2]*acc[2])*1000.0;
}

// get acceleration
void get_acceleration_axes(short int *acceleration) {
    const double* acc = wb_accelerometer_get_values(accelerometer_tags[0]);
    //printf("%f\n",acc_val[2]);
	acceleration[AXES_Y] = acc[1]*1000.0;
	acceleration[AXES_X] = acc[0]*1000.0;
	acceleration[AXES_Z] = acc[2]*1000.0;
}






// //////////////
// playing sound (credits: Jonas Fontana)


// play one of the default sounds
void play_sound(int sound) {
	printf("no support for playing sounds yet\n");
}

// stop sound
void stop_sound(void) {
	// does nothing on webots
}

void get_microphones(short int *soundlevels) {
    printf("no microphones in Webots\n");
	soundlevels[MICROPHONE_RIGHT] = -1;
	soundlevels[MICROPHONE_LEFT] = -1;
	soundlevels[MICROPHONE_BACK] = -1;
	soundlevels[MICROPHONE_FRONT] = -1;
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

// thanks to Daniel Borcard
int get_robot_ID() {
    int a;
    // parse robot name
    if (sscanf(wb_robot_get_name(), "%i",&a) < 1) {
        printf("please provide a valid name\n");
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
  init_rgbled();
}

void init_sensors() {
  init_prox();
  init_light();
  init_ground();
  init_accelerometer();
  init_tof();
}

int robot_go_on() {
  return wb_robot_step(TIME_STEP)!=-1;
}

void cleanup_robot() {
  wb_robot_cleanup();
}




