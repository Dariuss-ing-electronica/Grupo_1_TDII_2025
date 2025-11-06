/*
 * API_debounce.c
 *
 *  Created on: 24 jul. 2023
 *      Author: GRUPO 1
 *      LUCERO DARIO ALEJANDRO
 *      PISTAN ULISES
 *      LEGUIZAMON MARCELO
 *      RIVERO MARTIN
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include <stdint.h>
#include <stdbool.h>

/*Includes*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef enum{
	BUTTON_UP,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_RISING
	} debounceState_t;

/*Declaracion de prototipo de funciones*/
bool readKey(void);
void debounceFSM_init(void);
void debounceFSM_update(bool buttonRead);
void buttonPressed(void);
void buttonReleased(void);

#endif /* API_INC_API_DEBOUNCE_H_ */
