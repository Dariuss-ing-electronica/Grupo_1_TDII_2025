/*
 * DriverSHT30.h
 *
 *  Created on: Jan 20, 2026
 *      Author: Martin
 */

#ifndef DRIVER_SHT30_H_
#define DRIVER_SHT30_H_

#include <stdint.h>
#include <stdbool.h>

/* Definiciones de tipos de función (Abstracción) */
typedef bool (*sht_write_fn)(uint8_t addr, uint8_t *data, uint16_t len);
typedef bool (*sht_read_fn)(uint8_t addr, uint8_t *data, uint16_t len);
typedef void (*sht_delay_fn)(uint32_t ms);

/* Comandos internos del sensor (Portable) */
#define SHT30_CMD_MSB 0x24
#define SHT30_CMD_LSB 0x00

typedef struct {
    int temperatura;      /*!< Última temperatura leída */
    int humedad;         /*!< Última humedad leída */
    uint8_t address;        /*!< Dirección I2C (ej. 0x44 << 1) */

    /* Punteros a funciones externas */
    sht_write_fn write;
    sht_read_fn read;
    sht_delay_fn delay;
} SHT30_t;

/**
  * @brief  Inicializa el objeto SHT30.
  * @param  dev: Puntero a la instancia.
  * @param  addr: Dirección I2C.
  * @param  writeFn: Función de escritura I2C.
  * @param  readFn: Función de lectura I2C.
  * @param  delayFn: Función de delay bloqueante.
  */
void SHT30_Inicializar(SHT30_t *dev, uint8_t addr, sht_write_fn writeFn, sht_read_fn readFn, sht_delay_fn delayFn);

/**
  * @brief  Realiza una lectura completa (Comando -> Espera -> Lectura -> Conversión).
  * @param  dev: Puntero a la instancia.
  * @return true si éxito, false si error de comunicación.
  */
bool SHT30_Leer(SHT30_t *dev);

#endif /* DRIVERSHT30_H_ */
