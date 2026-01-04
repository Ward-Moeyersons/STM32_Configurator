# STM32 GPIO Control with LwIP, CGI, and SSI

**VERY IMPORTANT: After making any changes to the web interface files in the `site/html` directory, you MUST run the `compile.bat` script located in the `site` directory. This script converts the HTML and CSS files into a C source file (`fsdata_custom.c`) that is compiled into the firmware. If you do not run this script, your changes will not be reflected on the web page.**

This project demonstrates how to control and monitor GPIO pins on an STM32F746G microcontroller using a web interface served by the LwIP TCP/IP stack. It uses CGI (Common Gateway Interface) for handling POST requests to control output pins and SSI (Server-Side Includes) to dynamically display the status of input pins on the web page.

## Functionality

- **Input Pins (D4-D9):** The state of these pins (HIGH or LOW) is displayed on the main web page.
- **Output Pins (D10-D15):** These pins can be turned ON or OFF using checkboxes on the web page.

## Web Interface

The web interface is served from the device at its configured IP address (default: `192.168.1.100`).

- **index.html:** The main page that displays input states and provides controls for output pins.

### Input Status

The status of pins D4 through D9 is displayed in a table. The status is either "HIGH" or "LOW", reflecting the current voltage level at the pin.

### Output Control

A form on the page allows you to control pins D10 through D15.
- **Check the box** for a corresponding pin to set its output to HIGH (3.3V).
- **Uncheck the box** to set its output to LOW (0V).
- Click the "Set Outputs" button to send the new configuration to the device. The page will reload with the updates.

## Technical Implementation

### CGI (Common Gateway Interface)

- A CGI handler is implemented to process POST requests from the web page.
- The handler is registered for the path `/io.cgi`.
- When the form is submitted, the browser sends a POST request to `/io.cgi`. The request body contains the state of the checkboxes for the output pins.
- The CGI handler parses this data and calls the appropriate STM32 HAL functions (`HAL_GPIO_WritePin`) to set the output pins.
- After processing, the handler redirects the browser back to `index.html`.

### SSI (Server-Side Includes)

- The main web page (`index.html`) is parsed by the LwIP `httpd` server for SSI tags.
- SSI tags (e.g., `<!--#d4_state-->`) are used as placeholders for the input pin statuses.
- An SSI handler is implemented and registered with the `httpd` server.
- When `httpd` encounters an SSI tag while serving the page, it calls the SSI handler.
- The handler reads the corresponding GPIO input pin's state (`HAL_GPIO_ReadPin`) and returns the string "HIGH" or "LOW".
- This string replaces the SSI tag in the final HTML sent to the browser.

### Building the Web Assets

The web interface files (HTML, CSS) are located in the `site/html` directory. They must be converted into a virtual filesystem C source file (`fsdata_custom.c`) that gets compiled into the firmware.

To do this, run the `compile.bat` script in the `site` directory. This script uses the `makefsdata.exe` tool to generate the `fsdata_custom.c` file in the `Core/Inc` directory. This step is **mandatory** for any web file changes to take effect.