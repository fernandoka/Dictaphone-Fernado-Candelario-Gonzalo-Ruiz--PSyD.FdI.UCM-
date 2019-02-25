
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <pbs.h>
#include <timers.h>

extern void isr_PB_dummy( void );

void pbs_init( void )
{
    timers_init();
}

uint8 pb_scan( void )
{
    if(!(PDATG & PB_LEFT))
        return PB_LEFT;
    else if(!(PDATG & PB_RIGHT))
        return PB_RIGHT;
    else
        return PB_FAILURE;
}

uint8 pb_status( uint8 scancode )
{
    return ( PDATG & scancode ) ? !PB_DOWN  : !PB_UP ;
}

void pb_wait_keydown( uint8 scancode )
{

	while(PDATG & scancode);
	sw_delay_ms( PB_KEYDOWN_DELAY );
}

void pb_wait_keyup( uint8 scancode )
{

	pb_wait_keydown( scancode );
	while(!(PDATG & scancode));
	sw_delay_ms( PB_KEYUP_DELAY );
}

void pb_wait_any_keydown( void )
{

	while( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) );
	 sw_delay_ms( PB_KEYDOWN_DELAY );
}

void pb_wait_any_keyup( void )
{

	pb_wait_any_keydown();
	while(! ((PDATG & PB_LEFT) && (PDATG & PB_RIGHT)) );
	sw_delay_ms( PB_KEYUP_DELAY );
}

uint8 pb_getchar( void )
{
    uint8 scancode;

    while( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) );
    sw_delay_ms( PB_KEYDOWN_DELAY );

    scancode = pb_scan();

    while( !( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) ) );
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

uint8 pb_getchartime( uint16 *ms )
{
    uint8 scancode;
    
    while( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) );
    timer3_start();
    sw_delay_ms( PB_KEYDOWN_DELAY );
    
    scancode = pb_scan();
    
    while( !( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) ) );
    *ms = timer3_stop() / 10;
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

uint8 pb_timeout_getchar( uint16 ms )
{
    uint8 scancode;
    timer3_start_timeout(ms*10);
    while( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) && timer3_timeout( ));
    if (!timer3_timeout( )) return PB_TIMEOUT;
    sw_delay_ms( PB_KEYDOWN_DELAY );

    scancode = pb_scan();

    while( !( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) ) && timer3_timeout( ));
    if (!timer3_timeout( )) return PB_TIMEOUT;
    sw_delay_ms( PB_KEYUP_DELAY );
    return scancode;

}

void pbs_open( void (*isr)(void) )
{
    pISR_PB   = isr;
    EXTINTPND = (BIT_EINT7 | BIT_EINT6) ;
    I_ISPC  = BIT_PB ;
    INTMSK  &= ~(BIT_GLOBAL |BIT_PB);//BIT_EINT7|BIT_EINT6
}

void pbs_close( void )
{
    INTMSK  |= (BIT_PB);
    pISR_PB  = isr_PB_dummy;
}
