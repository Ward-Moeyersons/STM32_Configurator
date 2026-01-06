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
const char * cgi_get_input_state(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

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

// Main handler for setting output pins
const char * cgi_handler_set_io(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    static char redirect_url[256];
    int url_len = snprintf(redirect_url, sizeof(redirect_url), "/control.shtml?");

    for (int i = 0; i < iNumParams; i++) {
        if (pcParam[i][0] == 's' && pcParam[i][1] != '\0') {
            url_len += snprintf(redirect_url + url_len, sizeof(redirect_url) - url_len, "%s=%s&", pcParam[i], pcValue[i]);
            int pin_num = atoi(pcParam[i] + 1);
            if (pin_num >= 0 && pin_num < 16) {
                GPIO_PinState pin_state = (strcmp(pcValue[i], "1") == 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
                HAL_GPIO_WritePin(gpio_pins[pin_num].port, gpio_pins[pin_num].pin, pin_state);
            }
        }
    }

    if (url_len > 0 && (redirect_url[url_len - 1] == '&' || redirect_url[url_len - 1] == '?')) {
        redirect_url[url_len - 1] = '\0';
    } else if (iNumParams == 0) {
         redirect_url[url_len - 1] = '\0';
    }

    return redirect_url;
}

// Handler for getting individual input pin states
const char* cgi_get_input_state(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    // The pin number is derived from the CGI handler's index in the array.
    // Index 0 is /io.cgi, so the input pins start from index 1.
    int pin_num = iIndex - 1;

    if (pin_num >= 0 && pin_num < 16) {
        if (HAL_GPIO_ReadPin(gpio_pins[pin_num].port, gpio_pins[pin_num].pin) == GPIO_PIN_SET) {
            return "/high.html";
        }
    }
    return "/low.html";
}


// CGI handler table
const tCGI g_cgi_handlers[] = {
    // Handler for setting outputs
    {"/io.cgi", cgi_handler_set_io},
    // Handlers for reading each digital input
    {"/inputs/d0.cgi", cgi_get_input_state},
    {"/inputs/d1.cgi", cgi_get_input_state},
    {"/inputs/d2.cgi", cgi_get_input_state},
    {"/inputs/d3.cgi", cgi_get_input_state},
    {"/inputs/d4.cgi", cgi_get_input_state},
    {"/inputs/d5.cgi", cgi_get_input_state},
    {"/inputs/d6.cgi", cgi_get_input_state},
    {"/inputs/d7.cgi", cgi_get_input_state},
    {"/inputs/d8.cgi", cgi_get_input_state},
    {"/inputs/d9.cgi", cgi_get_input_state},
    {"/inputs/d10.cgi", cgi_get_input_state},
    {"/inputs/d11.cgi", cgi_get_input_state},
    {"/inputs/d12.cgi", cgi_get_input_state},
    {"/inputs/d13.cgi", cgi_get_input_state},
    {"/inputs/d14.cgi", cgi_get_input_state},
    {"/inputs/d15.cgi", cgi_get_input_state}
};


void cgi_ssi_init(void) {
    // We now have 1+16 CGI handlers
    http_set_cgi_handlers(g_cgi_handlers, 17);
}
