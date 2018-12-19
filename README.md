# sendHelp
CS 244P IoT Project <br/>
Complete automative device that display your current GPS to Azure cloud when you are in danger based on your heartbeat.

## Description

This is a school project about IoT (Internet of Things).
sendHelp works by collecting your heartbeat (BPM) from the fingertip,
and, when in danger, sends help signal along with your current GPS location to the end user through cloud (Microsoft Azure).

The motive of this project is to create a complete automative process of calling for help during any dangerous situation,
where you are unable to manually ask for help.

The basis of this project is that during a dangerous situation, your heartbeat spikes up;
thus, the sudden rises of heartbeat is used as a cue to create a seamlessly automative process to call for help.
<br/>
<br/>

## Prerequisites

Hardware: <br/>
- Arduino Uno. <br/>
- SparkFun ESP 8266.  <br/>
- Arduino Components: SparkFun Particle Sensor Breakout (MAX30105), Grove LCD RGB Backlight screen, Buzzer v1.2, Button v1.2, SparkFun Mini GPS Shield.

Software:
- Microsoft Azure

## Setup

- SparkFun Particle Sensor Breakout (to measure user's heartbeat) is connect to Arduino Uno through I2C communication <br/>
- Grove LCD RGB Backlight Screen (to display user's current health situation) is connect to Arduino Uno through I2C communication <br/>
- Buzzer v1.2 (to provide auditary alarm in danger) is connect to Arduino Uno through I2C communication <br/>
- Button v1.2 (to manually turn off sendHelp signal by user's choice) is connect to Arduino Uno through I2C communication <br/>
- SparkFun Mini GPS (to provide user's current GPS) is connect to Arduino Uno through UART communication <br/>
- SparkFun ESP 8266 (to send data from Arduino Uno to cloud) is connect to Arduino Uno through UART communication <br/>

Connection Images:

<img src="/images/48372706_337590350408913_3172796451562455040_n.jpg" width="80%">


- Arduino Code: the code is to collect user's heartbeat information, display current user's health status, and send GPS and BPM to ESP 8266 when in danger. <br/>
- ESP8266 Code: the code is to receive data from the Arduino, and send data to the cloud. <br/>
- Azure Code: the code is to display user's BPM and GPS in real time with charts.<br/>


### LCD

LCD displays Four different current status of user.

<img src="/images/48272794_735878023454913_5146507148695437312_n.jpg" width="40%"> <img src="/images/48275514_670422983352852_7548805541305253888_n.jpg" width="40%">

Left: User's current healthy BPM displayed in real time <br/>
Right: User's current BPM along with any related health information, when in danger, in real time <br/>



<img src="/images/48275682_1152752584875685_7666015864533024768_n.jpg" width="40%"> <img src="/images/48359082_208211280060924_8035831476639498240_n.jpg" width="40%">

Left: Displayed when the button is pressed in order to pause the system when the user think it's a false alarm <br/>
Right: Displayed when the user is not using the device. <br/>



 
### Azure

webapp IoT is deployed to present the user's current BPM in real-time in a chart format, along with user's current GPS location

<img src="/images/48337129_278306983038770_9189661873635590144_n.jpg" width="100%">


## Programming Language
C++, Javascript

## Tools/IDE
Arduino IDE, Atom
