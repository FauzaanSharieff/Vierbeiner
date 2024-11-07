#include <Servo.h>
#include <SPI.h>
#include <RF24.h>


// Declaring variables
int serv_move_delay;                                               // Delay servo write speed
int trans_angle_up;                                                // Translational angle - Upper threshold
int trans_angle_down;                                              // Translational angle - Lower threshold
int t;                                                             // Rotational angle threshold
int i;                                                             // Global loop variable
int joystick[3];                                                   // Array for controller values
int x_trans;                                                       // Joystick sideways input
int y_trans;                                                       // Joystick forwards input
int x_rot;                                                         // Joystick rotation input


// Deriving servo class for custom data members
class NewServo : public Servo
{
  public:
    int current_angle;
    int stance_angle;
    int target_angle;
    NewServo() : current_angle(0), stance_angle(0), target_angle(0) {}
};


// Array of servo objects
int servoPins[4][3] = { {2, 3, 4}, {5, 6, 7}, {8, 9, 10}, {11, 12, 13} };
NewServo serv[4][3];   


// Setting up radio
RF24 radio(7, 8);                                                  // Attaching CSN, CE, Radio object creation
const byte address[6] = "092004";                                  // Address for receiver


// Declaring functions

// Main functions
void stance();                                                     // Brings robot to a stance state 

// Directional movement functions
void forward1();                                                   // Takes the first step in forward direction
void forward2();                                                   // Takes the second step in forward direction
void backward1();                                                  // Takes the first step in backward direction
void backward2();                                                  // Takes the second step in backward direction
void right1();                                                     // Takes the first step in right direction
void right2();                                                     // Takes the second step in right direction
void left1();                                                      // Takes the first step in left direction
void left2();                                                      // Takes the second step in left direction

// Rotational movement functions
void rotateRight1();                                               // Turns a bit towards right
void rotateRight2();                                               // Turns a bit more towards right
void rotateLeft1();                                                // Turns a bit towards left
void rotateLeft2();                                                // Turns a bit more towards left

// Auxillary functions
void move(NewServo &serv, int angle);                              // Write angle to servo at controlled speed
void threshold(int t0, int t1, int t2, int t3);                    // Moves rotational motors simultaneously
void moveEvenUp();                                                 // Moves translational motors 0 and 2 up
void moveEvenDown();                                               // Moves translational motors 0 and 2 down
void moveOddUp();                                                  // Moves translational motors 1 and 3 up
void moveOddDown();                                                // Moves translational motors 1 and 3 down







// VOID SETUP AND LOOP ||| VOID SETUP AND LOOP ||| VOID SETUP AND LOOP ||| VOID SETUP AND LOOP 


void setup() {

  serv_move_delay = 10;                                            // Initializing variables
  trans_angle_up = 20;
  trans_angle_down = 30;
  t = 30;

  for(int i = 0; i < 4; i++)                                       // Attaching each servo to its respective pin
  {
    for(int j = 0; j < 3; j++) 
    {
      serv[i][j].attach(servoPins[i][j]);                          
    }
  }

  serv[0][0].stance_angle = 45;                                    // Setting rotational stance angles
  serv[1][0].stance_angle = 135;
  serv[2][0].stance_angle = 45;
  serv[3][0].stance_angle = 135;

  for(i = 0; i < 4; i++)
    move(serv[i][0], serv[i][0].stance_angle);                     // Moving rotational motors to stance manually
  stance();                                                        // Moving rotational, translational and redundant motors to stance

  for(i = 0; i < 4; i++)                                           // Setting current angles as stance angles
    serv[i][0].current_angle = serv[i][0].stance_angle;

  radio.begin();                                                   // Starting radio protocols
  radio.openReadingPipe(0, address);     
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); 	              

  
}




/*
MEGA: CSN = 8, CE = 7, SCK = 52, MOSI = 51, MISO = 50
UNO: SCK = 13, MOSI = 11, MISO = 12

RF24 radio(7, 8);                   // Attaching CSN, CE, Radio object creation

const byte address[6] = "092004";   // Address for both transmitter and receiver

void setup()
{
radio.begin();
radio.openWritingPipe(address);         // Transmitter only
radio.openReadingPipe(0, address);     // Receiver only
radio.setPALevel(RF24_PA_LOW);
radio.stopListening();                  // Transmitter only
radio.startListening(); 	              // Receiver only

}

// Transmitter
void loop{
  int joystick[3] = {x_axis_trans, y_axis_trans, x_axis_rot};
  radio.write(&joystick, sizeof(joystick));

}

// Receiver
void loop{
  if(radio.available())
  {
    joystick[] = {0, 0, 0};
    radio.read(&joystick, sizeof(joystick[]));
  }

  int x = joystick[0]; 
}

*/


void loop()
{

  if(radio.available())
  {
    joystick[] = {0, 0, 0};
    radio.read(&joystick, sizeof(joystick[]));
  }

  x_trans = joystick[0];
  y_trans = joystick[1];
  x_rot = joystick[2];

  if(y_trans > 1020)
  forward1();
  if(y_trans > 1020)                                   
  forward2();
  if(y_trans < 3)
  backward1();
  if(y_trans < 3)
  backward2();
  if(x_trans > 1020)
  right1();
  if(x_trans > 1020)                                   
  right2();
  if(x_trans < 3)
  left1();
  if(x_trans < 3)
  left2();
  if(x_rot > 1020)
  rotateRight1();
  if(x_rot > 1020)
  rotateRight2();
  if(x_rot < 3)
  rotateLeft1();
  if(x_rot < 3)
  rotateLeft2();
  
  stance();                                                 

}


// VOID SETUP AND LOOP ||| VOID SETUP AND LOOP ||| VOID SETUP AND LOOP ||| VOID SETUP AND LOOP 













// VOID STANCE ||| VOID STANCE ||| VOID STANCE ||| VOID STANCE ||| VOID STANCE 

void stance()
{
  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);

  if(serv[0][1].current_angle == trans_angle_up || serv[2][1].current_angle == trans_angle_up)
  moveEvenDown();

  if(serv[1][1].current_angle == trans_angle_up || serv[3][1].current_angle == trans_angle_up)
  moveOddDown();

}

// VOID STANCE ||| VOID STANCE ||| VOID STANCE ||| VOID STANCE ||| VOID STANCE 









// Y-AXIS FUNCTIONS ||| Y-AXIS FUNCTIONS ||| Y-AXIS FUNCTIONS ||| Y-AXIS FUNCTIONS 

void forward1()
{

  if(serv[0][1].current_angle == trans_angle_down || serv[2][1].current_angle == trans_angle_down)
  moveEvenUp();                                                   

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[0][3].stance_angle - t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[0][3].stance_angle + t);

}


void forward2()
{                                                                 
  
  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[0][3].stance_angle + t);
  
  moveOddDown();
  moveEvenUp();
  
  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[0][3].stance_angle - t);
  
}


void backward1()
{

  if(serv[0][1].current_angle == trans_angle_down || serv[2][1].current_angle == trans_angle_down)
  moveEvenUp();

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[0][3].stance_angle + t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[0][3].stance_angle - t);
  
}


void backward2()
{                                                 

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[0][3].stance_angle - t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[0][3].stance_angle + t);
  
}


// Y-AXIS FUNCTIONS ||| Y-AXIS FUNCTIONS ||| Y-AXIS FUNCTIONS ||| Y-AXIS FUNCTIONS ||| Y-AXIS FUNCTIONS 









// X-AXIS FUNCTIONS ||| X-AXIS FUNCTIONS ||| X-AXIS FUNCTIONS ||| X-AXIS FUNCTIONS ||| X-AXIS FUNCTIONS 


void right1()
{
  
  if(serv[1][1].current_angle == trans_angle_down || serv[1][1].current_angle == trans_angle_down)
  moveOddUp();

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle - t, serv[0][3].stance_angle + t);

  moveOddDown();
  moveEvenUp();

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle + t, serv[0][3].stance_angle - t);

}


void right2()
{

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle + t, serv[0][3].stance_angle - t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle - t, serv[0][3].stance_angle + t);

}


void left1()
{

  if(serv[1][1].current_angle == trans_angle_down || serv[1][1].current_angle == trans_angle_down)
  moveOddUp();

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle + t, serv[0][3].stance_angle - t);

  moveOddDown();
  moveEvenUp();

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle - t, serv[0][3].stance_angle + t);

}


void left2()
{

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle - t, serv[0][3].stance_angle + t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle + t, serv[0][3].stance_angle - t);

}


// X-AXIS FUNCTIONS ||| X-AXIS FUNCTIONS ||| X-AXIS FUNCTIONS ||| X-AXIS FUNCTIONS ||| X-AXIS FUNCTIONS 









// ROTATION FUNCTIONS ||| ROTATION FUNCTIONS ||| ROTATION FUNCTIONS ||| ROTATION FUNCTIONS


void rotateRight1()
{
  
  if(serv[0][1].current_angle == trans_angle_down || serv[2][1].current_angle == trans_angle_down)
  moveEvenUp();

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[0][3].stance_angle + t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[0][3].stance_angle - t);

}


void rotateRight2()
{

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[0][3].stance_angle - t);

  moveOddDown();
  moveEvenUp();

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[0][3].stance_angle + t);

}


void rotateLeft1()
{

  if(serv[1][1].current_angle == trans_angle_down || serv[3][1].current_angle == trans_angle_down)
  moveOddUp();

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[0][3].stance_angle + t);

  moveOddDown();
  moveEvenUp();

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[0][3].stance_angle - t);

}


void rotateLeft2()
{

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[0][3].stance_angle + t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[0][3].stance_angle - t);

}


// ROTATION FUNCTIONS ||| ROTATION FUNCTIONS ||| ROTATION FUNCTIONS ||| ROTATION FUNCTIONS 









// VOID MOVE ||| VOID MOVE ||| VOID MOVE ||| VOID MOVE ||| VOID MOVE ||| VOID MOVE

void move(NewServo &serv, int angle) 
{
 for (i = serv.current_angle; i != angle; i += (serv.current_angle < angle) ? 1 : -1)
 {
  serv.write(i);
  delay(serv_move_delay);
 }
  serv.current_angle = angle;
}

// VOID MOVE ||| VOID MOVE ||| VOID MOVE ||| VOID MOVE ||| VOID MOVE ||| VOID MOVE









// VOID THRESHOLD ||| VOID THRESHOLD ||| VOID THRESHOLD ||| VOID THRESHOLD ||| VOID THRESHOLD 

void threshold(int t0, int t1, int t2, int t3)
{
  serv[0][0].target_angle = t0;
  serv[1][0].target_angle = t1;
  serv[2][0].target_angle = t2;
  serv[3][0].target_angle = t3;
  bool flag = false;
  while(true)
  {
    for(i = 0; i < 4; i++)
    {
      if(serv[i][0].current_angle > serv[i][0].target_angle)
      move(serv[i][0], serv[i][0].current_angle - 1);
      else if (serv[i][0].current_angle < serv[i][0].target_angle)
      move(serv[i][0], serv[i][0].current_angle + 1);

      if(serv[3][0].current_angle == serv[3][0].target_angle)
      flag = true;
    }
    if(flag)
    break;
  }
}

// VOID THRESHOLD ||| VOID THRESHOLD ||| VOID THRESHOLD ||| VOID THRESHOLD ||| VOID THRESHOLD 









// TRANSLATIONAL AND REDUNDANT MOTOR FUNCTIONS ||| TRANSLATIONAL AND REDUNDANT MOTOR FUNCTIONS 


void moveEvenUp()                                                          // Moves 0 and 2 translational and redundant motors up
{
  for(; serv[0][1].current_angle > trans_angle_up || serv[2][1].current_angle > trans_angle_up; )
  {
    for(i = 0; i < 3; i++)                                        
    {
      if(i % 2 == 0)
      {
        move(serv[i][1], serv[i][1].current_angle - 1);
        move(serv[i][2], 90 - serv[i][1].current_angle);
      }
    }
  }
}

void moveEvenDown()                                                        // Moves 0 and 2 translational and redundant motors down
{
  for(; serv[0][1].current_angle < trans_angle_down || serv[2][1].current_angle < trans_angle_down; )
  {
    for(i = 0; i < 3; i++)                                            
    {
      if(i % 2 == 0)
      {
        move(serv[i][1], serv[i][1].current_angle + 1);
        move(serv[i][2], 90 - serv[i][1].current_angle);
      }
    }
  }
}

void moveOddUp()                                                          // Moves 1 and 3 translational and redundant motors up
{
  for(; serv[1][1].current_angle > trans_angle_up || serv[3][1].current_angle > trans_angle_up; )
  {
    for(i = 1; i < 4; i++)                                         
    {
      if(i % 2 != 0)
      {
        move(serv[i][1], serv[i][1].current_angle - 1);
        move(serv[i][2], 90 - serv[i][1].current_angle);
      }
    }
  }
}

void moveOddDown()                                                            // Moves 1 and 3 translational and redundant motors down
{
  for(; serv[1][1].current_angle < trans_angle_down || serv[3][1].current_angle < trans_angle_down; )
  {
    for(i = 1; i < 4; i++)                                            
    {
      if(i % 2 != 0)
      {
        move(serv[i][1], serv[i][1].current_angle + 1);
        move(serv[i][2], 90 - serv[i][1].current_angle);
      }
    }
  }
}


// TRANSLATIONAL AND REDUNDANT MOTOR FUNCTIONS ||| TRANSLATIONAL AND REDUNDANT MOTOR FUNCTIONS 




















/*

void diagonalRight1()
{

  if(serv[1][1].current_angle == trans_angle_down || serv[3][1].current_angle == trans_angle_down)
  moveOddUp();

  threshold(serv[0][0].stance_angle - 0, serv[1][0].stance_angle - t, serv[2][0].stance_angle + 0, serv[0][3].stance_angle + t);

  moveOddDown();
  moveEvenUp();

  threshold(serv[0][0].stance_angle + 0, serv[1][0].stance_angle + t, serv[2][0].stance_angle + 0, serv[0][3].stance_angle - t);

}




// SLOW SERVO LOOP ||| SLOW SERVO LOOP ||| SLOW SERVO LOOP ||| SLOW SERVO LOOP ||| SLOW SERVO LOOP

void loop() {
 for (i = 0; i != angle_thr; i++)
 {
  serv.write(i);
  delay(serv_move_delay);
 }
  for (i = angle_thr; i != 0; i--)
 {
  serv.write(i);
  delay(serv_move_delay);
 }
}

// SLOW SERVO LOOP ||| SLOW SERVO LOOP ||| SLOW SERVO LOOP ||| SLOW SERVO LOOP ||| SLOW SERVO LOOP









// OLD ROTATIONAL MOTOR ALGORITHM ||| OLD ROTATIONAL MOTOR ALGORITHM ||| OLD ROTATIONAL MOTOR ALGORITHM 

for(i = 0; i < 30; i++)                                     // Moves all rotational motors to their forward1() thresholds
  {
    for(j = 0; j < 4; j++)
    {
      if(j < 2)                                                  
        move(serv[j][0], serv[j][0].current_angle + 1);
      else
        move(serv[j][0], serv[j][0].current_angle - 1);
    }
  }

// OLD ROTATIONAL MOTOR ALGORITHM ||| OLD ROTATIONAL MOTOR ALGORITHM ||| OLD ROTATIONAL MOTOR ALGORITHM 









// SERVO OBJECT NAMES ||| SERVO OBJECT NAMES ||| SERVO OBJECT NAMES ||| SERVO OBJECT NAMES 

Servo serv11;
Servo serv12;
Servo serv13;

Servo serv21;
Servo serv22;
Servo serv23;

Servo serv31;
Servo serv32;
Servo serv33;

Servo serv41;
Servo serv42;
Servo serv43;

// SERVO OBJECT NAMES ||| SERVO OBJECT NAMES ||| SERVO OBJECT NAMES ||| SERVO OBJECT NAMES 

*/