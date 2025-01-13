# DIY Safe Box  

---

## Introduction  

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

## Hardware Design  

**Components List:**  
- 2 Microcontrollers (e.g., Arduino Uno for master and slave roles)  
- LCD Display (I2C or SPI communication)  
- Buttons (increment/confirm)  
- LEDs (for feedback on guesses)  
- RFID Reader Module (e.g., MFRC522)  
- Servo Motor (e.g., SG90)  
- Resistors  
- Jumper wires  
- Breadboard or PCB for circuit assembly  
- Power supply (e.g., battery or USB)  
- Switch (for turning the safe on and off)  

**Circuit Diagram:**  

(To be added later to visually represent the connections between components.)  

**Signal Diagrams:**  
- **I2C Communication:** Between the master and slave microcontrollers.  
- **PWM Signal:** For servo motor control.  
- **SPI Communication:** Between the slave microcontroller and the RFID reader.  

---

## Software Design  

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
   - The switch is turned on, and a message is displayed on the LCD ("Welcome to the Safe Box!").  
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

---

## Results  

1. A functional safe box system that integrates multiple components such as LEDs, LCD, servo motor, buttons, and an RFID reader.  
2. Successful implementation of multiple communication protocols:  
   - **I2C:** For communication between master and slave microcontrollers.  
   - **SPI:** For communication between the slave and RFID reader.  
3. Practical application of PWM for servo motor control and interrupts for button handling.  
4. An educational project that applies knowledge from laboratory exercises and demonstrates a real-world use case.  
