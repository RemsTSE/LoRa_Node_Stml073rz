/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include <stdio.h>


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LoRa.h"
#include "routing_table.h"
#include "scheduler.h"
#include "payload.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;



/* USER CODE BEGIN PV */
LoRa myLoRa;
uint8_t read_data[128];
uint8_t send_data[128];
int			RSSI;
Transmission transmissions[MAX_TRANSMISSIONS];
Schedule my_schedule;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C2_Init(void);
static void MX_I2C1_Init(void);
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
  MX_SPI1_Init();
  MX_I2C2_Init();
  MX_I2C1_Init();



  /* USER CODE BEGIN 2 */
  	myLoRa = newLoRa();

  	myLoRa.hSPIx                 = &hspi1;
  	myLoRa.CS_port               = NSS_GPIO_Port;
  	myLoRa.CS_pin                = NSS_Pin;
  	myLoRa.reset_port            = RESET_GPIO_Port;
  	myLoRa.reset_pin             = RESET_Pin;
  	myLoRa.DIO0_port						 = DIO0_GPIO_Port;
  	myLoRa.DIO0_pin							 = DIO0_Pin;

  	myLoRa.frequency             = 433;							  // default = 433 MHz
  	myLoRa.spredingFactor        = SF_7;							// default = SF_7
  	myLoRa.bandWidth			       = BW_125KHz;				  // default = BW_125KHz
  	myLoRa.crcRate				       = CR_4_5;						// default = CR_4_5
  	myLoRa.power					       = POWER_20db;				// default = 20db
  	myLoRa.overCurrentProtection = 120; 							// default = 100 mA
  	myLoRa.preamble				       = 10;		  					// default = 8;

  	LoRa_reset(&myLoRa);
  	LoRa_init(&myLoRa);

  	// START CONTINUOUS RECEIVING -----------------------------------
  	LoRa_startReceiving(&myLoRa);
  	//---------------------------------------------------------------

	int num_channels = 3;
	routing_entry_t entries[5];
	entries[0].dest_node_id = 1;
	entries[0].dominant_dest = true;
	entries[0].next_hop_id = 6;
	entries[0].cost = 1;
	entries[0].sf = 7;

	entries[1].dest_node_id = 3;
	entries[1].dominant_dest = false;
	entries[1].next_hop_id = 4;
	entries[1].cost = 2;
	entries[1].sf = 8;

	entries[2].dest_node_id = 2;
	entries[2].dominant_dest = true;
	entries[2].next_hop_id = 6;
	entries[2].cost = 3;
	entries[2].sf = 9;

	entries[3].dest_node_id = 4;
	entries[3].dominant_dest = false;
	entries[3].next_hop_id = 1;
	entries[3].cost = 4;
	entries[3].sf = 10;

	entries[4].dest_node_id = 5;
	entries[4].dominant_dest = true;
	entries[4].next_hop_id = 1;
	entries[4].cost = 5;
	entries[4].sf = 11;

	routing_table_t routing_table;
	routing_table.current_node_id = 6;
	routing_table.num_entries = 5;
	memcpy(routing_table.entries, entries, 5 * sizeof(routing_entry_t));

	// Create the list of transmissions from the routing table
	int num_transmissions;
	Transmission *transmissions = create_transmissions_from_routing_table(
			&routing_table, &num_transmissions);

	// Create the channel list
	int* known_dominants = NULL;
	int num_known_dominant_nodes = 0;
	Channel **channel_list = (Channel **)malloc(num_channels * sizeof(Channel *));
	for(int i = 0; i < num_channels; i++) {
	    channel_list[i] = (Channel *)malloc(sizeof(Channel));
	}
	known_dominants=get_known_dominant_nodes(&routing_table, &num_known_dominant_nodes);
	// Initialization code for channel_list here (or via separate function) ...
	create_channels_list(transmissions, num_transmissions,
	 routing_table.current_node_id, known_dominants, num_known_dominant_nodes,
	 channel_list, &num_channels);
	// Call the scheduling function
	// Initialize scheduled_transmissions
	ScheduledTransmission *scheduled_transmissions = (ScheduledTransmission *)malloc(sizeof(ScheduledTransmission));

	int num_scheduled_transmissions;
	schedule_transmissions(*channel_list, num_channels, &scheduled_transmissions,
			&num_scheduled_transmissions);

	// Output the results
	printf("Scheduled Transmissions:\n");
	for (int i = 0; i < num_scheduled_transmissions; i++) {
		ScheduledTransmission st = scheduled_transmissions[i];
		printf(
				"Channel: %d, Time Slot: %d, Source: %d, Destination: %d, Spreading Factor: %d\n",
				st.channel_index, st.time_slot, st.transmission.source,
				st.transmission.destination, st.transmission.spreading_factor);
	}

	// Calculate and print the efficiency score
	double efficiency_score = calculate_efficiency_score(
			scheduled_transmissions, num_scheduled_transmissions);
	//printf("Efficiency Score:", efficiency_score);

	// Create efficiency score payload
	uint8_t *efficiency_score_payload = create_efficiency_score_payload(efficiency_score);
	if(efficiency_score_payload == NULL) {
	    printf("Failed to create efficiency score payload!\n");
	    // handle error...
	} else {
	    printf("Efficiency score payload created successfully.\n");
	    // use payload...
	    free(efficiency_score_payload); // remember to free the allocated memory when you are done
	}

	// Create a single scheduled transmission payload
	unsigned char single_payload[sizeof(ScheduledTransmissionPayload)];
	create_scheduled_transmission_payload(&scheduled_transmissions[0], single_payload);
	printf("Single transmission payload created successfully.\n");
	// use single_payload...

	// Create multiple scheduled transmissions payload
	unsigned char multiple_payload[num_scheduled_transmissions * sizeof(ScheduledTransmissionPayload)];
	create_scheduled_transmissions_payload(scheduled_transmissions, num_scheduled_transmissions, multiple_payload);
	printf("Multiple transmissions payload created successfully.\n");
	// use multiple_payload...

	// Fragment a payload
	PayloadFragment *fragments;
	int num_fragments;
	fragment_payload(multiple_payload, sizeof(multiple_payload), SCHEDULED_TRANSMISSIONS_PACKET_TYPE, &fragments, &num_fragments);
	if(fragments == NULL) {
	    printf("Failed to fragment payload!\n");
	    // handle error...
	} else {
	    printf("Payload was successfully fragmented into %d fragments.\n", num_fragments);
	    // use fragments...
	    free(fragments); // remember to free the allocated memory when you are done
	}



	// Clean up
	free(transmissions);
	free(scheduled_transmissions);
  	/* USER CODE END 2 */

    /* Infinite loop */

    while (1)
    {
    	/* USER CODE BEGIN WHILE */
  		// SENDING DATA - - - - - - - - - - - - - - - - - - - - - - - - -
  		send_data[0] = 0x3B; // MY ADDRESS
  		for(int i=0; i<26; i++)
  			send_data[i+1] = 48+i;
  		LoRa_transmit(&myLoRa, send_data, 128, 500);
  		HAL_Delay(10);
  		printf("message sent");

  		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
  		HAL_Delay(1500);  // Delay to keep the LED on for a while
  		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
  		// RECEIVING DATA - - - - - - - - - - - - - - - - - - - - - - - -
  		LoRa_receive(&myLoRa, read_data, 128);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.Timing = 0x00707CBB;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
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
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00707CBB;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, NSS_Pin|GPIO_PIN_3, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : NSS_Pin PA3 */
  GPIO_InitStruct.Pin = NSS_Pin|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : USART_TX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
  HAL_GPIO_Init(USART_TX_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == DIO0_Pin){

	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
