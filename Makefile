all:
	g++ -std=c++11 -c *.cpp
	ar cr AdafruitStepperMotorHAT.a Adafruit_MotorHAT.o PWM.o
	g++ -std=c++11 -o StepperMotorTest -lwiringPi StepperMotorTest.cpp AdafruitStepperMotorHAT.a

clean:
	rm -rf StepperMotorTest *.a *.o

