#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define BATTERY_CNT 2
#define MAX_VOLT 4.2
#define MIN_VOLT 3.2
#define EMPTY_VOLT 3.0
#define DRIVE_SPEED 150
#define BACK_SPEED 120
#define DIRECTION_SPEED 140
#define DIRECTION_SPEED2 90

#define DM_SPEED_PIN1 5
#define DM_DRIVE_PIN1 2
#define DM_BACK_PIN1 3

#define DM_SPEED_PIN2 6
#define DM_DRIVE_PIN2 4
#define DM_BACK_PIN2 9

#define BATTERY_PIN A3
#define TRIG_PIN 7
#define ECHO_PIN 8
#define PIEZO_BUZZER A2

#define MIN_DISTANCE 20

#define LED_PIN A1
#define LED_NUM 4

Servo servo;
SoftwareSerial BTSerial(10, 11);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);

int leftDistance, rightDistance;
bool isLowBattery = true; // 배터리 상태 MIN_VOLT 이하면 true
boolean isAuto = false;
boolean isStarted = false;
bool nowLedOn = false;
int curBatteryRate = 0; // 현재 배터리 용량 (%)

void setup() {
  BTSerial.begin(9600);
  Serial.begin(9600);
  servo.attach(A0);
  servo.write(90);
  pixels.begin();
 
  pinMode(DM_SPEED_PIN1, OUTPUT);
  pinMode(DM_DRIVE_PIN1, OUTPUT);
  pinMode(DM_BACK_PIN1, OUTPUT);
 
  pinMode(DM_SPEED_PIN2, OUTPUT);
  pinMode(DM_DRIVE_PIN2, OUTPUT);
  pinMode(DM_BACK_PIN2, OUTPUT);
 
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  checkBattery(); // 전압 측정

  char cmd = getCommand();
  if (cmd == 'A') {
    isAuto = true;
  } else if (cmd == 'M') {
    isAuto = false;
    isStarted = false;
    stopRobot();
  } else if (cmd == 'E') {
    isStarted = false;
    stopRobot();
  } else if (cmd == 'F' || cmd == 'B' || cmd == 'L' || cmd == 'R' || cmd == '1' || cmd == '2' || cmd == '3' || cmd == '4') {
    isStarted = true;
    moveManual(cmd);
  } else if (cmd == 'a') {
    ledON();
  } else if (cmd == 'b') {
    
  } else if (cmd == 'c') {
    
  } else if (cmd == 'd') {
    
  }

  if (isAuto) {
    moveAuto();
  }
}

char getCommand() {
  char cmd;
  if (BTSerial.available()) { //블루투스에서 넘어온 값이 있으면
    cmd = (char)BTSerial.read(); //읽어들인 문자(한글자)를 data에 저장
  }
  return cmd;
}

void stopRobot() {
  analogWrite(DM_SPEED_PIN1, 0);
  analogWrite(DM_SPEED_PIN2, 0);
}

void moveManual(char cmd) {
  if (cmd == 'F') {
    changeDirection(DRIVE_SPEED, DRIVE_SPEED, HIGH, LOW, HIGH, LOW);
  } else if (cmd == 'B') {
    changeDirection(BACK_SPEED, BACK_SPEED, LOW, HIGH, LOW, HIGH);
  } else if (cmd == 'L') {
    changeDirection(DIRECTION_SPEED, DIRECTION_SPEED, HIGH, LOW, LOW, HIGH);
  } else if (cmd == 'R') {
    changeDirection(DIRECTION_SPEED, DIRECTION_SPEED, LOW, HIGH, HIGH, LOW);
  } else if (cmd == '1') {
    changeDirection(DIRECTION_SPEED, DIRECTION_SPEED2, HIGH, LOW, HIGH, LOW);
  } else if (cmd == '2') {
    changeDirection(DIRECTION_SPEED2, DIRECTION_SPEED, HIGH, LOW, HIGH, LOW);
  } else if (cmd == '3') {
    changeDirection(DIRECTION_SPEED, DIRECTION_SPEED2, LOW, HIGH, LOW, HIGH);
  } else if (cmd == '4') {
    changeDirection(DIRECTION_SPEED2, DIRECTION_SPEED, LOW, HIGH, LOW, HIGH);
  }
}

void changeDirection(int fSpeed, int bSpeed, int dPin1, int bPin1, int dPin2, int bPin2) {
  analogWrite(DM_SPEED_PIN1, fSpeed);
  analogWrite(DM_SPEED_PIN2, bSpeed);
  digitalWrite(DM_DRIVE_PIN1, dPin1);
  digitalWrite(DM_BACK_PIN1, bPin1);
  digitalWrite(DM_DRIVE_PIN2, dPin2);
  digitalWrite(DM_BACK_PIN2, bPin2);
}

void moveAuto() {
  int distanceFwd = ping();
  
  if (distanceFwd > MIN_DISTANCE) {
    changeDirection(DRIVE_SPEED, DRIVE_SPEED, HIGH, LOW, HIGH, LOW);
  } else if (distanceFwd < MIN_DISTANCE/2 && distanceFwd > 0) {
    changeDirection(BACK_SPEED, BACK_SPEED, LOW, HIGH, LOW, HIGH);
    delay(500);
  } else {
    stopRobot();
    
    servo.write(30);
    tone(PIEZO_BUZZER,1000,50);
    delay(80);
    tone(PIEZO_BUZZER,600,50);
    delay(100);
    tone(PIEZO_BUZZER,700,50);
    delay(100);
    tone(PIEZO_BUZZER,700,50);
    delay(180);
    tone(PIEZO_BUZZER,500,50);
    delay(80);
    tone(PIEZO_BUZZER,1000,50);
    delay(60);
    tone(PIEZO_BUZZER,1000,50);
    delay(100);
    rightDistance = ping();

    servo.write(150);
    tone(PIEZO_BUZZER,600,50);
    delay(100);
    tone(PIEZO_BUZZER,600,50);
    delay(100);
    tone(PIEZO_BUZZER,700,50);
    delay(100);
    tone(PIEZO_BUZZER,700,50);
    delay(80);
    tone(PIEZO_BUZZER,800,50);
    delay(80);
    tone(PIEZO_BUZZER,1000,50);
    delay(40);
    tone(PIEZO_BUZZER,15000,50);
    delay(100);
    leftDistance = ping();
    
    servo.write(90);
    delay(300);
    
    if (leftDistance > rightDistance) {
      changeDirection(DIRECTION_SPEED, DIRECTION_SPEED, HIGH, LOW, LOW, HIGH);
    } else if (leftDistance < rightDistance) {
      changeDirection(DIRECTION_SPEED, DIRECTION_SPEED, LOW, HIGH, HIGH, LOW);
    }
    delay(500);
  }
}

long ping() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, LOW);
 
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  return (duration / 2) / 29;
}

void ledON() {
  for (int i = 0; i < LED_NUM; i++) {
    if (!nowLedOn) { // led 꺼져있는 경우
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
      pixels.show();
    } else { // led 켜져있는 경우
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();
    }
  }
  if (nowLedOn == true) {
    nowLedOn = false;
  } else {
    nowLedOn = true;
  }
}

void checkBattery() {
  if (!isStarted) { // 운전중에는 전압 체크가 제대로 되지 않으므로 정지상태에서만 전압체크
    float curVolt = (analogRead(BATTERY_PIN) / (float)BATTERY_CNT * 3) / (float)1023 * 5;
    if (curVolt <= MIN_VOLT && !isLowBattery) {
      isLowBattery = true;
    } else if (isLowBattery) {
      isLowBattery = false;
    }

    int preRate = curBatteryRate;
    if (curVolt <= EMPTY_VOLT) {
      curBatteryRate = 0;
    } else if (curVolt <= MIN_VOLT) {
      curBatteryRate = 20;
    } else if (curVolt <= 3.5) {
      curBatteryRate = 40;
    } else if (curVolt <= 3.8) {
      curBatteryRate = 60;
    } else if (curVolt <= 4.0) {
      curBatteryRate = 80;
    } else {
      curBatteryRate = 100;
    }
    
    if (preRate != curBatteryRate) {
      String writeVal = "B:";
      writeVal.concat(curBatteryRate);
      BTSerial.println(writeVal);
    }
  }
}
