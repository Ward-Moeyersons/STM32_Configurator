/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "echo_server.h"
#include "lwipopts.h"
#include "httpd_opts.h"


#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/apps/httpd.h"
#include "lwip/apps/fs.h"
#include "lwip/opt.h"
#include <string.h>
#include "cJSON.h"
#include <stdio.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    int port;
    char* label;
    char* mode;
    int value;
    int isEnabled;
	GPIO_TypeDef* gpio_port;
	uint16_t gpio_pin;
} gpio_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NUM_GPIOS 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
typedef unsigned short u16_t;
typedef unsigned char  u8_t;

gpio_t gpios[NUM_GPIOS] = {
    {
        .port = 0,
        .label = "USER_LED_1",
        .mode = "output",
        .value = 0,
        .isEnabled = 1,
		.gpio_port = D13_GPIO_Port,
		.gpio_pin = D13_Pin
    }
};

char *post_data_buffer = NULL;
int post_data_len = 0;
int post_data_offset = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len) {
	for(int i = 0; i < len; i++){
		if(ptr[i]=='\n'){
			HAL_UART_Transmit(&huart1, (uint8_t*)"\r", 1, HAL_MAX_DELAY);
		}
		HAL_UART_Transmit(&huart1, (uint8_t*)&ptr[i], 1, HAL_MAX_DELAY);
	}
    return len;
}

void httpd_cgi_handler(struct fs_file *file, const char* uri, int iNumParams,
                              char **pcParam, char **pcValue )
{
	 if (strcmp(uri, "/api/gpio") == 0) {
		 cJSON *root = cJSON_CreateArray();
		 for (int i = 0; i < NUM_GPIOS; i++) {
			 cJSON *gpio_json = cJSON_CreateObject();
			 cJSON_AddNumberToObject(gpio_json, "port", gpios[i].port);
			 cJSON_AddStringToObject(gpio_json, "label", gpios[i].label);
			 cJSON_AddStringToObject(gpio_json, "mode", gpios[i].mode);
			 gpios[i].value = HAL_GPIO_ReadPin(gpios[i].gpio_port, gpios[i].gpio_pin);
			 cJSON_AddNumberToObject(gpio_json, "value", gpios[i].value);
			 cJSON_AddBoolToObject(gpio_json, "isEnabled", gpios[i].isEnabled);
			 cJSON_AddItemToArray(root, gpio_json);
		 }
		 char *json_string = cJSON_PrintUnformatted(root);
		 cJSON_Delete(root);

		 file->data = json_string;
		 file->len = strlen(json_string);
		 file->pextension = NULL; 
	 }
}


 err_t httpd_post_begin(void *connection, const char *uri,
                        const char *http_request, u16_t http_request_len,
                        int content_len, char *response_uri,
                        u16_t response_uri_len, u8_t *post_auto_wnd)
 {
	 printf("POST uri=%s len=%d\n", uri, content_len);


     LWIP_UNUSED_ARG(connection);
     LWIP_UNUSED_ARG(http_request);
     LWIP_UNUSED_ARG(http_request_len);
     LWIP_UNUSED_ARG(post_auto_wnd);

	 if (strcmp(uri, "/api/gpio") == 0) {
		 if (post_data_buffer != NULL) {
			 free(post_data_buffer);
			 post_data_buffer = NULL;
			 post_data_len = 0;
		 }
		 post_data_offset = 0;

		 if (content_len > 0) {
			 post_data_buffer = (char*)malloc(content_len + 1);
			 if (post_data_buffer == NULL) {
				 return ERR_MEM;
			 }
			 post_data_len = content_len;
		 }
	 }

     strncpy(response_uri, "/index.html", response_uri_len);

     return ERR_OK;
 }

 err_t httpd_post_receive_data(void *connection, struct pbuf *p)
 {
     LWIP_UNUSED_ARG(connection);


     struct pbuf *q = p;

        while (q != NULL)
        {
            if (post_data_offset + q->len <= post_data_len) {
            	memcpy(post_data_buffer + post_data_offset, q->payload, q->len);
            	post_data_offset += q->len;
            }

            q = q->next;
        }
        if (post_data_buffer != NULL) {
        	post_data_buffer[post_data_offset] = '\0';
        }


     pbuf_free(p);

     return ERR_OK;
 }


 void httpd_post_finished(void *connection, char *response_uri,
                           u16_t response_uri_len)
 {
     LWIP_UNUSED_ARG(connection);

     if (post_data_buffer != NULL)
     {
         printf("POST data received: %s\n", post_data_buffer);

         cJSON *root = cJSON_Parse(post_data_buffer);
         if (root && cJSON_IsArray(root))
         {
             cJSON *gpio_json;
             cJSON_ArrayForEach(gpio_json, root)
             {
                 cJSON *port_json = cJSON_GetObjectItem(gpio_json, "port");
                 if (!cJSON_IsNumber(port_json))
                     continue;

                 int port = port_json->valueint;
                 if (port < 0 || port >= NUM_GPIOS)
                     continue;

                 /* -------- mode -------- */
                 cJSON *mode_json = cJSON_GetObjectItem(gpio_json, "mode");
                 if (cJSON_IsString(mode_json) && mode_json->valuestring)
                 {
                     strncpy(gpios[port].mode,
                             mode_json->valuestring,
                             sizeof(gpios[port].mode) - 1);
                     gpios[port].mode[sizeof(gpios[port].mode) - 1] = '\0';
                 }

                 /* -------- isEnabled (BOOLEAN!) -------- */
                 cJSON *enabled_json = cJSON_GetObjectItem(gpio_json, "isEnabled");
                 if (cJSON_IsBool(enabled_json))
                 {
                     gpios[port].isEnabled = cJSON_IsTrue(enabled_json);
                 }

                 /* -------- value -------- */
                 cJSON *value_json = cJSON_GetObjectItem(gpio_json, "value");
                 if (cJSON_IsNumber(value_json))
                 {
                     gpios[port].value = value_json->valueint ? 1 : 0;
                 }

                 /* -------- GPIO OUTPUT -------- */
                 if (strcmp(gpios[port].mode, "output") == 0)
                 {
                     if (gpios[port].isEnabled)
                     {
                         HAL_GPIO_WritePin(
                             gpios[port].gpio_port,
                             gpios[port].gpio_pin,
                             gpios[port].value ? GPIO_PIN_SET : GPIO_PIN_RESET
                         );
                     }
                     else
                     {
                         /* Forceer UIT wanneer disabled */
                         HAL_GPIO_WritePin(
                             gpios[port].gpio_port,
                             gpios[port].gpio_pin,
                             GPIO_PIN_RESET
                         );
                     }
                 }
             }
         }

         cJSON_Delete(root);

         free(post_data_buffer);
         post_data_buffer = NULL;
         post_data_len = 0;
     }

     strncpy(response_uri, "/index.html", response_uri_len);
 }




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
  MX_USART1_UART_Init();
  MX_LWIP_Init();
  /* USER CODE BEGIN 2 */
  echo_init();
  httpd_init();
  printf("Server stared\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  MX_LWIP_Process();
	  //TODO: Write code to call the tcp_connect function each time the blue button is pressed.

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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 210;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, D15_Pin|D3_Pin|D14_Pin|D12_Pin
                          |D11_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, D9_Pin|D10_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, D7_Pin|D8_Pin|D13_Pin|D5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, D0_Pin|D1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, D4_Pin|D2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : D15_Pin D3_Pin D14_Pin D12_Pin
                           D11_Pin */
  GPIO_InitStruct.Pin = D15_Pin|D3_Pin|D14_Pin|D12_Pin
                          |D11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : D9_Pin D10_Pin */
  GPIO_InitStruct.Pin = D9_Pin|D10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : D7_Pin D8_Pin D13_Pin D5_Pin */
  GPIO_InitStruct.Pin = D7_Pin|D8_Pin|D13_Pin|D5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_Pin */
  GPIO_InitStruct.Pin = BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : D0_Pin D1_Pin */
  GPIO_InitStruct.Pin = D0_Pin|D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : D4_Pin D2_Pin */
  GPIO_InitStruct.Pin = D4_Pin|D2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : A4_Pin A5_Pin A1_Pin A2_Pin
                           A3_Pin */
  GPIO_InitStruct.Pin = A4_Pin|A5_Pin|A1_Pin|A2_Pin
                          |A3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : A0_Pin */
  GPIO_InitStruct.Pin = A0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(A0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : D6_Pin */
  GPIO_InitStruct.Pin = D6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(D6_GPIO_Port, &GPIO_InitStruct);

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
#ifdef USE_FULL_ASSERT
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
