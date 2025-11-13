/*
 * API_debounce.h
 *
 * Created on: 3 Oct. 2025.
 * Author: GRUPO 1
 * LUCERO DARIO ALEJANDRO
 * PISTAN ULISES
 * LEGUIZAMON MARCELO
 * RIVERO MARTIN
 *
 * @brief Módulo de antirrebote (debounce) para pulsadores mediante
 * una máquina de estados finitos (FSM).
 *
 * Modificado por: Gemini
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h> // Se incluye para el tipo estándar 'bool'
#include "main.h"

/* Exported types ------------------------------------------------------------*/

/**
  * @brief Define los estados posibles de la máquina de estados de debounce.
  */
typedef enum{
	BUTTON_UP,      // Estado: Botón liberado (en reposo)
	BUTTON_FALLING, // Estado: Detectado flanco descendente, esperando confirmación
	BUTTON_DOWN,    // Estado: Botón presionado (confirmado)
	BUTTON_RISING   // Estado: Detectado flanco ascendente, esperando confirmación
} debounceState_t;


/* Declaracion de prototipo de funciones -------------------------------------*/

/**
  * @brief  Lee el estado del flag de pulsación.
  * @brief  Esta función "consume" el evento de pulsación.
  * @brief  Devolverá 'true' solo una vez por cada vez que se presione
  * el botón, y luego 'false' hasta la próxima pulsación.
  * @param  None
  * @retval bool: 'true' si se detectó una nueva pulsación,
  * 'false' en caso contrario.
  */
bool readKey(void);

/**
  * @brief  Inicializa la máquina de estados de debounce.
  * @brief  Configura el estado inicial como BUTTON_UP y prepara el delay.
  * @param  None
  * @retval None
  */
void debounceFSM_init(void);

/**
  * @brief  Actualiza la máquina de estados de debounce.
  * @brief  Debe ser llamada periódicamente en el bucle principal.
  * @param  buttonRead: El estado actual del botón leído desde el pin
  * (true = presionado, false = liberado).
  * @retval None
  */
void debounceFSM_update(bool buttonRead);

/**
  * @brief  Función (callback) que se ejecuta una vez cuando se confirma
  * la pulsación del botón (flanco descendente).
  * @param  None
  * @retval None
  */
void buttonPressed(void);

/**
  * @brief  Función (callback) que se ejecuta una vez cuando se confirma
  * la liberación del botón (flanco ascendente).
  * @param  None
  * @retval None
  */
void buttonReleased(void);

#endif /* API_INC_API_DEBOUNCE_H_ */
