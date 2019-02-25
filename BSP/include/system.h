/*-------------------------------------------------------------------
**
**  Fichero:
**    system.h  25/2/2015
**
**    (c) J.M. Mendias
**    Programación de Sistemas y Dispositivos
**    Facultad de Informática. Universidad Complutense de Madrid
**
**  Propósito:
**    Contiene las definiciones de los prototipos de las funciones
**    para la inicialización del sistema
**
**  Notas de diseño:
**
**-----------------------------------------------------------------*/
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <common_types.h>

/* Modos de operación */
#define USRMODE (0x10)
#define FIQMODE (0x11)
#define IRQMODE (0x12)
#define SVCMODE (0x13)
#define ABTMODE (0x17)
#define UNDMODE (0x1b)
#define SYSMODE (0x1f)

#define NOP \
  asm volatile ( "nop" );

#define INT_DISABLE \
  asm volatile( "mrs   r0, cpsr" : : : "r0" ); \
  asm volatile( "stmfd sp!, {r0}" ); \
  asm volatile( "orr   r0, r0, #0b11000000" ); \
  asm volatile( "msr   cpsr_c, r0" );
    
#define INT_ENABLE \
  asm volatile( "ldmfd sp!, {r0}" : : : "r0" ); \
  asm volatile( "msr   cpsr_c, r0" );

#define IRQ_NESTING_ENABLE                      \
  asm volatile ( "mrs   lr, spsr" );            \
  asm volatile ( "stmfd sp!, {lr}" );           \
  asm volatile ( "msr   cpsr_c, #0b00011111" ); \
  asm volatile ( "stmfd sp!, {lr}" );

#define IRQ_NESTING_DISABLE                     \
  asm volatile ( "ldmfd sp!, {lr}" );           \
  asm volatile ( "msr   cpsr_c, #0b10010010" ); \
  asm volatile ( "ldmfd sp!, {lr}" );           \
  asm volatile ( "msr   spsr, lr" );

#define SET_OPMODE( mode )                          \
  asm volatile ( "mrs r0, cpsr" : : : "r0" );       \
  asm volatile ( "bic r0, r0, #0x1f" );             \
  asm volatile ( "orr r0, r0, %0" : : "i" (mode) ); \
  asm volatile ( "msr cpsr_c, r0" );

#define SET_IRQFLAG( value )                         \
  asm volatile ( "mrs r0, cpsr" : : : "r0" );        \
  asm volatile ( "bic r0, r0, #0x80" );              \
  asm volatile ( "orr r0, r0, %0" : : "i" (value<<7) ); \
  asm volatile ( "msr cpsr_c, r0" );

#define SET_FIQFLAG( value )                         \
  asm volatile ( "mrs r0, cpsr" : : : "r0" );        \
  asm volatile ( "bic r0, r0, #0x40" );              \
  asm volatile ( "orr r0, r0, %0" : : "i" (value<<6) ); \
  asm volatile ( "msr cpsr_c, r0" );

#define LOCK_MUTEX( mutex )                               \
  asm volatile ( "mov r0, #1" : : : "r0" );               \
  asm volatile ( "swpb r0, r0, [%0]" : : "r" (mutex) : ); \
  asm volatile ( "cmp r0, #1" );                          \
  asm volatile ( "subeq pc, pc, #20")

#define UNLOCK_MUTEX( mutex )                       \
  asm volatile ( "mov r0, #0" : : : "r0" );         \
  asm volatile ( "strb r0, [%0]" : : "r" (mutex) )

/*
**  Inicializa el sistema
**    Deshabilita el watchdog
**    Enmascara todas las interrupciones
**    Configura el gestor de reloj
**      Tiempo de estabilización del PLL máximo
**      MCLK = 64 MHz
**      MCLK_SLOW = 500 KHz
**      Modo de funcionamiento normal
**      Reloj distribuido a todos los controladores
**    Arbitro de bus con prioridades por defecto
**    Cache deshabilitada
**    Configura los puertos de E/S
**    Configura el controlador de interrupciones
**      Fija prioridades por defecto
**      Todas las fuentes de interrupción en modo IRQ
**      Instala dummy ISR
**      Borra interrupciones pendientes externas e internas
**      IRQ vectorizadas, linea IRQ activada, linea FIQ desactivada
**  Inicializa el UART0
**  Muestra información del sistema por la UART0
*/
void sys_init( void );

/*
**  Pone el procesador en estado IDLE
*/
inline void sleep( void );

#endif
