#include <SerialCommand.h>
#include <Servo.h>

#define TRIG_PIN 6
#define ECHO_PIN 5

#define SERVO_PIN 3
#define SERVO_CLOSE_DEGREE 180 // degree
#define SERVO_OPEN_DEGREE 90 // degree
#define SERVO_MAX_DETECT_DISTANCE 20 // centimeter
#define SERVO_TRANSITION_TIME 2000 // millisecond 

long keepOpenTime = 3000; // millisecond
bool isLocked = false;

SerialCommand SCmd;
Servo servo;

void setup() {
  Serial.begin(9600);
  SCmd.addCommand("BlockLid", blockLid);
  SCmd.addCommand("KeepOpenTime", updateKeepOpenTime);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  servo.attach(SERVO_PIN);
}

void blockLid() {
  int val;
  char *arg;
  arg = SCmd.next();
  val = atoi(arg);

  Serial.println(val);
  
  if (val == 0) {
    isLocked = false;
  } else {
    isLocked = true;
  }
}

void updateKeepOpenTime() {
  long val;
  char *arg;
  arg = SCmd.next();
  val = atol(arg);

  Serial.println(val);

  if (val >= 1000) {
    keepOpenTime = val;
  }
}

void loop() {
  SCmd.readSerial();

  if (isLocked) {
    if (servo.read() != SERVO_CLOSE_DEGREE) {
      // Close the lid
      controlRotation(servo, SERVO_CLOSE_DEGREE, SERVO_TRANSITION_TIME);
    }

    return;
  }

  digitalWrite(TRIG_PIN, HIGH);
  delay(1);
  digitalWrite(TRIG_PIN, LOW);

  int duration, distance;
  duration = pulseIn(ECHO_PIN, HIGH); // Measure the pulse input in echo pin
  distance = (duration / 2) / 29.1; // Distance is half the duration devided by 29.1

  // if distance less than MAX_DETECT_DISTANCE and more than 0, open lid
  if (distance <= SERVO_MAX_DETECT_DISTANCE && distance >= 0) {
    Serial.println("*");
    controlRotation(servo, SERVO_CLOSE_DEGREE - SERVO_OPEN_DEGREE, SERVO_TRANSITION_TIME);
    delay(keepOpenTime);
  } else if (servo.read() != SERVO_CLOSE_DEGREE) {
    // Close the lid
    controlRotation(servo, SERVO_CLOSE_DEGREE, SERVO_TRANSITION_TIME);
  }
}

void controlRotation(Servo s, int deg, int duration) {
  int changes = deg - s.read();
  int stepDelay = duration/100;
  int degPerStep = changes/stepDelay;

  while ((changes > 0 && s.read() < deg) || (changes < 0 && s.read() > deg)) {
    s.write(s.read() + degPerStep);
    delay(stepDelay);
  }

  // Make sure the result is expected
  s.write(deg);
}
