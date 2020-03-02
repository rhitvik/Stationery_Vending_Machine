# Stationery Vending Machine

## About the project
The Stationery Vending Machine is a wall-mountable vending machine that was prototyped to test the abilities of the vending machines to sell stationery products. After the first few systems test, we were able to establish that there is an existent market with enormous potential in this field and approaching it with proper methods can give rise to a lucrative business model. To further validate this, 3 of these machines were deployed in SKIT, GIT and JECRC University. Beyond our expectations, 3 of these machines sold more than 16,000 units of stationery in 4 months.

These machines dispensed the stationery product when the user inserted money into it. This was the machine the venture revolved around which then culminated to take the form of Cashless form of vending machines or so what came to be known as the [Smart Vending Machine](https://github.com/rhitvik/Smart_Vending_Machine)

## How to purchase from the machine?
A user can simply go to the machine and insert a ₹10 coin in the slot and wait till the product is dispensed. These models were designed to sell A4-Paper (Pack of 20) that university students used for assignment. Should there be any complication in the transaction process like machine runs out of product or one or more subsystems stops functioning properly (as these machines were exposed to the heated environments of Rajasthan and temperature often crossed 120°F), this machine will not only display the transactional status in reatime but will also return the money to the user.

## System
This was a single 8-Bit microcontroller based system that communicated with the Coin Acceptor, read from the sensors and controlled the motor drivers. An LCD was further employed to provide realtime feedback during the transaction or otherwise. Linear regulators were used to feeding power to the microcontroller and the logic assembly of the sensors and motor drivers. If powered by a 12 Volt SMPS rated less than 15 W, the Brown Out Reset Fuse settings need to be tweaked. Otherwise, the system will intermittently restart when the motor starts due to the heavy current draw that drives the supply voltage below the BOR levels. Also, disable JTAG functionality on PORT-C and setting the clock on 16MHz (External Crystal) full-swing oscillaton mode is highly recommended.

## Code
[Stationery Vending Machine](https://github.com/rhitvik/Stationery_Vending_Machine/blob/master/Codes/coinAcceptor-innterruptProgram/coinAcceptor-innterruptProgram/coinAcceptor-innterruptProgram.cpp)

## Simulation File 
The project was developed on Proteus-7 Design Suite and the Simulation File can be found [Here](https://github.com/rhitvik/Stationery_Vending_Machine/tree/master/SimluationFile)

## Schematics and PCB layout

### Simulation Circuit
![](https://github.com/rhitvik/Stationery_Vending_Machine/blob/master/Images_and_Screenshots/Simulation_circuit.JPG)

### PCB Layout
![](https://github.com/rhitvik/Stationery_Vending_Machine/blob/master/Images_and_Screenshots/PCB_Layout.JPG)

## Some additional Pictures

### Soldered Circuit and Fabricated PCB
![](https://github.com/rhitvik/Stationery_Vending_Machine/blob/master/Images_and_Screenshots/PSX_20190105_225800.jpg)

### Stationery Vending Machine (GIT College) 
![](https://github.com/rhitvik/Stationery_Vending_Machine/blob/master/Images_and_Screenshots/Vending_Machine_GIT_Jaipur.jpg)


