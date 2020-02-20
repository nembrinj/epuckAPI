
#include "epuckAPI.h"


unsigned char command[21];
unsigned char sensor[104];

unsigned char rgb565[CAMERA_WIDTH * CAMERA_HEIGHT * 2];
unsigned char bgr888[CAMERA_WIDTH * CAMERA_HEIGHT * 3];
int camera_updated = 0;


/*** TCP CONNECTION start ***/
struct sockaddr_in remote_addr;
int fd;
int ret_value;
int bytes_sent = 0, bytes_recv = 0;
unsigned char header;
struct timeval start_time, curr_time;	
int32_t time_diff_us = 0, refresh = 0;
uint16_t num_packets = 0;
/*** TCP CONNECTION end***/

void print_command(){
	for (int k = 0; k < 21; ++k){
		for (int j = 0; j < 8; ++j){
			printf("%d", !!((command[k] << j) & 0x80));
		}
		printf(" ");
	}
	printf("\n");
}

void init_TCP(char *ipaddr) {
	// INIT TCP CONNECTION
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(ipaddr);
	remote_addr.sin_port = htons(1000);
	
	fprintf(stderr, "Try to connect to %s:%d (TCP)\n", inet_ntoa(remote_addr.sin_addr), htons(remote_addr.sin_port));
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		perror("TCP cannot create socket: ");
		exit(1);
	}
	
	ret_value = connect(fd, (struct sockaddr *) &remote_addr, sizeof(remote_addr));
	if(ret_value < 0) {
		perror("TCP cannot connect: ");
		exit(1);
	}
	
	fprintf(stderr, "Connected to %s:%d (TCP)\n", inet_ntoa(remote_addr.sin_addr), htons(remote_addr.sin_port));
}

void init_command() {
	// http://www.gctronic.com/doc/index.php?title=e-puck2_PC_side_development#WiFi_2

	command[0] = 0x80;
	command[1] = 0;			// Sensors enabled.
	command[2] = 0;			// Calibrate proximity sensors.
	command[3] = 0;			// left motor LSB
	command[4] = 0;			// left motor MSB
	command[5] = 0;			// right motor LSB
	command[6] = 0;			// right motor MSB
	command[7] = 0xC0;		// LEDs
	command[8] = 0;			// LED2 red
	command[9] = 0;			// LED2 green
	command[10] = 0;		// LED2 blue
	command[11] = 0;		// LED4 red
	command[12] = 0;		// LED4 green
	command[13] = 0;		// LED4 blue
	command[14] = 0;		// LED6 red
	command[15] = 0;		// LED6 green
	command[16] = 0;		// LED6 blue
	command[17] = 0;		// LED8 red
	command[18] = 0;		// LED8 green
	command[19] = 0;		// LED8 blue
	command[20] = 0;		// speaker
}


void init_camera(){
	printf("Camera enabled\n");
	// Force last bit to 1
	command[1] = command[1] | 1;

	for (int i = 0; i < CAMERA_WIDTH * CAMERA_HEIGHT * 3; i++){
		bgr888[i] = 0;
	}

	for (int i = 0; i < CAMERA_WIDTH * CAMERA_HEIGHT * 2; i++){
		rgb565[i] = 0;
	}
	
	// create dir images
    struct stat st = {0};

    if (stat("./images", &st) == -1) {
        mkdir("./images", 0700);
    }

}

void disable_camera(){
	printf("Camera disabled\n");
	// Force last bit to 0
	command[1] = command[1] & 0xFE;
}

void init_sensors(){
	// Force second to last bit to 1
	command[1] = command[1] | (1 << 1);

	// Start sensor calibration
	//command[2] = 1;
	command[2] = 0; // do not calibrate on board, use API function
}

void disable_sensors(){
	// Force second to last bit to 0
	command[1] = command[1] & 0xFD;
}


void init_robot() {
	init_TCP(ip);
	init_command();
}

void cleanup_robot() {
  disable_camera();
  for (int i=0; i < 50; i++) {
    set_speed(0, 0);
    robot_go_on();
  }  
  shutdown(fd,2);
  //close(fd);
}


void send_command() {
	// print_command();

	bytes_sent = 0;
	while(bytes_sent < sizeof(command)) {
		bytes_sent += send(fd, (char *)&command[bytes_sent], sizeof(command)-bytes_sent, 0);
	}
	command[2] = 0; // Stop proximity calibration.
}


void receive_data() {
	bytes_recv = recv(fd, (char *)&header, 1, 0);
	if (bytes_recv != 1) {
		fprintf(stderr, "Failed to receive header: %d\n", bytes_recv);
		exit(1);
	}
	
	bytes_recv = 0;

	// DISTINGUISH ROBOT MESSAGE BY HEADER
	// 0x01 - Camera
	// 0x02 - Sensor data
	// 0x03 - Empty message: camera and sensor are disabled
	switch(header) {
		case 0x01:
			// printf("received camera\n");
			// Receive image				
			while (bytes_recv < sizeof(rgb565)){
				bytes_recv += recv(fd, (char *)&rgb565[bytes_recv], sizeof(rgb565) - bytes_recv, 0);
			}
			
			if (bytes_recv != sizeof(rgb565)) {
				fprintf(stderr, "Cannot read camera image: %d != %d\n", bytes_recv, (int)sizeof(rgb565));
				exit(1);
			}

			camera_updated = 1;
			break;
		
		case 0x02:
			// printf("received sensors\n");
			while(bytes_recv < sizeof(sensor)) {
				bytes_recv += recv(fd, (char *)&sensor[bytes_recv], sizeof(sensor)-bytes_recv, 0);
			}
			
			break;
		
		case 0x03:
			break;
		
		default:
			break;
	}
}



int robot_go_on() {
	send_command();
	receive_data();
	return 1;
}


void set_speed_left(int speed_left){
	if(speed_left > MAX_SPEED || speed_left < -MAX_SPEED){
		printf("Invalid speed_left: %d. Accepts -%d <= x <= %d. Left speed unchanged.\n", speed_left, MAX_SPEED, MAX_SPEED);
		return;
	}

	command[3] = speed_left & 0xFF;		// left motor Least Significant Byte
	command[4] = speed_left >> 8;		// left motor MSB
}

void set_speed_right(int speed_right){
	if(speed_right > MAX_SPEED || speed_right < -MAX_SPEED){
		printf("Invalid speed_right: %d. Accepts -%d <= x <= %d. Right speed unchanged.\n", speed_right, MAX_SPEED, MAX_SPEED);
		return;
	}

	command[5] = speed_right & 0xFF;	// right motor Least Significant Byte
	command[6] = speed_right >> 8;		// right motor MSB
}


void set_speed(double left, double right) {
	set_speed_left((int) left);
	set_speed_right((int) right);
}

double bounded_speed(double speed){
	return (speed > MAX_SPEED) ? MAX_SPEED : (speed < -MAX_SPEED) ? -MAX_SPEED : speed;
}

void toggle_led(int led_position){
	if(led_position > 3 || led_position < 0){
		printf("Invalid led_position: %d. Accepts 0 <= x <= 3. LEDs unchanged.\n", led_position);
		return;
	}

	// Shift 1 to the position of the current LED (binary representation), then bitwise XOR it with previous LEDs value
	command[7] = command[7] ^ 1 << led_position;
}

void enable_led(int led_position){
	if(led_position > 3 || led_position < 0){
		printf("Invalid led_position: %d. Accepts 0 <= x <= 3. LEDs unchanged.\n", led_position);
		return;
	}

	// Shift 1 to the position of the current LED (binary representation), then bitwise OR it with current LEDs value
	command[7] = command[7] | 1 << led_position;
}

void disable_led(int led_position){
	if(led_position > 3 || led_position < 0){
		printf("Invalid led_position: %d. Accepts 0 <= x <= 3. LEDs unchanged.\n", led_position);
		return;
	}

	// Shift 0 to the position of the led, 1 otherwise. Then bitwise AND with previous value to force that one led to 0.
	uint mask = ~(1 << led_position);
	command[7] = command[7] & mask;
}


void enable_body_led(void){

	// Shift 1 to the position of the current LED (binary representation), then bitwise OR it with current LEDs value
	command[7] = command[7] | 1 << 4;
}

void disable_body_led(void){

	// Shift 0 to the position of the led, 1 otherwise. Then bitwise AND with previous value to force that one led to 0.
	uint mask = ~(1 << 4);
	command[7] = command[7] & mask;
}

void enable_front_led(void){

	// Shift 1 to the position of the current LED (binary representation), then bitwise OR it with current LEDs value
	command[7] = command[7] | 1 << 5;
}

void disable_front_led(void){

	// Shift 0 to the position of the led, 1 otherwise. Then bitwise AND with previous value to force that one led to 0.
	uint mask = ~(1 << 5);
	command[7] = command[7] & mask;
}




// get the correction values for prox sensors
short int prox_corr[PROX_SENSORS_COUNT];

void calibrate_prox() {
  int i, j;
  short int tmp[PROX_SENSORS_COUNT];
 
  // init array for calibration values  
  for (i=0; i<PROX_SENSORS_COUNT; i++) {
    prox_corr[i] = 0;
  }

  printf("calibrating proximity ...");

  for (i=0; i<LED_COUNT; i++) {
    toggle_led(i);
  }

  // get multiple readings for each sensor
  for (j=0; j<(NBR_CALIB+OFFSET_CALIB) && robot_go_on(); j++) {  
    if (j >= OFFSET_CALIB) {
        
        get_prox(tmp);
        for (i=0; i<PROX_SENSORS_COUNT; i++) {
            prox_corr[i] += tmp[i];
        }
    }
  }
  
  // calculate average for each sensor
  for (i=0; i<PROX_SENSORS_COUNT; i++) {
    prox_corr[i] = prox_corr[i] / NBR_CALIB;
    printf("%d ",prox_corr[i]);
  }
  
  for (i=0; i<LED_COUNT; i++) {
    toggle_led(i);
  }
  printf(" done calibration\n");
}





void get_prox(short int *prox_values) {
	prox_values[PROX_RIGHT_FRONT] = *(short int *)(&sensor[37]);
	prox_values[PROX_RIGHT_FRONT_DIAG] = *(short int *)(&sensor[39]);
	prox_values[PROX_RIGHT_SIDE] = *(short int *)(&sensor[41]);
	prox_values[PROX_RIGHT_BACK] = *(short int *)(&sensor[43]);

	prox_values[PROX_LEFT_BACK] = *(short int *)(&sensor[45]);
	prox_values[PROX_LEFT_SIDE] = *(short int *)(&sensor[47]);
	prox_values[PROX_LEFT_FRONT_DIAG] = *(short int *)(&sensor[49]);
	prox_values[PROX_LEFT_FRONT] = *(short int *)(&sensor[51]);
}


void get_prox_calibrated(short int *prox_values) {
    short int diff;
    diff = *(short int *)(&sensor[37]) -  prox_corr[PROX_RIGHT_FRONT];
	prox_values[PROX_RIGHT_FRONT] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[39]) -  prox_corr[PROX_RIGHT_FRONT_DIAG];
	prox_values[PROX_RIGHT_FRONT_DIAG] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[41]) -  prox_corr[PROX_RIGHT_SIDE];
	prox_values[PROX_RIGHT_SIDE] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[43]) -  prox_corr[PROX_RIGHT_BACK];
	prox_values[PROX_RIGHT_BACK] = diff < 0 ? 0 : diff;

    diff = *(short int *)(&sensor[45]) -  prox_corr[PROX_LEFT_BACK];
	prox_values[PROX_LEFT_BACK] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[47]) -  prox_corr[PROX_LEFT_SIDE];
	prox_values[PROX_LEFT_SIDE] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[49]) -  prox_corr[PROX_LEFT_FRONT_DIAG];
	prox_values[PROX_LEFT_FRONT_DIAG] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[51]) -  prox_corr[PROX_LEFT_FRONT];
	prox_values[PROX_LEFT_FRONT] = diff < 0 ? 0 : diff;
}





// get the correction values for light sensors
short int light_corr[PROX_SENSORS_COUNT];

void calibrate_light() {
  int i, j;
  short int tmp[PROX_SENSORS_COUNT];
 
  // init array for calibration values  
  for (i=0; i<PROX_SENSORS_COUNT; i++) {
    light_corr[i] = 0;
  }

  printf("calibrating light ...");

  for (i=0; i<LED_COUNT; i+=2) {
    toggle_led(i);
  }

  // get multiple readings for each sensor
  for (j=0; j<(NBR_CALIB+OFFSET_CALIB) && robot_go_on(); j++) {  
    if (j >= OFFSET_CALIB) {
        
        get_light(tmp);
        for (i=0; i<PROX_SENSORS_COUNT; i++) {
            light_corr[i] += tmp[i];
        }
    }
  }
  
  // calculate average for each sensor
  for (i=0; i<PROX_SENSORS_COUNT; i++) {
    light_corr[i] = light_corr[i] / NBR_CALIB;
    printf("%d ",light_corr[i]);
  }
  
  for (i=0; i<LED_COUNT; i++) {
    toggle_led(i);
  }
  printf(" done calibration\n");
}




void get_light(short int *light_values) {
	light_values[PROX_RIGHT_FRONT] = *(short int *)(&sensor[53]);
	light_values[PROX_RIGHT_FRONT_DIAG] = *(short int *)(&sensor[55]);
	light_values[PROX_RIGHT_SIDE] = *(short int *)(&sensor[57]);
	light_values[PROX_RIGHT_BACK] = *(short int *)(&sensor[59]);

	light_values[PROX_LEFT_BACK] = *(short int *)(&sensor[61]);
	light_values[PROX_LEFT_SIDE] = *(short int *)(&sensor[63]);
	light_values[PROX_LEFT_FRONT_DIAG] = *(short int *)(&sensor[65]);
	light_values[PROX_LEFT_FRONT] = *(short int *)(&sensor[67]);
}

void get_light_calibrated(short int *light_values) {
    short int diff;
    diff = *(short int *)(&sensor[53]) -  light_corr[PROX_RIGHT_FRONT];
	light_values[PROX_RIGHT_FRONT] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[55]) -  light_corr[PROX_RIGHT_FRONT_DIAG];
	light_values[PROX_RIGHT_FRONT_DIAG] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[57]) -  light_corr[PROX_RIGHT_SIDE];
	light_values[PROX_RIGHT_SIDE] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[59]) -  light_corr[PROX_RIGHT_BACK];
	light_values[PROX_RIGHT_BACK] = diff < 0 ? 0 : diff;

    diff = *(short int *)(&sensor[61]) -  light_corr[PROX_LEFT_BACK];
	light_values[PROX_LEFT_BACK] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[63]) -  light_corr[PROX_LEFT_SIDE];
	light_values[PROX_LEFT_SIDE] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[65]) -  light_corr[PROX_LEFT_FRONT_DIAG];
	light_values[PROX_LEFT_FRONT_DIAG] = diff < 0 ? 0 : diff;
    diff = *(short int *)(&sensor[67]) -  light_corr[PROX_LEFT_FRONT];
	light_values[PROX_LEFT_FRONT] = diff < 0 ? 0 : diff;
}




void get_ground(short int *ground_values) {
	ground_values[GS_LEFT] = *(short int *)(&sensor[90]);
	ground_values[GS_CENTER] = *(short int *)(&sensor[92]);
	ground_values[GS_RIGHT] = *(short int *)(&sensor[94]);
}


void rgb565_to_bgr888(const unsigned char *rgb565, unsigned char *bgr888, int width, int height) {
	int counter = 0;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int index = 3 * (i + j * width);
			unsigned char red = rgb565[counter] & 0xf8;
			unsigned char green = (rgb565[counter++] << 5);
			green += (rgb565[counter] & 0xf8) >> 3;
			unsigned char blue = rgb565[counter++] << 3;
			bgr888[index] = blue;
			bgr888[index + 1] = green;
			bgr888[index + 2] = red;
		}
	}
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

int get_camera_height() {
	return CAMERA_HEIGHT;
}

int get_camera_width() {
	return CAMERA_WIDTH;
}

void get_camera(unsigned char *red, unsigned char *green, unsigned char *blue) {
	if(camera_updated){

		rgb565_to_bgr888(rgb565, bgr888, CAMERA_WIDTH, CAMERA_HEIGHT);
		
		save_bmp_image(bgr888);

		camera_updated = 0;
	}
	
	for (int i = 0; i < CAMERA_HEIGHT * CAMERA_WIDTH; i++) {
		red[i] = bgr888[3 * i + 2];
		green[i] = bgr888[3 * i + 1];
		blue[i] = bgr888[3 * i];
	}
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
	temp[0] = *(unsigned char *)(&sensor[36]);
}


// //////////////
// time-of-flight sensor (credits: Jonas Fontana)

// get distance given in millimeters
void get_tof(short int *tof_distance) {
	tof_distance[0] = *(short int *)(&sensor[69]);
}


// //////////////
// accelerometer values (credits: Jonas Fontana)

// get gyro
void get_gyro_axes( short *gyro) {
	gyro[AXES_X] = *( short *)(&sensor[18]);
	gyro[AXES_Y] = *( short *)(&sensor[20]);
	gyro[AXES_Z] = *( short *)(&sensor[22]);
}


// get orientation
void get_orientation(float *orientation) {
	orientation[0] = *(float *)(&sensor[10]);
}

// get inclination
void get_inclination(float *inclination) {
	inclination[0] = *(float *)(&sensor[14]);
}



// get acceleration
void get_acceleration(float *acceleration) {
	acceleration[0] = *(float *)(&sensor[6]);
}

// get acceleration
void get_acceleration_axes(short int *acceleration) {
	acceleration[AXES_Y] = *(short int *)(&sensor[0]);
	acceleration[AXES_X] = *(short int *)(&sensor[2]);
	acceleration[AXES_Z] = *(short int *)(&sensor[4]);
}






// //////////////
// playing sound (credits: Jonas Fontana)


// play one of the default sounds
void play_sound(int sound) {
		switch (sound)
		{
			case 0 :								//MARIO
				command[20] = 0x01;
				break;

			case 1 :								//UNDERWORLD
				command[20] = 0x02;
				break;

			case 2 :								//STARWARS
				command[20] = 0x04;
				break;

			case 3 :								//4KHz
				command[20] = 0x08;
				break;

			case 4 :								//10KHz
				command[20] = 0x10;
				break;

			default:
				break;
		}
}

// stop sound
void stop_sound(void) {
	command[20] = 0x20;
}

void get_microphones(short int *soundlevels) {
	soundlevels[MICROPHONE_RIGHT] = *(short int *)(&sensor[71]);
	soundlevels[MICROPHONE_LEFT] = *(short int *)(&sensor[73]);
	soundlevels[MICROPHONE_BACK] = *(short int *)(&sensor[75]);
	soundlevels[MICROPHONE_FRONT] = *(short int *)(&sensor[77]);
}




// //////////////
// communication
key_t key; 
int msgid,shmid; 
int *queues;

struct msg_buffer { 
    long mtype;       /* message type, must be > 0 */
    char text[MSG_LENGTH+1]; 
} message; 


int get_robot_ID() {
    int a, b, c, d;
    // parse ip address
    if (sscanf(ip, "%i.%i.%i.%i", &a, &b, &c, &d) < 4) {
      printf("please provide a valid ip\n");
      return -1;
    }
    else return d;
}


/*
The structure of the inter-process communication between the robot control process is the following:

1. an IPC queue for each robot controller to allow other controller to send messages.
2. a shared memory segment, to store the ID of the message queues and make them accessible for other processes. In this segment the first place is reserved for the number of Q IDs.

The IPC queue is deleted and recreated each time a controller starts since the controller normally terminates with ctrl-c and cannot clean-up resources.

The shared memory segment is only deleted and recreated if there is no process attached to it, i.e. the crurent process is the first to start.

*/

void init_communication() {
    
    int id = get_robot_ID();
    printf("robot id 4%d\n",id);

    // generate IPC Q for receiving messages
    // ftok to generate a unique key based on current directory and robot ip address 
    key = ftok(".", id); 
    if (DEBUG) printf("IPC msg Q key %d\n",key);
    
    // delete and re-create msg Q
    msgid = msgget(key, 0666);	
	if (msgid == -1) {
		printf("Message queue does not exists\n");
    } else if (msgctl(msgid, IPC_RMID, NULL) == -1) {
		printf("Message queue could not be deleted\n");
    } else {
      if (DEBUG) printf("Message queue reset\n");
    }
    msgid = msgget(key, 0666 | IPC_CREAT); 
    if (DEBUG) printf("IPC msg Q ID %d\n",msgid);
    
    // define shared memory segment
    key = ftok(".",COM_CHANNEL);
    if (DEBUG) printf("IPC shared mem key %d\n",key);
    // shmget returns an identifier in shmid 
    shmid = shmget(key,512,0666); 
    
    // check the number of processes attached to shared memory 
    struct shmid_ds buf;
    shmctl(shmid, IPC_STAT, &buf);
    if (DEBUG) printf("processes attached: %d\n",(int) buf.shm_nattch);
    if (DEBUG) printf("IPC shared mem ID check %d\n",shmid);
	if (shmid == -1) {
		printf("Shared memory does not exists\n");
    } else if (buf.shm_nattch == 0) {
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
		  printf("Shared memory could not be deleted\n");
		} else {
          if (DEBUG) printf("Shared memory reset\n");
        }
    }
    shmid = shmget(key,512,0666|IPC_CREAT); 
    if (DEBUG) printf("IPC shared mem ID %d\n",shmid);
    
    if (msgid > 0 && shmid > 0) {
        printf("msg Q and shared memory intialized\n");
    }
    
    // shmat to attach to shared memory 
    queues = (int*) shmat(shmid,(void*)0,0);  
    
    int nb = queues[0];
    if (DEBUG) printf("nb %d\n",nb);
    if (nb == 0) { // this is the first controller to store its msg Q ID
      queues[0] = 1;
      queues[1] = msgid;
    }
    else { // add the msg Q ID to the list of Q IDs
      nb = queues[0];
      queues[nb+1] = msgid;
      queues[0] = nb+1;
    }
    if (DEBUG) {
        for (int i=0;i<10;i++) {
          printf(" %d",queues[i]);
        }
        printf("\n");
    }
}


void disable_communication() {
    msgctl(msgid, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);
}

void send_msg_to_Q(const char *msg, int qid) {
  //printf("%s\n",msg);
  message.mtype = COM_CHANNEL;
  strcpy(message.text,msg);
  //printf("%s\n",message.text);
  msgsnd(qid, &message, sizeof(message), IPC_NOWAIT); 
}

// send the message
void send_msg(const char *msg) {
  // broadcast to all other queues
  for (int i=1;i<queues[0]+1;i++) {
    if (queues[i] != msgid)
      send_msg_to_Q(msg,queues[i]);
  }
}

// receive message
void receive_msg(char* buffer) {  
    //printf("%s\n",buffer);
    strcpy(buffer,MSG_NONE);
  
    if (msgrcv(msgid, &message, sizeof(message), COM_CHANNEL, IPC_NOWAIT) >= 0) {
        strcpy(buffer,message.text);
        //printf("%s\n",message.text);
    }
    //printf("%s\n",buffer);
}






