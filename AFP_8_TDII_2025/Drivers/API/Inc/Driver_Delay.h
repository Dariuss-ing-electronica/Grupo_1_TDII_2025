/*
 * DriverDelay.h
 *
 *  Created on: Jan 20, 2026
 *      Author: Martin
 */

#ifndef DRIVER_DELAY_H_
#define DRIVER_DELAY_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t tick_t;
typedef bool bool_t;

/* Puntero a función para obtener el tiempo (ticks) */
typedef tick_t (*tick_get_fn)(void);

typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
   tick_get_fn getTick; /* El driver preguntará la hora aquí */
} delay_t;

void Delay_Inicializar(delay_t* delay, tick_t duration, tick_get_fn tickFn);
bool_t Delay_Leer(delay_t* delay);
void Delay_Escribir(delay_t* delay, tick_t duration);

#endif /* DRIVERDELAY_H_ */
