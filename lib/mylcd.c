/*
 * Copyright 2016 Christian Högerle and Thomas Buck
 */

#include <asuro.h>
#include <mylcd.h>

void clearLCD() {
    unsigned char buf[2] = {254,128};
    SerWrite(buf, 2);
    SerPrint("                ");
    unsigned char buf2[2] = {254,192};
    SerWrite(buf2, 2);
    SerPrint("                ");
}

void lcdSetCursorToLine1() {
    unsigned char buf[2] = {254,128};
    SerWrite (buf, 2);
}

void lcdSetCursorToLine2() {
    unsigned char buf[2] = {254,192};
    SerWrite (buf, 2);
}
 
