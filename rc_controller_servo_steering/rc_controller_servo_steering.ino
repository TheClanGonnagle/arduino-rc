

#define enA 9
#define enB 8

#define in2 7
#define in1 6

#define in3 5
#define in4 4

#define steering_in 3
#define throttle_in 2

#define potPin 2    // select the input pin for the potentiometer

int ch1; // Here's where we'll keep our channel values
int ch2;

//Signal Conditioning limits
//1870 - 1078, stay 200 within the limits

// Servo limits: 
// 752 - 313
const int servo_hi = 752;
const int servo_lo = 313;
const int servo_center = 532;
const int servo_perm_error = 5;

const int lo=1193;
const int hi=1787;
const int deadlo=1450;
const int deadhi=1500;
const int center=1474;


// Based on: https://howtomechatronics.com/wp-content/uploads/2017/08/Arduino-and-L298N-Circuit-Diagram-DC-Motor-Control.png?x57244

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
   
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  pinMode(throttle_in, INPUT); // Set our input pins as such
  pinMode(steering_in, INPUT);

  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);


  Serial.begin(9600); // Pour a bowl of Serial
}

int normalizePulseInput(int inputValue) {

  // Handle transmitter being turned off entirely without freaking out
  if (inputValue == 0)
  {
    return center;                       
  }

  //Trim Noise from bottom end
  if (inputValue <= lo)             
  {
    return lo;
  }

  //Create Dead-Band
  if (inputValue <= deadhi && inputValue >= deadlo)     
  {
    return center;
  }

  //Trim Noise from top end
  if (inputValue >= hi)
  {
    return hi;
  }

  return inputValue;
}

int reversePWM(int inputValue) {
  return map(inputValue, deadlo, lo, 0, 255);
}

int forwardPWM(int inputValue) {
  return map(inputValue, deadhi, hi, 0, 255);
}

//int rightSteer(int inputValue) {
//  return map(inputValue, deadlo, lo, servo_lo, servo_center);
//}
//
//int leftSteer(int inputValue) {
//  return map(inputValue, deadhi, hi, servo_center, servo_hi);
//}

int getTargetPot(int inputValue) {
  return map(inputValue, lo, hi, servo_lo, servo_hi);
}

// motor location update interval markers
int lastSwitch = 0;
int stepTime = 50;


void loop() {
  ch1 = pulseIn(throttle_in, HIGH); // Read the pulse width for throttle
  ch2 = pulseIn(steering_in, HIGH); // Read the pulse width for steering
  
//  Serial.println(ch2); 

  int move;
  int steer;
  int val = analogRead(potPin);
  int targetPot = 0;

  // Call the thing
  ch1 = normalizePulseInput(ch1);
  ch2 = normalizePulseInput(ch2);

  targetPot = getTargetPot(ch2);
  
//  Serial.println(ch1);
//  Serial.println(ch2);
  
//  Serial.println(val);


  // Main Drive logic
  if (ch1 < center) {
    // Set Drive motor backward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    move = reversePWM(ch1);
  }
  else if (ch1 > center) {
    // Set Drive motor forward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    move = forwardPWM(ch1);
  } else {
    // Stop
    move = 0;
  }

  if (move < 70) {
    move = 0;
  }


  // Only steer if the target and actual are different by more than the allowable margin of error
  if (abs(val - targetPot) > servo_perm_error) {
    if (val < targetPot) { 
      // Steer motor left
//      Serial.println("left");
      
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      steer = 130;
    } else if ( val > targetPot) {
      // Steer motor right
//      Serial.println("right");
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      steer = 130;
    }
  } else {
    Serial.println("match");
    steer = 0;
  }
  


//  Serial.println(val - targetPot);
//  Serial.println(val);
//  Serial.println(targetPot);


  // Steering logic
  // Y-axis used for forward and backward control
  // This is technically working like before, just with a mapped pot value
//  if (targetPot < servo_center) {
//    // Steer motor left
//    digitalWrite(in3, HIGH);
//    digitalWrite(in4, LOW);
//    steer = reversePWM(ch2);
//  }
//  else if (targetPot > servo_center) {
//    // Steer motor right
//    digitalWrite(in3, LOW);
//    digitalWrite(in4, HIGH);
//    steer = forwardPWM(ch2);
//  } else {
//    // Stop
//    steer = 0;
//  }

  // Get the stick position as a 0-255 from all left to all right
  // Get the pot value that matches that stick position.
  // stick position is the target, pot is the position 
//  if (steer < 70) {
//    steer = 0;
//  }

  // Get the steering to chill
//  if (steer > 170) {
//    steer = 170;
//  }
//
//  if (val >= servo_hi || val <= servo_lo) {
//    steer = 0;
//  }
//   Serial.println(steer);
  //Serial.println(move);
//  Serial.println(targetPot);




  //if ((val > 582 && targetPot > val) || (val < 442 && targetPot < val)) {
//  if (val > 650  || val < 350) {
//    steer = 0;
//  }
//
//  if ( steer < 12 ) { 
//    steer = 0;
//  }
  Serial.println(steer);

// analogWrite(enA, move); // Send PWM signal to L298N Enable pin
//analogWrite(enB, steer); // Send PWM signal to L298N Enable pin

  int diff = millis() - lastSwitch;
  if (diff >= stepTime) {
    lastSwitch = millis();
    
//    Serial.println(steer);
    
    analogWrite(enB, steer); // Send PWM signal to L298N Enable pin
    
  }
}
