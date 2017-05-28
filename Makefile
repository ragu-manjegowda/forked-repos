all:
	g++ -std=c++11 -c *.cpp
	ar cr AdafruitStepperMotorHAT.a Adafruit_MotorHAT.o PWM.o
	g++ -std=c++11 -o StepperMotorTest StepperMotorTest.cpp AdafruitStepperMotorHAT.a -lwiringPi -lpthread -lcrypt -lrt

clean:
	rm -rf StepperMotorTest *.a *.o
	rm -rf StepperMotorTest
