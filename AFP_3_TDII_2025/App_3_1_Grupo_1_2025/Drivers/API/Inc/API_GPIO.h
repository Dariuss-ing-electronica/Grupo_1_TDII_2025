/*
 * API_GPIO.h
 *
 *  Created on: Jul 25, 2025
 *      Author: GRUPO 1
 */
#include <stdint.h>
#ifndef API_INC_API_GPIO_H_
#define API_INC_API_GPIO_H_
typedef uint16_t led_t;

void MX_GPIO_Init(void);
void writeLedon_GPIO(led_t LDx);
void writeLedoff_GPIO(led_t LDx);
void toggleLed_GPIO(led_t LDx);
#endif /* API_INC_API_GPIO_H_ */
