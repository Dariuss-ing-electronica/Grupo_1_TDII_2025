/*
 * API_debounce.c
 *
 *  Created on: 24 jul. 2023
 *      Author: quique
 */

/*Includes -------------------------------------------------------------------*/
#include "main.h"
#include "API_debounce.h"
#include "API_delay.h"
#include "API_GPIO.h"
/*Defines --------------------------------------------------------------------*/
#define DEBOUNCE_DELAY 40

/*Declaracion de variables */
static debounceState_t actualState;
static bool_t keyPressed = false;
static bool_t fallingEdge = false;
delay_t debounceDelay;
bool_t buttonRead = true;

/*Define functions -----------------------------------------------------------*/

/**
  * @brief Devuelve true si la tecla fue presionada. Asume tecla activa alta como caso placa
  * F413ZH que estamos usando
  * @param None
  * @retval Boolean
  */
bool_t readKey(void)
{
	bool_t keyPress = false;
	fallingEdge = false;

	if(keyPressed)
	{
		keyPress = true;
	}
	return keyPress;
}

/**
  * @brief Carga el estado inicial de la MEF
  * @param None
  * @retval None
  */
void debounceFSM_init(void)
{
	actualState = BUTTON_UP;
	delayInit(&debounceDelay, DEBOUNCE_DELAY);
	HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

	buttonRead = true; //USER_Btn_Pin no activado
}

/**
  * @brief Desarrollo de la MEF debounce
  * @param boolean buttonRead (tecla presionada)
  * @retval None
  */
void debounceFSM_update(bool buttonRead)
{
	switch (actualState)
	{
	case BUTTON_UP: //estado inicial 0
		//Chequear condicion de transición
		if(buttonRead == true) //se presionó USER_Btn_Pin
		{
			actualState = BUTTON_FALLING; //pasa al estado siguiente
			delayRead(&debounceDelay);   //arranca cuenta de DEBOUNCE_DELAY
		}
	break;

	case BUTTON_FALLING:
		//Chequea si paso el tiempo de 40 ms
		if(delayRead(&debounceDelay))
		{
			//Chequear condicion de transición
			if(buttonRead == true) //se presionó USER_Btn_Pin
			{
				buttonPressed(); //Dispara funcion buttonPresed()
				keyPressed = true; //Indica tecla presionada luego de 2 lecturas en 40 ms
				fallingEdge = true; //asume que en este estado hubo un flanco decreciente
				actualState = BUTTON_DOWN; //pasa al estado siguiente
			}
			else
			{
				actualState = BUTTON_UP; // regresa al estado anterior
			}
		}
	break;

	case BUTTON_DOWN:
		//Chequear condicion de transición
		if(buttonRead == false) //se liberó USER_Btn_Pin
		{
			actualState = BUTTON_RISING; //pasa al estado siguiente
			delayRead(&debounceDelay);   //arranca cuenta de DEBOUNCE_DELAY
		}
	break;

	case BUTTON_RISING:
		//Chequea si paso el tiempo de 40 ms
		if(delayRead(&debounceDelay))
		{
			//Chequear condicion de transición
			if(buttonRead == false) //se USER_Btn_Pin regresó a estado inactivo
			{
				buttonReleased(); //Dispara funcion buttonRelaased()
				keyPressed = false; //Indica tecla presionada luego de 2 lecturas en 40 ms
				actualState = BUTTON_UP; //pasa al estado siguiente, el inicial
			}
			else
			{
				actualState = BUTTON_DOWN; // regresa al estado anterior
			}
		}
	break;

	default:
		Error_Handler();
	break;
	}
}

/**
  * @brief Evento cuando se detecta tecla presionada
  * @param none
  * @retval None
  */


void buttonPressed(void)
{
	writeLedOff_GPIO(LD1_Pin);
	writeLedOff_GPIO(LD2_Pin);
	writeLedOff_GPIO(LD3_Pin);

}

/**
  * @brief Evento cuando se detecta tecla liberada
  * @param none
  * @retval None
  */
void buttonReleased(void)
{
	writeLedOff_GPIO(LD1_Pin);
    writeLedOff_GPIO(LD2_Pin);
	writeLedOff_GPIO(LD3_Pin);


}

