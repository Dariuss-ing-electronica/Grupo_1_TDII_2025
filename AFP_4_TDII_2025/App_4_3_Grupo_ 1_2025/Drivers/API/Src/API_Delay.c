/*
 * API_Delay.h
 *
 *  Created on: Sep 19, 2025
 *      Author: Grupo 1_TD2	Lucero Dario, Martin Rivero, Pistan Ulises, Leguizamon Marcelo
 */


#include "main.h"
#include "API_Delay.h"

/*
 * @brief Inicializacion del delay, establece la duracion
 * @param delay_t delay, tick_t duration
 * @retval None
 */
void delayInit(delay_t * delay, tick_t duration){
	delay->duration = duration;
	delay->running = false;
}

/*
 * @brief Revisa si ya paso el tiempo correspondiente al delay
 * @param delay_t delay
 * @retval 	false -> No transcurrio el tiempo del delay
 * 			true -> Ya transcurrio el tiempo del delay
 */
bool_t delayRead(delay_t* delay){
	if(delay->running){
		if ((HAL_GetTick() - delay->startTime) >= delay->duration){
			delay->running = false;
			return true;
		}else{
			return false;
		}
	}else{
		delay->startTime = HAL_GetTick();
		delay->running = true;
		return false;
	}
}

/*
 * @brief Reescribe el tiempo de duracion
 * @param delay_t delay, tick_t duration
 * @retval None
 */
void delayWrite(delay_t* delay, tick_t duration){
	delay->duration = duration;
}
