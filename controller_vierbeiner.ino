#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Create an RF24 object for communication with CE and CSN pins
RF24 radio(9, 10); 

// Define the communication address (same for both transmitter and receiver)
const byte address[6] = "00001"; 

// Variables for analog input readings and communication
int unmappedX;   // Raw X-axis reading from analog input
int x;           // Mapped X-axis value
int y;           // Y-axis reading from analog input
int rot;         // Rotational input reading
int arr[3] = {0, 0, 0}; // Array to store and send the data

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize the radio module
  radio.begin();
  radio.setPALevel(RF24_PA_LOW); // Set Power Amplifier level to LOW for shorter range
  radio.openWritingPipe(address); // Open the writing pipe using the specified address
  radio.stopListening(); // Set the module as a transmitter

  // Configure input pins for joystick analog readings
  pinMode(A0, INPUT); // Y-axis input
  pinMode(A3, INPUT); // X-axis input
  pinMode(A2, INPUT); // Rotational input
}

void loop() {
  // Read analog inputs
  rot = analogRead(A2);        // Read rotational input
  y = analogRead(A0);          // Read Y-axis input
  unmappedX = analogRead(A3);  // Read raw X-axis input
  
  // Map X-axis value from reverse scale (1023 to 0)
  x = map(unmappedX, 0, 1023, 1023, 0);
  
  // Store values in the array for transmission
  arr[0] = y;
  arr[1] = x;
  arr[2] = rot;

  // Send data via RF24 module
  if (!radio.write(&arr, sizeof(arr))) {
    // Print error if data transmission fails
    Serial.println("Data transmission failed");
  }

  // Debugging output (uncomment if needed for testing)
  /*
  Serial.print("Y = ");
  Serial.println(y);
  Serial.print("X = ");
  Serial.println(x);
  Serial.print("Rot = ");
  Serial.println(rot);
  Serial.println("");
  */

  // Delay for a short period to avoid spamming transmissions
  delay(50);
}


