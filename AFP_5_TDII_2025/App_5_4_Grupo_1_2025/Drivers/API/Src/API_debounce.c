/*
 * API_debounce.c
 *
 * Created on: 3 Oct. 2025
 * Author: GRUPO 1
 * LUCERO DARIO ALEJANDRO
 * PISTAN ULISES
 * LEGUIZAMON MARCELO
 * RIVERO MARTIN
 *
 * @brief Implementación de la FSM de antirrebote.
 *
 *
 */

/*Includes -------------------------------------------------------------------*/
#include <API_DEBOUNCE.h>
#include "API_delay.h" // Se asume que este módulo provee delayInit y delayRead
#include "API_GPIO.h"  // Se asume que este módulo provee writeLedOff_GPIO

/*Defines --------------------------------------------------------------------*/
#define DEBOUNCE_DELAY 40 // Tiempo (en ms) de espera para el antirrebote

/*Declaracion de variables (privadas del módulo) */
static debounceState_t actualState; // Almacena el estado actual de la FSM
static bool keyPressed = false;     // Flag para indicar una pulsación completada
delay_t debounceDelay;              // Estructura de delay para el antirrebote


/*Define functions -----------------------------------------------------------*/

/**
  * @brief  Devuelve 'true' si la tecla fue presionada (y consume el evento).
  * @brief  Asume tecla activa alta (true = presionado).
  * @param  None
  * @retval bool: 'true' si hubo una pulsación nueva, 'false' de lo contrario.
  */
bool readKey(void)
{
	bool keyPressDetected = false;

	// Si el flag de 'tecla presionada' está activo...
	if (keyPressed)
	{
		keyPressDetected = true; // Indicar que se detectó
		keyPressed = false;      // Limpiar el flag (consumir el evento)
	}

	// Devuelve 'true' solo una vez por pulsación
	return keyPressDetected;
}

/**
  * @brief  Carga el estado inicial de la MEF y configura el delay.
  * @param  None
  * @retval None
  */
void debounceFSM_init(void)
{
	actualState = BUTTON_UP; // Estado inicial: botón liberado
	keyPressed = false;      // Asegurar que el flag de pulsación inicie en falso
	// Inicializa la estructura de delay con el tiempo de antirrebote
	delayInit(&debounceDelay, DEBOUNCE_DELAY);
}

/**
  * @brief  Desarrollo de la MEF de debounce.
  * @brief  Recibe el estado actual del pin del botón.
  * @param  buttonRead: 'true' si el botón está presionado, 'false' si está liberado.
  * @retval None
  */
void debounceFSM_update(bool buttonRead)
{
	switch (actualState)
	{
	case BUTTON_UP: // Estado 0: Botón liberado
		// Condición de transición: Si se presiona el botón...
		if (buttonRead)
		{
			actualState = BUTTON_FALLING; // Pasa al estado de "espera de confirmación"
			delayRead(&debounceDelay);    // Inicia (o re-arma) el temporizador de antirrebote
		}
		break;

	case BUTTON_FALLING: // Estado 1: Flanco descendente detectado
		// Esperar a que pase el tiempo de DEBOUNCE_DELAY
		if (delayRead(&debounceDelay))
		{
			// Si pasó el tiempo, volver a chequear el botón
			if (buttonRead)
			{
				// Si sigue presionado, es una pulsación válida
				buttonPressed();          // Ejecutar la acción de "botón presionado"
				keyPressed = true;        // Levantar el flag para que readKey() lo reporte
				actualState = BUTTON_DOWN; // Pasa al estado de "botón presionado"
			}
			else
			{
				// Si no estaba presionado, fue ruido. Vuelve al estado inicial.
				actualState = BUTTON_UP;
			}
		}
		break;

	case BUTTON_DOWN: // Estado 2: Botón presionado (confirmado)
		// Condición de transición: Si se libera el botón...
		if (!buttonRead)
		{
			actualState = BUTTON_RISING; // Pasa al estado de "espera de liberación"
			delayRead(&debounceDelay);   // Inicia (o re-arma) el temporizador
		}
		break;

	case BUTTON_RISING: // Estado 3: Flanco ascendente detectado
		// Esperar a que pase el tiempo de DEBOUNCE_DELAY
		if (delayRead(&debounceDelay))
		{
			// Si pasó el tiempo, volver a chequear el botón
			if (!buttonRead)
			{
				// Si sigue liberado, es una liberación válida
				buttonReleased();         // Ejecutar la acción de "botón liberado"
				// 'keyPressed' no se modifica aquí, ya se reportó al presionar
				actualState = BUTTON_UP; // Vuelve al estado inicial
			}
			else
			{
				// Si no estaba liberado, fue ruido. Vuelve al estado anterior.
				actualState = BUTTON_DOWN;
			}
		}
		break;

	default:
		// En caso de un estado inválido, resetear la FSM
		debounceFSM_init();
		break;
	}
}

/**
  * @brief  Evento (callback) que se ejecuta cuando se detecta "tecla presionada".
  * @brief  Aquí se coloca la acción inmediata a realizar al presionar.
  * @param  none
  * @retval None
  */
void buttonPressed(void)
{

	writeLedOff_GPIO(LD1_Pin);
	writeLedOff_GPIO(LD2_Pin);
	writeLedOff_GPIO(LD3_Pin);
}

/**
  * @brief  Evento (callback) que se ejecuta cuando se detecta "tecla liberada".
  * @brief  Aquí se coloca la acción inmediata a realizar al liberar.
  * @param  none
  * @retval None
  */
void buttonReleased(void)
{

	writeLedOff_GPIO(LD1_Pin);
	writeLedOff_GPIO(LD2_Pin);
	writeLedOff_GPIO(LD3_Pin);
}
