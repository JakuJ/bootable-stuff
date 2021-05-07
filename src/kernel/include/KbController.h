#pragma once

#include <stdbool.h>
#include <VGA.h>

#define MAX_HANDLERS 16
#define MAX_SCANCODES 88

typedef void(*Handler)(char, unsigned char);

bool subscribePress(Handler h);

bool subscribeRelease(Handler h);

void onKeyPressed(unsigned char scancode);

void onKeyReleased(unsigned char scancode);
