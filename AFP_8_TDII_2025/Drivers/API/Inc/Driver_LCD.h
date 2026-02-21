/*
 * DriverLCD.h
 *
 *  Created on: Jan 20, 2026
 *      Author: Martin
 */

#ifndef DRIVER_LCD_H_
#define DRIVER_LCD_H_

#include <stdint.h>
#include <stdbool.h>

/* Definición de los enchufes (Punteros a Función) */
typedef void (*lcd_delay_fn)(uint32_t ms);
typedef bool (*lcd_write_fn)(uint8_t addr, uint8_t *data, uint16_t len);

typedef struct {
    uint8_t address;
    lcd_write_fn write;
    lcd_delay_fn delay;
} LCD_t;

void LCD_Inicializar(LCD_t *lcd, uint8_t addr, lcd_write_fn writeFn, lcd_delay_fn delayFn);
void LCD_EnviarCadena(LCD_t *lcd, char *str);
void LCD_Apuntar(LCD_t *lcd, uint8_t row, uint8_t col);
void LCD_Limpiar(LCD_t *lcd);

#endif /* DRIVERLCD_H_ */
