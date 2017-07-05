// Compile with
// g++ -lrt -lwiringPi -lpthread -lcrypt -o DCMotorTest PWM.cpp Adafruit_MotorHAT.cpp DCMotorTest.cpp

// Run with 
// ./DCMotorTest

#include "Adafruit_MotorHAT.h"
#include "stdio.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>

Adafruit_MotorHAT hat;

void ctrl_c_handler(int s){
  std::cout << "Caught signal " << s << std::endl;
  hat.resetAll();
  exit(1); 
}

int main(int argc,char ** argv)
{
  signal(SIGINT, ctrl_c_handler);
  Adafruit_DCMotor& myMotor = hat.getDC(3); 

  // set the speed to start, from 0 (off) to 255 (max speed)
  myMotor.setSpeed(150);
  myMotor.run(FORWARD);
  // turn on motor
  myMotor.run(RELEASE);


  while (true) {
    std::cout << "Forward! " << std::endl;
    myMotor.run(FORWARD);

    std::cout << "\tSpeed up..." << std::endl;
    for(int i=0; i <= 255; i++) {
      myMotor.setSpeed(i);
      usleep(0.01*1000*1000);
    }

    std::cout << "\tSlow down..." << std::endl;
    for(int i=255; i >= 0; i--) {
      myMotor.setSpeed(i);
      usleep(0.01*1000*1000);
    }

    std::cout << "Backward! " << std::endl;
    myMotor.run(BACKWARD);

    std::cout << "\tSpeed up..." << std::endl;
    for(int i=0; i <= 255; i++) {
      myMotor.setSpeed(i);
      usleep(0.01*1000*1000);
    }

    std::cout << "\tSlow down..." << std::endl;
    for(int i=255; i >= 0; i--) {
      myMotor.setSpeed(i);
      usleep(0.01*1000*1000);
    }

    std::cout << "Release" << std::endl;
    myMotor.run(RELEASE);
    usleep(1*1000*1000);

  }

  // Return value
  return 0;
}

