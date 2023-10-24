# FOTA
Firmware Over the Air (FOTA) system! This system utilizes an ESP8266-01 module to fetch a hex file from a website and then sends it via UART to an STM32F103C8T6 microcontroller. Allow me to walk you through the process:
1️⃣ First, the ESP8266-01 module connects to the internet and accesses a specific website. It retrieves the hex file containing the updated firmware for the STM32 chip.
2️⃣ The ESP8266-01 module communicates with the STM32F103C8T6 microcontroller via UART (Universal Asynchronous Receiver-Transmitter). It sends the received hex file to the STM32F103C8T6.
3️⃣ The STM32F103C8T6 microcontroller checks if there are any updates available by querying the ESP8266-01 module.
4️⃣ If a firmware update is available, the STM32F103C8T6 microcontroller proceeds to receive the new hex file sent by the ESP8266-01 module.
5️⃣ Once the STM32F103C8T6 receives the updated hex file, it runs the new application, integrating the latest firmware into its system.
