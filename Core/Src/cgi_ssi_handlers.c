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
#include <stdlib.h>

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
    "d0state", "d1state", "d2state", "d3state", "d4state", "d5state", 
    "d6state", "d7state", "d8state", "d9state", "d10state", "d11state",
    "d12state", "d13state", "d14state", "d15state"
};

// Helper struct and array to map pin numbers to GPIO ports and pins
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GpioPinMapping;

const GpioPinMapping gpio_pins[16] = {
    {D0_GPIO_Port, D0_Pin}, {D1_GPIO_Port, D1_Pin}, {D2_GPIO_Port, D2_Pin}, {D3_GPIO_Port, D3_Pin},
    {D4_GPIO_Port, D4_Pin}, {D5_GPIO_Port, D5_Pin}, {D6_GPIO_Port, D6_Pin}, {D7_GPIO_Port, D7_Pin},
    {D8_GPIO_Port, D8_Pin}, {D9_GPIO_Port, D9_Pin}, {D10_GPIO_Port, D10_Pin}, {D11_GPIO_Port, D11_Pin},
    {D12_GPIO_Port, D12_Pin}, {D13_GPIO_Port, D13_Pin}, {D14_GPIO_Port, D14_Pin}, {D15_GPIO_Port, D15_Pin}
};

void cgi_ssi_init(void) {
    http_set_cgi_handlers(g_cgi_handlers, 1);
    http_set_ssi_handler(ssi_handler, g_ssi_tags, NUM_SSI_TAGS);
}

const char * cgi_handler_set_io(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    static char redirect_url[256]; // Static buffer for the redirect URL. 256 is plenty now.

    // Start building the redirect URL
    int url_len = snprintf(redirect_url, sizeof(redirect_url), "/control.shtml?");

    for (int i = 0; i < iNumParams; i++) {
        // Only process state parameters 's'
        if (pcParam[i][0] == 's' && pcParam[i][1] != '\0') {
            // Append current parameter to the redirect URL
            url_len += snprintf(redirect_url + url_len, sizeof(redirect_url) - url_len, "%s=%s&", pcParam[i], pcValue[i]);

            int pin_num = atoi(pcParam[i] + 1);
            if (pin_num >= 0 && pin_num < 16) {
                GPIO_PinState pin_state = (strcmp(pcValue[i], "1") == 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
                HAL_GPIO_WritePin(gpio_pins[pin_num].port, gpio_pins[pin_num].pin, pin_state);
            }
        }
    }

    // Remove trailing '&' or '?'
    if (url_len > 0 && (redirect_url[url_len - 1] == '&' || redirect_url[url_len - 1] == '?')) {
        redirect_url[url_len - 1] = '\0';
    } else if (iNumParams == 0) {
        // If there were no parameters, remove the '?' to have a clean URL
         redirect_url[url_len - 1] = '\0';
    }

    return redirect_url;
}

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
    if (iIndex >= 0 && iIndex < 16) {
        if (HAL_GPIO_ReadPin(gpio_pins[iIndex].port, gpio_pins[iIndex].pin) == GPIO_PIN_SET) {
            return snprintf(pcInsert, iInsertLen, "HIGH");
        } else {
            return snprintf(pcInsert, iInsertLen, "LOW");
        }
    }
    return snprintf(pcInsert, iInsertLen, "Unknown");
}