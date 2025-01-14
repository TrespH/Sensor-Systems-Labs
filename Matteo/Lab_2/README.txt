1- Prepare the basics for the project “Play a song”:
a. Modify the status (switch on / off) of the NUCLEO green LED, every time you snap your fingers.
(Use the pin connected to the microphone as an External Interrupt)
Hint: look at the files “Green PCB board schematic” and “Nucleo Schematic” and “Nucleo user
manual” on Webeep in “Material/Laboratories/Documentation” and find the STM32 pin that
connects to SND_IN.
b. Make the NUCLEO green LED blink at a 1 Hz rate using PWM generation on the corresponding
channel.