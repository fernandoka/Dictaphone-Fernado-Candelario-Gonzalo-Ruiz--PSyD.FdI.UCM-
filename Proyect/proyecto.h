/*
 * proyecto.h
 *
 *  Created on: 10/01/2018
 *      Author: Fernando Candelario y Gonzalo Ruiz
 */

#ifndef PROYECTO_H_
#define PROYECTO_H_

#include <s3c44b0x.h>
#include <system.h>
#include <uart.h>
#include <timers.h>
#include <s3cev40.h>
#include <common_types.h>
#include <pbs.h>
#include <keypad.h>
#include <lcd.h>
#include <uda1341ts.h>
#include <iis.h>
#include<rtc.h>

 /* Macros */

#define SPS      (16000)                  // Frecuencia de muestreo
#define BUFFER   ((int16 *)0x0c200000)    // Dirección de inicio del buffer
#define MAXBYTES (640000)                 // Capacidad máxima del buffer = 10 s : (10 s) * (16000 muestras/s) * (2 canales) * (2 B/muestra) = 640000B

#define MAX_GRABS 4
#define MAX_OPTIONS 4
/* Definición de tipos */

typedef enum {waiting, recording, playing, erased} state_t;    // Estados en los que puede estar el sistema

typedef struct{
	uint32 ini;
	uint32 fin;
}grabacion_t;

volatile grabacion_t grabaciones[MAX_GRABS];

typedef struct {
	int16  *buffer;    // Puntero al inicio del buffer de muestras
    uint32 maxsize;    // Tamaño máximo del buffer (en numero de muestras, 1 muestra = 2B)
	uint32 size;       // Numero de muestras grabadas
    uint32 index;      // Numero de muestras reproducidas
	uint16 full;       // Flag para indicar si el buffer ha sido completamente grabado
	uint16 played;     // Flag para indicar si el buffer ha sido completamente reproducido
} audioBuffer_t;

typedef struct {
	volatile uint16  numSecs;    // Numero de segundos transcurridos desde el inicio de la grabación/reproducción
    volatile boolean updated;    // Flag para indicar si numSeconds se ha actualizado
} time_t;

/* Declaración de funciones */

void isr_keypad( void ) __attribute__ ((interrupt ("IRQ")));
void isr_rtc( void ) __attribute__ ((interrupt ("IRQ")));

void timer0_recording(void) __attribute__ ((interrupt ("IRQ")));
void timer0_playing(void) __attribute__ ((interrupt ("IRQ")));
void timer0_doubleSpeed(void) __attribute__ ((interrupt ("IRQ")));
void timer0_halfSpeed(void) __attribute__ ((interrupt ("IRQ")));
void timer0_Inverso(void) __attribute__ ((interrupt ("IRQ")));

void start_playing(void);
void start_recording(void);
void start_erased(void);
void grabar(void);
void reproducir(void (*fun)(void));
void esperar(void);
void borrar(void);

void init_program(void);

void print_menu(void);
void quit_menu(void);
void errores_keypad(uint8 scancode);
void clean(void);
void menu_reproducir(void);
void menu_modo_reproduccion(void);

/* Declaración de variables globales para comunicación foreground/background  */

volatile state_t state;

volatile audioBuffer_t audioBlock;
volatile time_t        time;

volatile uint8 key;
volatile uint8 numGrabaciones;
volatile uint8 numSong;
volatile uint8 goOut;
volatile boolean irAtras;


#endif /* PROYECTO_H_ */
