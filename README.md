# Radio Control Module

## Description
Arduino code to translate PPM signal (Pulse Period Modulation) into PWM signal (Pulse Width Modulation) to control DC motors.
It was used to control Hockey Pro robots for Forgers Robotics Group from Sao Paulo Federal University

## Some Features

### Simple Control
It is easy to control the robot combining two channels in a X and Y mode.
The learning process is faster than traditional control modes (tank control)

### Calibration Function
This code makes possible to identify radio control maximum and minimum values to grant the best fit no matter what radio controller is used

### Fail Safe
The fail safe mode prevents run aways if the robot loses radio control signal.
