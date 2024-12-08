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

//=======================================//================================================> READ COMMANDS

String command[7];  //le arrey qui stocke la commande

void splitStringIntoWords(String str) {  //separates the command into words

  //clear array words
  for (int x = 0; x < 7; x++) {
    command[x] = "";
  }

  int wordCount = 0, startIndex = 0;

  //magic (i actually have no idea how this works)
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

void loop() {  //loop here !!!

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
    } else if (type == "rotate") {
      Rotate();
    } else if (type == "define") {
      Define();
    } else if (type == "set") {
      Set(false, 0);
    } else if (type == "turn") {
      Turn();
    } else if (type == "play") {
      //Play();
    } else {
      Serial.println("! Command error at 'type' input");
    }
  }
}

//=======================================//================================================> WALK

void Walk() {

  int8_t times = command[2].toInt() * 2;

  if (command[1] == "f" || command[1] == "b") {  //front back
    int8_t direction = (command[1] == "f") ? 1 : -1;

    FrontBack(direction, times);

  } else {  //left right
    int8_t direction = (command[1] == "l") ? 1 : -1;

    LeftRight(direction, times);
  }
}

int degWK[6][2] = {
  { 90, 50 },   //front and back > base (-/+)
  { 80, 60 },   //front and back > coxa (->)
  { 35, 0 },    //front and back > tibia (->)
  { 65, 115 },  //middle > base (->)
  { 60, 70 },   //middle > coxa (->)
  { 10, 20 }    //middle > tibia (->)
};

int updates = 120;
int8_t rise = 30;

//=======================================//================================================> FRONT BACK

void FrontBack(int8_t direction, int8_t times) {

  int x, y;

  for (int cycle = 0; cycle < times; cycle++) {

    int8_t cycleBinary = (cycle % 2 == 0) ? 1 : -1;

    if (direction == 1) {
      //front
      x = (cycleBinary == 1) ? 0 : -updates;
      y = x + updates;
    } else {
      //back
      x = (cycleBinary == 1) ? -updates : 0;
      y = x + updates;
    }

    for (int a = x; a <= y; a++) {

      int aFixed = (a <= 0) ? a * -1 : a;  //LRL

      pca1.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + degWK[0][0])) * 2.694);
      pca1.setPWM(1, 0, 115 + (180 - MidleMov(degWK[1][1], degWK[1][0], 1, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
      pca1.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + degWK[2][0])) * 2.694);
      //
      pca2.setPWM(3, 0, 115 + (180 - (((float)(degWK[3][1] - degWK[3][0]) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + degWK[3][0])) * 2.694);
      pca2.setPWM(4, 0, 115 + (180 - MidleMov(degWK[4][1], degWK[4][0], 2, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
      pca2.setPWM(5, 0, 115 + (180 - (((float)(degWK[5][1] - degWK[5][0]) / (updates / 2)) * abs(SmoothFixed(aFixed, 1, x, times, cycle) - updates / 2) + degWK[5][0])) * 2.694);
      //
      pca1.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] + degWK[0][1])) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
      pca1.setPWM(7, 0, 115 + (180 - MidleMov(degWK[1][0], degWK[1][1], 3, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
      pca1.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + degWK[2][1])) * 2.694);

      int aInverted = updates - aFixed;  //RLR

      pca2.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] + degWK[0][1]) - degWK[0][0]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[0][0])) * 2.694);
      pca2.setPWM(1, 0, 115 + (180 - MidleMov(degWK[1][1], degWK[1][0], 1, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
      pca2.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[2][0])) * 2.694);
      //
      pca1.setPWM(3, 0, 115 + (180 - (((float)(degWK[3][0] - degWK[3][1]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[3][1])) * 2.694);
      pca1.setPWM(4, 0, 115 + (180 - MidleMov(degWK[4][1], degWK[4][0], 2, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
      pca1.setPWM(5, 0, 115 + (180 - (((float)(degWK[5][1] - degWK[5][0]) / (updates / 2)) * abs(SmoothFixed(aInverted, 2, x, times, cycle) - updates / 2) + degWK[5][0])) * 2.694);
      //
      pca2.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] - degWK[0][1])) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + (degWK[0][0] - degWK[0][1]))) * 2.694);
      pca2.setPWM(7, 0, 115 + (180 - MidleMov(degWK[1][0], degWK[1][1], 3, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
      pca2.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[2][1])) * 2.694);
    }

    // if (Serial.available() && Serial.readString() == "stop") {
    //   times = (cycle % 2 == 0) ? cycle + 2 : cycle + 3;
    //   Serial.println("> stop");
    // }
  }
}

//=======================================//================================================> LEFT RIGHT

void LeftRight(int8_t direction, int8_t times) {

  int x, y;

  for (int cycle = 0; cycle < times; cycle++) {

    int8_t cycleBinary = (cycle % 2 == 0) ? 1 : -1;

    if (direction == 1) {
      //left
      x = (cycleBinary == 1) ? 0 : -updates;
      y = x + updates;
    } else {
      //right
      x = (cycleBinary == 1) ? -updates : 0;
      y = x + updates;
    }

    for (int a = x; a <= y; a++) {

      int aFixed = (a <= 0) ? a * -1 : a;  //LRL

      pca1.setPWM(0, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] - degWK[0][1])) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + (degWK[0][0] - degWK[0][1]))) * 2.694);
      pca1.setPWM(1, 0, 115 + (180 - MidleMov(degWK[1][1], degWK[1][0], 1, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
      pca1.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + degWK[2][0])) * 2.694);
      //
      pca2.setPWM(3, 0, 115 + (90 * 2.694));
      pca2.setPWM(4, 0, 115 + (180 - MidleMov(degWK[1][0], degWK[1][1], 3, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);                    //its finishing at 70, need 60
      pca2.setPWM(5, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + degWK[2][1])) * 2.694);  //its finishing at 30, need 20
      //
      pca1.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] + degWK[0][1])) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
      pca1.setPWM(7, 0, 115 + (180 - MidleMov(degWK[1][1], degWK[1][0], 3, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
      pca1.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + degWK[2][0])) * 2.694);

      int aInverted = updates - aFixed;  //RLR

      pca2.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] + degWK[0][1]) - degWK[0][0]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[0][0])) * 2.694);
      pca2.setPWM(1, 0, 115 + (180 - MidleMov(degWK[1][0], degWK[1][1], 1, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
      pca2.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[2][1])) * 2.694);
      //
      pca1.setPWM(3, 0, 115 + (90 * 2.694));
      pca1.setPWM(4, 0, 115 + (180 - MidleMov(degWK[1][1], degWK[1][0], 3, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);                    //its finishing at 70, need 60
      pca1.setPWM(5, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[2][0])) * 2.694);  //its finishing at 30, need 20
      //
      pca2.setPWM(6, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[0][0])) * 2.694);
      pca2.setPWM(7, 0, 115 + (180 - MidleMov(degWK[1][0], degWK[1][1], 3, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
      pca2.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[2][1])) * 2.694);
    }
  }
}

//=======================================//================================================> WALK FUNTIONS

float MidleMov(int to, int from, int8_t posMotor, int8_t sideMotor, int a, int8_t nCycle, int8_t cycleTotal, int8_t times, int8_t direction) {

  if (cycleTotal == 0 || cycleTotal == (times - 1)) {  //does the same thing as SmoothFixed()
    int midle = (from + to) / 2;
    (sideMotor == 1) ? ((direction == 1) ? from = midle : to = midle) : ((direction == 1) ? to = midle : from = midle);
  }

  nCycle *= (sideMotor == 2) ? -1 : 1;

  if (nCycle == 1) {  //walk normal (line)

    if (posMotor == 1) {
      return ((float)(to - from) / updates) * a + from;
    } else if (posMotor == 2) {
      return ((float)(to - from) / (updates / 2)) * abs(a - updates / 2) + from;
    } else if (posMotor == 3) {
      return ((float)(to - from) / updates) * a + from;
    }
  }

  else if (nCycle == -1) {  //walk elevate (triangle)

    if (posMotor == 1) {
      // return max(((float)(to - (to - rise)) / (updates / 3)) * a + (to - (3 * rise)), ((float)((to - rise) - from) / ((updates / 3) * 2)) * a + from);
      return max(((float)(to - (to - rise)) / (updates / 2)) * a + (to - (2 * rise)), ((float)((to - rise) - from) / (updates / 2)) * a + from);
    } else if (posMotor == 2) {
      return ((float)(to - (to - rise)) / (updates / 2)) * abs(a - (updates / 2)) + (to - rise);
    } else if (posMotor == 3) {
      // return max(((float)(from - (from - rise)) / (updates / 3)) * (updates - a) + (from - (3 * rise)), ((float)((from - rise) - to) / ((updates / 3) * 2)) * (updates - a) + to);
      return max(((float)(from - (from - rise)) / (updates / 2)) * (updates - a) + (from - (2 * rise)), ((float)((from - rise) - to) / (updates / 2)) * (updates - a) + to);
    }
  }
}

float SmoothFixed(int a, int sideMotor, int begin, int times, int cycleTotal) {

  if (cycleTotal == 0 || cycleTotal == (times - 1)) {  //half cycle

    begin = (begin == 0) ? -updates : updates;

    int d1 = (sideMotor == 1) ? 0 : updates;
    int d2 = (sideMotor == 1) ? updates : 0;

    if (begin == -updates) {
      a = a - ((cycleTotal == 0) ? d1 : d2);
    } else if (begin == updates) {
      a = a - ((cycleTotal == 0) ? d2 : d1);
    }

    return (float)a / 2 + (updates / 2);

  } else {  //full cycle

    return a;
  }
}

//=======================================//================================================> ROTATE

void Rotate() {

  int8_t direction = (command[1] == "r") ? 1 : -1;
  int8_t times = command[2].toInt() * 2;

  int x, y;

  for (int cycle = 0; cycle < times; cycle++) {

    int8_t cycleBinary = (cycle % 2 == 0) ? 1 : -1;

    if (direction == 1) {
      //front
      x = (cycleBinary == 1) ? 0 : -updates;
      y = x + updates;
    } else {
      //back
      x = (cycleBinary == 1) ? -updates : 0;
      y = x + updates;
    }

    for (int a = x; a <= y; a++) {

      int aFixed = (a <= 0) ? a * -1 : a;  //LRL

      pca1.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + degWK[0][0])) * 2.694);
      pca1.setPWM(1, 0, 115 + (180 - MidleMov((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 1, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
      pca1.setPWM(2, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
      //
      pca2.setPWM(3, 0, 115 + (180 - (((float)(degWK[3][0] - degWK[3][1]) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + degWK[3][1])) * 2.694);
      pca2.setPWM(4, 0, 115 + (180 - MidleMov((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 2, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
      pca2.setPWM(5, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
      //
      pca1.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] + degWK[0][1])) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
      pca1.setPWM(7, 0, 115 + (180 - MidleMov((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 3, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
      pca1.setPWM(8, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * SmoothFixed(aFixed, 1, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);

      int aInverted = updates - aFixed;  //RLR

      pca2.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
      pca2.setPWM(1, 0, 115 + (180 - MidleMov((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 1, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
      pca2.setPWM(2, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
      //
      pca1.setPWM(3, 0, 115 + (180 - (((float)(degWK[3][0] - degWK[3][1]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[3][1])) * 2.694);
      pca1.setPWM(4, 0, 115 + (180 - MidleMov((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 2, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
      pca1.setPWM(5, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
      //
      pca2.setPWM(6, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + degWK[0][0])) * 2.694);
      pca2.setPWM(7, 0, 115 + (180 - MidleMov((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 3, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
      pca2.setPWM(8, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * SmoothFixed(aInverted, 2, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
    }
  }
}

//=======================================//================================================> MOVE

void Move() {

  String binary = command[2];
  int pos = 3;

  if (command[1] == "all") {  //move all motors

    (binary.charAt(0) == '1') ? PWM(6, -1, 0, command[pos++].toInt()) : 0;
    (binary.charAt(1) == '1') ? PWM(6, -1, 1, command[pos++].toInt()) : 0;
    (binary.charAt(2) == '1') ? PWM(6, -1, 2, command[pos++].toInt()) : 0;
  } else if (command[1] == "tri") {  //move three motors

    int side = (command[2] == "lrl") ? 1 : -1;
    binary = command[3];
    pos = 4;

    (binary.charAt(0) == '1') ? PWM(3, side, 0, command[pos++].toInt()) : 0;
    (binary.charAt(1) == '1') ? PWM(3, side, 1, command[pos++].toInt()) : 0;
    (binary.charAt(2) == '1') ? PWM(3, side, 2, command[pos++].toInt()) : 0;
  } else if (command[1].toInt() < 7) {  //move only one motor

    int leg = command[1].toInt() - 1;

    (binary.charAt(0) == '1') ? PWM(1, leg, 0, command[pos++].toInt()) : 0;
    (binary.charAt(1) == '1') ? PWM(1, leg, 1, command[pos++].toInt()) : 0;
    (binary.charAt(2) == '1') ? PWM(1, leg, 2, command[pos++].toInt()) : 0;
  }
}

int PWM(int amount, int leg, int mot, int degree) {

  if (amount == 1) {

    if (leg < 3) {
      pca1.setPWM(leg * 3 + mot, 0, 115 + (180 - degree) * 2.694);
    } else if (leg < 6) {
      pca2.setPWM((leg - 3) * 3 + mot, 0, 115 + (180 - degree) * 2.694);
    }

  } else if (amount == 3) {

    if (leg == 1) {
      pca1.setPWM(0 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 1
      pca2.setPWM(1 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 5
      pca1.setPWM(2 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 3
    } else if (leg == -1) {
      pca2.setPWM(0 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 4
      pca1.setPWM(1 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 2
      pca2.setPWM(2 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 6
    }

  } else if (amount == 6) {
    for (int8_t rep = 0; rep < 3; rep++) {
      pca1.setPWM(rep * 3 + mot, 0, 115 + (180 - degree) * 2.694);
      pca2.setPWM(rep * 3 + mot, 0, 115 + (180 - degree) * 2.694);
    }
  }

  return 1;
}

//=======================================//================================================> DEFINE

void Define() {

  String variable = command[1];

  if (variable == "upd") {
    updates = command[2].toInt();
  } else if (variable == "rise") {
    rise = command[2].toInt();
  } else {
    Serial.println("! Command error at 'variable' input");
  }
}

//=======================================//================================================> Set

void Set(bool called, int8_t movement) {

  if (!called) {
    movement = command[1].toInt();
  }

  if (movement == 0) {
    splitStringIntoWords("move all 111 90 0 0");
    Move();
  } else if (movement == 1) {
    splitStringIntoWords("move all 111 90 70 17");
    Move();
  }
  // else if (movement == "big") {
  //   splitStringIntoWords("move all 111 90 125 145");
  //   Move();
  // } else if (movement == "tall") {
  //   splitStringIntoWords("move all 111 90 180 105");
  //   Move();
  // }
}

//=======================================//================================================> Turn

void Turn() {
  if (command[1] == "off") {

    if (command[2] == "0") {
      Set(true, 0);
      delay(600);
    }

    for (int8_t disable = 0; disable < 9; disable++) {
      pca1.setPWM(disable, 1000, 115 + (90 * 2.694));
      pca2.setPWM(disable, 1000, 115 + (90 * 2.694));
    }

  } else if (command[1] == "all") {
    ((command[2].charAt(0) == '1') ? OffOn(6, -1, 0, ((command[3] == "off") ? false : true)) : 0);
    ((command[2].charAt(1) == '1') ? OffOn(6, -1, 1, ((command[3] == "off") ? false : true)) : 0);
    ((command[2].charAt(2) == '1') ? OffOn(6, -1, 2, ((command[3] == "off") ? false : true)) : 0);
  } else if (command[1].toInt() < 7) {
    int8_t leg = command[1].toInt();
    ((command[2].charAt(0) == '1') ? OffOn(1, leg, 0, ((command[3] == "off") ? false : true)) : 0);
    ((command[2].charAt(1) == '1') ? OffOn(1, leg, 1, ((command[3] == "off") ? false : true)) : 0);
    ((command[2].charAt(2) == '1') ? OffOn(1, leg, 2, ((command[3] == "off") ? false : true)) : 0);
  }
}

int8_t OffOn(int8_t amount, int8_t leg, int8_t motor, bool activate) {

  if (amount == 6) {
    for (int x = 0; x < 3; x++) {
      pca1.setPWM(x * 3 + motor, ((!activate) ? 1000 : 0), ((motor == 0) ? 357 : 600));
      pca2.setPWM(x * 3 + motor, ((!activate) ? 1000 : 0), ((motor == 0) ? 357 : 600));
    }
  } else if (amount == 1) {
    if (leg <= 3) {
      pca1.setPWM((leg - 1) * 3 + motor, ((!activate) ? 1000 : 0), ((motor == 0) ? 357 : 600));
    } else if (leg <= 6) {
      pca2.setPWM((leg - 1) * 3 + motor, ((!activate) ? 1000 : 0), ((motor == 0) ? 357 : 600));
    }
  }
  return 1;
}