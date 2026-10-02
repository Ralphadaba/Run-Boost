/*

- Speed is measured from Left-to-Left step and Right-to-Right step.
- As the interval decreases (shorter time between steps), the brightness increases.
- If a user slows down or stops, the lights dim back to zero.

Pin A1 : Sensor 1  ->  Pin 12 : LED 1
Pin A2 : Sensor 2  ->  Pin 27 : LED 2
*/

// Sensor pins
const int sensor1Pin = A1;
const int sensor2Pin = A2;

// LED pins
const int led1Pin = 12;
const int led2Pin = 27;

const int pressureThreshold = 1500; 

// Cadence Speed Ranges (in milliseconds)
// Max Speed: 200ms (Maximum Brightness = 255)
// Min Speed: 1000ms (Minimum Brightness = 0)
const unsigned long maxSpeedInterval = 200;  
const unsigned long minSpeedInterval = 1000; 

// Tracking Variables for Sensor 1 (Left)
unsigned long s1LastTime = 0;
bool s1Triggered = false;
int led1TargetBrightness = 0;
int led1CurrentBrightness = 0;

// Tracking Variables for Sensor 2 (Right)
unsigned long s2LastTime = 0;
bool s2Triggered = false;
int led2TargetBrightness = 0;
int led2CurrentBrightness = 0;

void setup() {
  Serial.begin(9600);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
}

void loop() {
  int sensor1Value = analogRead(sensor1Pin);
  int sensor2Value = analogRead(sensor2Pin);
  unsigned long currentTime = millis();

 
  // LEFT FOOT LOGIC (Sensor 1 -> LED 1)
  if (sensor1Value > pressureThreshold && !s1Triggered) {
    s1Triggered = true;
    
    if (s1LastTime > 0) {
      unsigned long leftInterval = currentTime - s1LastTime;
      
      // Map the time interval to a brightness level (0 - 255)
      // Shorter interval (faster) = higher brightness
      if (leftInterval <= maxSpeedInterval) {
        led1TargetBrightness = 255; // Full brightness for sprinting
      } else if (leftInterval >= minSpeedInterval) {
        led1TargetBrightness = 0;   // Walking/stopped
      } else {
        // Calculate brightness between min and max speed boundaries
        led1TargetBrightness = map(leftInterval, minSpeedInterval, maxSpeedInterval, 0, 255);
      }
      
      Serial.print("Left Step Interval: ");
      Serial.print(leftInterval);
      Serial.print("ms -> Target Brightness: ");
      Serial.println(led1TargetBrightness);
    }
    s1LastTime = currentTime;
  }

  // If they stop moving their left foot, fade the target brightness to 0
  if (currentTime - s1LastTime > minSpeedInterval) {
    led1TargetBrightness = 0;
  }

  // Reset trigger state when foot lifts
  if (sensor1Value < (pressureThreshold - 300) && s1Triggered) {
    s1Triggered = false;
  }


  // RIGHT FOOT LOGIC (Sensor 2 -> LED 2)
  if (sensor2Value > pressureThreshold && !s2Triggered) {
    s2Triggered = true;
    
    if (s2LastTime > 0) {
      unsigned long rightInterval = currentTime - s2LastTime;
      
      if (rightInterval <= maxSpeedInterval) {
        led2TargetBrightness = 255;
      } else if (rightInterval >= minSpeedInterval) {
        led2TargetBrightness = 0;
      } else {
        led2TargetBrightness = map(rightInterval, minSpeedInterval, maxSpeedInterval, 0, 255);
      }
      
      Serial.print("Right Step Interval: ");
      Serial.print(rightInterval);
      Serial.print("ms -> Target Brightness: ");
      Serial.println(led2TargetBrightness);
    }
    s2LastTime = currentTime;
  }

  // If they stop moving their right foot, fade the target brightness to 0
  if (currentTime - s2LastTime > minSpeedInterval) {
    led2TargetBrightness = 0;
  }

  // Reset trigger state when foot lifts
  if (sensor2Value < (pressureThreshold - 300) && s2Triggered) {
    s2Triggered = false;
  }

  // SMOOTH LED FADING AND OUTPUT
  // Smoothly slide current brightness toward target brightness to avoid harsh flickers
  if (led1CurrentBrightness < led1TargetBrightness) led1CurrentBrightness += 5;
  if (led1CurrentBrightness > led1TargetBrightness) led1CurrentBrightness -= 5;
  
  if (led2CurrentBrightness < led2TargetBrightness) led2CurrentBrightness += 5;
  if (led2CurrentBrightness > led2TargetBrightness) led2CurrentBrightness -= 5;

  // Constrain for analogWrite
  led1CurrentBrightness = constrain(led1CurrentBrightness, 0, 255);
  led2CurrentBrightness = constrain(led2CurrentBrightness, 0, 255);

  // Output to the LEDs
  analogWrite(led1Pin, led1CurrentBrightness);
  analogWrite(led2Pin, led2CurrentBrightness);

  delay(10); // Stability delay
}