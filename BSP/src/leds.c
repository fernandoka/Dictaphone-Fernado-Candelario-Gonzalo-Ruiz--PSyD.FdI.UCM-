
#include <s3c44b0x.h>
#include <leds.h>

static uint8 state;



void leds_init( void )
{
	PCONB &= ~(3<<9);
	PDATB |=(3<<9);
}

void led_on( uint8 led )
{
	PDATB &= ~(led<<9);
	state |= led;
}

void led_off( uint8 led )
{
	PDATB |= (led<<9);
	state &= ~led;

}

void led_toggle( uint8 led )
{
	uint8 value=led_status(led);
	if(!value)
		led_on(led);
	else
		led_off(led);

}

uint8 led_status( uint8 led )
{
	return (state & led) >> ( led == LEFT_LED ? 0 : 1 );
}

