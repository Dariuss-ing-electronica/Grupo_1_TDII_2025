/*
 * DriverBluetooth2.h
 *
 *  Created on: Jan 19, 2026
 *      Author: Martin
 */

#ifndef DRIVER_BLUETOOTH_H
#define DRIVER_BLUETOOTH_H

#include <stdint.h>
#include <stdbool.h> /* Para usar bool, true, false */

/**
  * @brief  Definición del puntero a función para ENVIAR datos por UART.
  * @param  data: Puntero al buffer de datos a enviar.
  * @param  len: Cantidad de bytes a enviar.
  * @retval None
  */
typedef void (*uart_write_fn)(uint8_t *data, uint16_t len);

/**
  * @brief  Definición del puntero a función para LEER datos por UART.
  * @brief  Esta función debe ser NO bloqueante (timeout corto).
  * @param  data: Puntero donde se guardará el dato leído.
  * @param  len: Cantidad de bytes a leer.
  * @retval bool: true si se leyó algo correctamente, false si no.
  */
typedef bool (*uart_read_fn)(uint8_t *data, uint16_t len);

/**
  * @brief  Estructura principal del objeto Bluetooth.
  */
typedef struct {
    uart_write_fn write; /*!< Función para transmitir */
    uart_read_fn  read;  /*!< Función para recibir */
} Bluetooth_t;

/**
  * @brief  Inicializa la estructura del Bluetooth.
  * @param  bt: Puntero a la instancia del driver.
  * @param  writeFn: Función de escritura del hardware específico.
  * @param  readFn: Función de lectura del hardware específico.
  * @retval None
  */
void Bluetooth_Inicializar(Bluetooth_t *bt, uart_write_fn writeFn, uart_read_fn readFn);

/**
  * @brief  Envía un mensaje de texto completo.
  * @param  bt: Puntero a la instancia del driver.
  * @param  mensaje: Cadena de caracteres terminada en null.
  * @retval None
  */
void Bluetooth_Enviar(Bluetooth_t *bt, char *mensaje);

/**
  * @brief  Intenta leer un carácter del Bluetooth.
  * @param  bt: Puntero a la instancia del driver.
  * @retval uint8_t: El carácter leído, o 0 si no hubo datos nuevos.
  */
uint8_t Bluetooth_Leer(Bluetooth_t *bt);

#endif // DRIVER_BLUETOOTH_H
