/*
 * DriverGPIO.c
 */

#include "Driver_GPIO.h"
#include <stddef.h> // Para NULL

void Actuador_Inicializar(Actuador_t *dev, void *port, uint16_t pin, gpio_write_fn writeFn) {
    dev->port = port;
    dev->pin = pin;
    dev->write = writeFn;
    dev->estadoActual = 0; // Asumimos apagado al iniciar

    // Opcional: Asegurar estado inicial apagado físicamente
    if (dev->write != NULL) {
        dev->write(dev->port, dev->pin, 0);
    }
}

void Actuador_On(Actuador_t *dev) {
    if (dev->write != NULL) {
        dev->write(dev->port, dev->pin, 1);
        dev->estadoActual = 1;
    }
}

void Actuador_Off(Actuador_t *dev) {
    if (dev->write != NULL) {
        dev->write(dev->port, dev->pin, 0);
        dev->estadoActual = 0;
    }
}

void Actuador_Toggle(Actuador_t *dev) {
    if (dev->write != NULL) {
        // Invertimos el estado lógico
        if (dev->estadoActual == 1) {
            dev->estadoActual = 0;
        } else {
            dev->estadoActual = 1;
        }
        // Escribimos el nuevo estado
        dev->write(dev->port, dev->pin, dev->estadoActual);
    }
}
