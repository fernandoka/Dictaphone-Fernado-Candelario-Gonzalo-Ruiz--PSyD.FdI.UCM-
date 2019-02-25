
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <iis.h>
#include <dma.h>

static void isr_bdma0( void ) __attribute__ ((interrupt ("IRQ")));

static uint8 flag;
static uint8 iomode;

void iis_init( uint8 mode )
{
    iomode = mode;

    if( mode == IIS_POLLING )
    {
        //¿?
    	IISPSR  = 0x77;
        IISMOD  = 0xC9;
        IISFCON = 0x300;
        IISCON  = 0x3;
    }
    if( mode == IIS_DMA )
    {
    	//¿?
        IISPSR  = 0x77;
        IISMOD  = 9;
        IISFCON = 0;
        IISCON  = 0;
        bdma0_init();
        bdma0_open( isr_bdma0 );
        flag = OFF;
    }
}

static void isr_bdma0( void )
{
	if( !(BDICNT0 & (1<<21)) )
	{
		IISCON &= ~1;
		flag = OFF;
	};
    I_ISPC = BIT_BDMA0; 
}

inline void iis_putSample( int16 ch0, int16 ch1 )
{

    //Dara problemas con la codificación ascii??
    ///6>
    while(((IISFCON & 0xf0) >> 4) > 6);
    IISFIF = ch0;
    IISFIF = ch1;
}

inline void iis_getSample( int16 *ch0, int16 *ch1 )
{
    //<2
    //Dara problemas con la codificación ascii??
    while((IISFCON & 0xf)<2);
	*ch0 = IISFIF;
	*ch1 = IISFIF;
}

void iis_play( int16 *buffer, uint32 length, uint8 loop )
{
    uint32 i;
    int16 ch1, ch2;

    if( iomode == IIS_POLLING )
        for( i=0; i<length/2; )
        {
            ch1 = buffer[i++];
            ch2 = buffer[i++];
            iis_putSample( ch1, ch2 );
        }
    if( iomode == IIS_DMA ){
    	while( flag != OFF );
		BDISRC0  = (1 << 30) | (1 << 28) | (uint32) buffer;
		BDIDES0  = (1 << 30) | (3 << 28) | (uint32) &IISFIF;
		BDCON0   = 0;
		BDICNT0  = (1 << 30) | (1 << 26) | (3 << 22) | (loop << 21) | (0xfffff & length);
		BDICNT0 |= (1 << 20);

		IISMOD  = (2 << 6 | 0 << 8 | 1 << 3 | 1 );
		IISFCON = (1 << 11 | 1 << 9);
		IISCON  = (1 << 5 | 3 << 0);
		flag = ON;
    }

}

void iis_rec( int16 *buffer, uint32 length )
{
    uint32 i;
    int16 ch1, ch2;

    if( iomode == IIS_POLLING )
    	 for( i=0; i<length/2; )
    	        {
    	            ch1 = buffer[i++];
    	            ch2 = buffer[i++];
    	            iis_getSample( &ch1, &ch2 );
    	        }
    if( iomode == IIS_DMA )
    {
        while( flag != OFF );
        BDISRC0  = (1 << 30) | (3 << 28) | (uint32) &IISFIF;
        BDIDES0  = (2 << 30) | (1 << 28) | (uint32) buffer;      
        BDCON0   = 0;
        BDICNT0  = (1 << 30) | (1 << 26) | (3 << 22) | (0xfffff & length); 
        BDICNT0 |= (1 << 20);

        IISMOD  = (1 << 6| 1 << 3 | 1 );
        IISFCON = (1 << 10 | 1 << 8);
        IISCON  = (1 << 4 | 3 << 0);
        flag = ON;
    }
}

void iis_pause( void )
{
	IISCON &= ~(1<<0);
    flag=OFF;
}

void iis_continue( void )
{
	IISCON |= (1<<0);
    flag=ON;
}

uint8 iis_status( void )
{
    return flag;
}

void iis_playwawFile( uint8 *fileAddr )
{
    uint32 size;

    while ( !(fileAddr[0] == 'd' && fileAddr[1] == 'a' && fileAddr[2] == 't' && fileAddr[3] == 'a') )
        fileAddr++;
    fileAddr += 4;

    size = (uint32) fileAddr[0];
    size += (uint32) fileAddr[1] << 8;
    size += (uint32) fileAddr[2] << 16;
    size += (uint32) fileAddr[3] << 24;
    fileAddr += 4;

    iis_play( (int16 *)fileAddr, size, OFF );
}
