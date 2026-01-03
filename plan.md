# Plan for Implementing GPIO Control via Web UI

This plan outlines the steps to add functionality for controlling and reading GPIO pins on the STM32 device through the existing web interface.

## 1. Project Analysis & API Definition

*   **Analyze `echo_server.c`:** Investigate the existing request handling logic in `echo_server.c` to understand how HTTP requests are parsed and how responses are generated. This will be the foundation for implementing the new GPIO endpoints.
*   **Analyze `main.c`:** Review `main.c` to understand how GPIOs are currently initialized and used. This will inform how to add the new GPIO control logic.
*   **Define REST API for GPIO:** Propose the following RESTful endpoints for GPIO interaction. These will be added to `README.md`.
    *   **Read GPIO State:** `GET /api/gpio/<pin_name>`
        *   Response: `{"pin": "<pin_name>", "state": "high|low"}`
    *   **Set GPIO State:** `POST /api/gpio`
        *   Request Body: `{"pin": "<pin_name>", "state": "high|low"}`
        *   Response: `{"pin": "<pin_name>", "status": "success|error"}`

## 2. Firmware Implementation (Backend)

*   **Modify Request Handler:** Update the request handling logic in `echo_server.c` to recognize and parse the new `/api/gpio/...` requests. This will involve:
    *   Adding string matching for the new URI patterns.
    *   Parsing the JSON body for `POST` requests using the existing `cJSON` library.
*   **Implement GPIO Control Logic:**
    *   Create new functions, likely within `echo_server.c` or a new dedicated file, to handle the GPIO operations.
        *   A function to read a GPIO pin's state using `HAL_GPIO_ReadPin()` and return it as a string ("high" or "low").
        *   A function to set a GPIO pin's state using `HAL_GPIO_WritePin()` based on the parsed request.
    *   Add comments to all new functions and code blocks explaining their purpose and operation.
*   **Generate JSON Responses:** Use the `cJSON` library to construct JSON response strings for the API endpoints, indicating success, failure, or the current pin state.

## 3. Web UI Implementation (Frontend)

*   **Modify `index.html`:** Update the main web page (`site/html/index.html`) to include UI elements for GPIO control.
    *   Add buttons to set specific GPIO pins HIGH or LOW.
    *   Add status indicators (e.g., text fields or colored icons) to display the current state of GPIO pins.
    *   Add a button to refresh the input pin statuses.
*   **Add JavaScript Logic:**
    *   Write JavaScript functions to handle button clicks.
    *   Implement `fetch` API calls to the new backend endpoints (`GET /api/gpio/...` and `POST /api/gpio`).
    *   Write functions to update the status indicators on the page based on the JSON data returned from the server.
    *   Add comments to the JavaScript code to explain its functionality.

## 4. Build Process

*   **Update Embedded Filesystem:** After modifying the HTML and JavaScript files, run the `site/makefsdata.exe` utility. This will regenerate `Core/Inc/fsdata_custom.c` to include the updated web content in the final firmware binary. A notification will be provided before this step is executed.

## 5. User Notification

*   **GPIO Configuration:** Before writing the code, I will notify you about which GPIO pins need to be configured as inputs or outputs. You will need to configure these in STM32CubeMX and regenerate the project, as I cannot modify the `.ioc` file myself.

I will await your confirmation before proceeding with these steps.
