/*
 * Copyright 2016 Christian Högerle and Thomas Buck
 */

#include "asuro.h"


int main(void) {
    Init();
    StatusLED(OFF);
    FrontLED(ON);
    
    SerPrint("Hello World");

    while(1){}
    
    return 0;
}
