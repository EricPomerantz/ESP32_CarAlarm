# ESP32 Automotive Alarm/Immobilizer System

### By: Eric Pomerantz  

## Objective

The main objective of this project was to emulate an automotive security system using an ESP32. The system replicates features found in typical vehicle security systems. Primary goals achieved include:

- **Remote Keyless Entry Locking System**: Lock and unlock doors using a remote.
- **Passkey System**: Allows authorized users to start the vehicle while preventing unauthorized startup.
- **Alarm System**: Alerts users of unauthorized startup attempts.

---

## Features

### 1. **Remote Door Lock Operation**
**Functionality**:  
When a button on the remote is pressed, the RF receiver sends a signal to the ESP32, triggering a servo to lock or unlock the vehicle doors.  
- **Button A**: Locks the vehicle (indicated by a red LED).  
- **Button B**: Unlocks the vehicle (indicated by a green LED).  

**Parts Used**:  
- RF transmitter and receiver ([eBay Link](https://www.ebay.com/itm/324520491119))  
- Micro servo  
- Red and Green LEDs  

**Methods**:  
- Button presses are detected in the main loop by monitoring high signals from the receiver.  
- Lock status is stored outside the loop to retain state.  
- Servo moves to lock/unlock positions based on button presses.

---

### 2. **Start Authorization**
**Functionality**:  
A PIN pad verifies authorized users.  
- **Correct PIN**: Authorizes vehicle startup (yellow LED and a buzzer sound indicate success).  
- **Incorrect PIN**: Triggers a buzzer sound for error.  
- Locking can be reset by pressing the Lock button or entering the wrong PIN.  

**Parts Used**:  
- 4x4 keypad  
- 5V electric motor (simulated engine starter)  
- Momentary push button (start button)  
- NPN transistor (PN2222A) with a flyback diode (1N4007)  
- External power module with a 9V battery

**Methods**:  
- PINs are validated within the main loop.  
- Correct PIN toggles the `startEnable` variable, lights the yellow LED, and plays the correct sound.  
- Incorrect PIN triggers an error sound, and the motor remains disabled.

---

### 3. **Alarm System**
**Functionality**:  
An alarm is triggered when an unauthorized startup attempt is made (e.g., the engine start button is pressed without a correct PIN).  
- **Deactivate Alarm**: Press and hold the Unlock button on the remote.  

**Parts Used**:  
- Passive buzzer  
- Momentary push button  

**Methods**:  
- If the start button is pressed while the `startEnable` variable is false, the alarm function activates.  
- The alarm continues until the Unlock button signal is detected.

---

## Final Demonstration
This project successfully demonstrates all functions, including remote locking/unlocking, PIN-based start authorization, and an alarm for unauthorized access attempts.

[![ESP32 Alarm Demo](https://img.youtube.com/vi/4V7zZ_QluNI/0.jpg)](https://youtu.be/4V7zZ_QluNI)

