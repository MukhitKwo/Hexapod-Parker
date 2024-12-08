#include Wire.h
#include Adafruit_PWMServoDriver.h

Adafruit_PWMServoDriver pca1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pca2 = Adafruit_PWMServoDriver(0x41);

Min pulse = 115
Max pulse = 600

int deg1 = 90;
 int deg2 = 180;
 int deg3 = 180;
int deg2 = 100;
int deg3 = 150;

void setup() {
  Serial.begin(9600);

  pca1.begin();
  pca1.setPWMFreq(60);
  pca2.begin();
  pca2.setPWMFreq(60);
}

void loop() {

  if (Serial.available()  0) {
    int a = Serial.parseInt();
    Serial.println(a);

    if (a == 1) {
      pca1.setPWM(0, 0, round(115 + deg1  2.694));
      pca1.setPWM(3, 0, round(115 + deg1  2.694));
      pca1.setPWM(6, 0, round(115 + deg1  2.694));

      pca2.setPWM(0, 0, round(115 + deg1  2.694));
      pca2.setPWM(3, 0, round(115 + deg1  2.694));
      pca2.setPWM(6, 0, round(115 + deg1  2.694));
    }

    if (a == 2) {
      pca1.setPWM(1, 0, round(115 + deg2  2.694));
      pca1.setPWM(4, 0, round(115 + deg2  2.694));
      pca1.setPWM(7, 0, round(115 + deg2  2.694));

      pca2.setPWM(1, 0, round(115 + deg2  2.694));
      pca2.setPWM(4, 0, round(115 + deg2  2.694));
      pca2.setPWM(7, 0, round(115 + deg2  2.694));
    }

    if (a == 3) {
      pca1.setPWM(2, 0, round(115 + deg3  2.694));
      pca1.setPWM(5, 0, round(115 + deg3  2.694));
      pca1.setPWM(8, 0, round(115 + deg3  2.694));

      pca2.setPWM(2, 0, round(115 + deg3  2.694));
      pca2.setPWM(5, 0, round(115 + deg3  2.694));
      pca2.setPWM(8, 0, round(115 + deg3  2.694));
    }

    if (a == 4) {

      pca1.setPWM(1, 0, round(115 + 150  2.694));
       pca1.setPWM(5, 0, round(115 + deg3  2.694));
      pca1.setPWM(7, 0, round(115 + 150  2.694));

       pca2.setPWM(2, 0, round(115 + deg3  2.694));
      pca2.setPWM(4, 0, round(115 + 150  2.694));
       pca2.setPWM(8, 0, round(115 + deg3  2.694));

      delay(3000);

      pca1.setPWM(1, 0, round(115 + 100  2.694));
       pca1.setPWM(5, 0, round(115 + deg3  2.694));
      pca1.setPWM(7, 0, round(115 + 100  2.694));

       pca2.setPWM(2, 0, round(115 + deg3  2.694));
      pca2.setPWM(4, 0, round(115 + 100  2.694));
       pca2.setPWM(8, 0, round(115 + deg3  2.694));
    }

    if (a == 5) {

       pca1.setPWM(2, 0, round(115 + 150  2.694));
      pca1.setPWM(4, 0, round(115 + 150  2.694));
       pca1.setPWM(8, 0, round(115 + 150  2.694));

      pca2.setPWM(1, 0, round(115 + 150  2.694));
       pca2.setPWM(5, 0, round(115 + 150  2.694));
      pca2.setPWM(7, 0, round(115 + 150  2.694));

      delay(3000);

       pca1.setPWM(2, 0, round(115 + 100  2.694));
      pca1.setPWM(4, 0, round(115 + 100  2.694));
       pca1.setPWM(8, 0, round(115 + 100  2.694));

      pca2.setPWM(1, 0, round(115 + 100  2.694));
       pca2.setPWM(5, 0, round(115 + 100  2.694));
      pca2.setPWM(7, 0, round(115 + 100  2.694));
    }

    if (a == 6) {

      float x1 = 180.0;
      float x2 = 180.0;

      do {
        pca1.setPWM(1, 0, round(115 + x1  2.694));
        pca1.setPWM(4, 0, round(115 + x1  2.694));
        pca1.setPWM(7, 0, round(115 + x1  2.694));

        pca2.setPWM(1, 0, round(115 + x1  2.694));
        pca2.setPWM(4, 0, round(115 + x1  2.694));
        pca2.setPWM(7, 0, round(115 + x1  2.694));


        pca1.setPWM(2, 0, round(115 + x2  2.694));
        pca1.setPWM(5, 0, round(115 + x2  2.694));
        pca1.setPWM(8, 0, round(115 + x2  2.694));

        pca2.setPWM(2, 0, round(115 + x2  2.694));
        pca2.setPWM(5, 0, round(115 + x2  2.694));
        pca2.setPWM(8, 0, round(115 + x2  2.694));

        if (x1  80) {
          x1 -= 1.2;
          x2 -= 0.6;
          delay(3);
        }

      } while (x1 = 100);
    }

    if (a == 7) {

      int k = 1;

      if (k  4) {
        pca1.setPWM(((k - 1)  3 + 0), 0, round(115 + 180  2.694));

         pca1.setPWM(((k - 1)  3 + 1), 0, round(115 + 180  2.694));
         pca1.setPWM(((k - 1)  3 + 2), 0, round(115 + 180  2.694));

         delay(5000);

         pca1.setPWM(((k - 1)  3 + 1), 0, round(115 + deg2  2.694));
         pca1.setPWM(((k - 1)  3 + 2), 0, round(115 + deg3  2.694));
      } else {
        pca2.setPWM(((k - 1)  3 + 0), 0, round(115 + 180  2.694));

         pca2.setPWM(((k - 4)  3 + 1), 0, round(115 + 180  2.694));
         pca2.setPWM(((k - 4)  3 + 2), 0, round(115 + 180  2.694));

         delay(5000);

         pca2.setPWM(((k - 4)  3 + 1), 0, round(115 + deg2  2.694));
         pca2.setPWM(((k - 4)  3 + 2), 0, round(115 + deg3  2.694));
      }
    }
  }
}
