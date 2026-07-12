# Portable-Programmable-DC-Power-Supply
A portable version of a DC adjustable bench supply which uses a lithium-ion battery with intuitive user controls and displays on the PCB surface.
<p align="center">
  <img src="Images/project_top.jpg" width="350">
  &nbsp;&nbsp;
  <img src="Images/pcb_top.png" width="600">
</p>

# Project overview
This project was done in the context of Polytechnique Montréal's ELE3000 course which is a third-year personnal project course. The idea was to replicate, with similar specs, an ajustable DC bench supply that you'd find at an electronics workbench. Bench supplies are often heavy and require AC power. With a somewhat solid background in power electronics myself, I knew that an alternative to these supplies was possible using a different approach. I chose to design a portable version, which would consist of a PCB and an battery.

The project consisted of three main design steps :
- Designing of a 4-layer PCB using Altium Designer;
- 3D-modeling a support using SolidWorks which would then be 3D printed;
- Writing the embedded firmware for the ESP32 microcontroller that would operate the hardware;
<p align="center">
  <img src="Images/pcb_bottom.png" width="350">
  &nbsp;&nbsp;
  <img src="Images/assembly.png" width="275">
  &nbsp;&nbsp;
  <img src="Images/Miscellaneous/code_preview.png" width="350">
</p>

This personnal project allowed me to build on my already solid PCB design skills (see my previous [Robot PCB project](https://github.com/justinlalonde/MachinePM---Robot-PCB/tree/main)) and made me realize the balance to be found between miniaturization and functionnality when implementing power electronics. The project ended with a presentation in front of professors and various evaluators. The poster used as visual support for this presentation can be found under [Documentation](https://github.com/justinlalonde/Portable-Programmable-DC-Power-Supply/tree/004b28a5eb4e6ffac3d20e264f36d698339840a9/Documentation) (poster is in French).

# Key features
Here are the key features and specs related to this project :
- 9.6V to 21.0V DC input voltage (3S or 4S lithium-ion batteries);
- 3V to 20V DC output voltage;
- 3A max output current for 60W peak operation;
- Overvoltage, undervoltage, reverse polarity and inrush current input protections ([LM74800](https://www.ti.com/lit/ds/symlink/lm7480-q1.pdf));
- Input on/off switching with rocker-switch;
- 80% global power efficiency at high loads in step-down (buck) operation;
- 90% global power effieciency at high loads in step-up (boost) operation;
- Reverse current, overcurrent and short-circuit output protections ([TPS259470](https://www.ti.com/lit/ds/symlink/tps25947.pdf));
- User-adjustable voltage and current limit targets with potentiometers;
- DC-DC conversion scheme with constant voltage or constant current operation depending on the user programmed current limit and load;
- Output on/off switching with rocker-switch;
- 7-segment displays for voltage, current and power with dedicated LED driver IC's ([LP5024](https://www.ti.com/lit/ds/symlink/lp5024.pdf));
- Red and green LED's indicate normal constant voltage operation and fault-enabled constant current operation;
- System detects and indicates output short-circuits by displaying "S.C." on the blue row of displays;
<p align="center">
  <img src="Images/pcb_picture_bottom.jpg" width="450">
  &nbsp;&nbsp;
  <img src="Images/pcb_perspective.png" width="500">
</p>

# Hardware architecture
The central feature of this project is the DC-DC conversion happening with the [TPS55287](https://www.ti.com/lit/ds/symlink/tps55287.pdf), which is a _4-A Buck-Boost Converter with I2C Interface_ IC from Texas Instruments. This IC converts the input battery voltage to a wide output range using either step-up or step-down operation. This main voltage conversion IC is one amongst the 11 subcircuits which are identified with their schematic sheet names in the following image.
<p align="center">
  <img src="Images/pcb_id.png" width="500">
</p>

**POWER STAGES** :
1. The input voltage from the connected DC power supply (battery) first goes through an input protection / switching stage. This input stage implements undervoltage and overvoltage lockout protections, inrush current limiting and enable the rocker-switch to disable supply to the rest of the board ([LM74800](https://www.ti.com/lit/ds/symlink/lm7480-q1.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1783800356800));
2. The input voltage is then directed to an independant 5V buck converter [LMR51450](https://www.ti.com/lit/ds/symlink/lmr51440.pdf) as well as the main buck/boost converter ([TPS55287](https://www.ti.com/lit/ds/symlink/tps55287.pdf)) which converts it to an arbitrary, user-selected DC voltage target from 3V to 20V.
3. The 5V that is generated from the independant buck converter is multiplexed with the bus voltage of the USBC device port using a power multiplexer IC ([TPS2115](https://www.ti.com/lit/ds/symlink/tps2115.pdf?ts=1783787434649&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FTPS2115)) before being regulated to 3.3V ([TLV76733](https://www.ti.com/lit/ds/symlink/tlv767.pdf?ts=1783784618184)) to power many IC's present on the board, more importantly the ESP32 module itself. This power multiplexing is to ensure that the ESP32 microcontroller can be programmed with only a USBC connexion without needing the presence of the main power source; 
4. On the other hand, the output voltage that is generated from the buck/boost converter then goes through an output protection / switching stage ([TPS259470](https://www.ti.com/lit/ds/symlink/tps25947.pdf)). This stage implements an E-Fuse integrated circuit which has output current limiting, short-circuit protection and detection, undervoltage lockout, overvoltage lockout, reverse current protection. This stage enables the rocker switch on the output side to enable and disable the output supply completely.

**DIGITAL IC's** :
1. Serial communication with the EPS32 requires a USB-To-UART bridge IC ([CP2102N](https://www.silabs.com/documents/public/data-sheets/cp2102n-datasheet.pdf));
2. Two Analog-to-Digital Converter (ADC) IC's are also present on the board to read power rail voltages, the user potentiometer center-taps and some other analog signals generated by various IC's ([ADS1015](https://www.ti.com/lit/ds/symlink/ads1013.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1783787787332&ref_url=https%253A%252F%252Fwww.ti.com%252Fgeneral%252Fdocs%252Fsuppproductinfo.tsp%253FdistId%253D10%2526gotoUrl%253Dhttps%253A%252F%252Fwww.ti.com%252Flit%252Fgpn%252Fads1013));
3. The nine 7-segment displays present on the PCB's top side are driven by three LED driver IC's, each capable of driving 24 discrete LED's ([LP5024](https://www.ti.com/lit/ds/symlink/lp5024.pdf));

The different integrated circuits present on the board communication via I2C. The following image shows the I2C bus as it is routed on the PCB.
<p align="center">
  <img src="Images/I2C_bus.png" width="500">
</p>

# Output fault response
Between the adjustable buck / boost converter IC and the output protection / switching stage IC, many built-in protections are implemented thanks to the two involved integrated circuits. However, during output short-circuit, both the buck / boost converter ([TPS55287](https://www.ti.com/lit/ds/symlink/tps55287.pdf)) and the output E-Fuse IC implement their own built-in protections ([TPS259470](https://www.ti.com/lit/ds/symlink/tps25947.pdf)) to respond to this fault. This can cause issues as the two IC's, placed successive to one another, both try to trigger their own responses. The solution to this was to disable some of the buck-boost converter's protections and program the E-Fuse's protection timing and voltage thresholds appropriatly to ensure compatibility between these two power IC's. 

By placing carefully selected capacitors and resistors on the [TPS259470](https://www.ti.com/lit/ds/symlink/tps25947.pdf)'s pins, timing and voltage protection thresholds were programmed in a way that both IC's would trigger successively depending on the severity of a given overcurrent event instead of simultaneously, wether it be a controlled overcurrent or a short-circuit. The following graph shows which protection from which IC will be triggered and take over depending on the duration and magnitude of an overcurrent event.
<p align="center">
  <img src="Images/Miscellaneous/output_fault_response.png" width="500">
</p>

# Firmware
Just short of 1000 total lines of C code were written to the ESP32 for this project's firmware using the ESP Integrated Development Framework (ESP-IDF), which can easily be set-up with as a Visual Studio Code extension. The source code can be found under [Firmware](https://github.com/justinlalonde/Portable-Programmable-DC-Power-Supply/tree/d0015435451c163994e33e4b49d970070d409dda/Firmware) and the ESP-IDF project can be downloaded with the [ESP-IDF Project.zip](https://github.com/justinlalonde/Portable-Programmable-DC-Power-Supply/tree/main) file. 

Essentially, what the ESP32 does during board execution is run a state machine cycle every 50ms. Each cycle, the ESP32 reads four flags. These flags represent the state of the harware and polled from IC registers via I2C or by checking GPIO state of the ESP32. These flags and the present state of the state machine determine which next state should be selected. The following table shows these four flags and their hardware definitions.
<p align="center">
  <img src="Images/Miscellaneous/firmware_flags.png" width="400">
</p>

The following table shows all states of the state machine and how they relate to each other. Note that a vast part of the hardware and voltage conversion circuitry operates independently from the ESP32 as the microcontroller is mainly there for managing the user potentiometer reads, the 7-segment displays and to give the voltage target to the buck/boost conversion IC. The switching of the input and output supply with the rocker-switches as well as the input and output protection circuitry were designed to be completely independant for hardware integrity purposes.
<p align="center">
  <img src="Images/Miscellaneous/machine_states.png" width="700">
</p>

# Testing
After the PCB assembly and basic functional tests followed a thourough testing phase of the project's thermal performance and energy efficiency. The following screenshot of a thermal camera shows the bottom side of the PCB as it is operating near its maximum output power of 60W (20V output at 3A load).
<p align="center">
  <img src="Images/Miscellaneous/thermal_image.jpg" width="400">
</p>
Furthermore, current and voltage measurements were done using an external DC supply for input and an ajustable electronic load for output. The input current and voltage were compared with the electronic load's current consumption along with the boards output voltage and the two following graphs were drawn, which illustrate the efficiency of the board for votlage step-down (buck) and step-up (boost) operation. The global efficiency represents the real measured power ratio whereas the conversion effeciency substracts the PCB's idle power consumption due to the LED displays and such.

<p align="center">
  <img src="Images/Miscellaneous/buck_efficiency.png" width="450">
  &nbsp;&nbsp;
  <img src="Images/Miscellaneous/boost_efficiency.png" width="475">
</p>

# Documentation 
You can find the full PCB-schematics as well as the project report (report is in French) under [Documentation](https://github.com/justinlalonde/Portable-Programmable-DC-Power-Supply/tree/50b5dae2a2250634cf91fb3bcf46d55140ee1e9c/Documentation). 
<p align="center">
  <img src="Images/Miscellaneous/schematic_preview.png" width="500">
</p>
