#include "Adafruit_MotorHAT.h" 
#include <exception>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "PWM.h"
#include <iostream>

const unsigned char Adafruit_StepperMotor::MICROSTEP_CURVE[9] = {0, 50, 98, 142, 180, 212, 236, 250, 255};
const int Adafruit_StepperMotor::step2coils[8][4] = {
                           {1, 0, 0, 0},
                           {1, 1, 0, 0},
                           {0, 1, 0, 0},
                           {0, 1, 1, 0},
                           {0, 0, 1, 0},
                           {0, 0, 1, 1},
                           {0, 0, 0, 1},
                           {1, 0, 0, 1} };

Adafruit_StepperMotor::Adafruit_StepperMotor(Adafruit_MotorHAT* _hat, int num, int steps) {
    hat = _hat;
	revsteps = steps;
	motornum = num;
	sec_per_step = 0.1;
	steppingcounter = 0;
	currentstep = 0;

	num -= 1;

	if (num == 0) {
		PWMA = 8;
		AIN2 = 9;
		AIN1 = 10;
		PWMB = 13;
		BIN2 = 12;
		BIN1 = 11;
	} else if (num == 1) {
		PWMA = 2;
		AIN2 = 3;
		AIN1 = 4;
		PWMB = 7;
		BIN2 = 6;
		BIN1 = 5;
	} else throw "MotorHAT Stepper must be between 1 and 2 inclusive";
}

void Adafruit_StepperMotor::setSpeed(int rpm) {
	sec_per_step = 60.0 / (revsteps * rpm);
	steppingcounter = 0;
}

int Adafruit_StepperMotor::oneStep(Direction dir, Style style) {
	unsigned char pwm_a = 255;
        unsigned char pwm_b = 255;

	// first determine what sort of stepping procedure we're up to
	if (style == SINGLE) {
		if ((currentstep/(MICROSTEPS/2)) % 2 == 1) {
			// we're at an odd step, weird
			if (dir == FORWARD) {
				currentstep += MICROSTEPS/2;
			} else {
				currentstep -= MICROSTEPS/2;
			}
		} else {
			// go to next even step
			if (dir == FORWARD) {
				currentstep += MICROSTEPS;
			} else {
				currentstep -= MICROSTEPS;
			}
		}
	} else if (style == DOUBLE) {
		if (currentstep/(MICROSTEPS/2) % 2 == 0) {
			// we're at an even step, weird
			if (dir == FORWARD) {
				currentstep += MICROSTEPS/2;
			} else {
				currentstep -= MICROSTEPS/2;
			}
		} else {
			// go to next odd step
			if (dir == FORWARD) {
				currentstep += MICROSTEPS;
			} else {
				currentstep -= MICROSTEPS;
			}
		}
	} else if (style == INTERLEAVE) {
		if (dir == FORWARD) {
			currentstep += MICROSTEPS/2;
		} else {
			currentstep -= MICROSTEPS/2;
		}

	} else if (style == MICROSTEP) {
		if (dir == FORWARD) {
			currentstep += 1;
		} else {
			currentstep -= 1;

			// go to next 'step' and wrap around
			currentstep += MICROSTEPS * 4;
			currentstep %= MICROSTEPS * 4;
		}

		pwm_a = pwm_b = 0;
		if ((currentstep >= 0) && (currentstep < MICROSTEPS)) {
			pwm_a = MICROSTEP_CURVE[MICROSTEPS - currentstep];
			pwm_b = MICROSTEP_CURVE[currentstep];
		} else if ((currentstep >= MICROSTEPS) && (currentstep < MICROSTEPS*2)) {
			pwm_a = MICROSTEP_CURVE[currentstep - MICROSTEPS];
			pwm_b = MICROSTEP_CURVE[MICROSTEPS*2 - currentstep];
		} else if ((currentstep >= MICROSTEPS*2) && (currentstep < MICROSTEPS*3)) {
			pwm_a = MICROSTEP_CURVE[MICROSTEPS*3 - currentstep];
			pwm_b = MICROSTEP_CURVE[currentstep - MICROSTEPS*2];
		} else if ((currentstep >= MICROSTEPS*3) && (currentstep < MICROSTEPS*4)) {
			pwm_a = MICROSTEP_CURVE[currentstep - MICROSTEPS*3];
			pwm_b = MICROSTEP_CURVE[MICROSTEPS*4 - currentstep];
		}
	}

	// go to next 'step' and wrap around
	currentstep += MICROSTEPS * 4;
	currentstep %= MICROSTEPS * 4;

	// only really used for microstepping, otherwise always on!
	setPWM(PWMA, 0, pwm_a*16);
	setPWM(PWMB, 0, pwm_b*16);

	// set up coil energizing!
	memset(coils,0,sizeof(int)*4);

	if (style == MICROSTEP) {
		if ((currentstep >= 0) and (currentstep < MICROSTEPS)) {
			coils[0] = 1;
                        coils[1] = 1;
		} else if ((currentstep >= MICROSTEPS) and (currentstep < MICROSTEPS*2)) {
			coils[1] = 1;
                        coils[2] = 1;
		} else if ((currentstep >= MICROSTEPS*2) and (currentstep < MICROSTEPS*3)) {
			coils[2] = 1;
                        coils[3] = 1;
		} else if ((currentstep >= MICROSTEPS*3) and (currentstep < MICROSTEPS*4)) {
			coils[0] = 1;
                        coils[3] = 1;
		}
	} else {
		memcpy(&coils[0], &step2coils[currentstep/(MICROSTEPS/2)][0], sizeof(int)*4);
	}

	hat->setPin(AIN2, coils[0]);
	hat->setPin(BIN1, coils[1]);
	hat->setPin(AIN1, coils[2]);
	hat->setPin(BIN2, coils[3]);

	return currentstep;
}

void Adafruit_StepperMotor::step(int steps, Direction direction, Style stepstyle) {
	float s_per_s = sec_per_step;
	int lateststep = 0;

	if (stepstyle == INTERLEAVE) {
		s_per_s = s_per_s / 2.0;
	} else if (stepstyle == MICROSTEP) {
		s_per_s /= MICROSTEPS;
		steps *= MICROSTEPS;
	}

	std::cout << s_per_s << " sec per step" << std::endl;

	for(int s = 0;s < steps; s++) {
		lateststep = oneStep(direction, stepstyle);
		usleep(1000*1000*s_per_s);
	}

	if (stepstyle == MICROSTEP) {
		// this is an edge case, if we are in between full steps, lets just keep going
		// so we end on a full step
		while ((lateststep != 0) and (lateststep != MICROSTEPS)) {
			lateststep = oneStep(direction, stepstyle);
			usleep(1000*1000*s_per_s);
		}
	}
}

Adafruit_DCMotor::Adafruit_DCMotor(Adafruit_MotorHAT* _hat, int num) {
    hat = _hat;
	motornum = num;
	
	PWM = 0;
	IN1 = 0;
	IN2 = 0;
	if (num == 0) {
		PWM = 8;
		IN1 = 9;
		IN2 = 10;
	} else if (num == 1) {
		PWM = 13;
		IN1 = 12;
		IN2 = 11;
	} else if (num == 2) {
		PWM = 2;
		IN1 = 3;
		IN2 = 4;
	} else if (num == 3) {
		PWM = 7;
		IN1 = 6;
		IN2 = 5;
	} else throw "MotorHAT Motor must be between 1 and 4 inclusive";
}

void Adafruit_DCMotor::run(Direction dir) {
	if ( dir == FORWARD) {
		hat->setPin(IN2,0);
		hat->setPin(IN1,1);
	}
	if ( dir == BACKWARD) {
		hat->setPin(IN1,0);
		hat->setPin(IN2,1);
	}
	if ( dir == RELEASE) {
		hat->setPin(IN1,0);
		hat->setPin(IN2,0);
	}
}

void Adafruit_DCMotor::setSpeed(int speed) {
	if (speed < 0) {
		speed = 0;
	}
	if (speed > 255) {
		speed = 255;
	}
	setPWM(PWM, 0, speed*16);
}

Adafruit_MotorHAT::Adafruit_MotorHAT(int addr, int freq, int i2c, int i2c_bus) {
	_frequency = freq;
	for(int idx = 1; idx <= 2; idx++) {
		steppers.push_back(Adafruit_StepperMotor(this, idx));
    }
	for(int idx = 0; idx <= 3; idx++) {
		dcs.push_back(Adafruit_DCMotor(this, idx));
    }
	initPWM(addr);
	setPWMFreq(_frequency);
}

void Adafruit_MotorHAT::setPin(int pin, int value) {
	if ((pin < 0) || (pin > 15)) {
		throw "PWM pin must be between 0 and 15 inclusive";
        }
	if ((value != 0) && (value != 1)) {
		throw "Pin value must be 0or 1!";
        }
	if (value == 0) {
		setPWM(pin, 0, 4096);
        }
	if (value == 1) {
		setPWM(pin, 4096, 0);
        }
}

Adafruit_StepperMotor& Adafruit_MotorHAT::getStepper(int num) {
	if ((num < 1) || (num > 2))
		throw "MotorHAT Stepper must be between 1 and 2 inclusive";
	return steppers[num-1];
}

Adafruit_DCMotor& Adafruit_MotorHAT::getDC(int num) {
	if ((num < 1) || (num > 4))
		throw "MotorHAT DC must be between 1 and 4 inclusive";
	return dcs[num-1];
}

void Adafruit_MotorHAT::resetAll() {
  setAllPWM(0,0);
}


