/*
 * MotionControl.c
 *
 *  Created on: Apr 13, 2016
 *      Author: wiblack
 */




#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f0xx.h"
#include <string.h>
#include "MotionControl.h"

//this is the global variable that keeps track of the motor position.
int MOTOR_POSITION;

//these are a bunch of constants that are used to define motor position.
//we were using a semi stepper motor driver.
//static const uint16_t NORTH =0;
//static const uint16_t NORTH_EAST=200;
//static const uint16_t EAST =400;
//static const uint16_t SOUTH_EAST= 600;
//static const uint16_t SOUTH= 800;
//static const uint16_t SOUTH_WEST=1000;
//static const uint16_t WEST=1200;
//static const uint16_t NORTH_WEST=1400;
static const uint16_t NORTH =0;
static const uint16_t NORTH_EAST=1600;
static const uint16_t EAST =3200;
static const uint16_t SOUTH_EAST= 4800;
static const uint16_t SOUTH= 6400;
static const uint16_t SOUTH_WEST=8000;
static const uint16_t WEST=9600;
static const uint16_t NORTH_WEST=11200;
//MOTOR_POSITIONS;
//0:North;
//200: North East
//400: East
//600: South East
//800: South
//1000: South West
//1200: West
//1400: North West

void motion_init(){
	// initialize all of the motion controls
	MOTOR_POSITION=0;

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->APB2ENR|= RCC_APB2ENR_SYSCFGCOMPEN;
	//GPIOB->MODER |=
	GPIOB ->MODER &= ~(GPIO_MODER_MODER4|GPIO_MODER_MODER5|GPIO_MODER_MODER6|GPIO_MODER_MODER7 );//Put all of the motions sensors
  // GPIOB->MODER |=(GPIO_MODER_MODER4_1 |GPIO_MODER_MODER5_1 |GPIO_MODER_MODER6_1|GPIO_MODER_MODER7_1  );
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR4_1|GPIO_PUPDR_PUPDR5_1|GPIO_PUPDR_PUPDR6_1|GPIO_PUPDR_PUPDR7_1);

//


}

uint8_t get_pir_information(){
	int northSensor = ((GPIOB -> IDR)>>4)&1;
	int eastSensor = ((GPIOB -> IDR)>>5)&1;
	int southSensor = ((GPIOB -> IDR)>>6)&1;
	int westSensor = ((GPIOB -> IDR)>>7)&1;

	uint8_t pirInformation = 0;
	pirInformation = pirInformation | northSensor;
	pirInformation = (pirInformation << 1) | eastSensor;
	pirInformation = (pirInformation << 1) | southSensor;
	pirInformation = (pirInformation << 1) |westSensor;

	return pirInformation;

}

//This method senses motions and moves the motor accordingly to the right position from wherever it's last postion was

void sense_motion(){
	//Priority Level:
	//north Sensor :1
	// East Sensor: 2
	//South Sensor: 3
	//West Sensor: 4

	//define each sensor from the inputs from their respective pins
	uint8_t instruction[6];

	int northSensor = ((GPIOB -> IDR)>>4)&1;
	int eastSensor = ((GPIOB -> IDR)>>5)&1;
	int southSensor = ((GPIOB -> IDR)>>6)&1;
    int westSensor = ((GPIOB -> IDR)>>7)&1;
    GPIOC->ODR ^=(1<<9);//toggle the light for testing purposes.

	instruction[0] = (char)2;
	instruction[1] = (char)0xff;
	instruction[2] = (char)0xff;
	instruction[3] = (char)0xff;
	instruction[4] = (char)0x02;
	instruction[5] = '\0';

	if(northSensor && eastSensor && southSensor && westSensor)
		return;

	//If a legitimate threat was found while scanning set sentry mode to false so it is no longer called.
	is_in_ScanningMode = 0;
    if(northSensor && eastSensor){
    	go_to_quadrant(7);
    	MOTOR_POSITION=NORTH_EAST;
    	set_STM_cotrolled(0);
    	UART_PutStr(instruction);
    }
    else if(eastSensor && southSensor){
		go_to_quadrant(5);
		MOTOR_POSITION=SOUTH_EAST;
		set_STM_cotrolled(0);
		UART_PutStr(instruction);
    }
    else if(westSensor && southSensor){
		go_to_quadrant(3);
		MOTOR_POSITION=SOUTH_WEST;
		set_STM_cotrolled(0);
		UART_PutStr(instruction);
    }
    else if(westSensor && northSensor){
    	go_to_quadrant(1);
		MOTOR_POSITION= NORTH_WEST;
		set_STM_cotrolled(0);
		UART_PutStr(instruction);
    }
    else if( northSensor){
		go_to_quadrant(0);
		MOTOR_POSITION= NORTH;
		set_STM_cotrolled(0);
		UART_PutStr(instruction);
    }
    else if( eastSensor){
		go_to_quadrant(6);
		MOTOR_POSITION= EAST;
		set_STM_cotrolled(0);
		UART_PutStr(instruction);
    }
    else if( southSensor){
		go_to_quadrant(4);
		MOTOR_POSITION= SOUTH;
		set_STM_cotrolled(0);
		UART_PutStr(instruction);

     }
    else if( westSensor){
		go_to_quadrant(2);
		MOTOR_POSITION= WEST;
		set_STM_cotrolled(0);
		UART_PutStr(instruction);


    }


}
//resets the motor to the north position based upon whatever postion it was in last
//This method will have to be changed to a different method once we get the proper sensors. At the moment it should be fine, but I will
//need to work on it.
void motor_reset(){

		go_to_quadrant(0);

		MOTOR_POSITION= NORTH;

}
