#include "Adafruit_MotorHAT.h"
#include "stdio.h"
#include <iostream>
#include <signal.h>

Adafruit_MotorHAT hat;

void ctrl_c_handler(int s){
  std::cout << "Caught signal " << s << std::endl;
  hat.resetAll();
  exit(1); 
}

int main(int argc, char** argv) {
  signal(SIGINT, ctrl_c_handler);

  auto& myStepper = hat.getStepper(1); // # motor port #1
  std::cout << "Got stepper" << std::endl;
  myStepper.setSpeed(30); //  # 30 RPM

  while (true) {
    std::cout << "Single coil steps" << std::endl;
    myStepper.step(100, FORWARD,  SINGLE);
    myStepper.step(100, BACKWARD, SINGLE);

    std::cout << "Double coil steps" << std::endl;
    myStepper.step(100, FORWARD,  DOUBLE);
    myStepper.step(100, BACKWARD, DOUBLE);

    std::cout << "Interleaved coil steps" << std::endl;
    myStepper.step(100, FORWARD,  INTERLEAVE);
    myStepper.step(100, BACKWARD, INTERLEAVE);

    std::cout << "Microsteps" << std::endl;
    myStepper.step(100, FORWARD,  MICROSTEP);
    myStepper.step(100, BACKWARD, MICROSTEP);
  }
}
