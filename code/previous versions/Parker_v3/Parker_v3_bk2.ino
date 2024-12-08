//pca.setPWM(leg, 1000, 0); //turns them off?
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

Adafruit_PWMServoDriver pca1 = Adafruit_PWMServoDriver(0x40);  // Left side
Adafruit_PWMServoDriver pca2 = Adafruit_PWMServoDriver(0x41);  // right side

void setup() {
  Serial.begin(9600);

  pca1.begin();
  pca1.setPWMFreq(60);
  pca2.begin();
  pca2.setPWMFreq(60);
}

String command[6];  //le string

void splitStringIntoWords(String str) {  // > Separate command into words

  //clear array words
  for (int x = 0; x < 6; x++) {
    command[x] = "";
  }

  int wordCount = 0, startIndex = 0;

  //separates words here
  while (true) {
    int spaceIndex = str.indexOf(' ', startIndex);
    if (spaceIndex == -1) {
      command[wordCount++] = str.substring(startIndex);
      break;
    }
    command[wordCount++] = str.substring(startIndex, spaceIndex);
    startIndex = spaceIndex + 1;
  }
}

void loop() {  // > LmaOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOP here

  if (Serial.available()) {
    String commandFull = Serial.readString();
    splitStringIntoWords(commandFull);
    Serial.print("> ");
    Serial.println(commandFull);

    String type = command[0];

    if (type == "move") {
      Move();
    } else if (type == "walk") {
      Walk();
    }
  }
}

void Walk() {

  // int times = command[1].toInt();

  // for (int b = 0; b < times; b++) {

  //   Step();
  // }

  Step();
}

int vel = 180;

void Step() {

  int riseAct = 0;  //may delete
  int riseDeg = 0;  //same here

  int dir = 1;    //direction of the walk
  int times = 5;  //amout of cycles (how much he walks)

  int x, y;  //temporary

  // int aFixed = 0;     //important
  // int aInverted = 0;  //important

  for (int cycle = 0; cycle < (2 * times); cycle++) {

    int cycleBinary = (cycle % 2 == 0) ? 1 : -1;  //1 motors to back / -1 motors to front

    if (dir == 1) {
      //front
      x = (cycleBinary == 1) ? 0 : -vel;
      y = (cycleBinary == 1) ? vel : 0;
    } else {
      //back
      x = (cycleBinary == 1) ? -vel : 0;
      y = (cycleBinary == 1) ? 0 : vel;
    }

    for (int a = x; a <= y; a++) {

      int aFixed = (a <= 0) ? a * -1 : a;  //RLR

      pca1.setPWM(0, 0, 115 + (180 - (((float)(40 - 90) / vel) * aFixed + 90)) * 2.694);
      pca1.setPWM(1, 0, 115 + (180 - (((float)(60 - 80) / vel) * aFixed + (80 - (riseDeg * riseAct)))) * 2.694);
      pca1.setPWM(2, 0, 115 + (180 - (((float)(10 - 50) / vel) * aFixed + 50)) * 2.694);
      //
      pca2.setPWM(3, 0, 115 + (180 - (((float)(115 - 65) / vel) * aFixed + 65)) * 2.694);
      pca2.setPWM(4, 0, 115 + (180 - (((float)(70 - 60) / (vel / 2)) * abs(aFixed - vel / 2) + (60 - (riseDeg * riseAct)))) * 2.694);
      pca2.setPWM(5, 0, 115 + (180 - (((float)(30 - 20) / (vel / 2)) * abs(aFixed - vel / 2) + 20)) * 2.694);
      //
      pca1.setPWM(6, 0, 115 + (180 - (((float)(90 - 140) / vel) * aFixed + 140)) * 2.694);
      pca1.setPWM(7, 0, 115 + (180 - (((float)(80 - 60) / vel) * aFixed + (60 - (riseDeg * riseAct)))) * 2.694);
      pca1.setPWM(8, 0, 115 + (180 - (((float)(50 - 10) / vel) * aFixed + 10)) * 2.694);

      int aInverted = vel - aFixed;  //LRL

      pca2.setPWM(0, 0, 115 + (180 - (((float)(140 - 90) / vel) * aInverted + 90)) * 2.694);
      pca2.setPWM(1, 0, 115 + (180 - (((float)(60 - 80) / vel) * aInverted + (80 - (riseDeg * riseAct)))) * 2.694);
      pca2.setPWM(2, 0, 115 + (180 - (((float)(10 - 50) / vel) * aInverted + 50)) * 2.694);
      //
      pca1.setPWM(3, 0, 115 + (180 - (((float)(65 - 115) / vel) * aInverted + 115)) * 2.694);
      pca1.setPWM(4, 0, 115 + (180 - (((float)(70 - 60) / (vel / 2)) * abs(aInverted - vel / 2) + (60 - (riseDeg * riseAct)))) * 2.694);
      pca1.setPWM(5, 0, 115 + (180 - (((float)(30 - 20) / (vel / 2)) * abs(aInverted - vel / 2) + 20)) * 2.694);
      //
      pca2.setPWM(6, 0, 115 + (180 - (((float)(90 - 40) / vel) * aInverted + 40)) * 2.694);
      pca2.setPWM(7, 0, 115 + (180 - (((float)(80 - 60) / vel) * aInverted + (60 - (riseDeg * riseAct)))) * 2.694);
      pca2.setPWM(8, 0, 115 + (180 - (((float)(50 - 10) / vel) * aInverted + 10)) * 2.694);
    }
  }
}

float MidleMov(int to, int from, int a, int posMotor, int nCycle, int direction) {

  if (nCycle % 2 == 1) {
    //nromal functions
    if (posMotor % 2 == 0) {
      return ((float)(to - from) / vel) * a + from;
    } else {
      return ((float)(to - from) / (vel / 2)) * abs(a - vel / 2) + from;
    }

  } else {
    if (direction == 1) {

      //walking front (motors to back)
      if (posMotor == 1) {
        return max(((60 - 30) / (vel / 3)) * a - 30, ((30 - 80) / ((vel / 3) * 2)) * a + 80);

      } else if (posMotor == 2) {
        return ((float)(70 - 30) / (vel / 2)) * abs(a - (vel / 2.0)) + 30;

      } else {
        return max(((60 - 30) / (vel / 3)) * (180 - a) - 30, ((30 - 80) / ((vel / 3) * 2)) * (180 - a) + 80);
      }

    } else {

      //walking back (motors to front)
      if (posMotor == 1) {
        return max(((60 - 30) / (vel / 3)) * a - 30, ((30 - 80) / ((vel / 3) * 2)) * a + 80);

      } else if (posMotor == 2) {
        return ((float)(70 - 30) / (vel / 2)) * abs(a - (vel / 2.0)) + 30;

      } else {
        return max(((60 - 30) / (vel / 3)) * (180 - a) - 30, ((30 - 80) / ((vel / 3) * 2)) * (180 - a) + 80);
      }
    }
  }
}

void Move() {

  String mode = command[1];    //amount of legs basically
  String binary = command[2];  //what motors to move
  int pos = 3;                 //because of the binary thing

  if (mode == "1" || mode == "2" || mode == "3" || mode == "4" || mode == "5" || mode == "6") {

    int leg = mode.toInt();

    (binary.charAt(0) == '1') ? PWM(1, leg, 0, command[pos++].toInt()) : 0;
    (binary.charAt(1) == '1') ? PWM(1, leg, 1, command[pos++].toInt()) : 0;
    (binary.charAt(2) == '1') ? PWM(1, leg, 2, command[pos++].toInt()) : 0;

  } else if (mode == "trio" || mode == "trie") {

    int side = (mode.charAt(3) == 'o') ? 1 : 0;

    (binary.charAt(0) == '1') ? PWM(3, side, 0, command[pos++].toInt()) : 0;
    (binary.charAt(1) == '1') ? PWM(3, side, 1, command[pos++].toInt()) : 0;
    (binary.charAt(2) == '1') ? PWM(3, side, 2, command[pos++].toInt()) : 0;

  } else if (mode == "all") {

    (binary.charAt(0) == '1') ? PWM(6, -1, 0, command[pos++].toInt()) : 0;
    (binary.charAt(1) == '1') ? PWM(6, -1, 1, command[pos++].toInt()) : 0;
    (binary.charAt(2) == '1') ? PWM(6, -1, 2, command[pos++].toInt()) : 0;
  }
}

int PWM(int amount, int leg, int mot, int degree) {

  if (amount == 1) {

    if (leg <= 3) {
      pca1.setPWM((leg - 1) * 3 + mot, 0, round(115 + (180 - degree) * 2.694));
    } else if (leg <= 6) {
      pca2.setPWM((leg - 4) * 3 + mot, 0, round(115 + (180 - degree) * 2.694));
    } else {
      Serial.println("This leg does not exist");
    }

  } else if (amount == 3) {

    if (leg == 1) {
      pca1.setPWM(0 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));  //leg 1
      pca2.setPWM(1 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));  //leg 5
      pca1.setPWM(2 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));  //leg 3
    } else {
      pca2.setPWM(0 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));  //leg 4
      pca1.setPWM(1 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));  //leg 2
      pca2.setPWM(2 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));  //leg 6
    }

  } else if (amount == 6) {

    pca1.setPWM(0 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));
    pca1.setPWM(1 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));
    pca1.setPWM(2 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));

    pca2.setPWM(0 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));
    pca2.setPWM(1 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));
    pca2.setPWM(2 * 3 + mot, 0, round(115 + (180 - degree) * 2.694));
  }

  return 1;
}