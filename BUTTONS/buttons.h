/*
 * buttons.h
 *
 *  Created on: 26.05.2018
 *      Author: Przemek
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <libs_config/buttons_settings.h>
#include BUTTONS_SYSTEM_INCLUDE
#include BUTTONS_DEBUG_LIBRARY_INCLUDE


// utworzenie typu callbacka od eventu przycisku
typedef void (button_Callback_type)();

//! struktura inicjalizacyjna przycisku
typedef struct BUTTON_settings{
	uint8_t 	debounce_time_high; // ilosc "tykniec" podczas ktorych musi byc wcisniety, zeby zostal zaliczony jako wcisniety
	uint8_t 	debounce_time_low;	// ilosc "tykniec" podczas ktorych musi byc wcisniety, zeby zostal zaliczony jako puszczony
	uint16_t 	long_push_time;		// ilosc "tykniec" podczas ktorych musi byc wcisniety, zeby zostal zaliczony jako dlugie wcisniecie
	uint16_t 	repeat_time;		// ilosc "tykniec" podczas ktorych musi byc wcisniety od momentu dlugiego wcisniecia po ktorych wysylany jest event repeat

	// callbacki dla funkcji eventow od przycisku
	button_Callback_type* push_function;
	button_Callback_type* push_long_function;
	button_Callback_type* repeat_function;
	button_Callback_type* release_function;
}BUTTON_settings;


//! struktura przechowujaca same callbacki - do ich latwiejszej podmiany
typedef struct BUTTON_callbacks{
	button_Callback_type* push_function;
	button_Callback_type* push_long_function;
	button_Callback_type* repeat_function;
	button_Callback_type* release_function;
}BUTTON_callbacks;

// Enum dla okreslenia typu eventu
typedef enum Button_event_e{
	event_typ_push 		= 0x01,
	event_typ_release 	= 0x02,
	event_typ_long		= 0x04,
	event_typ_repeat	= 0x08,
}Button_event_e;

//! inicjalizacja GPIO dla przyciskow - na podstawie danych z pliku ustawien
void init_buttons();

//! inicjalizacja pojedynczego przycisku w "starym stylu" dla kompatybilnosci kodow
void button_init( uint8_t numer, uint8_t debounce_time_high, uint16_t debounce_time_low, uint16_t long_push_time,uint16_t repeat_time, button_Callback_type* push_function,button_Callback_type* push_long_function, button_Callback_type* repeat_function, button_Callback_type* release_function );

//! podmienienie callbackow dla okreslonego przycisku w "starym stylu" dla kompatybilnosci kodow
void button_change_callbacks(uint8_t numer, button_Callback_type* push_function,button_Callback_type* push_long_function, button_Callback_type* repeat_function, button_Callback_type* release_function );

//! inicjalizacja pojedynczego przycisku
void button_init_with_struct(uint8_t numer, BUTTON_settings *btn);

//! podmiana callbackow dla przycisku
void button_change_callbacks_with_struct(uint8_t numer, BUTTON_callbacks *btn_cb);


//! poczatkowe poedjscie z mozliwoscia robienia roznych rzeczy na roznych poziomach przerwan - potem zaniechane, zalecane jest uzywanie funkcji "button_check_states_and_run_callbacks"
//! ale mozliwosc zostawiona dla kompatybilnosci starszych projektow
void buttons_check_states();
void buttons_check_events();
void buttons_callbacks();

//! funkcja "all-in-one" - poniewaz wczesniejsza koncepcja wywolywaniem funkcji na roznych poziomach przerwan moze spowodowac problemy z atomowoscia dostepu do zmiennych
//! ta wersja robi wszystko na tym samym poziomie - do wywolywania cyklicznie co ustalony interwal
void buttons_check_states_and_run_callbacks();

//! wyzerowanie licznikow przyciskow
void buttons_reset_counters( uint8_t numer );

//! zasymulowanie eventu na zadanym przycisku - dzieki temu mozna "automatycznie" testowac przebiegi przechodzenia po aplikacji
//! lub zrobic w latwy sposob "demo" pokazowe poszczegolnych funkcji
void button_simulate_event(uint8_t numer, Button_event_e event_type );

#endif
