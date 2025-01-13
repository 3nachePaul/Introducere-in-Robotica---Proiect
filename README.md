# DIY Safe Box  

---

# Introduction  

**Project Name:** DIY Safe Box 

**Functionality:**  
An interactive safe that allows the user to guess a numeric combination displayed on a screen. Once the correct value is entered, a servo motor unlocks the safe mechanism. RFID cards can also be used for access.  

**Purpose:**  
To develop a simple security device based on knowledge of electronics, programming, and communication protocols.  

**Inspiration:**  
I wanted to make a safe and then I found a game made by someone and I wanted to do the same thing, but on my own. I will attach the video in the Resources paragraph.  

**Usefulness:**  
- **For me:** Practical understanding of protocols and hardware components.  
- **For others:** Inspiration for similar projects, use as an educational device, or as a game.  

---

**Block Diagram:**  

(To be added at a later stage of the project)

| **Modules**            | **Description** |  
|------------------------|-----------------|  
| **LCD** (I2C/SPI)      | Displays the guessed combination and status messages (e.g., "Wrong code!" or "Safe opened"). |  
| **Buttons**            | Allow the user to select and confirm the entered value. |  
| **Servo Motor**        | Controls the locking/unlocking mechanism of the safe. |  
| **Microcontroller (Master)** | Coordinates the operation of components by managing I2C communication, PWM, and interrupts. |  
| **Microcontroller (Slave)** | Handles the RFID reader and communicates the access status to the master via I2C. |  
| **LEDs**               | Indicate correct numbers and their positions during guessing. |  
| **RFID Reader**        | Provides an additional method for safe access by scanning RFID cards. |  
| **Switch**             | Turns the safe on and off, initializing and resetting the game. |  

All components interact through the master microcontroller, which handles the application logic and communication with the slave microcontroller for RFID access validation.

---

# Hardware Design  

## Bill of Materials: 
| **Components**| **Quantity** | **Description** | **Datasheet** | **Source/Link** |
|---------------|--------------|-----------------|---------------|
| Arduino Uno Microcontroller | 2 | The master coordinates the operation of components by managing I2C communication, PWM, and interrupts. The slave handles the RFID reader and communicates the access status to the master via I2C. | | Personal kit and faculty kit. |
| OLED Display SSD1306 | 1 | Displays the guessed combination and status messages. | | [SSD1306](https://www.emag.ro/afisaj-oled-ssd1306-oled-i2c-compatibil-arduino-si-raspberry-pi-27x27x4-mm-albastru-c9/pd/D3C7C1YBM/?ref=history-shopping_405308918_158626_1) | 
| Rotary Encoder with Pushbutton | 1 | It's used to change the digits that are guessed and to submit the answers. It also restarts the game. | | [Rotary Encoder w Pushbutton](https://www.emag.ro/modul-encoder-rotativ-cu-buton-rosfix-360-grade-20-impulsuri-rotire-26x19mm-pzxo-cq39/pd/DYC8PSYBM/?ref=history-shopping_405308918_186146_1) |
| LEDs | 8 | For feedback on guesses as well as a little display of lights. | | Personal kit. |  
| RFID Reader Module RC522 | 1 | It's used as the only option to open the safe withouth guessing the code. | | [RC522](https://www.optimusdigital.ro/en/wireless-rfid/67-mfrc522-rfid-module.html?search_query=rfid&results=30)  
| Servo Motor SG90 | 1| Locks and unlocks the safe. | | Personal kit. |
| 330Ω Resistors| 8 | Makes sure the LEDs are working properly. | | Personal kit. |  
| Jumper Wires | N/A | Connects pins to some component by using a soldering kit. | | [Wires](https://www.optimusdigital.ro/ro/fire-fire-mufate/8731-cablu-12p-125-mm-mufat-la-un-singur-capat-20-cm.html?srsltid=AfmBOoqENSjVWkFWeGit2W4nyUuQcLfRHr1fEtEcQdCk4jS_TgpcTiDA)
| Breadboard | 2 | Used to connect components to a power source and ground. | | Faculty kit.|  
| Power supply 4xAA Battery Support | 1 | Powers source for the components. | | [Battery Support](https://www.optimusdigital.ro/ro/suporturi-de-baterii/12375-suport-baterii-4-x-aa.html?srsltid=AfmBOoofXrcJw2xVfQ2PUKit96xbxR78K6Rq58X9t2aqwo0t1SOdFPyv) 
| Switch | 1 | Turns the device on and off. | | Personal item. |

## Circuit Diagram:  

(To be added later to visually represent the connections between components.)  

**Signal Diagrams:**  
- **I2C Communication:** Between the master and slave microcontrollers.  
- **PWM Signal:** For servo motor control.  
- **SPI Communication:** Between the slave microcontroller and the RFID reader.  

---

# Software Design  

**Description:**  
- **Development Environment:** Arduino IDE.  
- **Libraries Used:**  
  - `Wire.h` for I2C communication.  
  - `SPI.h` for SPI communication.  
  - `MFRC522.h` for RFID reader functionality.  
  - `Servo.h` for servo motor control.  

**Master Microcontroller Responsibilities:**  
1. Displaying status messages and user input on the LCD.  
2. Managing buttons for user input.  
3. Controlling the servo motor using PWM signals.  
4. Communicating with the slave microcontroller via I2C to check RFID access status.  

**Slave Microcontroller Responsibilities:**  
1. Handling RFID card reading using the MFRC522 module.  
2. Validating RFID cards against a predefined list of valid UIDs.  
3. Sending a stop signal to the master microcontroller via I2C when access is granted.  

**Key Features:**  
- **Interrupt Handling:** To detect button presses and manage input actions on the master.  
- **Communication Protocols:**  
  - I2C for master-slave communication.  
  - SPI for RFID reader communication with the slave.  
- **RFID Validation Logic:** To authenticate users with predefined card UIDs.  
- **Servo Control:** Unlocks the safe when the correct combination is entered or RFID access is granted.  

---

## Flow  

1. **Turning the Safe On:**  
   - The switch is turned on, and a message is displayed on the LCD ("CRACK THE CODE!").  
   - The game begins, and the player can start guessing the combination.  

2. **Guessing the Combination:**  
   - The player enters a combination using the buttons.  
   - LEDs indicate the number of correct digits and their positions.  
   - The process continues until the correct combination is entered.  

3. **RFID Access:**  
   - The slave microcontroller reads RFID cards using the MFRC522 module.  
   - Valid cards unlock the safe by sending a stop signal to the master via I2C.  
   - The LCD displays "Access Granted!" when a valid card is scanned.  

4. **Unlocking the Safe:**  
   - If the correct combination is entered or a valid RFID card is scanned, the servo motor unlocks the safe.  
   - The LCD displays "Safe Opened!"  

5. **Safe Turned Off:**  
   - If the switch is turned off, the safe is powered down and the game is reset.  

---

## Resources  

- [Video Inspiration](https://www.youtube.com/watch?v=33jPNMU3N5Q)  
- [Rotary Encoder with pushbutton](https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/)
- [RFID](https://www.youtube.com/watch?v=pdBrvLGH0PE)
- [OLED Display](https://randomnerdtutorials.com/guide-for-oled-display-with-arduino/)
---

## Results  

1. A functional safe box system that integrates multiple components such as LEDs, LCD, servo motor, buttons, and an RFID reader.  
2. Successful implementation of multiple communication protocols:  
   - **I2C:** For communication between master and slave microcontrollers.  
   - **SPI:** For communication between the slave and RFID reader.  
3. Practical application of PWM for servo motor control and interrupts for button handling.  
4. An educational project that applies knowledge from laboratory exercises and demonstrates a real-world use case.  
