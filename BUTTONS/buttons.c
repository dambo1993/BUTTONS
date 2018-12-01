/*
 * buttons.c
 *
 *  Created on: 26.05.2018
 *      Author: Przemek
 */


#include "buttons.h"
#include <libs_config/buttons_settings.h>
#include BUTTONS_SYSTEM_INCLUDE
#include BUTTONS_DEBUG_LIBRARY_INCLUDE

// flaga od pobrania nowego stanu przyciskow
static volatile uint8_t buttons_flag = 0;

// przyjalem zasade, ze funkcja release moze wystapic tylko wtedy, kiedy jestesmy po dlugim przytrzymaniu, nie po krotkim
typedef struct
{
	// zmienne "wewnetrzne"
	volatile uint8_t current_state; // wlasna zmienna potrzebna w funkcji

	volatile uint8_t events;  			// tutaj przechowywana jest informacja o eventach na przycisku
	volatile uint8_t events_clear;  	// tutaj przechowywana jest informacja o eventach do zgaszenia - jesli funkcje s¹ na ró¿nych priorytetach

	uint16_t h_counter; // licznik do odliczania "wcisniecia"

	uint16_t h_counter2; // licznik do odliczania "long" i "repeat"

	uint16_t l_counter; // licznik stanu niskiego

	// zmienne podawane przez uzytkownika

	// czas po jakim uruchomi sie wcisniecie przycisku
	uint8_t debounce_time_high;

	// czas jaki musi byc minimalny stan niski, ktory nie jest rozpoznany jako drganie
	uint8_t debounce_time_low;

	// czas po jakim uruchomi sie dlugie przycisniecie
	uint16_t long_push_time;

	// czas pomiedzy funkcjami repeat
	uint16_t repeat_time;

	// callbacki do funkcji
	button_Callback_type *push_function;
	button_Callback_type *push_long_function;
	button_Callback_type *release_function;
	button_Callback_type *repeat_function;
} BUTTON;

//! glowna tablica z przyciskami
static BUTTON buttons_array[ BUTTONS_NUMBER ];

static button_effect_callback_t button_effect_callback;

void button_init( uint8_t numer,
		uint8_t debounce_time_high,
		uint16_t debounce_time_low,
		uint16_t long_push_time,
		uint16_t repeat_time,
		const button_Callback_type* push_function,
		const button_Callback_type* push_long_function, const
		const button_Callback_type* repeat_function,
		const button_Callback_type* release_function )
{
	// variables:
	buttons_array[numer].debounce_time_high 	= debounce_time_high;
	buttons_array[numer].debounce_time_low 		= debounce_time_low;
	buttons_array[numer].long_push_time 		= long_push_time;
	buttons_array[numer].repeat_time 			= repeat_time;

	// callbacks:
	buttons_array[numer].push_function 			= push_function;
	buttons_array[numer].push_long_function 	= push_long_function;
	buttons_array[numer].release_function 		= release_function;
	buttons_array[numer].repeat_function 		= repeat_function;

	// wyzerowanie zmiennych jakby co - zakladam, ze inicjalizacja jest na poczatku
	buttons_array[numer].current_state 	= 0;
	buttons_array[numer].h_counter 		= 0;
	buttons_array[numer].h_counter2 	= 0;
	buttons_array[numer].l_counter 		= 0;

	buttons_array[numer].events 		= 0;
	buttons_array[numer].events_clear	= 0;
}

void button_init_with_struct(uint8_t numer, const BUTTON_settings *btn)
{
	// variables:
	buttons_array[numer].debounce_time_high 	= btn->debounce_time_high;
	buttons_array[numer].debounce_time_low 		= btn->debounce_time_low;
	buttons_array[numer].long_push_time 		= btn->long_push_time;
	buttons_array[numer].repeat_time 			= btn->repeat_time;

	// callbacks:
	buttons_array[numer].push_function 			= btn->push_function;
	buttons_array[numer].push_long_function 	= btn->push_long_function;
	buttons_array[numer].release_function 		= btn->release_function;
	buttons_array[numer].repeat_function 		= btn->repeat_function;

	// wyzerowanie zmiennych jakby co - zakladam, ze inicjalizacja jest na poczatku
	buttons_array[numer].current_state 	= 0;
	buttons_array[numer].h_counter 		= 0;
	buttons_array[numer].h_counter2 	= 0;
	buttons_array[numer].l_counter 		= 0;

	buttons_array[numer].events 		= 0;
	buttons_array[numer].events_clear	= 0;
}

void button_register_universal_efect_callback(button_effect_callback_t effect)
{
	button_effect_callback = effect;
}

void button_change_callbacks(uint8_t numer,
		const button_Callback_type* push_function,
		const button_Callback_type* push_long_function,
		const button_Callback_type* repeat_function,
		const button_Callback_type* release_function )
{
	// callbacks:
	if(numer < BUTTONS_NUMBER)
	{
		buttons_array[numer].push_function 			= push_function;
		buttons_array[numer].push_long_function 	= push_long_function;
		buttons_array[numer].release_function 		= release_function;
		buttons_array[numer].repeat_function 		= repeat_function;
	}
}

void button_change_callbacks_with_struct(uint8_t numer, const BUTTON_callbacks *btn_cb)
{
	// callbacks:
	if(numer < BUTTONS_NUMBER)
	{
		buttons_array[numer].push_function 			= btn_cb->push_function;
		buttons_array[numer].push_long_function 	= btn_cb->push_long_function;
		buttons_array[numer].release_function 		= btn_cb->release_function;
		buttons_array[numer].repeat_function 		= btn_cb->repeat_function;
	}
}


void init_buttons()
{
#if BUTTONS_NUMBER >= 1
	BUTTON1_INIT;
	BUTTON1_PULL_UP;
#endif

#if BUTTONS_NUMBER >= 2
	BUTTON2_INIT;
	BUTTON2_PULL_UP;
#endif

#if BUTTONS_NUMBER >= 3
	BUTTON3_INIT;
	BUTTON3_PULL_UP;
#endif

#if BUTTONS_NUMBER >= 4
	BUTTON4_INIT;
	BUTTON4_PULL_UP;
#endif

#if BUTTONS_NUMBER >= 5
	BUTTON5_INIT;
	BUTTON5_PULL_UP;
#endif
}

//! wywolanie uniwersalnej akcji od eventu z przycisku
static inline void buttons_call_effect_callback(void)
{
	if(button_effect_callback)
	{
		button_effect_callback();
	}
}

//! sprawdzenie eventow i wykonanie ewentualnych callbackow
void buttons_callbacks(void)
{
	// dla kazdego przycisku
	for( uint8_t i = 0; i < BUTTONS_NUMBER; i++ )
	{
		// jesli wykryto jakis event na przycisku - sprawdzamy jaki to event
		if(buttons_array[i].events != 0)
		{
			// kopia zmiennej, bo jest ona volatile
			uint8_t events = buttons_array[i].events;

			if(events & event_typ_push)
			{
				// zaznaczenie eventu do wylaczenia
				buttons_array[i].events_clear |= event_typ_push;

				if(buttons_array[i].push_function)
				{
					buttons_array[i].push_function();
					buttons_call_effect_callback();
				}
			}
			if(events & event_typ_release)
			{
				// zaznaczenie eventu do wylaczenia
				buttons_array[i].events_clear |= event_typ_release;

				if(buttons_array[i].release_function)
				{
					buttons_array[i].release_function();
					buttons_call_effect_callback();
				}
			}
			if(events & event_typ_long)
			{
				// zaznaczenie eventu do wylaczenia
				buttons_array[i].events_clear |= event_typ_long;

				if(buttons_array[i].push_long_function)
				{
					buttons_array[i].push_long_function();
					buttons_call_effect_callback();
				}
			}
			if(events & event_typ_repeat)
			{
				// zaznaczenie eventu do wylaczenia
				buttons_array[i].events_clear |= event_typ_repeat;

				if(buttons_array[i].repeat_function)
				{
					buttons_array[i].repeat_function();
					buttons_call_effect_callback();
				}
			}
		}
	}
}

//! weryfikacja eventow i zapalenie ich flag
void buttons_check_events(void)
{
	BUTTON * actual_button;

	for( uint8_t i = 0; i < BUTTONS_NUMBER; i++ )
	{
		if(buttons_array[i].events_clear != 0)
		{
			buttons_array[i].events = buttons_array[i].events & ~(buttons_array[i].events_clear);
			buttons_array[i].events_clear = 0;
		}
	}

	// jesli nastapil nowy odczyt:
	if( buttons_flag )
	{
		buttons_flag = 0;

		// dla kazdego przycisku
		for( uint8_t i = 0; i < BUTTONS_NUMBER; i++ )
		{
			actual_button = &buttons_array[i];

			// jesli przycisk jest wduszony:
			// if button is push:
			if( actual_button->current_state == 1 )
			{
				actual_button->l_counter = 0;
				actual_button->h_counter++;

				// sprawdzenie krotkiego wcisniecia
				// check short push
				if( actual_button->h_counter == actual_button->debounce_time_high )
				{
					actual_button->events |= event_typ_push;
				}

				// check long push
				// sprawdzenie dlugiego wcisniecia
				if( actual_button->h_counter == actual_button->long_push_time )
				{
					actual_button->events |= event_typ_long;
				}

				// sprawdzenie repeata
				// check repeat
				if( actual_button->h_counter >= actual_button->long_push_time )
				{
					actual_button->h_counter2++;

					if( actual_button->h_counter2 == actual_button->repeat_time )
					{
						actual_button->h_counter2 = 0;
						actual_button->events |= event_typ_repeat;
					}
				}
			}
			else
			{
				if( actual_button->l_counter < 10000 )
				{
					actual_button->l_counter++;
				}

				if( actual_button->l_counter == actual_button->debounce_time_low )
				{
					actual_button->events |= event_typ_release;

					// wyzerowania licznika stanu wcisniecia
					actual_button->h_counter = 0;
					actual_button->h_counter2 = 0;
				}
			}
		}
	}
}

// funkcja sprawdzajaca stany wszystkich przyciskow
void buttons_check_states(void)
{
#if BUTTONS_NUMBER >= 1
	if( BUTTON1_IS_LOW )
	{
		buttons_array[0].current_state = 1;
	}
	else
	{
		buttons_array[0].current_state = 0;
	}
#endif

#if BUTTONS_NUMBER >= 2
	if( BUTTON2_IS_LOW )
	{
		buttons_array[1].current_state = 1;
	}
	else
	{
		buttons_array[1].current_state = 0;
	}
#endif

#if BUTTONS_NUMBER >= 3
	if( BUTTON3_IS_LOW )
	{
		buttons_array[2].current_state = 1;
	}
	else
	{
		buttons_array[2].current_state = 0;
	}
#endif

#if BUTTONS_NUMBER >= 4
	if( BUTTON4_IS_LOW )
	{
		buttons_array[3].current_state = 1;
	}
	else
	{
		buttons_array[3].current_state = 0;
	}
#endif

#if BUTTONS_NUMBER >= 5
	if( BUTTON5_IS_LOW )
	{
		buttons_array[4].current_state = 1;
	}
	else
	{
		buttons_array[4].current_state = 0;
	}
#endif
	buttons_flag = 1;
}

void button_reset_counters( uint8_t numer )
{
	buttons_array[numer].current_state 	= 0;
	buttons_array[numer].h_counter 		= 0;
	buttons_array[numer].h_counter2 	= 0;
	buttons_array[numer].l_counter 		= 0;
}

//! zasymulowanie eventu na zadanym przycisku - dzieki temu mozna "automatycznie" testowac przebiegi przechodzenia po aplikacji
//! lub zrobic w latwy sposob "demo" pokazowe poszczegolnych funkcji
void button_simulate_event(uint8_t numer, Button_event_e event_type )
{
	buttons_array[numer].events |= event_type;
}

//! funkcja "all-in-one" - poniewaz wczesniejsza koncepcja wywolywaniem funkcji na roznych poziomach przerwan moze spowodowac problemy z atomowoscia dostepu do zmiennych
//! ta wersja robi wszystko na tym samym poziomie - do wywolywania cyklicznie co ustalony interwal
void buttons_check_states_and_run_callbacks(void)
{
	buttons_check_states();
	buttons_check_events();
	buttons_callbacks();
}

//! sprawdzenie, czy przycisk jest aktualnie wcisniety - mozna wykorzystywac np przed uruchomieniem timerow w celu
//! wprowadzenia urzadzenia w tryb serwisowy itp
uint8_t buttons_is_button_pressed( uint8_t numer )
{
	if(numer > BUTTONS_NUMBER )
	{
		return 0;
	}

	switch(numer)
	{
		#if BUTTONS_NUMBER >= 1
		case 0:
		{
			if( BUTTON1_IS_LOW )
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		#endif
		#if BUTTONS_NUMBER >= 2
		case 1:
		{
			if( BUTTON2_IS_LOW )
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		#endif
		#if BUTTONS_NUMBER >= 3
		case 2:
		{
			if( BUTTON3_IS_LOW )
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		#endif
		#if BUTTONS_NUMBER >= 4
		case 3:
		{
			if( BUTTON4_IS_LOW )
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		#endif
		#if BUTTONS_NUMBER >= 5
		case 4:
		{
			if( BUTTON5_IS_LOW )
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		#endif

		default:
		{
			return 0;
		}
	}
	return 0;
}
