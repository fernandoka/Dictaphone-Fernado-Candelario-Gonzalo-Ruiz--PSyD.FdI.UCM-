
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <rtc.h>

#define BIN2BCD(val) ((((val)/10)<<4) + (val)%10) //conversion Binario a Bcd
#define BCD2BIN(val) (((val)&15) + ((val)>>4)*10)  //conversion Bcd a Binario

extern void isr_TICK_dummy( void );

void rtc_init( void )
{
    TICNT   = (1<<7);
    RTCALM  = 0x0;
    RTCRST  = 0x0;
        
    RTCCON  |= 0x1;
    
    BCDYEAR = 0x13;
    BCDMON  = 0x01;
    BCDDAY  = 0x01;
    BCDDATE = 0x02;
    BCDHOUR = 0x00;
    BCDMIN  = 0x00;
    BCDSEC  = 0x00;

    ALMYEAR = 0;
    ALMMON  = 0;
    ALMDAY  = 0;
    ALMHOUR = 0;
    ALMMIN  = 0;
    ALMSEC  = 0;

    RTCCON &= 0x0;
}

void rtc_puttime( rtc_time_t *rtc_time )
{
	RTCCON  |= 0x1;
    
    BCDYEAR = BIN2BCD(rtc_time->year);
    BCDMON  = BIN2BCD(rtc_time->mon);
    BCDDAY  = BIN2BCD(rtc_time->mday);
    BCDDATE = BIN2BCD(rtc_time->wday);
    BCDHOUR = BIN2BCD(rtc_time->hour);
    BCDMIN  = BIN2BCD(rtc_time->min);
    BCDSEC  = BIN2BCD(rtc_time->sec);
        
    RTCCON &= 0x0;
}

void rtc_gettime( rtc_time_t *rtc_time )
{
	RTCCON  |= 0x1;
    
    rtc_time->year = BCD2BIN(BCDYEAR);
    rtc_time->mon  = BCD2BIN(BCDMON);
    rtc_time->mday = BCD2BIN(BCDDAY);
    rtc_time->wday = BCD2BIN(BCDDATE);
    rtc_time->hour = BCD2BIN(BCDHOUR);
    rtc_time->min  = BCD2BIN(BCDMIN);
    rtc_time->sec  = BCD2BIN(BCDSEC);
    if( ! rtc_time->sec ){
    	rtc_time->year = BCD2BIN(BCDYEAR);
		rtc_time->mon  = BCD2BIN(BCDMON);
		rtc_time->mday = BCD2BIN(BCDDAY);
		rtc_time->wday = BCD2BIN(BCDDATE);
		rtc_time->hour = BCD2BIN(BCDHOUR);
		rtc_time->min  = BCD2BIN(BCDMIN);
		rtc_time->sec  = BCD2BIN(BCDSEC);
    };

    RTCCON &= 0x0;
}


void rtc_open( void (*isr)(void), uint8 tick_count )
{
    pISR_TICK = isr;
	//borra flags de interrupciones pasadas
    I_ISPC  &= ~BIT_TICK;
	//desenmascara interrupciones globales y las tick
    INTMSK  &= ~(BIT_GLOBAL |BIT_TICK);
    TICNT  = (1 << 7| tick_count<<0);
}

void rtc_close( void )
{
    TICNT   &= ~(1<< 7);
	//enmascara las interrupciones tick
    INTMSK   |= BIT_TICK;
    pISR_TICK = isr_TICK_dummy;
}
