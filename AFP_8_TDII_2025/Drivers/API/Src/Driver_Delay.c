/*
 * DriverDelay.c
 *
 *  Created on: Jan 15, 2026
 *      Author: Martin
 */


#include "Driver_Delay.h"
#include <stddef.h>

void Delay_Inicializar(delay_t * delay, tick_t duration, tick_get_fn tickFn){
   delay->duration = duration;
   delay->running = false;
   delay->getTick = tickFn;
}

bool_t Delay_Leer(delay_t* delay){
   if(delay->getTick == NULL) return false;

   if(delay->running){
      if ((delay->getTick() - delay->startTime) >= delay->duration){
         delay->running = false;
         return true;
      } else {
         return false;
      }
   } else {
      delay->startTime = delay->getTick();
      delay->running = true;
      return false;
   }
}

void Delay_Escribir(delay_t* delay, tick_t duration){
   delay->duration = duration;
}
