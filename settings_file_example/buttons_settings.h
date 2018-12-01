/*
 * buttons_settings.h
 *
 *  Created on: 27.05.2018
 *      Author: Przemek
 */

#ifndef LIBS_CONFIG_BUTTONS_SETTINGS_H_
#define LIBS_CONFIG_BUTTONS_SETTINGS_H_

#include <stdint.h>
#include "DMB/rejestry.h"

#define BUTTONS_SYSTEM_INCLUDE "stm32f3xx.h"

#define BUTTONS_DEBUG_LIBRARY_INCLUDE "DMB/debug.h"

//! biblioteka zawierajaca funkcje wykonujaca efekt przy wcisnieciu przycisku
#define BUTTONS_CB_EFFECT_INCLUDE <buzzer.h>

//! co ma sie wykonywac dodatkowo przy wcisnieciu przycisku - np buzzer, migniecie diodka itp
#define BUTTONS_CB_EFFECT buzzer_play()


//! ilosc wykorzystywanych przyciskow
#define BUTTONS_NUMBER 5

#define BUTTON1_GPIO 		GPIOE
#define BUTTON1_PIN 		9

#define BUTTON2_GPIO 		GPIOE
#define BUTTON2_PIN 		11

#define BUTTON3_GPIO 		GPIOE
#define BUTTON3_PIN 		12

#define BUTTON4_GPIO 		GPIOE
#define BUTTON4_PIN 		10

#define BUTTON5_GPIO 		GPIOE
#define BUTTON5_PIN 		8


// inicjalizacja GPIO dla przyciskow - rozna dla roznych platform dlatego w pliku ustawien
#define BUTTON1_INIT 		GPIO_MODER(BUTTON1_GPIO) &= ~( 0x03 << ( 2 * BUTTON1_PIN));
#define BUTTON1_PULL_UP 	GPIO_PUPDR(BUTTON1_GPIO) |= ( 0x01 << ( 2 * BUTTON1_PIN));
#define BUTTON1_PULL_DOWN	GPIO_PUPDR(BUTTON1_GPIO) |= ( 0x02 << ( 2 * BUTTON1_PIN));
#define BUTTON1_IS_LOW 		!(GPIO_IDR(BUTTON1_GPIO) & (1 << BUTTON1_PIN))
#define BUTTON1_IS_HIGH 	!(GPIO_IDR(BUTTON1_GPIO) & (1 << BUTTON1_PIN))

#define BUTTON2_INIT 		GPIO_MODER(BUTTON2_GPIO) &= ~( 0x03 << ( 2 * BUTTON2_PIN));
#define BUTTON2_PULL_UP 	GPIO_PUPDR(BUTTON2_GPIO) |= ( 0x01 << ( 2 * BUTTON2_PIN));
#define BUTTON2_PULL_DOWN	GPIO_PUPDR(BUTTON2_GPIO) |= ( 0x02 << ( 2 * BUTTON2_PIN));
#define BUTTON2_IS_LOW 		!(GPIO_IDR(BUTTON2_GPIO) & (1 << BUTTON2_PIN))
#define BUTTON2_IS_HIGH 	!(GPIO_IDR(BUTTON2_GPIO) & (1 << BUTTON2_PIN))

#define BUTTON3_INIT 		GPIO_MODER(BUTTON3_GPIO) &= ~( 0x03 << ( 2 * BUTTON3_PIN));
#define BUTTON3_PULL_UP 	GPIO_PUPDR(BUTTON3_GPIO) |= ( 0x01 << ( 2 * BUTTON3_PIN));
#define BUTTON3_PULL_DOWN	GPIO_PUPDR(BUTTON3_GPIO) |= ( 0x02 << ( 2 * BUTTON3_PIN));
#define BUTTON3_IS_LOW 		!(GPIO_IDR(BUTTON3_GPIO) & (1 << BUTTON3_PIN))
#define BUTTON3_IS_HIGH 	!(GPIO_IDR(BUTTON3_GPIO) & (1 << BUTTON3_PIN))

#define BUTTON4_INIT 		GPIO_MODER(BUTTON4_GPIO) &= ~( 0x03 << ( 2 * BUTTON4_PIN));
#define BUTTON4_PULL_UP 	GPIO_PUPDR(BUTTON4_GPIO) |= ( 0x01 << ( 2 * BUTTON4_PIN));
#define BUTTON4_PULL_DOWN	GPIO_PUPDR(BUTTON4_GPIO) |= ( 0x02 << ( 2 * BUTTON4_PIN));
#define BUTTON4_IS_LOW 		!(GPIO_IDR(BUTTON4_GPIO) & (1 << BUTTON4_PIN))
#define BUTTON4_IS_HIGH 	!(GPIO_IDR(BUTTON4_GPIO) & (1 << BUTTON4_PIN))

#define BUTTON5_INIT 		GPIO_MODER(BUTTON5_GPIO) &= ~( 0x03 << ( 2 * BUTTON5_PIN));
#define BUTTON5_PULL_UP 	GPIO_PUPDR(BUTTON5_GPIO) |= ( 0x01 << ( 2 * BUTTON5_PIN));
#define BUTTON5_PULL_DOWN	GPIO_PUPDR(BUTTON5_GPIO) |= ( 0x02 << ( 2 * BUTTON5_PIN));
#define BUTTON5_IS_LOW 		!(GPIO_IDR(BUTTON5_GPIO) & (1 << BUTTON5_PIN))
#define BUTTON5_IS_HIGH 	!(GPIO_IDR(BUTTON5_GPIO) & (1 << BUTTON5_PIN))



#endif /* LIBS_CONFIG_BUTTONS_SETTINGS_H_ */
