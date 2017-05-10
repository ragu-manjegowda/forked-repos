all:
	g++ -std=c++11 -o StepperMotorTest -lwiringPi *.cpp

clean:
	rm -rf StepperMotorTest

