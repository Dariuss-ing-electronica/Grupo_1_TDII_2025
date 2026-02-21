/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Driver_GPIO.h"
#include "Driver_LCD.h"
#include "Driver_Delay.h"
#include "Driver_SensorSuelo.h"
#include "Driver_SHT30.h"
#include "Driver_Bluetooth.h" // <--- Tu driver portable
#include "Infraestructura.h" // <--- El puente al hardware
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TIEMPO_LECTURA    2000
#define TIEMPO_BOMBA      3000
#define TIEMPO_VENTILADOR 3000
#define TIEMPO_FOCO       3000
// Definimos los "nombres" de cada etapa
typedef enum {
    ETAPA_BLUETOOTH = 0,
    ETAPA_SENSORES_LCD,
    ETAPA_LOGICA_ENCENDIDO,
    ETAPA_AUTO_APAGADO
} Etapa_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

ETH_TxPacketConfig TxConfig;
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

ADC_HandleTypeDef hadc1;

ETH_HandleTypeDef heth;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
Actuador_t bomba;
Actuador_t ventilador;
Actuador_t foco;
Actuador_t LED1;
Actuador_t LED2;
Actuador_t LED3;
//------>delay_t delayLector;//Esto queda en forma de comentario por si lo queremos usar despues de otra forma
SensorHumedad_t sensorSuelo;
SHT30_t sensorAire;
Bluetooth_t miBluetooth; // Instancia Bluetooth
LCD_t miLCD;
// --- Temporizadores ---
delay_t timerLectura;     // Para leer sensores cada 1 seg
delay_t timerBomba;       // Para contar los 30 seg de riego
delay_t timerVentilador;  // Para contar los 30 seg de ventilación
delay_t timerFoco;  // Para contar los 30 seg de luz


// --- Variables de Estado (Esto usaremos dentro del while) ---
char bufferDisplay[50];

// --- Constantes de Calibración ---
#define UMBRAL_SECO    20  // Si baja de 20%, regar
#define TEMP_UMBRAL_ALTA   35.0f
#define TEMP_UMBRAL_BAJA   10.0f

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_UART5_Init(void);
/* USER CODE BEGIN PFP */

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

  /* USER CODE END 1 */

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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */
  // 1. Init Infraestructura y Drivers

    // LCD: I2C y Delay
    Infra_Delay(1000);
    LCD_Inicializar(&miLCD, (0x27 << 1), Infra_I2C_Write, Infra_Delay);

    // SHT30: I2C (Write/Read) y Delay
    SHT30_Inicializar(&sensorAire, (0x44 << 1), Infra_I2C_Write, Infra_I2C_Read, Infra_Delay);

    // Sensor Suelo: ADC
    SensorHumedad_Inicializar(&sensorSuelo, 4095, 950, Infra_ADC_Read);
    // Bluetooth: UART (Write/Read) -> AQUI CONECTAMOS CON INFRA
    Bluetooth_Inicializar(&miBluetooth, Infra_UART_Write, Infra_UART_Read);

    // Tiempos
//----->    delayInit(&delayLector, 1000, Infra_GetTick);

    // 2. Configuración de Tiempos
    Delay_Inicializar(&timerLectura,    TIEMPO_LECTURA,    Infra_GetTick);
    Delay_Inicializar(&timerBomba,      TIEMPO_BOMBA,      Infra_GetTick);
    Delay_Inicializar(&timerVentilador, TIEMPO_VENTILADOR, Infra_GetTick);
    Delay_Inicializar(&timerFoco,       TIEMPO_FOCO,       Infra_GetTick);

    LCD_Limpiar(&miLCD);
    LCD_Apuntar(&miLCD, 0, 0);
    LCD_EnviarCadena(&miLCD, "INICIANDO: ");
    LCD_Apuntar(&miLCD, 1, 0);
    LCD_EnviarCadena(&miLCD, "Sistema de riego");

    Actuador_Inicializar(&bomba,      (void*)GPIOF, RELE_BOMBA_Pin, Infra_GPIO_Write);
    Actuador_Inicializar(&ventilador, (void*)GPIOF, RELE_VENTILADOR_Pin, Infra_GPIO_Write);
    Actuador_Inicializar(&foco,       (void*)GPIOF, RELE_FOCO_Pin, Infra_GPIO_Write);

    Actuador_Inicializar(&LED1,      (void*)GPIOB, LD1_Pin, Infra_GPIO_Write);
    Actuador_Inicializar(&LED2, (void*)GPIOB, LD2_Pin, Infra_GPIO_Write);
    Actuador_Inicializar(&LED3,       (void*)GPIOB, LD3_Pin, Infra_GPIO_Write);




    Etapa_t ETAPA_ACTUAL = ETAPA_BLUETOOTH;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  while (1)
	  {
		    /* USER CODE END WHILE */
			  switch(ETAPA_ACTUAL) {

			  case ETAPA_BLUETOOTH:
		      // ============================================================
		      // CASO 1: RECEPCIÓN BLUETOOTH (Alta Prioridad)
		      // ============================================================
		      uint8_t datoRecibido = Bluetooth_Leer(&miBluetooth);
		      if (datoRecibido != 0) {
		          switch(datoRecibido) {
		              case 'A': // Bomba Manual
		            	  Actuador_Toggle(&bomba); // Invierte estado (Si estaba ON pasa a OFF y viceversa)
		            	  Actuador_Toggle(&LED1); // Invierte estado (Si estaba ON pasa a OFF y viceversa)

		            	      if(bomba.estadoActual == 1) {
		            	          Delay_Escribir(&timerBomba, TIEMPO_BOMBA); // Reiniciamos timer
		            	          // No hace falta poner GPIO_On, el Toggle ya lo hizo
		            	      }
		            	      break;

		              case 'B': // Ventilador Manual
		            	  Actuador_Toggle(&foco); // Invierte estado (Si estaba ON pasa a OFF y viceversa)
		            	  Actuador_Toggle(&LED3); // Invierte estado (Si estaba ON pasa a OFF y viceversa)

		            	      if(foco.estadoActual == 1) {
		            	    	  Delay_Escribir(&timerFoco, TIEMPO_FOCO); // Reiniciamos timer
		            	          // No hace falta poner GPIO_On, el Toggle ya lo hizo
		            	      }
		            	      break;

		              case 'C': // Ventilador Manual

			            	  Actuador_Toggle(&ventilador); // Invierte estado (Si estaba ON pasa a OFF y viceversa)
			            	  Actuador_Toggle(&LED2); // Invierte estado (Si estaba ON pasa a OFF y viceversa)

			            	      if(ventilador.estadoActual == 1) {
			            	    	  Delay_Escribir(&timerVentilador, TIEMPO_VENTILADOR); // Reiniciamos timer
			            	          // No hace falta poner GPIO_On, el Toggle ya lo hizo
			            	      }
			            	      break;


		          }
		      }

		      ETAPA_ACTUAL = ETAPA_SENSORES_LCD;
		      break;

			  case ETAPA_SENSORES_LCD:
		      // ============================================================
		      // 2. LÓGICA DE SENSORES (Temporizada)
		      // ============================================================
		      if (Delay_Leer(&timerLectura)) {

		          // A. Actualizar Valores
		          SHT30_Leer(&sensorAire); // Actualiza .temperatura y .humedad en la estructura
		          SensorHumedad_LeerPorcentaje(&sensorSuelo);
		          // Leemos el suelo y lo guardamos en una variable local auxiliar
		         // LCD_Limpiar(&miLCD);---------------------------------------> Dejamos sin uso esta linea de limpiar LCD ya que sobre escribimos con los espacios correspondientes
		          // B. Mostrar en LCD - FILA 0: Datos del Sensor de Aire (SHT30)
		          LCD_Apuntar(&miLCD, 0, 0);
		          sprintf(bufferDisplay, "T:%d C HA:%d %  %",sensorAire.temperatura, sensorAire.humedad);
		          LCD_EnviarCadena(&miLCD, bufferDisplay);

		          // C. Mostrar en LCD - FILA 1: Datos del Sensor de Suelo
		          LCD_Apuntar(&miLCD, 1, 0);
		          sprintf(bufferDisplay, "Suelo: %d %%        ", sensorSuelo.humedadSuelo);
		          LCD_EnviarCadena(&miLCD, bufferDisplay);

//		          uint32_t valorCrudo = HAL_ADC_GetValue(&hadc1);
//		          // 2. Imprímelo en el LCD o Bluetooth
//		          sprintf(bufferDisplay, "RAW: %lu      ", valorCrudo);
//		          LCD_EnviarCadena(&miLCD, bufferDisplay);

		          // D. Enviar por Bluetooth (Todo junto separado por ;)
		          // Formato: T:25.5;H:60;S:40
		          sprintf(bufferDisplay, " %d; %d; %d; \n", sensorAire.humedad, sensorAire.temperatura, sensorSuelo.humedadSuelo);
		          Bluetooth_Enviar(&miBluetooth, bufferDisplay);



		          ETAPA_ACTUAL = ETAPA_LOGICA_ENCENDIDO;
		          break;

		          case ETAPA_LOGICA_ENCENDIDO:
		            // ============================================================
			        // CASE 3: LÓGICA DE CONTROL (Decidir encender)
			        // ============================================================
		          // Usamos la variable local 'humedadSuelo' que acabamos de leer
		          if (sensorSuelo.humedadSuelo < UMBRAL_SECO && bomba.estadoActual == 0) {
		        	  Actuador_On(&bomba); // Rele ON
		        	  Actuador_On(&LED1); // Rele ON

		        	  Delay_Escribir(&timerBomba, TIEMPO_BOMBA); // Iniciamos cuenta regresiva
		          }

		          // --------------------------------------------------------
		          // F. AUTOMATIZACIÓN VENTILADOR (Calor)
		          // --------------------------------------------------------
		          if (sensorAire.temperatura < TEMP_UMBRAL_BAJA && foco.estadoActual == 0) {
		        	  Actuador_On(&foco);   // Foco ON
		        	  Actuador_On(&LED3);   // Foco ON

		        	  Delay_Escribir(&timerFoco, TIEMPO_FOCO); // Iniciamos cuenta regresiva
		          }

		          // --------------------------------------------------------
		          // G. AUTOMATIZACIÓN BOMBILLA (Frio) - Histéresis
		          // --------------------------------------------------------
		        	if (sensorAire.temperatura > TEMP_UMBRAL_ALTA && ventilador.estadoActual == 0) {
		        	  Actuador_On(&ventilador); // Rela Vent ON
		        	  Actuador_On(&LED2); // Rela Vent ON

		        	  Delay_Escribir(&timerVentilador, TIEMPO_VENTILADOR); // Iniciamos cuenta regresiva
		          }

		      }

		      ETAPA_ACTUAL = ETAPA_AUTO_APAGADO;
		      break;

			  case ETAPA_AUTO_APAGADO:

		      // ============================================================
		            // CASE 4: CHEQUEO DE TIEMPOS Y APAGADO (Seguridad)
		            // ============================================================

		      // Chequeo Bomba
		      if (sensorSuelo.humedadSuelo > UMBRAL_SECO+10.0f && bomba.estadoActual == 1) {
		          if (Delay_Leer(&timerBomba)) {
		        	  Actuador_Off(&bomba); // Relay OFF
		        	  Actuador_Off(&LED1); // Relay OFF

		              Bluetooth_Enviar(&miBluetooth, ";;;Bomba: Auto-Off\n");
		          }
		      }
		      // Chequeo foco
		      if (sensorAire.temperatura > TEMP_UMBRAL_BAJA+5.0f && foco.estadoActual == 1) {
		          if (Delay_Leer(&timerFoco)) {
		        	  Actuador_Off(&foco);
		        	  Actuador_Off(&LED3);

		              Bluetooth_Enviar(&miBluetooth, ";;;Foco: Auto-Off\n");
		          }
		      }
		      // Chequeo Ventilador
		      if (sensorAire.temperatura < TEMP_UMBRAL_ALTA-4.0f && ventilador.estadoActual == 1) {
		          if (Delay_Leer(&timerVentilador)) {
		        	  Actuador_Off(&ventilador); // Relay OFF
		        	  Actuador_Off(&LED2); // Relay OFF

		              Bluetooth_Enviar(&miBluetooth, ";;;Ventilador: Auto-Off\n");
		          }

		      }



		                ETAPA_ACTUAL = ETAPA_BLUETOOTH;
		                break;
		    /* USER CODE BEGIN 3 */
		  }
		  /* USER CODE END 3 */
		}
		}
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

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

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, RELE_BOMBA_Pin|RELE_VENTILADOR_Pin|RELE_FOCO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RELE_BOMBA_Pin RELE_VENTILADOR_Pin RELE_FOCO_Pin */
  GPIO_InitStruct.Pin = RELE_BOMBA_Pin|RELE_VENTILADOR_Pin|RELE_FOCO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
