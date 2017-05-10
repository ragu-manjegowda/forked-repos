#include "Adafruit_MotorHAT.h"
#include "stdio.h"
#include <iostream>
#include <signal.h>

Adafruit_MotorHAT hat;

void my_handler(int s){
  printf("Caught signal %d\n",s); fflush(stdout);
  hat.resetAll();
  exit(1); 
}

int main(int argc, char** argv) {
  signal(SIGINT, my_handler);

  auto& myStepper = hat.getStepper(1); // # motor port #1
  std::cout << "Got stepper" << std::endl << std::flush;
  myStepper.setSpeed(30); //  # 30 RPM

  while (true) {
    printf("Single coil steps");
    myStepper.step(100, FORWARD,  SINGLE);
    myStepper.step(100, BACKWARD, SINGLE);

    printf("Double coil steps");
    myStepper.step(100, FORWARD,  DOUBLE);
    myStepper.step(100, BACKWARD, DOUBLE);

    printf("Interleaved coil steps");
    myStepper.step(100, FORWARD,  INTERLEAVE);
    myStepper.step(100, BACKWARD, INTERLEAVE);

    printf("Microsteps");
    myStepper.step(100, FORWARD,  MICROSTEP);
    myStepper.step(100, BACKWARD, MICROSTEP);
  }
}
