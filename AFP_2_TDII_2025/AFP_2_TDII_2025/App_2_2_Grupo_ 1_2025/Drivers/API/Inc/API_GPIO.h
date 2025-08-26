/*
 * API_GPIO.h
 *
 *  Created on: Jul 24, 2025
 *      Author: GRUPO 1_2025:
 *              LUCERO DARIO
 *              PISTAN ULISES
 *              LEGUIZAMON MARCELO
 *              RIVERO MARTIN
 */

#ifndef API_INC_API_GPIO_H_
#define API_INC_API_GPIO_H_

#include <stdint.h>
#include <stdbool.h>


void MX_GPIO_Init(void);

// variables para transportar datos

typedef uint16_t led_t;
typedef bool buttonStatus_t;

//Funciones para modificar los estados de los leds

void writeLedOn_GPIO(led_t LDx);
void writeLedOff_GPIO(led_t LDx);
void toggleLed_GPIO(led_t LDx);

buttonStatus_t readButton_GPIO(void);

#endif /* API_INC_API_GPIO_H_ */

