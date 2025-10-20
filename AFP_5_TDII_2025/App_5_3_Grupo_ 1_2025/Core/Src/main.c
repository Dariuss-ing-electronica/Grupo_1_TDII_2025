/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "API_GPIO.h"
#include "string.h"
#include "API_Delay.h"
#include "API_debounce.h"
#include <stdint.h>
#include <stdbool.h>
/* Private includes ----------------------------------------------------------*/

#define retardo1 150
#define retardo2 300
#define retardo3 100
#define retardo4 600

#define CANT_LED 3
#define FRECUENCIAS 4
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

 uint16_t LEDS[CANT_LED] = {LD1_Pin, LD2_Pin, LD3_Pin}; /*Creo vector de LEDs de usuario*/
 uint16_t vector_frecuencias[FRECUENCIAS] = {retardo1, retardo2, retardo3 ,retardo4};
/* USER CODE END PD */
 bool readButton = true;

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

ETH_TxPacketConfig TxConfig;
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

ETH_HandleTypeDef heth;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

//Elimino static por que la uso para los drivers

// static void MX_GPIO_Init(void);

static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
/* USER CODE BEGIN PFP */

//Declaración de funciones

void Secuencia1(uint16_t VECTOR_Led[3]);
void Secuencia2(uint16_t VECTOR_Led[3]);
void Secuencia3(uint16_t VECTOR_Led[3]);
void Secuencia4(uint16_t VECTOR_Led[3]);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */


int main(void)
{


	  /* USER CODE BEGIN 1 */
        //variables de delay para secuencias 1 y 2

	    delay_t timer_inicio;

        delayInit(&timer_inicio, vector_frecuencias[FRECUENCIAS]);

	    //variables para el manejo de secuencias 3 y 4

	    delay_t timer_led1, timer_led2, timer_led3;   // temporizadores independientes
	    bool_t estado_led1 = false, estado_led2 = false, estado_led3 = false;
        delayInit(&timer_led1, vector_frecuencias[2]);
	    delayInit(&timer_led2, vector_frecuencias[1]);
	    delayInit(&timer_led3, vector_frecuencias[3]);

        //Variables y def para la lectura de boton

	    int16_t contador=0;
	    int16_t i=0;

	    bool_t estado_led = false; // Almacena en que fase de la secuencia se encuentra el led. true-> Encendido, false-> Apagado.
	    bool_t statusButton = false;
	    bool_t botonPresionadoAntes = false; // Flag para detectar el flanco (true = suelto)


  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  //Inicializo la MSF

  debounceFSM_init();
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//SELECCION DE SECUENCIA
          switch (contador)
               {
      case 0:
    	  // Secuencia 1
    	  delayWrite(&timer_inicio, vector_frecuencias[0]);
    	  	  if(delayRead(&timer_inicio))
    	  	 	      {
    	  	 	 	 if(estado_led)

    	  	 	 	 {
    	  	 	 	   writeLedOff_GPIO(LEDS[i]);
    	  	 	 	    //inicio de la secuencia
    	  	 	 	   if(i==(CANT_LED-1))

    	  	 	 	   {    i=0;

    	  	 	 	         }

    	  	 	 	   else{
    	  	 	 	    	i++;

    	  	 	 	    			  }

    	  	 	 	   estado_led = false;

    	  	 	 	    }
    	  	 	      }

    	  	 	 	 else{
    	  	 	 	 			  writeLedOn_GPIO(LEDS[i]);
    	  	 	 	 			  estado_led = true;
    	  	 	 	 		  }



                     break;

      case 1:
    	  // Secuencia 2
        	  delayWrite(&timer_inicio, vector_frecuencias[1]);

        	  if(delayRead(&timer_inicio))
        	  {
        		  if(estado_led)
        		  {
        			  writeLedOff_GPIO(LEDS[0]);
        	      	  writeLedOff_GPIO(LEDS[1]);
        	      	  writeLedOff_GPIO(LEDS[2]);
        	      	  //inicio de la secuencia
        	      	  estado_led = false;
        	      }
        	      else
        	      {
        	      	writeLedOn_GPIO(LEDS[0]);
    	      		writeLedOn_GPIO(LEDS[1]);
        	      	writeLedOn_GPIO(LEDS[2]);
        	      	estado_led = true;
        	      }
        	   }



                     break;

case 2:

// Secuencia 3

delayWrite(&timer_led1, vector_frecuencias[2]);

if(delayRead(&timer_led1))
{
  if(estado_led1)
   {
    writeLedOff_GPIO(LEDS[0]);

    //inicio de la secuencia
    estado_led1 = false;
    	          	                       }
    else
    {
     writeLedOn_GPIO(LEDS[0]);

      estado_led1 = true;
    }
 }

delayWrite(&timer_led2, vector_frecuencias[1]);

if(delayRead(&timer_led2))
   {
     if(estado_led2)
      {
    	 writeLedOff_GPIO(LEDS[1]);

   //inicio de la secuencia
    estado_led2 = false;
      }
   else
  {
   writeLedOn_GPIO(LEDS[1]);

    	estado_led2 = true;
    	        }
    	   }

delayWrite(&timer_led3, vector_frecuencias[3]);

if(delayRead(&timer_led3))
   {
     if(estado_led3)
  {
    	 writeLedOff_GPIO(LEDS[2]);

   //inicio de la secuencia
    estado_led3 = false;
    	       }
   else
  {
   writeLedOn_GPIO(LEDS[2]);

    	estado_led3 = true;
    	        }
    	   }
                     break;

 case 3:
//secuencia 4
delayWrite(&timer_led1, vector_frecuencias[0]);

if(delayRead(&timer_led1))
{
if(estado_led1)
{  writeLedOn_GPIO(LEDS[0]);
writeLedOn_GPIO(LEDS[2]);

//inicio de la secuencia
estado_led1 = false;
 }

else
{    writeLedOff_GPIO(LEDS[0]);
     writeLedOff_GPIO(LEDS[2]);

      estado_led1 = true;
 }
    	    	   }

delayWrite(&timer_led2, vector_frecuencias[0]);

if(delayRead(&timer_led2))
{
  if(estado_led2)
  { writeLedOff_GPIO(LEDS[1]);

  //inicio de la secuencia
  estado_led2 = false;
    	     	    	 }
   else
  {   writeLedOn_GPIO(LEDS[1]);

   estado_led2 = true;
    	     	   }
    	     	    }
break;

default:
// Secuencia 1
delayWrite(&timer_inicio, vector_frecuencias[0]);
if(delayRead(&timer_inicio))
{
   if(estado_led)

  {
    writeLedOff_GPIO(LEDS[i]);
   //inicio de la secuencia
   if(i==(CANT_LED-1))

   {    i=0;

    	      }

   else{
    	 i++;

    	      }

   estado_led = false;

    	      }
    	   }

   else{
    	  writeLedOn_GPIO(LEDS[i]);
    	   estado_led = true;
    	   }
break;

 }
readButton = readButton_GPIO();
//Paso la lectura del boton
debounceFSM_update(readButton);

//lectura del estado actual del boton (debounced)
statusButton = readKey();

// --- LÓGICA DE DETECCIÓN DE FLANCO ---
// Verificamos si el botón ESTÁ presionado AHORA (true)
// Y si ANTES NO ESTABA presionado (false)
if (statusButton == true && botonPresionadoAntes == false)
 {
 // ¡Flanco detectado! Incrementar el contador SOLO UNA VEZ
 contador = contador + 1;

// Reiniciar el contador si supera el número de secuencias (0, 1, 2, 3)
if (contador >= 4)
{
   contador = 0;
    }
 }

// Actualizamos nuestro flag para la próxima vuelta del buclez
// "El estado actual ahora será el estado anterior"
 botonPresionadoAntes = statusButton;




    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

//******************************************************
//FUNCIONES DE SECUENCIA

void Secuencia1(uint16_t VECTOR_Led[3])

{


for (uint8_t i = 0; i < 3; i++){

writeLedOn_GPIO( VECTOR_Led[i]);
HAL_Delay(retardo1);
writeLedOff_GPIO(VECTOR_Led[i]);

}

}

void Secuencia2(uint16_t VECTOR_Led[3])

{



writeLedOn_GPIO( VECTOR_Led[0]);
writeLedOn_GPIO( VECTOR_Led[1]);
writeLedOn_GPIO( VECTOR_Led[2]);

HAL_Delay(retardo2);

writeLedOff_GPIO(VECTOR_Led[0]);
writeLedOff_GPIO(VECTOR_Led[1]);
writeLedOff_GPIO(VECTOR_Led[2]);

HAL_Delay(retardo2);

		}



void Secuencia3(uint16_t VECTOR_Led[3])
{
     

	 toggleLed_GPIO(VECTOR_Led[0]);
	 HAL_Delay(retardo3);
	 toggleLed_GPIO(VECTOR_Led[0]);
	 HAL_Delay(retardo3);
	 toggleLed_GPIO(VECTOR_Led[0]);
	 HAL_Delay(retardo3);

	 toggleLed_GPIO(VECTOR_Led[0]);
	 toggleLed_GPIO(VECTOR_Led[1]);
	 HAL_Delay(retardo3);

	 toggleLed_GPIO(VECTOR_Led[0]);
	 HAL_Delay(retardo3);
	 toggleLed_GPIO(VECTOR_Led[0]);
	 HAL_Delay(retardo3);

	 toggleLed_GPIO(VECTOR_Led[0]);
	 toggleLed_GPIO(VECTOR_Led[1]);
	 toggleLed_GPIO(VECTOR_Led[2]);
	 HAL_Delay(retardo3);
}

void Secuencia4(uint16_t VECTOR_Led[3])
{
    

	toggleLed_GPIO(VECTOR_Led[0]);
	toggleLed_GPIO(VECTOR_Led[2]);
	HAL_Delay(retardo1);

    toggleLed_GPIO(VECTOR_Led[0]);
	toggleLed_GPIO(VECTOR_Led[2]);

	toggleLed_GPIO(VECTOR_Led[1]);
	HAL_Delay(retardo1);
	toggleLed_GPIO(VECTOR_Led[1]);
}










/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
