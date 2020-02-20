#ifndef EPUCKAPI_H
#define EPUCKAPI_H

#include <stdlib.h>
#include <stdio.h>

// for opening socket to the robot
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

// for communication API
#include <sys/ipc.h> 
#include <sys/types.h>
#include <sys/msg.h> 
#include <sys/shm.h> 

// to create dir
#include <sys/stat.h>
#include <unistd.h>

#include <string.h>

#define DEBUG 0

char *ip;

void init_robot();
void init_sensors();
void disable_sensors();
int get_robot_ID();

void cleanup_robot();

int robot_go_on();

/*** ROBOT CONTROL start ***/
#define MAX_SPEED 1000
#define NORM_SPEED 400
#define LED_COUNT 4

void set_speed(double left, double right);
double bounded_speed(double speed);

void toggle_led(int led_position);
void disable_led(int led_position);
void enable_led(int led_position);

void enable_body_led(void);
void disable_body_led(void);

void enable_front_led(void);
void disable_front_led(void);


/*** ROBOT CONTROL end ***/



/*** PROXIMITY SENSORS start ***/
#define PROX_SENSORS_COUNT 8
#define NBR_CALIB 50
#define OFFSET_CALIB 5

#define PROX_RIGHT_FRONT 0
#define PROX_RIGHT_FRONT_DIAG 1
#define PROX_RIGHT_SIDE 2
#define PROX_RIGHT_BACK 3
#define PROX_LEFT_BACK 4
#define PROX_LEFT_SIDE 5
#define PROX_LEFT_FRONT_DIAG 6
#define PROX_LEFT_FRONT 7

#define MAX_PROX 200

void get_prox(short int *prox_values);
void get_prox_calibrated(short int *prox_values);
void calibrate_prox();

void get_light(short int *prox_values);
void get_light_calibrated(short int *prox_values);
void calibrate_light();

/*** PROXIMITY SENSORS end ***/


/*** GROUND SENSORS start ***/
#define GROUND_SENSORS_COUNT 3
#define GS_LEFT 0
#define GS_CENTER 1
#define GS_RIGHT 2

void get_ground(short int *ground_values);
/*** GROUND SENSORS end ***/


/*** CAMERA start ***/
#define CAMERA_WIDTH 160
#define CAMERA_HEIGHT 120

void get_camera(unsigned char *red, unsigned char *green, unsigned char *blue);

void init_camera();
void disable_camera();
/*** CAMERA end ***/




/*** TEMPERATURE start ***/

#define TEMP_SENSOR_COUNT 1
void get_temp(unsigned char *temp);

/*** TEMPERATURE stop ***/

/*** TOF start ***/

#define TOF_SENSOR_COUNT 1
void get_tof(short int *tof_distance);

/*** TOF stop ***/

/*** ACCELEROMETER start ***/


#define AXES_X 0
#define AXES_Y 1
#define AXES_Z 2

// instantaneous rotational speed for each axis 
#define GYRO_SENSOR_COUNT 3
void get_gyro_axes( short *gyro);


// planar orientation of acceleration vector (relative to robot)
#define ORIENT_SENSOR_COUNT 1
void get_orientation(float *orientation);

// inclination to vertical of acceleration vector  
#define INCLIN_SENSOR_COUNT 1
void get_inclination(float *inclination);

// magnitude of acceleration vector  
#define ACC_SENSOR_COUNT 1
void get_acceleration(float *acceleration);

// raw acceleration values for each axis
#define ACC_RAW_SENSOR_COUNT 3
void get_acceleration_axes(short int *acceleration);

/*** ACCELEROMETER stop ***/






/*** SOUND start ***/

void play_sound(int sound);
void stop_sound(void);

#define MICROPHONE_COUNT 4

#define MICROPHONE_FRONT 0
#define MICROPHONE_RIGHT 1
#define MICROPHONE_BACK 2
#define MICROPHONE_LEFT 3

void get_microphones(short int *soundlevels);


/*** SOUND stop ***/



/*** COMMUNICATION start ***/

#define COM_CHANNEL 1
#define MSG_NONE "ZZZZ"

#define MSG_LENGTH 4


void init_communication();
void disable_communication();
void send_msg(const char *snd);
void receive_msg(char *rcv);


/*** COMMUNICATION end ***/



#endif // EPUCKAPI_H
