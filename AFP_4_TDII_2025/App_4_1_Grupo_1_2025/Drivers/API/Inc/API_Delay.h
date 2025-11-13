/*
 * API_Delay.h
 *
 *  Created on: August 31, 2025
 *      Author: Grupo 1_TD2	Pistan Ulises, Lucero Dario Alejandro, Rivero Martin, Leguizamon Marcelo
 */

#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

/*	Includes	*/
#include <stdint.h>
#include <stdbool.h>

/*	Typedefs 	*/
typedef uint32_t tick_t;
typedef bool bool_t;
typedef struct{
	tick_t startTime;
	tick_t duration;
	bool_t running;
} delay_t;

/*	Declaracion de Funciones	*/
void delayInit(delay_t* delay, tick_t duration);
bool_t delayRead(delay_t* delay);
void delayWrite(delay_t* delay, tick_t duration);

#endif /* API_INC_API_DELAY_H_ */
