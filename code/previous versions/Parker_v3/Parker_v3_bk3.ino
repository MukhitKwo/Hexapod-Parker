//pca.setPWM(leg, 1000, 0); //turns them off?
#include <Adafruit_PWMServoDriver.h>

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

int degFB[6][3] = {
  { 40, 90, 140 },  //front and back > base
  { 60, 80, 0 },    //front and back > coxa
  { 10, 50, 0 },    //front and back > tibia
  { 115, 65, 0 },   //middle > base
  { 70, 60, 0 },    //middle > coxa
  { 30, 20, 0 }     //middle > tibia
};

int vel = 180;
int rise = 30;

void Walk() {

  int dir = (command[1] == "f") ? 1 : -1;
  int times = command[2].toInt();

  FrontBack(dir, times);
}

void FrontBack(int direction, int times) {

  int x, y;  //temporary

  for (int cycle = 0; cycle < (2 * times); cycle++) {

    int cycleBinary = (cycle % 2 == 0) ? 1 : -1;  //1 -> legs to back / -1 -> legs to front

    if (direction == 1) {
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

      pca1.setPWM(0, 0, 115 + (180 - (((float)(degFB[0][0] - degFB[0][1]) / vel) * aFixed + 90)) * 2.694);
      pca1.setPWM(1, 0, 115 + (180 - MidleMov(degFB[1][0], degFB[1][1], 1, 1, aFixed, cycleBinary)) * 2.694);
      pca1.setPWM(2, 0, 115 + (180 - (((float)(degFB[2][0] - degFB[2][1]) / vel) * aFixed + 50)) * 2.694);
      //
      pca2.setPWM(3, 0, 115 + (180 - (((float)(degFB[3][0] - degFB[3][1]) / vel) * aFixed + 65)) * 2.694);
      pca2.setPWM(4, 0, 115 + (180 - MidleMov(degFB[4][0], degFB[4][1], 2, 1, aFixed, cycleBinary)) * 2.694);
      pca2.setPWM(5, 0, 115 + (180 - (((float)(degFB[5][0] - degFB[5][1]) / (vel / 2)) * abs(aFixed - vel / 2) + 20)) * 2.694);
      //
      pca1.setPWM(6, 0, 115 + (180 - (((float)(degFB[0][1] - degFB[0][2]) / vel) * aFixed + 140)) * 2.694);
      pca1.setPWM(7, 0, 115 + (180 - MidleMov(degFB[1][1], degFB[1][0], 3, 1, aFixed, cycleBinary)) * 2.694);
      pca1.setPWM(8, 0, 115 + (180 - (((float)(degFB[2][1] - degFB[2][0]) / vel) * aFixed + 10)) * 2.694);

      int aInverted = vel - aFixed;  //LRL

      pca2.setPWM(0, 0, 115 + (180 - (((float)(degFB[0][2] - degFB[0][1]) / vel) * aInverted + 90)) * 2.694);
      pca2.setPWM(1, 0, 115 + (180 - MidleMov(degFB[1][0], degFB[1][1], 1, 2, aInverted, cycleBinary)) * 2.694);
      pca2.setPWM(2, 0, 115 + (180 - (((float)(degFB[2][0] - degFB[2][1]) / vel) * aInverted + 50)) * 2.694);
      //
      pca1.setPWM(3, 0, 115 + (180 - (((float)(degFB[3][1] - degFB[3][0]) / vel) * aInverted + 115)) * 2.694);
      pca1.setPWM(4, 0, 115 + (180 - MidleMov(degFB[4][0], degFB[4][1], 2, 2, aInverted, cycleBinary)) * 2.694);
      pca1.setPWM(5, 0, 115 + (180 - (((float)(degFB[5][0] - degFB[5][1]) / (vel / 2)) * abs(aInverted - vel / 2) + 20)) * 2.694);

      pca2.setPWM(6, 0, 115 + (180 - (((float)(degFB[0][1] - degFB[0][0]) / vel) * aInverted + 40)) * 2.694);
      pca2.setPWM(7, 0, 115 + (180 - MidleMov(degFB[1][1], degFB[1][0], 3, 2, aInverted, cycleBinary)) * 2.694);
      pca2.setPWM(8, 0, 115 + (180 - (((float)(degFB[2][1] - degFB[2][0]) / vel) * aInverted + 10)) * 2.694);
    }
  }
}

float MidleMov(int to, int from, int posMotor, int sideMotor, int a, int nCycle) {

  if (sideMotor == 2) {
    nCycle = nCycle * -1;
  }

  if (nCycle == 1) {
    //touching floor
    if (posMotor == 1) {
      return ((float)(to - from) / vel) * a + from;
    } else if (posMotor == 2) {
      return ((float)(to - from) / (vel / 2)) * abs(a - vel / 2) + from;
    } else {
      return ((float)(to - from) / vel) * a + from;
    }

  } else if (nCycle == -1) {
    //touching air
    if (posMotor == 1) {

      // return max(((float)(60 - 30) / (vel / 3)) * a - 30, ((float)(30 - 80) / ((vel / 3) * 2)) * a + 80);
      return max(((float)(to - (to - rise)) / (vel / 3)) * a + (to - (3 * rise)), ((float)((to - rise) - from) / ((vel / 3) * 2)) * a + from);

    } else if (posMotor == 2) {

      // return ((float)(70 - 40) / (vel / 2)) * abs(a - (vel / 2.0)) + 40;
      return ((float)(to - (to - rise)) / (vel / 2)) * abs(a - (vel / 2)) + (to - rise);

    } else {

      //return max(((float)(60 - 30) / (vel / 3)) * (vel - a) - 30, ((float)(30 - 80) / ((vel / 3) * 2)) * (vel - a) + 80);
      return max(((float)(from - (from - rise)) / (vel / 3)) * (vel - a) + (from - (3 * rise)), ((float)((from - rise) - to) / ((vel / 3) * 2)) * (vel - a) + to);
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