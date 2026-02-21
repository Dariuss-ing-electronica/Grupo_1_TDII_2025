/*
 * Infraestructura.h
 *
 *  Created on: Jan 20, 2026
 *      Author: Martin
 */

#ifndef INFRAESTRUCTURA_H_
#define INFRAESTRUCTURA_H_

#include <stdint.h>
#include <stdbool.h>

/* --- Funciones I2C (LCD y SHT30) --- */
bool Infra_I2C_Write(uint8_t addr, uint8_t *data, uint16_t len);
bool Infra_I2C_Read(uint8_t addr, uint8_t *data, uint16_t len);

/* --- Funciones UART (Bluetooth) --- */
void Infra_UART_Write(uint8_t *data, uint16_t len);
bool Infra_UART_Read(uint8_t *data, uint16_t len);

void Infra_GPIO_Write(void *port, uint16_t pin, uint8_t estado);

/* --- Funciones de Tiempo --- */
void Infra_Delay(uint32_t ms);
uint32_t Infra_GetTick(void);

/* --- Funciones de ADC (Humedad Suelo) --- */
uint16_t Infra_ADC_Read(void);

#endif /* INFRAESTRUCTURA_H_ */
