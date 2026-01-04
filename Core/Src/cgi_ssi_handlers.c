/*
 * cgi_ssi_handlers.c
 *
 *  Created on: Jan 4, 2026
 *      Author: Gemini
 */

#include "main.h"
#include "lwip/apps/httpd.h"
#include "cgi_ssi_handlers.h"
#include <string.h>
#include <stdio.h>

// Forward declarations
const char * cgi_handler_set_io(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen);

// CGI handler for setting GPIOs
const tCGI g_cgi_handlers[] = {
    {
        "/io.cgi",
        cgi_handler_set_io
    }
};

// SSI handler
#define NUM_SSI_TAGS 6
const char * g_ssi_tags[NUM_SSI_TAGS] = {
    "d4_state", "d5_state", "d6_state", "d7_state", "d8_state", "d9_state"
};


void cgi_ssi_init(void) {
    http_set_cgi_handlers(g_cgi_handlers, 1);
    http_set_ssi_handler(ssi_handler, g_ssi_tags, NUM_SSI_TAGS);
}

const char * cgi_handler_set_io(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    // Turn off all output pins first
    HAL_GPIO_WritePin(D10_GPIO_Port, D10_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D11_GPIO_Port, D11_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D12_GPIO_Port, D12_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D13_GPIO_Port, D13_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D14_GPIO_Port, D14_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D15_GPIO_Port, D15_Pin, GPIO_PIN_RESET);

    // Turn on pins that are present in the request
    for (int i = 0; i < iNumParams; i++) {
        if (strcmp(pcParam[i], "D10") == 0) {
            HAL_GPIO_WritePin(D10_GPIO_Port, D10_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D11") == 0) {
            HAL_GPIO_WritePin(D11_GPIO_Port, D11_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D12") == 0) {
            HAL_GPIO_WritePin(D12_GPIO_Port, D12_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D13") == 0) {
            HAL_GPIO_WritePin(D13_GPIO_Port, D13_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D14") == 0) {
            HAL_GPIO_WritePin(D14_GPIO_Port, D14_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D15") == 0) {
            HAL_GPIO_WritePin(D15_GPIO_Port, D15_Pin, GPIO_PIN_SET);
        }
    }
    return "/index.shtml"; // Redirect to the main page via HTTP 302
}

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
    // D4-D9 are inputs
    switch (iIndex) {
        case 0: // d4_state
            if (HAL_GPIO_ReadPin(D4_GPIO_Port, D4_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 1: // d5_state
            if (HAL_GPIO_ReadPin(D5_GPIO_Port, D5_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 2: // d6_state
            if (HAL_GPIO_ReadPin(D6_GPIO_Port, D6_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 3: // d7_state
            if (HAL_GPIO_ReadPin(D7_GPIO_Port, D7_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 4: // d8_state
            if (HAL_GPIO_ReadPin(D8_GPIO_Port, D8_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 5: // d9_state
            if (HAL_GPIO_ReadPin(D9_GPIO_Port, D9_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        default:
            return snprintf(pcInsert, iInsertLen, "Unknown");
    }
}
