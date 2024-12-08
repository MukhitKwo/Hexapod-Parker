//pca.setPWM(leg, 1000, 0); //turns them off?
// convert all small ints to int8_t
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

String command[7];  //le string

void splitStringIntoWords(String str) {  // > Separate command into words

  //clear array words
  for (int x = 0; x < 7; x++) {
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

int degFB[6][2] = {
  { 90, 50 },   //front and back > base (-/+)
  { 80, 60 },   //front and back > coxa (->)
  { 50, 10 },   //front and back > tibia (->)
  { 65, 115 },  //middle > base (->)
  { 60, 70 },   //middle > coxa (->)
  { 20, 30 }    //middle > tibia (->)
};

int vel = 120;
int8_t rise = 30;
int lastDir = 0;

void Walk() {

  int8_t dir = (command[1] == "f") ? 1 : -1;
  int8_t times = command[2].toInt();

  FrontBack(dir, times);
}

void FrontBack(int8_t direction, int8_t times) {

  int x, y;

  if (lastDir != direction) {
    times += 1;
  }

  for (int cycle = ((direction == lastDir) ? 0 : 1); cycle < times * 2; cycle++) {

    int8_t cycleBinary = (cycle % 2 == 0) ? 1 : -1;  //1 -> legs to back / -1 -> legs to front

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

      int aFixed = (a <= 0) ? a * -1 : a;  //LRL

      pca1.setPWM(0, 0, 115 + (180 - (((float)((degFB[0][0] - degFB[0][1]) - degFB[0][0]) / vel) * SmoothFixed(aFixed, 1, x, times, cycle, direction) + degFB[0][0])) * 2.694);
      pca1.setPWM(1, 0, 115 + (180 - MidleMov(degFB[1][1], degFB[1][0], 1, 1, aFixed, cycleBinary)) * 2.694);
      pca1.setPWM(2, 0, 115 + (180 - (((float)(degFB[2][1] - degFB[2][0]) / vel) * aFixed + degFB[2][0])) * 2.694);
      //
      pca2.setPWM(3, 0, 115 + (180 - (((float)(degFB[3][1] - degFB[3][0]) / vel) * SmoothFixed(aFixed, 1, x, times, cycle, direction) + degFB[3][0])) * 2.694);
      pca2.setPWM(4, 0, 115 + (180 - MidleMov(degFB[4][1], degFB[4][0], 2, 1, aFixed, cycleBinary)) * 2.694);
      pca2.setPWM(5, 0, 115 + (180 - (((float)(degFB[5][1] - degFB[5][0]) / (vel / 2)) * abs(aFixed - vel / 2) + degFB[5][0])) * 2.694);
      //
      pca1.setPWM(6, 0, 115 + (180 - (((float)(degFB[0][0] - (degFB[0][0] + degFB[0][1])) / vel) * SmoothFixed(aFixed, 1, x, times, cycle, direction) + (degFB[0][0] + degFB[0][1]))) * 2.694);
      pca1.setPWM(7, 0, 115 + (180 - MidleMov(degFB[1][0], degFB[1][1], 3, 1, aFixed, cycleBinary)) * 2.694);
      pca1.setPWM(8, 0, 115 + (180 - (((float)(degFB[2][0] - degFB[2][1]) / vel) * aFixed + degFB[2][1])) * 2.694);

      int aInverted = vel - aFixed;  //RLR

      pca2.setPWM(0, 0, 115 + (180 - (((float)((degFB[0][0] + degFB[0][1]) - degFB[0][0]) / vel) * SmoothFixed(aInverted, 2, x, times, cycle, direction) + degFB[0][0])) * 2.694);
      pca2.setPWM(1, 0, 115 + (180 - MidleMov(degFB[1][1], degFB[1][0], 1, 2, aInverted, cycleBinary)) * 2.694);
      pca2.setPWM(2, 0, 115 + (180 - (((float)(degFB[2][1] - degFB[2][0]) / vel) * aInverted + degFB[2][0])) * 2.694);
      //
      pca1.setPWM(3, 0, 115 + (180 - (((float)(degFB[3][0] - degFB[3][1]) / vel) * SmoothFixed(aInverted, 2, x, times, cycle, direction) + degFB[3][1])) * 2.694);
      pca1.setPWM(4, 0, 115 + (180 - MidleMov(degFB[4][1], degFB[4][0], 2, 2, aInverted, cycleBinary)) * 2.694);
      pca1.setPWM(5, 0, 115 + (180 - (((float)(degFB[5][1] - degFB[5][0]) / (vel / 2)) * abs(aInverted - vel / 2) + degFB[5][0])) * 2.694);
      //
      pca2.setPWM(6, 0, 115 + (180 - (((float)(degFB[0][0] - (degFB[0][0] - degFB[0][1])) / vel) * SmoothFixed(aInverted, 2, x, times, cycle, direction) + (degFB[0][0] - degFB[0][1]))) * 2.694);
      pca2.setPWM(7, 0, 115 + (180 - MidleMov(degFB[1][0], degFB[1][1], 3, 2, aInverted, cycleBinary)) * 2.694);
      pca2.setPWM(8, 0, 115 + (180 - (((float)(degFB[2][0] - degFB[2][1]) / vel) * aInverted + degFB[2][1])) * 2.694);
    }
  }

  lastDir = direction;
}

float SmoothFixed(int a, int sideMotor, int from, int times, int cycleNow, int direction) {

  if (cycleNow == ((direction == lastDir) ? 0 : 1) || cycleNow == (times * 2 - 1)) {
    
    from = (from == 0) ? -vel : vel;

    int f = (sideMotor == 1) ? 0 : vel;
    int t = (sideMotor == 1) ? vel : 0;

    if (from == -vel) {
      a = a - ((cycleNow == ((lastDir == direction) ? 0 : 1)) ? f : t);
    } else if (from == vel) {
      a = a - ((cycleNow == ((lastDir == direction) ? 0 : 1)) ? t : f);
    }

    return (float)a / 2 + (vel / 2);

  } else {

    return a;
  }
}

float MidleMov(int to, int from, int8_t posMotor, int8_t sideMotor, int a, int8_t nCycle) {

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

      return max(((float)(to - (to - rise)) / (vel / 3)) * a + (to - (3 * rise)), ((float)((to - rise) - from) / ((vel / 3) * 2)) * a + from);

    } else if (posMotor == 2) {

      return ((float)(to - (to - rise)) / (vel / 2)) * abs(a - (vel / 2)) + (to - rise);

    } else {

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