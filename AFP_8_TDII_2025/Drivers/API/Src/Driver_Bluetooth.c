/*
 * DriverBluetooth2.c
 *
 *  Created on: Jan 19, 2026
 *      Author: Martin
 */
#include "Driver_Bluetooth.h"
#include <string.h> // Para strlen
#include <stddef.h> // Para NULL

/* @brief  Inicializa el objeto Bluetooth y envía mensaje de bienvenida.
 * @param  bt: Instancia del driver.
 * @param  writeFn: Función de envío.
 * @param  readFn: Función de recepción.
 * @retval None
 */
void Bluetooth_Inicializar(Bluetooth_t *bt, uart_write_fn writeFn, uart_read_fn readFn) {
    bt->write = writeFn;
    bt->read = readFn;

    // Enviamos mensaje de bienvenida usando la propia función abstracta
    if (bt->write != NULL) {
        Bluetooth_Enviar(bt, "Bluetooth Iniciado(LISTO)");
    }
}

/* @brief  Envía una cadena de texto usando el puntero de función.
 * @param  bt: Instancia del driver.
 * @param  mensaje: Texto a enviar.
 * @retval None
 */
void Bluetooth_Enviar(Bluetooth_t *bt, char *mensaje) {
    if (bt->write != NULL) {
        // Llamamos a la función genérica, sin saber qué hardware hay debajo
        bt->write((uint8_t*)mensaje, (uint16_t)strlen(mensaje));
    }
}

/* @brief  Lee un byte usando el puntero de función.
 * @param  bt: Instancia del driver.
 * @retval uint8_t: Dato leído o 0.
 */
uint8_t Bluetooth_Leer(Bluetooth_t *bt) {
    uint8_t dato = 0;

    if (bt->read != NULL) {
        // Pedimos leer 1 byte. Si la función devuelve true, retornamos el dato.
        if (bt->read(&dato, 1) == true) {
            return dato;
        }
    }

    return 0; // No llegó nada
}
