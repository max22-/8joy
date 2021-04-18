#include <Arduino.h>
#include <unordered_map>

#define JOY_X A0
#define JOY_Y A3
#define ANALOG_AMPLITUDE 4096
#define THRESHOLD 0.5
#define QS 70 // quadrant size, in degrees

std::unordered_map<std::string, char> codes = {
  {"01", 'a'},
  {"3012", 'b'},
  {"2103", 'c'},
};

char get_quadrant(double r, double theta)
{
  if(r < THRESHOLD)
    return 'c';   // center
  if(theta > 360 - QS/2 || theta < QS / 2)
    return '0';
  if(theta > 90 - QS/2 && theta < 90 + QS / 2)
    return '1';
  if(theta > 180 - QS/2 && theta < 180 + QS / 2)
    return '2';
  if(theta > 270 - QS/2 && theta < 270 + QS / 2)
    return '3';
  return 'b';   // in-between
}

void handle_quadrant(char q)
{
  static std::string code = "";
  switch(q) {
    case '0':
    case '1':
    case '2':
    case '3':
      code = code + q;
      break;
    case 'c':
      if(codes.count(code) == 1)
        Serial.printf("\n%c\n", codes[code]);
      code.clear();
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
}

void loop() {
  static char quadrant = 'c';

  double x = (double)analogRead(JOY_X) / ((ANALOG_AMPLITUDE - 1) / 2) - 1;
  double y = (double)analogRead(JOY_Y) / ((ANALOG_AMPLITUDE - 1) / 2) - 1;
  double r = sqrt(pow(x, 2) + pow(y, 2));
  double theta = degrees(atan2(y, x));
  if(theta < 0)
    theta += 360;
  //printf("%f\t%f\tr=%f\ttheta=%f°\n", x, y, r, theta);

  char new_quadrant = get_quadrant(r, theta);
  if(new_quadrant != quadrant && new_quadrant != 'b') {
    quadrant = new_quadrant;
    Serial.printf("%c ", quadrant);
    handle_quadrant(quadrant);
  }

  delay(10);
}