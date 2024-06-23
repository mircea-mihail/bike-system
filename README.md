# Bike-System
This repo is made to track my progress developing a complete bike system for my Elops Speed 500 bike from Dechatlon.

## Goal of the project
My goal is to have a bike calculator that tracks your speed, acceleration, turning, breaking and everything else. Then, it uses this information to: turn on turn signals, light up break lights and display relevant information on an E-Ink display.

I also want a nice user interface, with buttons / encoders to control an interactive menu and separate buttons to manually turn on turn signals (Overriding the automatic one).

I'd also like to add automatic charging using the wheel's momentum in order to avoid manually removing and charging the battery.

## Inspiration
This project is mandated by my university as I'm a third year student in an engineering degree. It is also an homage to the thing that got me into arduino and therefore microcontroller programming in the first place: bike-calc, the bike calculator that did indeed turn into a basic prototype but ended up being scrapped. It was my first project and needed a major redesign to meet my current standards, therefore this is bike-calc 2.0, or rather bike-system.  

# Development Information

## E-Ink display

### Documentation Link:
https://files.waveshare.com/upload/e/e5/1.54inch_e-paper_V2_Datasheet.pdf

### Relevant sections:
* page 14: SPI section
* page 17: Temperature Sensor
* page 18: Opearation Flow
* page 20: Command Table

### Screen Requirements:
* full refresh every 180 s
* keep white while not in use
* operating temperatures: 0 - 50 C
* storing temperatures: -25 - 50 C

### Project presentation advice

intro -> poza cu proiectul
overview proiect
de unde a pornit motivatie etc
contributia noastra (ce e nou fata de ce exista deja)
ce nu suntem multumiti
ce am face in plus, unde se poate continua pe viitor, poate vorbesc si de licenta

numerotare slideuri
ultimul slide -> un rezumat, cu ce vrem sa ramana publicul la final