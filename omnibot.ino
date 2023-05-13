#include <CarPWMMotorControl.hpp>

#define PWM_MIN 0
#define PWMRANGE 255

PWMDcMotor lfMotor;
PWMDcMotor rfMotor;
PWMDcMotor lbMotor;
PWMDcMotor rbMotor;

struct twist {
  float x_speed;
  float y_speed;
  float r_speed;
};

twist current_twist = { 0, 0, 0 };

void setup() {
  // forward pin, back pin, pwm pin
  lfMotor.init(5, 6, 7);
  rfMotor.init(13, 11, 12);
  lbMotor.init(3, 4, 2);
  rbMotor.init(9, 10, 8);

  Serial3.begin(9600);
  Serial.begin(9600);


}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // Read the incoming line

    float values[3];
    int count = 0;
    int index = 0;

    while (count < 3 && index < input.length()) {
      if (input[index] == ',') {
        values[count] = input.substring(0, index).toFloat();
        input = input.substring(index + 1);
        count++;
        index = 0;
      } else {
        index++;
      }
    }

    if (count == 2 && index <= input.length()) {
      values[count] = input.substring(0, index).toFloat();
      count++;
    }

    if (count == 3) {

      // // Do something with the parsed values
      // // For example, print them to the serial monitor
      for (int i = 0; i < 3; i++) {
        Serial.print("Value ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(values[i]);
      }
      current_twist.x_speed = values[0];
      current_twist.y_speed = values[1];
      current_twist.r_speed = values[2];
    } else {
      // Invalid input, print an error message
      Serial.println("Invalid input! Expecting: <x_speed>,<y_speed>,<r_speed>");
    }
  }
  handleMovement();
  //   lfMotor.setSpeedPWMAndDirection(80, true);
  // rfMotor.setSpeedPWMAndDirection(80, true);
  // lbMotor.setSpeedPWMAndDirection(80, true);
  // rbMotor.setSpeedPWMAndDirection(80, true);
}



void handleMovement() {
  // if (!movement || updating)
  //   return;

  // Mecanum drive:
  // ------------------------

  // Taken and simplified from: http://robotsforroboticists.com/drive-kinematics/
  float lf = current_twist.x_speed - current_twist.y_speed - current_twist.r_speed * 0.5;
  float rf = current_twist.x_speed + current_twist.y_speed + current_twist.r_speed * 0.5;
  float lb = current_twist.x_speed + current_twist.y_speed - current_twist.r_speed * 0.5;
  float rb = current_twist.x_speed - current_twist.y_speed + current_twist.r_speed * 0.5;

  // Map values to PWM intensities.
  // PWMRANGE = full speed, PWM_MIN = the minimal amount of power at which the motors begin moving.
  uint16_t lfPwm = mapPwm(fabs(lf), PWM_MIN, PWMRANGE);
  uint16_t rfPwm = mapPwm(fabs(rf), PWM_MIN, PWMRANGE);
  uint16_t lbPwm = mapPwm(fabs(lb), PWM_MIN, PWMRANGE);
  uint16_t rbPwm = mapPwm(fabs(rb), PWM_MIN, PWMRANGE);

// Serial.println("-------");
// Serial.println(lfPwm);
// Serial.println(rfPwm);
// Serial.println(lbPwm);
// Serial.println(rbPwm);
  //  Serial.printf("%f, %f, %f, %f\n", lf, rf, lb, rb);
  //  Serial.printf("---%d, %d, %d, %d\n", lfPwm, rfPwm, lbPwm, rbPwm);
  //  Serial.printf("------%d, %d, %d, %d\n", lfPwm * (lf > 0), lfPwm * (lf < 0), lbPwm * (lb > 0), lbPwm * (lb < 0));

  lfMotor.setSpeedPWMAndDirection(lfPwm, (lf > 0) ? DIRECTION_FORWARD : DIRECTION_BACKWARD);
  rfMotor.setSpeedPWMAndDirection(rfPwm, (rf > 0) ? DIRECTION_FORWARD : DIRECTION_BACKWARD);
  lbMotor.setSpeedPWMAndDirection(lbPwm, (lb > 0) ? DIRECTION_FORWARD : DIRECTION_BACKWARD);
  rbMotor.setSpeedPWMAndDirection(rbPwm, (rb > 0) ? DIRECTION_FORWARD : DIRECTION_BACKWARD);


  // movement = false;
}

// Map x value from [0 .. 1] to [out_min .. out_max]
float mapPwm(float x, float out_min, float out_max) {
  return x * (out_max - out_min) + out_min;
}