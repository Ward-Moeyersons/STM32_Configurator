# STM32 GPIO REST API

This project implements a REST API on an STM32F746G microcontroller to control GPIO pins.

## API Endpoints

### Get GPIO Status

- **URL:** `/api/gpio`
- **Method:** `GET`
- **Description:** Retrieves the current status of all configured GPIO pins.
- **Response:**
  ```json
  [
    {
      "port": 0,
      "label": "USER_LED_1",
      "mode": "output",
      "value": 1,
      "isEnabled": true
    }
  ]
  ```

### Set GPIO Status

- **URL:** `/api/gpio`
- **Method:** `POST`
- **Description:** Sets the value and/or mode of one or more GPIO pins.
- **Request Body:**
  ```json
  [
    {
      "port": 0,
      "mode": "output",
      "value": 0
    }
  ]
  ```
- **Response:**
  - `200 OK` on success.
  - `400 Bad Request` if the request is malformed or violates constraints.

## Constraints

The following constraints can be applied to each GPIO pin:

- `port`: The GPIO port number (e.g., 0).
- `mode`: `input` or `output`.
- `value`: For output mode, `0` or `1`. For input mode, this field is read-only.
- `label`: A string to identify the GPIO pin.
- `isEnabled`: `true` or `false`.

Constraints are defined in the `gpios` array in `Core/Src/main.c`.
