/*
 * DriverHumedad.h
 *
 *  Created on: Jan 20, 2026
 *      Author: Martin
 */

#ifndef DRIVER_SENSORSUELO_H_
#define DRIVER_SENSORSUELO_H_

#include <stdint.h>

/**
  * @brief  Definición del tipo de función para leer el ADC (Hardware Abstraction).
  * @retval uint16_t: Valor crudo leído del ADC (0-4095).
  */
typedef uint16_t (*sensor_read_fn)(void);

/**
  * @brief  Estructura del objeto Sensor de Humedad.
  */
typedef struct {
    uint16_t val_seco;      /*!< Valor de calibración para seco (0%) */
    uint16_t val_mojado;    /*!< Valor de calibración para mojado (100%) */
    uint8_t humedadSuelo;  // <--- Aquí se guardará el último valor leído (0-100%)
    sensor_read_fn read;    /*!< Puntero a la función externa de lectura */
} SensorHumedad_t;

/**
  * @brief  Inicializa el sensor de humedad.
  * @param  sensor: Puntero a la estructura del sensor.
  * @param  seco: Valor de calibración ADC para tierra seca.
  * @param  mojado: Valor de calibración ADC para tierra mojada.
  * @param  readFn: Función externa que devuelve el valor crudo del ADC.
  * @retval None
  */
void SensorHumedad_Inicializar(SensorHumedad_t *sensor, uint16_t mojado, uint16_t seco, sensor_read_fn readFn);

/**
  * @brief  Lee el valor crudo directamente (útil para calibrar).
  * @param  sensor: Puntero a la estructura del sensor.
  * @retval uint16_t: Valor crudo del ADC.
  */
uint16_t SensorHumedad_LeerValorMedido(SensorHumedad_t *sensor);

/**
  * @brief  Calcula el porcentaje de humedad basado en la calibración.
  * @param  sensor: Puntero a la estructura del sensor.
  * @retval uint8_t: Porcentaje de humedad (0-100).
  */
uint8_t SensorHumedad_LeerPorcentaje(SensorHumedad_t *sensor);

#endif/* DRIVERHUMEDAD_H_ */
