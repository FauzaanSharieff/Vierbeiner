#include <Servo.h>
#include <SPI.h>
#include <RF24.h>

RF24 radio(48, 49);                                                  // Attaching CSN, CE, Radio object creation
const byte address[6] = "00001";                                     // Address for receiver

int arr[3] = {0, 0, 0};                                              // Control array init
int x_trans = 512;                                                   
int y_trans = 512;
int rot = 512;


int rot_delay;                                                       // Delay for rotational motors
int trans_delay;                                                     // Delay fir translational motors
int t;                                                               // Movement threshold angle
int i;                                                               // Loop variable
int trans_up_r;                                                      // Upper angle threshold for right translational motors
int trans_down_r;                                                    // Lower angle threshold for right translational motors
int trans_up_l;                                                      // Upper angle threshold for left translational motors
int trans_down_l;                                                    // Lower angle threshold for right translational motors
int right_trans_offset;                                              // Angle offset for right translational motors
int left_trans_offset;                                               // Angle offset for right translational motors
int upper;                                                           // Upper analog input threshold
int lower;                                                           // Lower analog input threshold

// Directional movement functions
void forward1();                                                   // Takes the first step in forward direction
void forward2();                                                   // Takes the second step in forward direction
void backward1();                                                  // Takes the first step in backward direction
void backward2();                                                  // Takes the second step in backward direction
void right1();                                                     // Takes the first step in right direction
void right2();                                                     // Takes the second step in right direction
void left1();                                                      // Takes the first step in left direction
void left2();                                                      // Takes the second step in left direction
void rotateRight1();                                               // Rotates a step towards right
void rotateRight2();
void rotateLeft1();                                                // Rotates a step towards left
void rotateLeft2();
void readValues();                                                 // Reads radio signal
void moveEvenUp();                                                 // Moves translational motors 0 and 2 up
void moveEvenDown();                                               // Moves translational motors 0 and 2 down
void moveOddUp();                                                  // Moves translational motors 1 and 3 up
void moveOddDown();                                                // Moves translational motors 1 and 3 down
void stance();                                                     // Assumes stance position


class NewServo : public Servo                                      // Inheriting from Servo class for custom data members
{
  public:
    int current_angle;
    int stance_angle;
    int target_angle;
    NewServo() : current_angle(0), stance_angle(0), target_angle(0) {}
};

NewServo serv[4][3];                                               // Array of servo objects


void setup()
{
  upper = 900;
  lower = 120;
  Serial.begin(9600);

  radio.begin();                                                  // Setting up radio protocols
  radio.openReadingPipe(0, address);     
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); 
 
  pinMode(46, INPUT_PULLUP);                                      // Safety switch 

  serv[0][0].attach(2);                                           // Attaching servos
  serv[1][0].attach(3);
  serv[2][0].attach(4);
  serv[3][0].attach(5);
  serv[0][1].attach(6);
  serv[1][1].attach(7);
  serv[2][1].attach(8);
  serv[3][1].attach(9);
  serv[0][2].attach(10);
  serv[1][2].attach(11);
  serv[2][2].attach(12);
  serv[3][2].attach(13);

  serv[0][0].stance_angle = 40;                                    // Setting rotational stance angles
  serv[1][0].stance_angle = 140;
  serv[2][0].stance_angle = 45;                                    
  serv[3][0].stance_angle = 135;
  serv[0][0].current_angle = 40;                                   // Initializing system states (joint angles)
  serv[1][0].current_angle = 140;
  serv[2][0].current_angle = 45;
  serv[3][0].current_angle = 135;
  
  trans_up_r = 20;                                                 // Setting thresholds and offsets
  trans_down_r = 70;
  trans_up_l = 70;
  trans_down_l = 10;

  right_trans_offset = 30;
  left_trans_offset = 10;

  rot_delay = 3;                                            
  trans_delay = 5;
  t = 30;


  for(i = 0; i < 4; i++)                                           // Initializing with stance
  {
    if(i == 0 || i == 3)
    {  
      serv[i][1].write(trans_down_r);
      serv[i][1].current_angle = trans_down_r;
      serv[i][2].write(trans_down_r - right_trans_offset);
      serv[i][2].current_angle = trans_down_r - right_trans_offset;
    }
    else
     {
      serv[i][1].write(trans_down_l);
      serv[i][1].current_angle = trans_down_l + left_trans_offset;
      serv[i][2].write(trans_down_l);
      serv[i][2].current_angle = trans_down_l + left_trans_offset;
     }
  } 


  serv[3][0].write(serv[3][0].stance_angle);
  serv[2][0].write(serv[2][0].stance_angle);
  serv[1][0].write(serv[1][0].stance_angle);
  serv[0][0].write(serv[0][0].stance_angle);
  
  stance();
  delay(5000);

}


void loop()
{

  if(digitalRead(46) == 1)                                          // Safety switch case
  {
    stance();
    Serial.println("Switch is off.");
    delay(400);
    return;
  }

  if(!radio.available())                                             // Checking radio
  { 
  Serial.println("Radio not available.");
  delay(100);
  return;
  }
  
  readValues();                                                      // Moving based on input
  if(y_trans > upper)
  forward1();

  readValues();
  if(y_trans > upper)                                   
  forward2();

  readValues();
  if(y_trans < lower)
  backward1();

  readValues();
  if(y_trans < lower)
  backward2();

  readValues();
  if(x_trans > upper)
  right1();

  readValues();
  if(x_trans > upper)                                   
  right2();

  readValues();
  if(x_trans < lower)
  left1();

  readValues();
  if(x_trans < lower)
  left2();

  readValues();
  if(rot > upper)
  rotateRight1();

  readValues();
  if(rot < lower)
  rotateLeft1();

  if(x_trans > 3 && x_trans < 1020 && y_trans > 3 && y_trans < 1020)
   stance(); 
  
  delay(50);
}


void stance()
{
  moveOddDown();                                                 
  moveEvenDown();
}


void forward1()
{
  if(serv[0][1].current_angle == trans_down_r || serv[2][1].current_angle == trans_down_l)
  {
    moveOddDown();
    moveEvenUp(); 
  }                                                  
   
  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[3][0].stance_angle - t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);
}


void forward2()
{           
  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[3][0].stance_angle + t);
  
  moveOddDown();
  moveEvenUp();
  
  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);
}


void backward1()
{
  if(serv[0][1].current_angle == trans_down_r || serv[2][1].current_angle == trans_down_l)
  {
    moveOddDown();
    moveEvenUp();
  }

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle - t, serv[2][0].stance_angle + t, serv[3][0].stance_angle + t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle); 
}


void backward2()
{                                                 
  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[3][0].stance_angle - t);

  moveOddDown();
  moveEvenUp();

  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);  
}


void right1()
{
  if(serv[1][1].current_angle == trans_down_l || serv[3][1].current_angle == trans_down_r)
  {
    moveEvenDown();
    moveOddUp();
  }

  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle - t, serv[3][0].stance_angle + t);

  moveOddDown();
  moveEvenUp();

  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);
}


void right2()
{
  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle + t, serv[3][0].stance_angle - t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);
}


void left1()
{
  if(serv[1][1].current_angle == trans_down_l || serv[3][1].current_angle == trans_down_r)
  {
    moveEvenDown();
    moveOddUp();
  }

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle + t, serv[3][0].stance_angle - t);

  moveOddDown();
  moveEvenUp();

  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);
}


void left2()
{
  threshold(serv[0][0].stance_angle + t, serv[1][0].stance_angle - t, serv[2][0].stance_angle - t, serv[3][0].stance_angle + t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);
}


void rotateRight1()
{
  
  if(serv[0][1].current_angle == trans_down_r || serv[2][1].current_angle == trans_down_l)
  {
    moveOddDown();
    moveEvenUp();
  }

  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[3][0].stance_angle + t);

  moveEvenDown();
  moveOddUp();

  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);

}


void rotateLeft1()
{

  if(serv[1][1].current_angle == trans_down_l || serv[3][1].current_angle == trans_down_r)
  {
    moveEvenDown();
    moveOddUp();
  }
  threshold(serv[0][0].stance_angle - t, serv[1][0].stance_angle + t, serv[2][0].stance_angle - t, serv[3][0].stance_angle + t);

  moveOddDown();
  moveEvenUp();

  threshold(serv[0][0].stance_angle, serv[1][0].stance_angle, serv[2][0].stance_angle, serv[3][0].stance_angle);

}


void threshold(int t0, int t1, int t2, int t3)
{
  serv[0][0].target_angle = t0;
  serv[1][0].target_angle = t1;
  serv[2][0].target_angle = t2;
  serv[3][0].target_angle = t3;
  
  bool flag = true;

  while(flag)
  {
    flag = false;  // Assume flag can be false unless proven otherwise in this iteration

    for(i = 0; i < 4; i++)
    {
      if(serv[i][0].current_angle > serv[i][0].target_angle)
      {
        serv[i][0].write(serv[i][0].current_angle - 1);
        serv[i][0].current_angle--;
        flag = true; // Still work to do
        delay(rot_delay);
      }
      else if (serv[i][0].current_angle < serv[i][0].target_angle)
      {
        serv[i][0].write(serv[i][0].current_angle + 1);
        serv[i][0].current_angle++;
        flag = true; // Still work to do
        delay(rot_delay);
      }
      
    }
  }
}


void readValues()
{
  int a = 0;
  int b = 0;
  int c = 0;
  if(radio.available())
  {
    Serial.println("Reads radio");
    radio.read(&arr, sizeof(arr));
    a = arr[0];
    b = arr[1];
    c = arr[2];
  }
  
  Serial.println("READ VALUES HAPENNED!!!!");
  
  y_trans = a;
  x_trans = b;
  rot = c;

    Serial.print("Y-axis: ");
    Serial.println(y_trans);
    Serial.print("X-axis: ");
    Serial.println(x_trans);
    Serial.print("Rotation: ");
    Serial.println(rot);
    Serial.println("");


}


void moveEvenUp()                                                          // Moves 0 and 2 translational and redundant motors up
{
  for(; serv[0][1].current_angle > trans_up_r || serv[2][1].current_angle < trans_up_l; )
  {
    serv[0][1].write(serv[0][1].current_angle - 1);
    serv[0][1].current_angle--;
    serv[0][2].write(serv[0][1].current_angle - right_trans_offset);
    serv[0][2].current_angle = serv[0][1].current_angle - right_trans_offset;

    serv[2][1].write(serv[2][1].current_angle + 1);
    serv[2][1].current_angle++;
    serv[2][2].write(serv[2][1].current_angle + left_trans_offset);
    serv[2][2].current_angle = serv[2][1].current_angle + left_trans_offset;
    delay(trans_delay);

  }
}

void moveEvenDown()                                                        // Moves 0 and 2 translational and redundant motors down
{
    for(; serv[0][1].current_angle < trans_down_r || serv[2][1].current_angle > trans_down_l; )
  {
    serv[0][1].write(serv[0][1].current_angle + 1);
    serv[0][1].current_angle++;
    serv[0][2].write(serv[0][1].current_angle - right_trans_offset);
    serv[0][2].current_angle = serv[0][1].current_angle - right_trans_offset;

    serv[2][1].write(serv[2][1].current_angle - 1);
    serv[2][1].current_angle--;
    serv[2][2].write(serv[2][1].current_angle + left_trans_offset);
    serv[2][2].current_angle = serv[2][1].current_angle + left_trans_offset;
    delay(trans_delay);
  }
}

void moveOddUp()                                                           // Moves 1 and 3 translational and redundant motors up
{
  for(; serv[3][1].current_angle > trans_up_r || serv[1][1].current_angle < trans_up_l; )
  {
    serv[3][1].write(serv[3][1].current_angle - 1);
    serv[3][1].current_angle--;
    serv[3][2].write(serv[3][1].current_angle - right_trans_offset);
    serv[3][2].current_angle = serv[3][1].current_angle - right_trans_offset;

    serv[1][1].write(serv[1][1].current_angle + 1);
    serv[1][1].current_angle++;
    serv[1][2].write(serv[1][1].current_angle + left_trans_offset);
    serv[1][2].current_angle = serv[1][1].current_angle + left_trans_offset;
    delay(trans_delay);
  }
}

void moveOddDown()                                                         // Moves 1 and 3 translational and redundant motors down
{
   for(; serv[3][1].current_angle < trans_down_r || serv[1][1].current_angle > trans_down_l; )
  {
    serv[3][1].write(serv[3][1].current_angle + 1);
    serv[3][1].current_angle++;
    serv[3][2].write(serv[3][1].current_angle - right_trans_offset);
    serv[3][2].current_angle = serv[3][1].current_angle - right_trans_offset;

    serv[1][1].write(serv[1][1].current_angle - 1);
    serv[1][1].current_angle--;
    serv[1][2].write(serv[1][1].current_angle + left_trans_offset);
    serv[1][2].current_angle = serv[1][1].current_angle + left_trans_offset;
    delay(trans_delay);
  }
}



