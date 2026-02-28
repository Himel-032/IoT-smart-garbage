// #include "HX711.h"

// #define DOUT 4
// #define CLK 2

// HX711 scale;

// // Use your calibrated value from Arduino
// float calibration_factor = -375;  

// void setup() {
//   Serial.begin(115200);
  
//   scale.begin(DOUT, CLK);
  
//   Serial.println("HX711 with ESP32");
  
//   delay(2000);   // allow sensor to stabilize
  
//   scale.set_scale();
//   scale.tare();   // reset scale to 0
  
//   Serial.println("Tare done...");
// }

// void loop() {
  
//   scale.set_scale(calibration_factor);
  
//   float units = scale.get_units(10);

//   if (units < 0)
//     units = 0.00;

//   Serial.print("Weight: ");
//   Serial.print(units);
//   Serial.println(" grams");

//   delay(500);
// }

// /*
//  Setup your scale and start the sketch WITHOUT a weight on the scale
//  Once readings are displayed place the weight on the scale
//  Press +/- or a/z to adjust the calibration_factor until the output readings match the known weight
//  Arduino pin 5 -> HX711 CLK
//  Arduino pin 6 -> HX711 DOUT
//  Arduino pin 5V -> HX711 VCC
//  Arduino pin GND -> HX711 GND 
// */

// #include "HX711.h"

// HX711 scale(6, 5);

// float calibration_factor = -375; // this calibration factor is adjusted according to my load cell
// float units;
// float ounces;

// void setup() {
//   Serial.begin(9600);
//   Serial.println("HX711 calibration sketch");
//   Serial.println("Remove all weight from scale");
//   Serial.println("After readings begin, place known weight on scale");
//   Serial.println("Press + or a to increase calibration factor");
//   Serial.println("Press - or z to decrease calibration factor");

//   scale.set_scale();
//   scale.tare();  //Reset the scale to 0

//   long zero_factor = scale.read_average(); //Get a baseline reading
//   Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
//   Serial.println(zero_factor);
// }

// void loop() {

//   scale.set_scale(calibration_factor); //Adjust to this calibration factor

//   Serial.print("Reading: ");
//   units = scale.get_units(), 10;
//   if (units < 0)
//   {
//     units = 0.00;
//   }
//   ounces = units * 0.035274;
//   Serial.print(units);
//   Serial.print(" grams"); 
//   Serial.print(" calibration_factor: ");
//   Serial.print(calibration_factor);
//   Serial.println();

//   if(Serial.available())
//   {
//     char temp = Serial.read();
//     if(temp == '+' || temp == 'a')
//       calibration_factor += 1;
//     else if(temp == '-' || temp == 'z')
//       calibration_factor -= 1;
//   }
// }

#include "HX711.h"
#define TRIG 15
#define ECHO 16
HX711 scale;

float calibration_factor = -420;
float units;
float ounces;

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 calibration sketch");

  scale.begin(4, 2);  // DOUT pin, CLK pin
  scale.set_scale();
  scale.tare();  // reset to zero

  long zero_factor = scale.read_average();
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);


  // Sonar
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.println("System Ready");
}

void loop() {
  scale.set_scale(calibration_factor);

  units = scale.get_units(10);
  if (units < 0) units = 0.00;

  Serial.print("Reading: ");
  Serial.print(units);
  Serial.print(" grams");
  Serial.print(" | calibration_factor: ");
  Serial.println(calibration_factor);

  if (Serial.available()) {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a') calibration_factor += 1;
    else if (temp == '-' || temp == 'z') calibration_factor -= 1;
  }


  // --------- Sonar ----------
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  float distance = 0;

  if (duration != 0) {
    distance = duration * 0.0343 / 2;
  }

    // --------- Print ----------
  Serial.print("Distance: ");
  Serial.print(distance, 2);
  Serial.println(" cm");

  delay(1000);
}
