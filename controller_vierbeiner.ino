#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);                   // Attaching CSN, CE, Radio object creation

const byte address[6] = "092004";   // Address for transmitter

int px_axis_trans;
int py_axis_trans;
int px_axis_rot;

int x_axis_trans;
int y_axis_trans;
int x_axis_rot;

int joystick[3];


void setup() {
  
  Serial.begin(9600);

  px_axis_trans = 15;
  py_axis_trans = 14;
  px_axis_rot = 16;

  pinMode(px_axis_trans, INPUT);
  pinMode(py_axis_trans, INPUT);
  pinMode(px_axis_rot, INPUT);

  radio.begin();                          // Start protocols
  radio.openWritingPipe(address);         // Set as transmitter
  radio.setPALevel(RF24_PA_LOW);          // Set power amplifier to LOW
  radio.stopListening();                  // Start transmitting



}

void loop() {
  
  x_axis_trans = analogRead(px_axis_trans);
  y_axis_trans = analogRead(py_axis_trans);
  x_axis_rot = analogRead(px_axis_rot);

  joystick[3] = {x_axis_trans, y_axis_trans, x_axis_rot};
  radio.write(&joystick, sizeof(joystick));

   
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


