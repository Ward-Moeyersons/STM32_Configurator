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
#define NUM_SSI_TAGS 16
const char * g_ssi_tags[NUM_SSI_TAGS] = {
    "d0_state", "d1_state", "d2_state", "d3_state", "d4_state", "d5_state", 
    "d6_state", "d7_state", "d8_state", "d9_state", "d10_state", "d11_state",
    "d12_state", "d13_state", "d14_state", "d15_state"
};


void cgi_ssi_init(void) {
    http_set_cgi_handlers(g_cgi_handlers, 1);
    http_set_ssi_handler(ssi_handler, g_ssi_tags, NUM_SSI_TAGS);
}

const char * cgi_handler_set_io(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    // Turn off all output pins first
    HAL_GPIO_WritePin(D0_GPIO_Port, D0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_RESET);    
    HAL_GPIO_WritePin(D9_GPIO_Port, D9_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D10_GPIO_Port, D10_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D11_GPIO_Port, D11_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D12_GPIO_Port, D12_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D13_GPIO_Port, D13_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D14_GPIO_Port, D14_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(D15_GPIO_Port, D15_Pin, GPIO_PIN_RESET);

    // Turn on pins that are present in the request
    for (int i = 0; i < iNumParams; i++) {
        if (strcmp(pcParam[i], "D0state") == 0) {
            HAL_GPIO_WritePin(D0_GPIO_Port, D0_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D1state") == 0) {
            HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D2state") == 0) {
            HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D3state") == 0) {
            HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D4state") == 0) {
            HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D5state") == 0) {
            HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D6state") == 0) {
            HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D7state") == 0) {
            HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D8state") == 0) {
            HAL_GPIO_WritePin(D8_GPIO_Port, D8_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D9state") == 0) {
            HAL_GPIO_WritePin(D9_GPIO_Port, D9_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D10state") == 0) {
            HAL_GPIO_WritePin(D10_GPIO_Port, D10_Pin, GPIO_PIN_SET);    
        } else if (strcmp(pcParam[i], "D11state") == 0) {
            HAL_GPIO_WritePin(D11_GPIO_Port, D11_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D12state") == 0) {
            HAL_GPIO_WritePin(D12_GPIO_Port, D12_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D13state") == 0) {
            HAL_GPIO_WritePin(D13_GPIO_Port, D13_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D14state") == 0) {
            HAL_GPIO_WritePin(D14_GPIO_Port, D14_Pin, GPIO_PIN_SET);
        } else if (strcmp(pcParam[i], "D15state") == 0) {
            HAL_GPIO_WritePin(D15_GPIO_Port, D15_Pin, GPIO_PIN_SET);
        }
    }
    return "/control.shtml"; // Redirect to the main page via HTTP 302
}

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
    switch (iIndex) {
        case 0: // d0_state
            if (HAL_GPIO_ReadPin(D0_GPIO_Port, D0_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 1: // d1_state
            if (HAL_GPIO_ReadPin(D1_GPIO_Port, D1_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 2: // d2_state
            if (HAL_GPIO_ReadPin(D2_GPIO_Port, D2_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 3: // d3_state
            if (HAL_GPIO_ReadPin(D3_GPIO_Port, D3_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 4: // d4_state
            if (HAL_GPIO_ReadPin(D4_GPIO_Port, D4_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 5: // d5_state
            if (HAL_GPIO_ReadPin(D5_GPIO_Port, D5_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 6: // d6_state
            if (HAL_GPIO_ReadPin(D6_GPIO_Port, D6_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 7: // d7_state
            if (HAL_GPIO_ReadPin(D7_GPIO_Port, D7_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 8: // d8_state
            if (HAL_GPIO_ReadPin(D8_GPIO_Port, D8_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 9: // d9_state
            if (HAL_GPIO_ReadPin(D9_GPIO_Port, D9_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 10: // d10_state
            if (HAL_GPIO_ReadPin(D10_GPIO_Port, D10_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 11: // d11_state
            if (HAL_GPIO_ReadPin(D11_GPIO_Port, D11_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 12: // d12_state
            if (HAL_GPIO_ReadPin(D12_GPIO_Port, D12_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 13: // d13_state
            if (HAL_GPIO_ReadPin(D13_GPIO_Port, D13_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 14: // d14_state
            if (HAL_GPIO_ReadPin(D14_GPIO_Port, D14_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        case 15: // d15_state
            if (HAL_GPIO_ReadPin(D15_GPIO_Port, D15_Pin) == GPIO_PIN_SET) {
                return snprintf(pcInsert, iInsertLen, "HIGH");
            } else {
                return snprintf(pcInsert, iInsertLen, "LOW");
            }
            break;
        default:
            return snprintf(pcInsert, iInsertLen, "Unknown");
    }
}
