// Compile with
// g++ -lrt -lwiringPi -lpthread -lcrypt -o DCMotorTest PWM.cpp Adafruit_MotorHAT.cpp DCMotorTest.cpp

// Run with 
// ./DCMotorTest

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

int main(int argc,char ** argv)
{
  signal(SIGINT, ctrl_c_handler);
  Adafruit_DCMotor& rMotor = hat.getDC(2); // # motor port #1
  Adafruit_DCMotor& lMotor = hat.getDC(1); // # motor port #2
  
  while (true) {
    rMotor.setSpeed(75); 
    lMotor.setSpeed(75); 
    rMotor.run(FORWARD);
    lMotor.run(BACKWARD);
  }
  
  rMotor.run(RELEASE);
  lMotor.run(RELEASE);
  
  // Return value
  return 0;
}

