/*
 * pid.c
 *
 * Copyright (c) 2013, Thomas Buck <xythobuz@me.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <asuro.h>
#include <pid.h>

double pidExecute(double should, double is, PIDState *state) {
    unsigned long now = Gettime();
    double timeChange = (double)(now - state->last);
    double error = should - is;
    double newErrorSum = state->sumError + (error * timeChange);
    if ((newErrorSum >= state->intMin) && (newErrorSum <= state->intMax))
        state->sumError = newErrorSum; // Prevent Integral Windup
    double dError = (error - state->lastError) / timeChange;
    double output = (state->kp * error) + (state->ki * state->sumError) + (state->kd * dError);
    state->lastError = error;
    state->last = now;
    if (output > state->outMax) {
        output = state->outMax;
    }
    if (output < state->outMin) {
        output = state->outMin;
    }
    return output;
}

void pidSet(PIDState *pid, double kp, double ki, double kd, double min, double max, double iMin, double iMax) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->outMin = min;
    pid->outMax = max;
    pid->intMin = iMin;
    pid->intMax = iMax;
    pid->lastError = 0;
    pid->sumError = 0;
    pid->last = 0;
}

