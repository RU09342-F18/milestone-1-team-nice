# Milestone 1: Communicating with Will Byers
## Details
- Created by Scott Wood and David Sheppard
- Created: October 8, 2018
- Last Updated: October 20, 2018 (added detail about the testing of the program and RealTerm, Explain Software PWM)
- Designed for the MSP430G2553 Microprocessor
## Purpose
This program allows the user to control an RGB LED using UART communication. The user is able to set the R, G, and B values using 0x00 through 0xFF, where 0x00 is off and 0xFF is fully on. The implementation allows for the MSP430 to be linked to other devices in a chain.
## Use
The microporcessor begins by recieving a specific number of bits through UART. The first bit represents the total number of bits being sent (not including itself). The microprocessor accepts the 2nd, 3rd, and 4th bytes as the R, G, and B values and then sends on the updated size byte along with the remaining bytes out through the TX line.
## Implementation
### LED
The dimness of each LED is set using PWM in the software. The first value (size byte) is recorded and sent to the next user after subtracting 3. The next 3 values are taken in and set as the R, G, and B values for the light. All remaining values are then passed on to the next device through the TX line. 
### Software PWM
The decision to implement the PWM through software was a result of trial and error with hardware PWM, which concluded in the realization...NEED TO FURTHER EXPLAIN
### Input Values
Any byte-sized value is accepted. This is a range of 0x00 to 0xFF, where 0xFF represents maximum brightness and 0x00 is off. Anything in between gives partial brightness. After sending through all of the data according to the given size, the code can now accept new input for different RGB values.
### Functionality
The functionality of this program was verified with the utilzation of the serial terminal program: RealTerm. Upon entering the following unique string of bytes into the terminal: 0x03 0xFF 0xFF 0xFF. As mentioned above, 0x03 represents the total number byte length; as a result, three more corresponding bytes are sent, which all modify the PWM for all LEDS to maximum duty cycle, hence maximum brightness, and the color white is achieved, and the UART function is verified since 0x00 is returned on the terminal display.

## Notes
- This can be implemented on a breadboard using a separate RGB LED connected to P2.1, 2.3, and 2.5 on the microprocessor.
- The code is dependenty upon the MSP430G2553.h header file.
