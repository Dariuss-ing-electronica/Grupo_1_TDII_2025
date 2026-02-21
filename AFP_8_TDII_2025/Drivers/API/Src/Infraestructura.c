/*
 * Infraestructura.c
 *
 *  Created on: Jan 20, 2026
 *      Author: Martin
 */


#include "Infraestructura.h"
#include "main.h" // Acceso a la HAL y a los handles (huart5, hi2c1, etc)

/* Importamos los handles definidos en main.c */
extern I2C_HandleTypeDef hi2c1;
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart5;

/* --- Implementación I2C --- */
/**
  * @brief  Envía datos a un dispositivo esclavo a través del bus I2C.
  * @note   Utiliza HAL_I2C_Master_Transmit con un timeout fijo de 100ms.
  * @param  addr: Dirección del dispositivo esclavo (desplazada a la izquierda 1 bit).
  * @param  data: Puntero al buffer de datos a enviar.
  * @param  len: Cantidad de bytes a enviar.
  * @retval bool: true si la transmisión fue exitosa (HAL_OK), false en caso contrario.
  */
bool Infra_I2C_Write(uint8_t addr, uint8_t *data, uint16_t len) {
    if (HAL_I2C_Master_Transmit(&hi2c1, addr, data, len, 100) == HAL_OK) return true;
    return false;
}
/**
  * @brief  Lee datos desde un dispositivo esclavo a través del bus I2C.
  * @note   Utiliza HAL_I2C_Master_Receive con un timeout fijo de 100ms.
  * @param  addr: Dirección del dispositivo esclavo (desplazada a la izquierda 1 bit).
  * @param  data: Puntero al buffer donde se guardarán los datos recibidos.
  * @param  len: Cantidad de bytes a leer.
  * @retval bool: true si la recepción fue exitosa (HAL_OK), false en caso contrario.
  */
bool Infra_I2C_Read(uint8_t addr, uint8_t *data, uint16_t len) {
    if (HAL_I2C_Master_Receive(&hi2c1, addr, data, len, 100) == HAL_OK) return true;
    return false;
}

/* --- Implementación UART (Bluetooth) --- */
/**
  * @brief  Envía datos por el puerto UART (Bluetooth).
  * @note   Esta función es bloqueante hasta que se completen el envío o el timeout (100ms).
  * @param  data: Puntero al buffer de datos a enviar.
  * @param  len: Cantidad de bytes a enviar.
  * @retval None
  */
void Infra_UART_Write(uint8_t *data, uint16_t len) {
    HAL_UART_Transmit(&huart5, data, len, 100);
}

/**
  * @brief  Intenta leer datos del puerto UART (Bluetooth).
  * @note   Utiliza un timeout muy corto (1ms) para comportarse como NO bloqueante.
  * @param  data: Puntero al buffer donde se guardará el dato leído.
  * @param  len: Cantidad de bytes a leer.
  * @retval bool: true si se recibió el dato correctamente, false si hubo timeout o error.
  */
bool Infra_UART_Read(uint8_t *data, uint16_t len) {
    // Timeout de 1ms para lectura no bloqueante
    if (HAL_UART_Receive(&huart5, data, len, 1) == HAL_OK) {
        return true;
    }
    return false;
}

/* --- Implementación ADC --- */
/**
  * @brief  Realiza una conversión analógica a digital (ADC) en modo Polling.
  * @note   Inicia el ADC, espera la conversión y detiene el periférico.
  * @param  None
  * @retval uint16_t: Valor crudo de la conversión (0 a 4095 para 12 bits).
  */
uint16_t Infra_ADC_Read(void) {
    uint16_t val = 0;
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 50) == HAL_OK) {
        val = (uint16_t)HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    return val;
}
// Función puente que conecta tu DriverGPIO con la HAL de STM32
void Infra_GPIO_Write(void *port, uint16_t pin, uint8_t estado) {
    // Casteamos el void* al tipo real de STM32 (GPIO_TypeDef*)
    GPIO_PinState pinState = (estado == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin((GPIO_TypeDef*)port, pin, pinState);
}

/* --- Implementación Tiempo --- */
/**
  * @brief  Genera un retardo bloqueante en milisegundos.
  * @note   Utiliza la función HAL_Delay basada en el SysTick.
  * @param  ms: Tiempo de espera en milisegundos.
  * @retval None
  */
void Infra_Delay(uint32_t ms) {
    HAL_Delay(ms);
}
/**
  * @brief  Obtiene el valor actual del contador de ticks del sistema.
  * @note   Se utiliza para medir intervalos de tiempo de forma no bloqueante.
  * @param  None
  * @retval uint32_t: Cantidad de milisegundos transcurridos desde el inicio.
  */
uint32_t Infra_GetTick(void) {
    return HAL_GetTick();
}
