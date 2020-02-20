#ifndef WEBOTSAPI_H
#define WEBOTSAPI_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 0

#include <webots/robot.h>
#define TIME_STEP 64

void init_robot();
void init_sensors();
void disable_sensors();
int get_robot_ID();

void cleanup_robot();

int robot_go_on();

/*** ROBOT CONTROL start ***/
#include <webots/motor.h>
#define MAX_SPEED 6.0
#define NORM_SPEED 2.0
#include <webots/led.h>
#define LED_COUNT 4

void set_speed(double left, double right);
double bounded_speed(double speed);

void toggle_led(int led_position);
void disable_led(int led_position);
void enable_led(int led_position);

/*** ROBOT CONTROL end ***/


/*** PROXIMITY SENSORS start ***/
#include <webots/distance_sensor.h>
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
/*** PROXIMITY SENSORS end ***/


/*** GROUND SENSORS start ***/
#define GROUND_SENSORS_COUNT 3
#define GS_LEFT 0
#define GS_CENTER 1
#define GS_RIGHT 2

void get_ground(short int *ground_values);
/*** GROUND SENSORS end ***/


/*** CAMERA start ***/
#include <webots/camera.h>
#define CAM_RATE 8
#define CAMERA_WIDTH 160
#define CAMERA_HEIGHT 120

void get_camera(unsigned char *red, unsigned char *green, unsigned char *blue);

void init_camera();
void disable_camera();
/*** CAMERA end ***/

/*** COMMUNICATION start ***/
#include <webots/emitter.h>
#include <webots/receiver.h>

#define COM_CHANNEL 1
#define MSG_NONE "ZZZZ"

#define MSG_LENGTH 4

void init_communication();
void disable_communication();
void send_msg(const char *snd);
void receive_msg(char *rcv);

/*** COMMUNICATION end ***/

#endif // WEBOTSAPI_H
