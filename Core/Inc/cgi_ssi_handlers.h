/*
 * cgi_ssi_handlers.h
 *
 *  Created on: Jan 4, 2026
 *      Author: Gemini
 */

#ifndef INC_CGI_SSI_HANDLERS_H_
#define INC_CGI_SSI_HANDLERS_H_

#include "lwip/apps/httpd.h"

void cgi_ssi_init(void);
const char * cgi_handler_set_io(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);


#endif /* INC_CGI_SSI_HANDLERS_H_ */
