# Milestone 1: Communicating with Will Byers
## Details
- Created by Scott Wood and David Sheppard
- October 17 2018
- Designed for the MSP430G2553 Microprocessor
## Purpose
This program allows the user to control an RGB LED using UART communication. The user is able to set the R, G, and B values using 0x00 through 0xFF, where 0x00 is off and 0xFF is fully on. The implementation allows for the MSP430 to be linked to other devices in a chain.
## Use
The microporcessor begins by recieving a specific number of bits. The first bit represents the total number of bits being sent (not including itself). The microprocessor accepts the 2nd, 3rd, and 4th bytes as the R, G, and B values sends on the updated size byte along with the remaining bytes out through the TX line.
## Implementation
## Notes
