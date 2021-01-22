#ifndef _MAIN_H
#define _MAIN_H

#include "http_server.h"
#include "sendData.h"
#include "tiltBridge.h"
#include "wifi_setup.h"
#include "serialhandler.h"
#include "jsonconfig.h"
#include <Arduino.h>
#include <Ticker.h>

#ifdef LOG_LOCAL_LEVEL
#include <esp_log.h>
#endif

void printMem();
void setup();
void loop();

extern uint64_t board_reset_pressed_at;

#endif