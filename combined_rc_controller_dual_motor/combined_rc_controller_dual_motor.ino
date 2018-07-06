

#define enA 9
#define enB 8

#define in2 7
#define in1 6

#define in3 5
#define in4 4

#define steering_in 3
#define throttle_in 2

int ch1; // Here's where we'll keep our channel values
int ch2;

//Signal Conditioning limits
//1870 - 1078, stay 200 within the limits
const int lo=1278;
const int hi=1670;
const int deadlo=1459;
const int deadhi=1491;
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


void loop() {
  ch1 = pulseIn(throttle_in, HIGH); // Read the pulse width for throttle
  ch2 = pulseIn(steering_in, HIGH); // Read the pulse width for steering

//  Serial.println(ch1);
  int move;
  int steer;

  // Call the thing
  ch1 = normalizePulseInput(ch1);
  ch2 = normalizePulseInput(ch2);
  
//  Serial.println(ch1);
//  Serial.println(ch2);


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


  // Steering logic
  // Y-axis used for forward and backward control
  if (ch2 < center) {
    // Steer motor left
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    steer = reversePWM(ch2);
  }
  else if (ch2 > center) {
    // Steer motor right
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    steer = forwardPWM(ch2);
  } else {
    // Stop
    steer = 0;
  }

  if (steer < 70) {
    steer = 0;
  }

  // Get the steering to chill
  if (steer > 170) {
    steer = 170;
  }
  
  //Serial.println(move);
//  Serial.println(steer);
    
 analogWrite(enA, move); // Send PWM signal to L298N Enable pin
 analogWrite(enB, steer); // Send PWM signal to L298N Enable pin
}
