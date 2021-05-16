#pragma once

#include <stdbool.h>
#include <VGA/VGA.h>

typedef void(*Handler)(char, unsigned char);

bool subscribePress(Handler h);

bool subscribeRelease(Handler h);

void onKeyPressed(unsigned char scancode);

void onKeyReleased(unsigned char scancode);
