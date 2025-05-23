# Bike-System
This repo is made to track my progress developing a complete bike system for my Elops Speed 500 bike from Dechatlon.

## Original goal of the project
My goal is to have a bike calculator that tracks your speed, acceleration, turning, breaking and everything else. Then, it uses this information to: turn on turn signals, light up break lights and display relevant information on an E-Ink display.

I also want a nice user interface, with buttons / encoders to control an interactive menu and separate buttons to manually turn on turn signals (Overriding the automatic one).

I'd also like to add automatic charging using the wheel's momentum in order to avoid manually removing and charging the battery.

## Current state of the project
Although the project ended up a little different compared to the initial plan, it still retained the main shape envisioned in the beggining.
Right now the project is split into 3 main parts: 
* the real time bike calculator, showing information about the current trip
* the data analysis part that involves storing data from every trip and drawing conclusions from it
* the sign detection and behaviour analysis

### Real Time Bike Calculator
The code for this part is C++. It involves reading data from the sensor on the wheel, writing it to an SD Card and regurarly displaying it on the E-Paper screen. I would also say that the casing for the project fits here.

#### Main goal
* it needs to be reliable and fast (solder some connections in for better reliability)
* display real-time trip data in a nice and readable fashion
* the case needs to be sturdy and easy to carry
* the buttons need to be reliable (maybe replace the current buttons)
* add a rechargeable battery and charge it from usb
* design and order a custom PCB and a new 3D printed case for it

### Data Analisys
This is written in Python. This part re-structures the data stored on the SD in trip files and shows some interesting graphs to better visualise some aspects of the trip.

#### Main goal
* organise the collected data
* display said data and draw interesting conclusions
* compute averages 
* observe behaviour and tendecies
* can you classify trips using key features? Interesting classes and features:
	- potential classes: 
		* going to work
		* coming from work
		* going to university
		* going to gym
		* coming back from gym
	- potential features: 
		* length of trip
		* number of stops
		* average speed
		* acceleration spikes
		* deceleration spikes
		* velocity evolution during trip (am I faster in the beginning and slower at the end?)

### Sign Detection and Behaviour analiysis
Detect street signs in real time and store the moment of detection in a csv file. Look at the velocity evolution after seeing key street signs and observe behaviour. This part is a mix between the real time calculator and the data analisys. 

#### Main goal
* mount camera to current case (make an attachment?)
* detect important street signs in real time
* store the moment of detection, how well it maches and the type of sign
* warn the rider if necessary
* observe how speed evolves after sign detection. Does the rider react in any way?

## Inspiration
This project is mandated by my university as I'm a third year student in an engineering degree. It is also an homage to the thing that got me into arduino and therefore microcontroller programming in the first place: bike-calc, the bike calculator that did indeed turn into a basic prototype but ended up being scrapped. It was my first project and needed a major redesign to meet my current standards, therefore this is bike-calc 2.0, or rather bike-system.  

# Development Information

## E-Ink display

### Documentation Link:
https://files.waveshare.com/upload/e/e5/1.54inch_e-paper_V2_Datasheet.pdf

### Screen Requirements:
* full refresh every 180 s
* keep white while not in use
* operating temperatures: 0 - 50 C
* storing temperatures: -25 - 50 C

## Advice for the project

### Advice for the camera and sign detection
as putea sa am un flag in care ma uit daca am ceva rosu care vine inspre mine, sau un marker ca ar putea exista un semn de interes in poza si abia atunci sa incerc sa fac detectia

tabel cu ce optiuni sunt, cat de bine se descurca

esp camera
	posibil underpowered
	ar putea fi good enough

arduino camera
	https://store.arduino.cc/products/nicla-vision

raspberry pi zero camera
	camera mai buna si mai puternica

golden standard
	opencv oak one camera

### Project presentation advice

intro -> show a picture of the project
project overview 
where did everyghing start, motivation
my contribution, did I come up with something new 
what would i change, what i don't like about the project
what would i add, would i like to continue developing the projcet?
numerotare slideuri
last slide -> a short summary, something to stick to the viewer

### Plans for near future

* design PCB
* order from jlcpcb 
* order ESP32-cam and lead free solder
* put together a pcb and working calculator
* put together sign detection module 
* link them up using battery
* establish comms using serial or esp-now for less battery
