/* Rob the Robot Code

Contains elements from codes by Scott Fitzgerald, osoyoo.com, makerguides.com

*/

// Including the necessary libraries
#include <Servo.h>
#include <Adafruit_TCS34725.h>
#include <SoftwareSerial.h>

SoftwareSerial bluetooth(10, 11); // RX, TX pins for the Bluetooth module

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Servo myservo;

int countUnknown; // a variable to keep track of how many 'unknown' colors have been detected in a row; it's used for error detection
String receivedData; // a variable to store the data received from the bluetooth module

void setup() {
  myservo.attach(9);  // attaching the servo on pin 9 to the servo object
  Serial.begin(9600); // initializing serial communication
  bluetooth.begin(9600); // initializing Bluetooth communication
  if (tcs.begin()) {
    // Serial.println("Color sensor found!");
  } else {
    Serial.println("Error: no color sensor found; check connections"); // Error 1: no color sensor found
    while (1); // stop the code if no color sensor is detected
  }
  tcs.setInterrupt(false); // disabling the color sensor interrupt
}

void loop() {
  // determine the detected color using the RGB values
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  
  float colorTemp = tcs.calculateColorTemperature(r, g, b);
  float lux = tcs.calculateLux(r, g, b);

  // Code used for debugging
  // Serial.print("r: ");
  // Serial.print(r);
  // Serial.print(" g: ");
  // Serial.print(g);
  // Serial.print(" b: ");
  // Serial.println(b);

  if (bluetooth.available()) { // check if data is available to be read from Bluetooth
    receivedData = bluetooth.readString(); // read the data received from Bluetooth and store it in a string variable
    // Serial.println(receivedData); // print the received data for debugging
    if (receivedData == "0"){ // the white button on the app has been pushed
      Serial.println("Currently pushing WHITE disks into the box.");
    }
    if (receivedData == "1"){ // the black button on the app has been pushed
      Serial.println("Currently pushing BLACK disks into the box.");
    }
  }

  if (receivedData == "0"){ // the robot is in the mode where it pushes white disks
    back(); // move the servo back to the initial position
    if (determineColor(r, g, b) == 1) {
      delay(900); // wait 900 ms before reading next color
      countUnknown = 0; // reset the count for unknown colors to 0
    } else if (determineColor(r, g, b) == 0) {
        Serial.println("Detected a WHITE disk; pushing it into the box");
        delay(900); // wait 900 ms before actioning the servo motor/pushing the disk
        pushDisk(); // push the white disk
        delay(900); // wait 900 ms before reading the next color
        countUnknown = 0; // reset the count for unknown colors to 0

    } else { // the color is neither white or black
      delay(100); // wait 100 ms before reading the next color
      countUnknown++; // increase the count of unknown colors
    }

    if (countUnknown == 2){
      Serial.println("Error: forbidden color"); // Error 3: the color detected a color other than white or black
    }

    if (countUnknown == 5){
      Serial.println("Error: color sensor blocked"); // Error 4: the color sensor is blocked
      // while (1); // the robot stops
    }
  }

  else if (receivedData == "1"){ // the robot is in the mode where it pushes black disks
    pushBlackDisk(); // the pushing mechanism is set to push black disks
    if (determineColor(r, g, b) == 1) {
      pushBlackDisk(); // the pushing mechanism is set to push black disks
      delay(900); // wait 900 ms before reading the next color 
      countUnknown = 0; // reset the count for unknown colors to 0
    } else if (determineColor(r, g, b) == 0) {
        Serial.println("Detected a WHITE disk; letting it pass to the box at the end of the belt");
        delay(750); // wait 750 ms before moving servo back to initial position
        back();
        delay(900); // wait 900 ms before reading the next color 
        countUnknown = 0;

    } else { // the color is neither white or black
      delay(100);
      countUnknown++;
    }

    if (countUnknown == 2){
      Serial.println("Error: forbidden color"); // Error 3: the color detected a color other than white or black
    }

    if (countUnknown == 5){
      Serial.println("Error: color sensor blocked"); // Error 4: the color sensor is blocked
      // while (1); // the robot stops
    }
  }

  else if (receivedData == ""){ // the robot has been woken up
    receivedData = "0";
    back();
    Serial.println("Thanks for waking me up!");
    Serial.println("Use the app to tell me what color you want me to push in the box.");
    Serial.println("Currently pushing WHITE disks into the box.");
  }
  else{
    receivedData = "0";
    Serial.println("Error: buttons clicked too fast; the mode is set to default");
  }

}


// A function to the determine the detected color using RGB values read from the color sensor

int determineColor(uint16_t r, uint16_t g, uint16_t b){
  if (r < 100 && g < 100 && b < 100) return 1; // black
  else if (r > 180 && g > 180 && b > 180) return 0; // white
  else return 2; // unknown
}

// A function to push the disks

void pushDisk(){
  forth();
  delay(80);
  back();
  delay(80);
}

// A function to push the black disks

void pushBlackDisk(){
  myservo.write(20);
}

// A function to move the servo motor back to its initial position

void back(){
  myservo.write(-30);
}

// A function to move the servo motor to the pushing position

void forth(){
  myservo.write(30);
}


  
