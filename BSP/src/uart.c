
#include <s3c44b0x.h>
#include <uart.h>

void uart0_init( void )
{
    UFCON0 = 0x1;
    UMCON0 = 0x0;

    ULCON0 = 0x3;

    UBRDIV0 = 0x22;
    UCON0 =0x5;
}

void uart0_putchar( char ch )
{
    while(UFSTAT0 & (1<<9));
    UTXH0 = ch;
}        

char uart0_getchar( void )
{
    while(!(UFSTAT0 & (0xf<<0 )));
    return URXH0;
}

void uart0_puts( char *s )
{
    int32 i=0;
    while(*(s+i)!='\0'){
    	uart0_putchar(*(s+i));
    	++i;
    }
}

void uart0_putint( int32 i )
{
    char buf[10 + 1 + 1];
    char *p = buf + 10 + 1;
    char neg=0,prueba;

    *p = '\0';
    if(i<0){
     	neg=1;
     	i= -i;
    }

   do{
	   prueba=(i%10);

	   *--p= '0' + prueba;
	    i= i/10;
   }while(i);

   if(neg)
	   *(--p)='-';

   uart0_puts( p );

}

void uart0_puthex( uint32 i )
{
    char buf[8 + 1];
    char *p = buf + 8;
    uint8 c;

    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            *--p = '0' + c;
        else
            *--p = 'a' + c - 10;
        i = i >> 4;
    } while( i );

    uart0_puts( p );
}

void uart0_gets( char *s )
{
	int32 i=0;
	char c;

	c=uart0_getchar();
    while(c!='\n'){
    	*(s+i)=c;
    	c=uart0_getchar();
    	++i;
    }

    *(s+i)='\0';

}

int32 uart0_getint( void )
{
	char buf[8 + 1];
	char *p=buf;
	char neg=0,aux;
	int32 c=0;

	uart0_gets(p);

	if(*p=='-') {
		neg=1;
		++p;
	}

	while( *p!='\0'){
		aux=*p -'0';
		c+=aux;
		c=c*10;
		++p;
	}

	return !neg ? c/10 : -(c/10);


}

uint32 uart0_gethex( void )
{
    char buf[8+1];
    char *p=buf;
    char aux;
    int64 c=0;

    uart0_gets(p);

	while( *p!='\0'){

		if(*p <'a')
			aux=*p -'0';
		else
			aux=*p -'a'+10;

		c+=aux;
		c=c*16;
		++p;
	}

	return c/16;


}
