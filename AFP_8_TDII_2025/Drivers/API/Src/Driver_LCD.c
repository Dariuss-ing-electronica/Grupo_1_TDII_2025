/*
 * DriverLCD.c
 *
 *  Created on: Jan 20, 2026
 *      Author: Martin
 */


#include "Driver_LCD.h"

/* --- Funciones Privadas (Auxiliares) --- */

/* @brief  Función interna para enviar 4 bits (nibble) + bits de control.
 * @note   Gestiona el "pulse" del pin Enable (EN).
 * @param  lcd: Puntero a la instancia del LCD.
 * @param  data: Byte que contiene el nibble a enviar.
 * @param  flags: Flags de control (RS, BL, etc.).
 * @retval None
 */
static void LCD_SendInternal(LCD_t *lcd, uint8_t data, uint8_t flags) {
    uint8_t upper = (data & 0xF0);
    uint8_t lower = ((data << 4) & 0xF0);
    uint8_t buffer[4];

    // flags: bit 0=RS, bit 2=EN, bit 3=BL(1)
    buffer[0] = upper | flags | 0x0C; // EN=1
    buffer[1] = upper | flags | 0x08; // EN=0
    buffer[2] = lower | flags | 0x0C; // EN=1
    buffer[3] = lower | flags | 0x08; // EN=0

    if (lcd->write != 0) {
        lcd->write(lcd->address, buffer, 4);
    }
}

/* @brief  Envía un COMANDO al LCD (RS=0).
 * @param  lcd: Puntero a la instancia del LCD.
 * @param  cmd: Comando a ejecutar (ej. 0x01 para limpiar).
 * @retval None
 */
static void LCD_Cmd(LCD_t *lcd, uint8_t cmd) {
    LCD_SendInternal(lcd, cmd, 0x00); // RS=0
}

/* @brief  Envía un DATO al LCD (RS=1).
 * @param  lcd: Puntero a la instancia del LCD.
 * @param  data: Caracter ASCII a escribir.
 * @retval None
 */
static void LCD_Data(LCD_t *lcd, uint8_t data) {
    LCD_SendInternal(lcd, data, 0x01); // RS=1
}

/* @brief  Inicializa el objeto LCD y realiza la secuencia de arranque de hardware.
 * @note   Esta función es bloqueante debido a los retardos necesarios para el controlador HD44780.
 * @param  lcd: Puntero a la instancia de la estructura LCD_t.
 * @param  addr: Dirección I2C del dispositivo (ej. 0x27 << 1).
 * @param  writeFn: Puntero a la función de escritura I2C (Infraestructura).
 * @param  delayFn: Puntero a la función de retardo bloqueante (Infraestructura).
 * @retval None
 */
void LCD_Inicializar(LCD_t *lcd, uint8_t addr, lcd_write_fn writeFn, lcd_delay_fn delayFn) {
    lcd->address = addr;
    lcd->write = writeFn;
    lcd->delay = delayFn;

    if (lcd->delay == 0 || lcd->write == 0) return;

    // Secuencia de inicialización 4 bits
    lcd->delay(50);
    LCD_Cmd(lcd, 0x30);
    lcd->delay(5);
    LCD_Cmd(lcd, 0x30);
    lcd->delay(1);
    LCD_Cmd(lcd, 0x30);
    lcd->delay(10);
    LCD_Cmd(lcd, 0x20); // 4-bit mode
    lcd->delay(10);

    // Configuración
    LCD_Cmd(lcd, 0x28); // 4-bit, 2 líneas, fuente 5x8
    lcd->delay(1);
    LCD_Cmd(lcd, 0x08); // Display OFF
    lcd->delay(1);
    LCD_Cmd(lcd, 0x01); // Limpiar pantalla (Clear)
    lcd->delay(2);
    LCD_Cmd(lcd, 0x06); // Modo de entrada (Entry mode)
    lcd->delay(1);
    LCD_Cmd(lcd, 0x0C); // Display ON, Cursor OFF, Blink OFF
}

/* @brief  Escribe una cadena de texto completa en la posición actual del cursor.
 * @param  lcd: Puntero a la instancia del LCD.
 * @param  str: Puntero a la cadena de caracteres (terminada en null '\0').
 * @retval None
 */
void LCD_EnviarCadena(LCD_t *lcd, char *str) {
    while (*str) {
        LCD_Data(lcd, (uint8_t)(*str));
        str++;
    }
}

/* @brief  Mueve el cursor a una posición específica (Fila, Columna).
 * @param  lcd: Puntero a la instancia del LCD.
 * @param  row: Número de fila (0 para la primera, 1 para la segunda).
 * @param  col: Número de columna (0 a 15).
 * @retval None
 */
void LCD_Apuntar(LCD_t *lcd, uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    address += col;
    LCD_Cmd(lcd, address);
}

/* @brief  Borra todo el contenido de la pantalla y devuelve el cursor al inicio.
 * @note   Este comando requiere un tiempo de ejecución mayor (~2ms).
 * @param  lcd: Puntero a la instancia del LCD.
 * @retval None
 */
void LCD_Limpiar(LCD_t *lcd) {
    LCD_Cmd(lcd, 0x01);
    if(lcd->delay) lcd->delay(2);
}
