/*
 * DriverHumedad.c
 *
 *  Created on: Jan 18, 2026
 *      Author: Martin
 */

#include "Driver_SensorSuelo.h"
#include <stddef.h> // Para NULL

/**
  * @brief  Inicializa la estructura del sensor.
  * @param  sensor: Puntero a la instancia.
  * @param  seco: Calibración Seco.
  * @param  mojado: Calibración Mojado.
  * @param  readFn: Puntero a función de lectura.
  * @retval None
  */
void SensorHumedad_Inicializar(SensorHumedad_t *sensor, uint16_t seco, uint16_t mojado, sensor_read_fn readFn) {
    sensor->val_seco = seco;
    sensor->val_mojado = mojado;
    sensor->read = readFn;
}

/**
  * @brief  Obtiene el valor crudo usando la función abstracta.
  * @param  sensor: Puntero a la instancia.
  * @retval uint16_t: Valor leído o 0 si hay error.
  */
uint16_t SensorHumedad_LeerValorMedido(SensorHumedad_t *sensor) {
    if (sensor->read != NULL) {
        return sensor->read();
    }
    return 0;
}

/**
  * @brief  Convierte la lectura a porcentaje.
  * @param  sensor: Puntero a la instancia.
  * @retval uint8_t: Porcentaje 0-100.
  */
uint8_t SensorHumedad_LeerPorcentaje(SensorHumedad_t *sensor) {
    uint32_t raw = (uint32_t)SensorHumedad_LeerValorMedido(sensor);

    // Protección de límites (Clamping)
    if (raw > sensor->val_seco ) raw = sensor->val_seco;
    if (raw < sensor->val_mojado) raw = sensor->val_mojado;

    // Fórmula inversa: (Seco - Leido) * 100 / (Seco - Mojado)
    uint32_t numerador = (uint32_t)(sensor->val_seco - raw) * 100;
    uint32_t denominador = (uint32_t)(sensor->val_seco - sensor->val_mojado);

    if (numerador == 0) return 0; // Evitar división por cero
    sensor->humedadSuelo = (uint32_t)(numerador / denominador);

    return (uint32_t)(numerador / denominador);

}
