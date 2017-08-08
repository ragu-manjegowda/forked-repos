#pragma once
#include <vector>
#include <wiringPi.h>
#include <softPwm.h>

enum Direction {
   FORWARD,
   BACKWARD,
   BRAKE,
   RELEASE
};

enum Style {
 SINGLE,
 DOUBLE,
 INTERLEAVE,
 MICROSTEP
};

class Adafruit_MotorHAT; 

class Adafruit_StepperMotor {
public:
  static const int MICROSTEPS=8;
  static const unsigned char MICROSTEP_CURVE[9];

  Adafruit_StepperMotor(Adafruit_MotorHAT* hat, int num, int steps=200);
  Adafruit_MotorHAT* hat;

  int PWMA;
  int AIN2;
  int AIN1;
  int PWMB;
  int BIN2;
  int BIN1;

  int coils[4];

  static const int step2coils[8][4];
  int revsteps;
  int motornum;
  float sec_per_step;
  int steppingcounter;
  int currentstep;

  void setSpeed(int rpm);

  int oneStep(Direction dir, Style style);

  void step(int steps, Direction direction, Style stepstyle);
};

class Adafruit_DCMotor {
public:
  Adafruit_DCMotor(Adafruit_MotorHAT* _hat, int num); 
  Adafruit_MotorHAT* hat;

  int PWM;
  int IN2;
  int IN1;

  int motornum;

  void run(Direction dir);

  void setSpeed(int speed);

};

class Adafruit_MotorHAT {
public:
  int _frequency;
  std::vector<Adafruit_StepperMotor> steppers;
  std::vector<Adafruit_DCMotor> dcs;

  Adafruit_MotorHAT(int addr = 0x60, int freq = 1600, int i2c=-1, int i2c_bus=-1);

  void setPin(int pin, int value);

  Adafruit_StepperMotor& getStepper(int num);
  Adafruit_DCMotor& getDC(int num);
  void resetAll();
};

