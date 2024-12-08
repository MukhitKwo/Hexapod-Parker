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
      //Rotate();
    } else if (type == "define") {
      Define();
    } else if (type == "set") {
      Set(false, 0);
    } else if (type == "turn") {
      Turn();
    } else if (type == "play") {
      //Play();
    } else {
      Serial.println("! Command error at 'type' input: " + String(command[0]));
    }
  }
}

//=======================================//================================================> WALK FRONT BACK

void Walk() {
  int8_t direction = (command[1] == "f") ? 1 : -1;
  int8_t times = command[2].toInt();

  FrontBack(direction, times);
}

int degFB[6][2] = {
  { 90, 50 },   //front and back > base (-/+)
  { 80, 60 },   //front and back > coxa (->)
  { 50, 10 },   //front and back > tibia (->)
  { 65, 115 },  //middle > base (->)
  { 60, 70 },   //middle > coxa (->)
  { 20, 30 }    //middle > tibia (->)
};

int vel = 180;
int8_t rise = 30;
int lastDir = 0;

void FrontBack(int8_t direction, int8_t times) {

  int x, y;

  if (lastDir != direction) {
    times += 1;
  }

  for (int cycle = ((direction == lastDir) ? 0 : 1); cycle < times * 2; cycle++) {

    int8_t cycleBinary = (cycle % 2 == 0) ? 1 : -1;

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

  lastDir = direction;  // -I'm just here, existing...
}

float SmoothFixed(int a, int sideMotor, int from, int times, int cycleNow, int direction) {

  if (cycleNow == ((direction == lastDir) ? 0 : 1) || cycleNow == (times * 2 - 1)) {  //slow (x0.5)

    from = (from == 0) ? -vel : vel;

    int d1 = (sideMotor == 1) ? 0 : vel;
    int d2 = (sideMotor == 1) ? vel : 0;

    if (from == -vel) {
      a = a - ((cycleNow == ((lastDir == direction) ? 0 : 1)) ? d1 : d2);
    } else if (from == vel) {
      a = a - ((cycleNow == ((lastDir == direction) ? 0 : 1)) ? d2 : d1);
    }

    return (float)a / 2 + (vel / 2);

  } else {  //fast (x1)

    return a;
  }
}

float MidleMov(int to, int from, int8_t posMotor, int8_t sideMotor, int a, int8_t nCycle) {

  if (sideMotor == 2) {
    nCycle = nCycle * -1;
  }

  if (nCycle == 1) {  //touching floor

    if (posMotor == 1) {
      return ((float)(to - from) / vel) * a + from;
    } else if (posMotor == 2) {
      return ((float)(to - from) / (vel / 2)) * abs(a - vel / 2) + from;
    } else {
      return ((float)(to - from) / vel) * a + from;
    }

  } else if (nCycle == -1) {  //touching air

    if (posMotor == 1) {
      return max(((float)(to - (to - rise)) / (vel / 3)) * a + (to - (3 * rise)), ((float)((to - rise) - from) / ((vel / 3) * 2)) * a + from);
    } else if (posMotor == 2) {
      return ((float)(to - (to - rise)) / (vel / 2)) * abs(a - (vel / 2)) + (to - rise);
    } else {
      return max(((float)(from - (from - rise)) / (vel / 3)) * (vel - a) + (from - (3 * rise)), ((float)((from - rise) - to) / ((vel / 3) * 2)) * (vel - a) + to);
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

    if (leg <= 3) {
      pca1.setPWM(leg * 3 + mot, 0, 115 + (180 - degree) * 2.694);
    } else if (leg <= 6) {
      pca2.setPWM(leg * 3 + mot, 0, 115 + (180 - degree) * 2.694);
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

  if (variable == "vel") {
    Serial.println(vel);
    vel = command[2].toInt();
    Serial.println(vel);
  } else if (variable == "rise") {
    rise = command[2].toInt();
  } else if (variable == "degfb") {
    //
  } else if (variable == "deglr") {
    //
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
    splitStringIntoWords("move all 111 90 60 30");
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
