/*
 > Hexapod Parker Main Code
 > 
 > This code embodies the core functionality of the Hexapod, my final year project. 
 > It is provided as is, with no ongoing maintenance unless essential for project completion.
 > 
 > You are free to utilize, adapt, and share this code for non-commercial purposes, 
 > provided proper credit is attributed to the original author (MukhitKwo).
 > 
 > Welcome to this hellscape of a project!
 > Proceed with caution, and may the code be in your favor.
 */

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

//==========================================<//>============================================> SplitStringIntoWords
//Separates the command (String) into separate words (array)

String command[7];

void splitStringIntoWords(String str) {

  for (int x = 0; x < 7; x++) {
    command[x] = "";
  }

  int wordCount = 0, startIndex = 0;

  //magic (i have actually no idea how this works)
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

//==========================================<//>============================================> LOOP
//Read the command and send it to the correct funtion

bool showDone = true;

void loop() {

  if (Serial.available()) {
    String commandFull = Serial.readString();
    splitStringIntoWords(commandFull);
    Serial.print("> ");
    Serial.print(commandFull);

    String type = command[0];

    if (type == "walk") {  //line 99
      Walk();
    } else if (type == "rotate") {  //line 238
      Rotate();
    } else if (type == "play") {  //line 347
      Play();
    } else if (type == "move") {  //line 513
      Move();
    } else if (type == "set") {  //line 584
      Set(false, 0);
    } else if (type == "turn") {  //line 612
      Turn();
    } else if (type == "define") {  //line 665
      Define();
    } else if (type == "show") {  //line 682
      Show();
    } else {
      Serial.print("\n! Command error at 'type' input");
    }

    if (!showDone) {
      Serial.println("");
    } else {
      Serial.println(" <Done>");
    }
  }
}

//==========================================<//>============================================> WALK
//Execute the correct walk function depending of the direction

void Walk() {

  int8_t times = command[2].toInt() * 2;

  if (command[1] == "f" || command[1] == "b") {  //front back
    int8_t direction = (command[1] == "f") ? 1 : -1;
    MovWalk(1, direction, times);

  } else if (command[1] == "l" || command[1] == "r") {  //left right
    int8_t direction = (command[1] == "l") ? 1 : -1;
    MovWalk(2, direction, times);

  } else {
    Serial.print("\n! Command error at 'direction' input");
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

int updates = 100;
int8_t rise = 30;

void MovWalk(int typeWalk, int8_t direction, int8_t times) {

  int x, y;

  for (int cycle = 0; cycle < times; cycle++) {

    int8_t cycleBinary = (cycle % 2 == 0) ? 1 : -1;

    if (direction == 1) {

      x = (cycleBinary == 1) ? 0 : -updates;
      y = x + updates;
    } else {

      x = (cycleBinary == 1) ? -updates : 0;
      y = x + updates;
    }

    switch (typeWalk) {
      case 1:
        FrontBack(cycle, cycleBinary, x, y, times, direction);
        break;
      case 2:
        LeftRight(cycle, cycleBinary, x, y, times, direction);
        break;
      case 3:
        RotateRL(cycle, cycleBinary, x, y, times, direction);
        break;
      case 4:
        Wave(cycle, cycleBinary, x, y, times, 1);
        break;
    }
  }
}

//==========================================<//>============================================> FRONT BACK WALK
//Walk forwards or backwards

void FrontBack(int8_t cycle, int8_t cycleBinary, int x, int y, int8_t times, int8_t direction) {
  for (int a = x; a <= y; a++) {

    int aFixed = (a < 0) ? a * -1 : a;  //LRL

    pca1.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[0][0])) * 2.694);
    pca1.setPWM(1, 0, 115 + (180 - MovRise(degWK[1][1], degWK[1][0], 1, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
    pca1.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[2][0])) * 2.694);
    //
    pca2.setPWM(3, 0, 115 + (180 - (((float)(degWK[3][1] - degWK[3][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[3][0])) * 2.694);
    pca2.setPWM(4, 0, 115 + (180 - MovRise(degWK[4][1], degWK[4][0], 2, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
    pca2.setPWM(5, 0, 115 + (180 - (((float)(degWK[5][1] - degWK[5][0]) / (updates / 2)) * abs(MovHalf(aFixed, 1, x, times, cycle) - updates / 2) + degWK[5][0])) * 2.694);
    //
    pca1.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] + degWK[0][1])) / updates) * MovHalf(aFixed, 1, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
    pca1.setPWM(7, 0, 115 + (180 - MovRise(degWK[1][0], degWK[1][1], 3, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
    pca1.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[2][1])) * 2.694);

    int aInverted = updates - aFixed;  //RLR

    pca2.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] + degWK[0][1]) - degWK[0][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[0][0])) * 2.694);
    pca2.setPWM(1, 0, 115 + (180 - MovRise(degWK[1][1], degWK[1][0], 1, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
    pca2.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[2][0])) * 2.694);
    //
    pca1.setPWM(3, 0, 115 + (180 - (((float)(degWK[3][0] - degWK[3][1]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[3][1])) * 2.694);
    pca1.setPWM(4, 0, 115 + (180 - MovRise(degWK[4][1], degWK[4][0], 2, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
    pca1.setPWM(5, 0, 115 + (180 - (((float)(degWK[5][1] - degWK[5][0]) / (updates / 2)) * abs(MovHalf(aInverted, 2, x, times, cycle) - updates / 2) + degWK[5][0])) * 2.694);
    //
    pca2.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] - degWK[0][1])) / updates) * MovHalf(aInverted, 2, x, times, cycle) + (degWK[0][0] - degWK[0][1]))) * 2.694);
    pca2.setPWM(7, 0, 115 + (180 - MovRise(degWK[1][0], degWK[1][1], 3, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
    pca2.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[2][1])) * 2.694);
  }
}

//==========================================<//>============================================> LEFT RIGHT WALK
//Walk to left or right side

void LeftRight(int8_t cycle, int8_t cycleBinary, int x, int y, int8_t times, int8_t direction) {
  for (int a = x; a <= y; a++) {

    int aFixed = (a < 0) ? a * -1 : a;  //LRL

    pca1.setPWM(0, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] - degWK[0][1])) / updates) * MovHalf(aFixed, 1, x, times, cycle) + (degWK[0][0] - degWK[0][1]))) * 2.694);
    pca1.setPWM(1, 0, 115 + (180 - MovRise(degWK[1][1], degWK[1][0], 1, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
    pca1.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[2][0])) * 2.694);
    //
    pca2.setPWM(3, 0, 115 + (90 * 2.694));
    pca2.setPWM(4, 0, 115 + (180 - MovRise(degWK[1][0], degWK[1][1], 3, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);                 //its finishing at 70, need 60
    pca2.setPWM(5, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[2][1])) * 2.694);  //its finishing at 30, need 20
    //
    pca1.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] + degWK[0][1])) / updates) * MovHalf(aFixed, 1, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
    pca1.setPWM(7, 0, 115 + (180 - MovRise(degWK[1][1], degWK[1][0], 3, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
    pca1.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[2][0])) * 2.694);

    int aInverted = updates - aFixed;  //RLR

    pca2.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] + degWK[0][1]) - degWK[0][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[0][0])) * 2.694);
    pca2.setPWM(1, 0, 115 + (180 - MovRise(degWK[1][0], degWK[1][1], 1, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
    pca2.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[2][1])) * 2.694);
    //
    pca1.setPWM(3, 0, 115 + (90 * 2.694));
    pca1.setPWM(4, 0, 115 + (180 - MovRise(degWK[1][1], degWK[1][0], 3, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);                 //its finishing at 70, need 60
    pca1.setPWM(5, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[2][0])) * 2.694);  //its finishing at 30, need 20
    //
    pca2.setPWM(6, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[0][0])) * 2.694);
    pca2.setPWM(7, 0, 115 + (180 - MovRise(degWK[1][0], degWK[1][1], 3, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
    pca2.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][0] - degWK[2][1]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[2][1])) * 2.694);
  }
}

//==========================================<//>============================================> ROTATE
//Rotate to the rigth or left side

void Rotate() {

  int8_t times = command[2].toInt() * 2;

  if (command[1] == "r" || command[1] == "l") {
    int8_t direction = (command[1] == "r") ? 1 : -1;
    MovWalk(3, direction, times);

  } else {
    Serial.print("\n! Command error at 'direction' input");
  }
}

void RotateRL(int8_t cycle, int8_t cycleBinary, int x, int y, int8_t times, int8_t direction) {
  for (int a = x; a <= y; a++) {

    int aFixed = (a < 0) ? a * -1 : a;  //LRL

    pca1.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[0][0])) * 2.694);
    pca1.setPWM(1, 0, 115 + (180 - MovRise((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 1, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
    pca1.setPWM(2, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * MovHalf(aFixed, 1, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
    //
    pca2.setPWM(3, 0, 115 + (180 - (((float)(degWK[3][0] - degWK[3][1]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[3][1])) * 2.694);
    pca2.setPWM(4, 0, 115 + (180 - MovRise((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 2, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
    pca2.setPWM(5, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * MovHalf(aFixed, 1, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
    //
    pca1.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] + degWK[0][1])) / updates) * MovHalf(aFixed, 1, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
    pca1.setPWM(7, 0, 115 + (180 - MovRise((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 3, 1, aFixed, cycleBinary, cycle, times, direction)) * 2.694);
    pca1.setPWM(8, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * MovHalf(aFixed, 1, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);

    int aInverted = updates - aFixed;  //RLR

    pca2.setPWM(0, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
    pca2.setPWM(1, 0, 115 + (180 - MovRise((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 1, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
    pca2.setPWM(2, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * MovHalf(aInverted, 2, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
    //
    pca1.setPWM(3, 0, 115 + (180 - (((float)(degWK[3][0] - degWK[3][1]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[3][1])) * 2.694);
    pca1.setPWM(4, 0, 115 + (180 - MovRise((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 2, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
    pca1.setPWM(5, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * MovHalf(aInverted, 2, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
    //
    pca2.setPWM(6, 0, 115 + (180 - (((float)((degWK[0][0] - degWK[0][1]) - degWK[0][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[0][0])) * 2.694);
    pca2.setPWM(7, 0, 115 + (180 - MovRise((degWK[4][1] + degWK[4][0]) / 2, (degWK[4][1] + degWK[4][0]) / 2, 3, 2, aInverted, cycleBinary, cycle, times, direction)) * 2.694);
    pca2.setPWM(8, 0, 115 + (180 - (((float)((degWK[2][1] + degWK[2][0]) / 2 - (degWK[2][1] + degWK[2][0]) / 2) / updates) * MovHalf(aInverted, 2, x, times, cycle) + ((degWK[2][1] + degWK[2][0]) / 2))) * 2.694);
  }
}

//==========================================<//>============================================> MOV FUNTIONS
//Essencial functions needed to execute Walk and Rotate correctly

float MovRise(int to, int from, int8_t posMotor, int8_t sideMotor, int a, int8_t nCycle, int8_t cycleTotal, int8_t times, int8_t direction) {

  if (cycleTotal == 0 || cycleTotal == (times - 1)) {  //does the same thing as MovHalf()
    int midle = (from + to) / 2;
    (sideMotor == 1) ? ((direction == 1) ? from = midle : to = midle) : ((direction == 1) ? to = midle : from = midle);
  }

  nCycle *= (sideMotor == 2) ? -1 : 1;

  if (nCycle == 1) {  //push the robot

    if (posMotor == 1) {
      return ((float)(to - from) / updates) * a + from;
    } else if (posMotor == 2) {
      return ((float)(to - from) / (updates / 2)) * abs(a - updates / 2) + from;
    } else if (posMotor == 3) {
      return ((float)(to - from) / updates) * a + from;
    }
  }

  else if (nCycle == -1) {  //send leg back

    if (posMotor == 1) {
      return max(((float)(to - (to - rise)) / (updates / 2)) * a + (to - (2 * rise)), ((float)((to - rise) - from) / (updates / 2)) * a + from);
    } else if (posMotor == 2) {
      return ((float)(to - (to - rise)) / (updates / 2)) * abs(a - (updates / 2)) + (to - rise);
    } else if (posMotor == 3) {
      return max(((float)(from - (from - rise)) / (updates / 2)) * (updates - a) + (from - (2 * rise)), ((float)((from - rise) - to) / (updates / 2)) * (updates - a) + to);
    }
  }
}

//Mov Functions -> MovHalf

float MovHalf(int a, int sideMotor, int begin, int times, int cycleTotal) {

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

//==========================================<//>============================================> PLAY
//Play a certain movement

void Play() {

  int8_t times = command[2].toInt() * 2;

  if (command[1] == "step") {  //tecnically stepping, but almost the same thing
    Step(times);
  } else if (command[1] == "jump") {  //not jump, but bouncing the robot
    Jump(times);
  } else if (command[1] == "spin") {  //not spin, but oscillate the robot
    Spin(times);
  } else if (command[1] == "wave") {  //bonce left and right, forgot name
    MovWalk(4, 1, times);
  }
}

//Play -> Step

void Step(int8_t times) {

  for (int z = 0; z < times / 2; z++) {

    int deg1 = 70;
    int deg2 = 40;

    for (int h = 0; h < 6; h++) {

      for (int x = 0; x < updates; x++) {

        float a = ((float)(deg2 - deg1) / updates) * ((1 - pow(1 - (float)x / updates, 3)) * updates) + deg1;

        if (h < 3) {
          pca1.setPWM(h * 3 + 1, 0, 115 + ((180 - a) * 2.694));
        } else {
          pca2.setPWM((5 - h) * 3 + 1, 0, 115 + ((180 - a) * 2.694));
        }
        delay(3);
      }
      for (int x = 0; x < updates; x++) {

        float b = ((float)(deg1 - deg2) / updates) * ((1 - pow(1 - (float)x / updates, 3)) * updates) + deg2;

        if (h < 3) {
          pca1.setPWM(h * 3 + 1, 0, 115 + ((180 - b) * 2.694));
        } else {
          pca2.setPWM((5 - h) * 3 + 1, 0, 115 + ((180 - b) * 2.694));
        }
        delay(3);
      }
    }
  }
}

//Play -> Jump

void Jump(int8_t times) {

  int8_t sense = 1;

  for (int z = 0; z < times; z++) {

    int degC1 = (sense == 1) ? 50 : 90;  //50 90
    int degC2 = (sense == 1) ? 90 : 50;

    int degT1 = (sense == 1) ? 6 : 28;  //6 28
    int degT2 = (sense == 1) ? 28 : 6;

    if (z == 0) {
      degC1 = (degC1 + degC2) / 2;
      degT1 = (degT1 + degT2) / 2;
    } else if (z == (times - 1)) {
      degC2 = (degC1 + degC2) / 2;
      degT2 = (degT1 + degT2) / 2;
    }

    for (int x = 0; x < updates; x++) {

      float c = ((float)(degC2 - degC1) / updates) * ((1 - pow(1 - (float)x / updates, 3)) * updates) + degC1;
      float t = ((float)(degT2 - degT1) / updates) * ((1 - pow(1 - (float)x / updates, 3)) * updates) + degT1;

      for (int h = 0; h < 3; h++) {
        pca1.setPWM(h * 3 + 1, 0, 115 + ((180 - c) * 2.694));
        pca1.setPWM(h * 3 + 2, 0, 115 + ((180 - t) * 2.694));

        pca2.setPWM(h * 3 + 1, 0, 115 + ((180 - c) * 2.694));
        pca2.setPWM(h * 3 + 2, 0, 115 + ((180 - t) * 2.694));
      }
    }
    sense *= -1;
  }
}

//Play -> Spin

void Spin(int8_t times) {

  Set(true, 1);
  delay(1000);

  int8_t sense = 1;

  for (int z = 0; z < times; z++) {

    int deg1 = (sense == 1) ? 30 : 150;
    int deg2 = (sense == 1) ? 150 : 30;

    if (z == 0) {
      deg1 = (deg1 + deg2) / 2;
    } else if (z == (times - 1)) {
      deg2 = (deg1 + deg2) / 2;
    }

    for (int x = 0; x < updates; x++) {

      float b = ((float)(deg2 - deg1) / updates) * ((1 - pow(1 - (float)x / updates, 3)) * updates) + deg1;

      pca1.setPWM(0, 0, 115 + ((180 - b) * 2.694));
      pca1.setPWM(3, 0, 115 + ((180 - b) * 2.694));
      pca1.setPWM(6, 0, 115 + ((180 - b) * 2.694));

      pca2.setPWM(0, 0, 115 + ((180 - b) * 2.694));
      pca2.setPWM(3, 0, 115 + ((180 - b) * 2.694));
      pca2.setPWM(6, 0, 115 + ((180 - b) * 2.694));
      delay(5);
    }
    sense *= -1;
  }
}

//Play -> Wave

void Wave(int8_t cycle, int8_t cycleBinary, int x, int y, int8_t times, int8_t direction) {
  for (int a = x; a <= y; a++) {

    int aFixed = (a < 0) ? a * -1 : a;  //LRL

    pca1.setPWM(0, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] - degWK[0][1])) / updates) * MovHalf(aFixed, 1, x, times, cycle) + (degWK[0][0] - degWK[0][1]))) * 2.694);
    pca1.setPWM(1, 0, 115 + (180 - (((float)(degWK[1][1] - degWK[1][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[1][0])) * 2.694);
    pca1.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[2][0])) * 2.694);
    //
    pca1.setPWM(3, 0, 115 + (90 * 2.694));
    pca1.setPWM(4, 0, 115 + (180 - (((float)(degWK[1][1] - degWK[1][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[1][0])) * 2.694);
    pca1.setPWM(5, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[2][0])) * 2.694);
    //
    pca1.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] + degWK[0][1])) / updates) * MovHalf(aFixed, 1, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
    pca1.setPWM(7, 0, 115 + (180 - (((float)(degWK[1][1] - degWK[1][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[1][0])) * 2.694);
    pca1.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aFixed, 1, x, times, cycle) + degWK[2][0])) * 2.694);

    int aInverted = updates - aFixed;  //RLR

    pca2.setPWM(0, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] + degWK[0][1])) / updates) * MovHalf(aInverted, 2, x, times, cycle) + (degWK[0][0] + degWK[0][1]))) * 2.694);
    pca2.setPWM(1, 0, 115 + (180 - (((float)(degWK[1][1] - degWK[1][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[1][0])) * 2.694);
    pca2.setPWM(2, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[2][0])) * 2.694);
    //
    pca2.setPWM(3, 0, 115 + (90 * 2.694));
    pca2.setPWM(4, 0, 115 + (180 - (((float)(degWK[1][1] - degWK[1][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[1][0])) * 2.694);
    pca2.setPWM(5, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[2][0])) * 2.694);
    //
    pca2.setPWM(6, 0, 115 + (180 - (((float)(degWK[0][0] - (degWK[0][0] - degWK[0][1])) / updates) * MovHalf(aInverted, 2, x, times, cycle) + (degWK[0][0] - degWK[0][1]))) * 2.694);
    pca2.setPWM(7, 0, 115 + (180 - (((float)(degWK[1][1] - degWK[1][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[1][0])) * 2.694);
    pca2.setPWM(8, 0, 115 + (180 - (((float)(degWK[2][1] - degWK[2][0]) / updates) * MovHalf(aInverted, 2, x, times, cycle) + degWK[2][0])) * 2.694);
  }
}

//==========================================<//>============================================> MOVE
//Move one, three, all or custom amount of legs for x degrees

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

  } else if (command[1].toInt() > 0) {  //move one or custom motors
    for (int b = 0; b < command[1].length(); b++) {

      int8_t leg = command[1].charAt(b) - '0' - 1;

      (binary.charAt(0) == '1') ? PWM(1, leg, 0, command[pos++].toInt()) : 0;
      (binary.charAt(1) == '1') ? PWM(1, leg, 1, command[pos++].toInt()) : 0;
      (binary.charAt(2) == '1') ? PWM(1, leg, 2, command[pos++].toInt()) : 0;
      pos = 3;
    }
  }
}

//Move -> PWM

int PWM(int amount, int leg, int mot, int degree) {

  if (mot == 0 && (degree < 30 || degree > 150)) {
    degree = (degree < 30) ? 30 : 150;
  }

  if (amount == 1) {  //one motor
    if (leg < 3) {
      pca1.setPWM(leg * 3 + mot, 0, 115 + (180 - degree) * 2.694);
    } else if (leg < 6) {
      pca2.setPWM((leg - 3) * 3 + mot, 0, 115 + (180 - degree) * 2.694);
    }

  } else if (amount == 3) {  //three motors
    if (leg == 1) {
      pca1.setPWM(0 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 1
      pca2.setPWM(1 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 5
      pca1.setPWM(2 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 3
    } else if (leg == -1) {
      pca2.setPWM(0 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 4
      pca1.setPWM(1 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 2
      pca2.setPWM(2 * 3 + mot, 0, 115 + (180 - degree) * 2.694);  //leg 6
    }

  } else if (amount == 6) {  //all motors
    for (int8_t rep = 0; rep < 3; rep++) {
      pca1.setPWM(rep * 3 + mot, 0, 115 + (180 - degree) * 2.694);
      pca2.setPWM(rep * 3 + mot, 0, 115 + (180 - degree) * 2.694);
    }
  }

  return 1;
}

//==========================================<//>============================================> Set
//Set the position of the legs to some default state

void Set(bool called, int8_t movement) {

  if (!called) {
    movement = command[1].toInt();
  }

  if (movement == 0) {  //set the robot to its zero position (all zero)
    splitStringIntoWords("move all 111 90 0 0");
    Move();
  } else if (movement == 1) {  //set the robot to its normal position (all one)
    splitStringIntoWords("move all 111 90 70 17");
    Move();
  } else if (movement == 2) {  //mix betwen zero postion and walk/rotate position
    Set(true, 3);
    splitStringIntoWords("move all 011 0 0");
    Move();
  } else if (movement == 3) {  //set the robot to its walk/rotate postition
    Set(true, 1);
    pca1.setPWM(0 * 3, 0, 115 + (180 - (((degWK[0][0] - degWK[0][1]) + degWK[0][0]) / 2)) * 2.694);
    pca1.setPWM(2 * 3, 0, 115 + (180 - (((degWK[0][0] + degWK[0][1]) + degWK[0][0]) / 2)) * 2.694);
    pca2.setPWM(0 * 3, 0, 115 + (180 - (((degWK[0][0] + degWK[0][1]) + degWK[0][0]) / 2)) * 2.694);
    pca2.setPWM(2 * 3, 0, 115 + (180 - (((degWK[0][0] - degWK[0][1]) + degWK[0][0]) / 2)) * 2.694);
  }
}

//==========================================<//>============================================> Turn
//Turn one, all or custon amount of motors on or off

void Turn() {
  if (command[1] == "off") {

    if (command[2] != "") {  //set position zero before turning OFF
      Set(true, command[2].toInt());
      delay(600);
    }

    for (int8_t disable = 0; disable < 9; disable++) {  //just turns OFF all motors
      pca1.setPWM(disable, 1000, 115 + (90 * 2.694));
      pca2.setPWM(disable, 1000, 115 + (90 * 2.694));
    }

  } else if (command[1] == "all") {  //turn x motors of all legs ON or OFF
    ((command[2].charAt(0) == '1') ? OnOff(6, -1, 0, ((command[3] == "off") ? false : true)) : 0);
    ((command[2].charAt(1) == '1') ? OnOff(6, -1, 1, ((command[3] == "off") ? false : true)) : 0);
    ((command[2].charAt(2) == '1') ? OnOff(6, -1, 2, ((command[3] == "off") ? false : true)) : 0);

  } else if (command[1].toInt() > 0) {  // turn x motor of x legs ON or OFF

    for (int b = 0; b < command[1].length(); b++) {

      int8_t leg = command[1].charAt(b) - '0' - 1;

      ((command[2].charAt(0) == '1') ? OnOff(1, leg, 0, ((command[3] == "off") ? false : true)) : 0);
      ((command[2].charAt(1) == '1') ? OnOff(1, leg, 1, ((command[3] == "off") ? false : true)) : 0);
      ((command[2].charAt(2) == '1') ? OnOff(1, leg, 2, ((command[3] == "off") ? false : true)) : 0);
    }
  }
}

//Turn -> OnOff

int8_t OnOff(int8_t amount, int8_t leg, int8_t motor, bool activate) {

  if (amount == 1) {  //one motor
    if (leg < 3) {
      pca1.setPWM(leg * 3 + motor, ((!activate) ? 1000 : 0), ((motor == 0) ? 357 : 600));
    } else if (leg < 6) {
      pca2.setPWM((leg - 3) * 3 + motor, ((!activate) ? 1000 : 0), ((motor == 0) ? 357 : 600));
    }

  } else if (amount == 6) {  //all motors
    for (int x = 0; x < 3; x++) {
      pca1.setPWM(x * 3 + motor, ((!activate) ? 1000 : 0), ((motor == 0) ? 357 : 600));
      pca2.setPWM(x * 3 + motor, ((!activate) ? 1000 : 0), ((motor == 0) ? 357 : 600));
    }
  }
  return 1;
}

//==========================================<//>============================================> DEFINE
//Set some global value to a new one

void Define() {

  String variable = command[1];

  if (variable == "upc") {  //change 'updates' value (updates per cycle)
    updates = command[2].toInt();
  } else if (variable == "rise") {  //change 'rise' value
    rise = command[2].toInt();
  } else {
    Serial.print("\n! Command error at 'variable' input");
  }
}

//==========================================<//>============================================> Show
//Show the values of the global values or show if a command was executed

void Show() {
  if (command[1] == "info") {
    Serial.println("\n< Updates: " + String(updates));
    Serial.println("< Rise: " + String(rise));
    Serial.print("< degWK[6][2]: ");

    for (int g = 0; g < 6; g++) {
      Serial.println("");
      for (int h = 0; h < 2; h++) {
        Serial.print("[" + String(degWK[g][h]) + "]");
      }
    }

  } else if (command[1] == "done") {
    showDone = (command[2] == "true") ? true : false;
  }
}

//Welcome to the 700th line :D, also 'End of Code'