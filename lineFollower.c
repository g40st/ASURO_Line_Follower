/*
 * Copyright 2016 Christian Högerle and Thomas Buck
 */

#include "asuro.h"
#include "mylcd.h"
#include "pid.h"

#define SHOULD 0
#define KP 4.5
#define KI 0.00
#define KD 4.2

#define BASE_SPEED 170
#define MIN_VALUE -BASE_SPEED
#define MAX_VALUE BASE_SPEED

unsigned int dataBlack[2];
unsigned int data[2];

int i, don, doff, speedLeft, speedRight;

uint8_t switch1, switch2;

double pidValue;
double pidValue2;

int counter = 0;

PIDState pidState;

void lineFollower(void) {
    unsigned char leftDir = FWD, rightDir = FWD;
    FrontLED(OFF);
    LineData(data);
    doff = (data[0] - data[1]);
    Msleep(1);
    FrontLED(ON);
    LineData(data);
    
    // Strecke verlassen
    if((dataBlack[0] + 30 < data[0]) && (dataBlack[1] + 30 < data[1])) {
        counter ++;
        if(counter > 5) {
            MotorSpeed(0, 0);
            MotorDir(BREAK,BREAK);
            MotorDir(RWD, RWD);
            MotorSpeed(110, 112);
            while(1) {
                LineData(data);
                if((dataBlack[0] + 10 > data[0]) && (dataBlack[1] + 10 > data[1])) {
                    MotorSpeed(0, 0);
                    MotorDir(BREAK, BREAK);
                    counter = 0;
                    break;
                }
            }
        }
    }

    don = (data[0] - data[1]);
    i = don - doff;
    
    pidValue = pidExecute(SHOULD, i, &pidState);
    pidValue2 = pidValue2 / 2;
    
    if(pidValue == 0) {
        speedLeft = speedRight = BASE_SPEED;
    } else if(pidValue > 0) {   // nach rechts
        speedLeft = BASE_SPEED + pidValue2;
        if (speedLeft > 255) {
            speedLeft = 255;            
            pidValue2 = speedLeft - BASE_SPEED;
        }
        pidValue = pidValue - pidValue2;
        speedRight = BASE_SPEED - pidValue;
        if(speedRight < 0) {
            speedRight = 0;
        }
    } else {    // nach links
        speedRight = BASE_SPEED + pidValue2;
        if (speedRight > 255) {
            speedRight = 255;            
            pidValue2 = speedRight - BASE_SPEED;
        }
        pidValue = pidValue - pidValue2;
        speedLeft = BASE_SPEED + pidValue;
        if(speedLeft < 0){
            speedLeft = 0;
        }
    }

    // bremsen
    if(speedLeft < 20) {  
        leftDir = BREAK;
    } 
    if(speedRight < 20) {
        rightDir = BREAK;
    }
    
    MotorDir(leftDir, rightDir);
    MotorSpeed(abs(speedLeft), abs(speedRight));
}


int main(void) {
    Init();
    StatusLED(GREEN);
    pidSet(&pidState, KP, KI, KD, MIN_VALUE, MAX_VALUE, MIN_VALUE, MAX_VALUE);
    // schwarzen Wert speichern
    FrontLED(ON);
    LineData(dataBlack);

    while(1) {
        lineFollower();

        switch1 = PollSwitch();
        switch2 = PollSwitch();
        if (switch1 && switch2 && switch1 == switch2) {
            // umdrehen
            if(switch1 == 18) {
                MotorSpeed(0, 0);
                MotorDir(BREAK,BREAK);
                
                unsigned int dataBlack2[2];
                LineData(dataBlack2);

                Msleep(10);
                MotorDir(RWD, FWD);
                MotorSpeed(112, 107);
                Msleep(200);

                while(1) {
                    LineData(data);
                    if((dataBlack2[0] + 10 > data[0]) && (dataBlack2[1] + 10 > data[1])) {
                        MotorSpeed(0, 0);
                        MotorDir(BREAK,BREAK);
                        break;
                    }
                }
            }
        }
    }   
    return 0;
}
