/*
 * DriverSHT30.c
 *
 *  Created on: Jan 20, 2026
 *      Author: Martin
 */


#include "Driver_SHT30.h"
#include <stddef.h> // Para NULL

void SHT30_Inicializar(SHT30_t *dev, uint8_t addr, sht_write_fn writeFn, sht_read_fn readFn, sht_delay_fn delayFn) {
    dev->address = addr;
    dev->write = writeFn;
    dev->read = readFn;
    dev->delay = delayFn;
    dev->temperatura = 0.0f;
    dev->humedad = 0.0f;
}

bool SHT30_Leer(SHT30_t *dev) {
    // Validar punteros
    if (!dev->write || !dev->read || !dev->delay) return false;

    uint8_t cmd[2];
    uint8_t buffer[6];

    // 1. Preparar comando (High Repeatability, No Clock Stretch)
    cmd[0] = SHT30_CMD_MSB;
    cmd[1] = SHT30_CMD_LSB;

    // 2. Enviar comando
    if (dev->write(dev->address, cmd, 2) == false) {
        return false;
    }

    // 3. Esperar conversión (aprox 15ms según datasheet)
    dev->delay(20);

    // 4. Leer respuesta (6 bytes: T_MSB, T_LSB, CRC, H_MSB, H_LSB, CRC)
    if (dev->read(dev->address, buffer, 6) == false) {
        return false;
    }

    // 5. Conversión Matemática (Pura y Portable)
    uint16_t temp_raw = (buffer[0] << 8) | buffer[1];
    uint16_t hum_raw  = (buffer[3] << 8) | buffer[4];

    // Fórmulas del Datasheet SHT3x-DIS
    dev->temperatura = -45.0f + 175.0f * ((int)temp_raw / 65535.0f);
    dev->humedad = 100.0f * ((int)hum_raw / 65535.0f);

    return true;
}
