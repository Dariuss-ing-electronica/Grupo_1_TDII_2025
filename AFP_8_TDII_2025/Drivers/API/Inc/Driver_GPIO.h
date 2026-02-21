/*
 * DriverGPIO.h
 * Descripción: Driver genérico para salidas digitales (Relés, LEDs, etc.)
 * Sin dependencias de hardware específico.
 */

#ifndef DRIVER_GPIO_H_
#define DRIVER_GPIO_H_

#include <stdint.h>
#include <stdbool.h> // Para true/false

/* --- 1. Definición de Tipos de Función (Abstracción) --- */
// Definimos cómo debe ser la función que escriba en el hardware.
// port: Puntero genérico al puerto (GPIOB, GPIOA, etc.)
// pin: Número de pin
// estado: 1 (Set) o 0 (Reset)
typedef void (*gpio_write_fn)(void *port, uint16_t pin, uint8_t estado);

/* --- 2. Estructura del Objeto (Salida Digital) --- */
typedef struct {
    void *port;           /*!< Puerto del hardware (ej. GPIOB) */
    uint16_t pin;         /*!< Pin del hardware (ej. GPIO_PIN_0) */
    uint8_t estadoActual; /*!< Guardamos el estado (1 o 0) para poder hacer Toggle lógico */

    gpio_write_fn write;  /*!< Puntero a la función de escritura externa */
} Actuador_t;

/* --- 3. Prototipos de Funciones --- */

// Inicializa el objeto
void Actuador_Inicializar(Actuador_t *dev, void *port, uint16_t pin, gpio_write_fn writeFn);

// Enciende la salida
void Actuador_On(Actuador_t *dev);

// Apaga la salida
void Actuador_Off(Actuador_t *dev);

// Invierte el estado (Toggle)
void Actuador_Toggle(Actuador_t *dev);

#endif /* DRIVERGPIO_H_ */
