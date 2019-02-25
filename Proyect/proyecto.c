/*
 * proyecto.h
 *
 *  Created on: 10/01/2018
 *      Author: Fernando Candelario y Gonzalo Ruiz
 */
#include<proyecto.h>
/************************************/

void main( void ){
	init_program();

    while(!goOut){
    	esperar();

    	switch(state){
    		case waiting:
    			goOut=1;
    			break;
    		case recording:
    			start_recording();				
    			break;
    		case playing:
    			start_playing();
    			break;
			case erased:
				start_erased();
			break;
    	}

    }


    lcd_puts_x2( 30, 70, BLACK, "    ADIOS !!" );
    while(1);
}

void isr_keypad(void){
	uint8 scancode;
	scancode = keypad_getchar();
	    switch( scancode ){

	    	case KEYPAD_TIMEOUT:
	            key=3;
	            break;
	        case KEYPAD_FAILURE:
	        	key=4;
	        	break;
	    }

	    if((state==playing && scancode > MAX_GRABS  && scancode!=7) && (scancode > 2 ))
	    	errores_keypad(scancode);
	    else
	    	key=scancode;

	    I_ISPC = BIT_KEYPAD;
}

void timer0_recording(void){
	static uint16 numTicks = 8000;
	int16 ch0, ch1;
	if( audioBlock.size < audioBlock.maxsize ) {                        // Si el sistema esta grabando sonido y el buffer de audio no esta completo...
		iis_getSample( &ch0, &ch0 );                                    // ...recibe una muestra por canal y
		iis_getSample( &ch1, &ch1 );
		ch0=(ch0+ch1)/2;
		audioBlock.buffer[audioBlock.size++] = ch0;
		if( !--numTicks ) {                                             // Decrementa el contador de ticks, y si llega a cero...
			numTicks = 8000;
			time.numSecs++;                                             // ...actualiza y señaliza el número de segundos de grabación
			time.updated = TRUE;
		}
	} else {
		audioBlock.full = TRUE;                                         // Si el buffer esta completo lo señaliza
		numTicks = 8000;
	}

	I_ISPC = BIT_TIMER0;
}

void timer0_playing(void){
	static uint16 numTicks = 8000;
	int16 ch0;
	//Es igual por que aun queda una muestra que reproducir ?¿

	switch(irAtras){

		case FALSE:
			if( audioBlock.index <= grabaciones[numSong].fin ) {                          // Si el sistema esta reproducciendo sonido y quedan muestras por enviar...
				ch0 = audioBlock.buffer[audioBlock.index++];                    // ...lee las muestras del audio buffer y
				iis_putSample( ch0, ch0 );
				iis_putSample( ch0, ch0 );    									// ...las envia
				if( !--numTicks ) {                                             // Decrementa el contador de ticks, y si llega a cero...
					numTicks = 8000;
					time.numSecs++;                                             // ...actualiza y señaliza el número de segundos de reproducci´pn
					time.updated = TRUE;
				}

			}else{
				audioBlock.played = TRUE;                                       // Si se ha finalizado la reproducción del buffer lo señaliza
				numTicks = 8000;
				time.numSecs=0;
				}
			break;

		case TRUE:


			if( audioBlock.index > grabaciones[numSong].ini ) {                          // Si el sistema esta reproducciendo sonido y quedan muestras por enviar...
				ch0 = audioBlock.buffer[audioBlock.index--];                    // ...lee las muestras del audio buffer y
				iis_putSample( ch0, ch0 );
				iis_putSample( ch0, ch0 );    									// ...las envia
				if( !--numTicks ) {                                             // Decrementa el contador de ticks, y si llega a cero...
					numTicks = 8000;
					if(time.numSecs!=0)
						time.numSecs--;

					time.updated = TRUE;
				}

			}else{
				numTicks = 8000;
				time.numSecs=0;
				}
			break;

		}
	I_ISPC = BIT_TIMER0;
}

void timer0_doubleSpeed(void){
	static uint16 numTicks = 8000;
	int16 ch0;

	if( audioBlock.index <= grabaciones[numSong].fin ) {                          // Si el sistema esta reproducciendo sonido y quedan muestras por enviar...
		ch0 = audioBlock.buffer[audioBlock.index++];                    // ...lee las muestras del audio buffer y
		iis_putSample( ch0, ch0 );  									// ...las envia
		if( !--numTicks ) {                                             // Decrementa el contador de ticks, y si llega a cero...
			numTicks = 8000;
			time.numSecs++;                                             // ...actualiza y señaliza el número de segundos de reproducci´pn
			time.updated = TRUE;
		}

	}else{
		audioBlock.played = TRUE;                                       // Si se ha finalizado la reproducción del buffer lo señaliza
		numTicks = 8000;
		time.numSecs=0;
		}

	I_ISPC = BIT_TIMER0;
}

void timer0_halfSpeed(void){
	static uint16 numTicks = 8000;
	int16 ch0;

	if( audioBlock.index <= grabaciones[numSong].fin ) {                          // Si el sistema esta reproducciendo sonido y quedan muestras por enviar...
		ch0 = audioBlock.buffer[audioBlock.index++];                    // ...lee las muestras del audio buffer y
		iis_putSample( ch0, ch0 );
		iis_putSample( ch0, ch0 );

		iis_putSample( ch0, ch0 );
		iis_putSample( ch0, ch0 ); // ...las envia
		if( !--numTicks ) {                                             // Decrementa el contador de ticks, y si llega a cero...
			numTicks = 8000;
			time.numSecs++;                                             // ...actualiza y señaliza el número de segundos de reproducci´pn
			time.updated = TRUE;
		}

	}else{
		audioBlock.played = TRUE;                                       // Si se ha finalizado la reproducción del buffer lo señaliza
		numTicks = 8000;
		time.numSecs=0;
		}
	I_ISPC = BIT_TIMER0;
}

void timer0_Inverso(void){
	static uint16 numTicks = 8000;
	int16 ch0;
	if( audioBlock.index > grabaciones[numSong].ini ) {                          // Si el sistema esta reproducciendo sonido y quedan muestras por enviar...
		ch0 = audioBlock.buffer[audioBlock.index--];                    // ...lee las muestras del audio buffer y
		iis_putSample( ch0, ch0 );
		iis_putSample( ch0, ch0 );										 // ...las envia
		if( !--numTicks ) {                                             // Decrementa el contador de ticks, y si llega a cero...
			numTicks = 8000;
			time.numSecs++;                                             // ...actualiza y señaliza el número de segundos de reproducci´pn
			time.updated = TRUE;
		}

	}else{
		audioBlock.played = TRUE;                                       // Si se ha finalizado la reproducción del buffer lo señaliza
		numTicks = 8000;
		time.numSecs=0;
		}

	I_ISPC = BIT_TIMER0;
}

void isr_rtc( void )
{
	rtc_time_t rtc_time;

	rtc_gettime( &rtc_time );

	lcd_puts( 170, 20, BLACK, "Fecha:" );
	lcd_putint(220 , 20, BLACK, rtc_time.mday  );

	lcd_puts( 240, 20, BLACK, "/" );
	lcd_putint( 250, 20, BLACK, rtc_time.mon );

	lcd_puts( 270, 20, BLACK, "/");
	lcd_putint( 280, 20, BLACK, rtc_time.year);

	lcd_puts( 170, 40, BLACK, "Hora:" );
	lcd_putint(220 , 40, BLACK, rtc_time.hour);

	lcd_puts( 240, 40, BLACK, ":");
	lcd_putint( 250, 40, BLACK, rtc_time.min);

	lcd_puts( 270, 40, BLACK, ":" );
	lcd_putint( 280, 40, BLACK, rtc_time.sec);

	I_ISPC = BIT_TICK;
}

void start_playing(void){
	menu_reproducir();
	clean();

	if(numSong!=7)
		menu_modo_reproduccion();

	clean();
	key=8;		//Para que no se cuele en el while.
}

void start_recording(void){
	if(numGrabaciones<MAX_GRABS && ! audioBlock.full)
		grabar();
	else
		lcd_puts( 30, 100, BLACK, "-> No queremos grabar" );
	clean();
	key=8;		//Para que no se cuele en el while.
}

void start_erased(void){
	menu_reproducir();
	clean();

	if(numSong!=7)
		borrar();

	clean();
	key=8;		//Para que no se cuele en el while.
}

void grabar(void){
	grabaciones[numGrabaciones].ini = audioBlock.size;
	time.numSecs=0;
	lcd_puts_x2( 50, 100, BLACK, "GRABANDO:0" );
	lcd_puts( 50, 132, BLACK, "KEYPAD 3 para" );
	timer0_open_tick( timer0_recording, SPS );                                   // Inicia la generación de interrupciones periodicas para muestreo del audio codec
	while( keypad_status(3)==KEY_UP && !audioBlock.full )                   // Espera la presión de un pulsador o el llenado completo del buffer de audio
		if( time.updated )                                                  // Si ha transcurrido un segundo de grabación...
		{
			lcd_puts_x2( 176, 50, BLACK, "  " );                            // ...actualiza en pantalla la cuenta de segundos
			lcd_putint_x2( 176, 50, BLACK, time.numSecs );
			time.updated = FALSE;
		}
	timer0_close();                                                         // Finaliza la generación de interrupciones periódicas
	grabaciones[numGrabaciones].fin = audioBlock.size-1;
	numGrabaciones++;
	if( keypad_status(3)==KEY_DOWN )                                              // Si finalizó la toma de muestras por presión de un pulsador...
		keypad_wait_keyup(3);                                           // ...espera su depresión

}

void reproducir( void (*fun)(void)){
	time.numSecs = 0;
	time.updated = FALSE;
	lcd_puts_x2( 50, 100, BLACK, "REPRODUCIENDO:0" );
	lcd_puts( 50, 132, BLACK, "KEYPAD 3 para" );
	timer0_open_tick( fun, SPS );                                   // Inicia la generación de interrupciones periodicas para muestreo del audio codec
	while( keypad_status(3)==KEY_UP && !audioBlock.played ){                   // Espera la presión de un pulsador o la reproducción completa del buffer de audio

		if(keypad_status(7)==KEY_DOWN)
			irAtras=TRUE;
		else
			irAtras=FALSE;

		if( time.updated )                                                  // Si ha transcurrido un segundo de reproducción...
		{
			lcd_puts_x2( 256, 50, BLACK, "  " );                            // ...actualiza en pantalla la cuenta de segundos
			lcd_putint_x2( 256, 50, BLACK, time.numSecs );
			time.updated = FALSE;
		}

	}
	timer0_close();// Finaliza la generación de interrupciones periódicas

	audioBlock.played =FALSE;
	if( keypad_status(3)==KEY_DOWN )                                              // Si finalizó la reproducción de muestras por presión de un pulsador...
		keypad_wait_keyup(3);                                        // ...espera su depresión
}

void borrar(void){
	uint32 i,j;
	uint32 aux;
	uint32 auxSize;
	auxSize = audioBlock.size - (grabaciones[numSong].fin-grabaciones[numSong].ini)-1;
	if(numSong < numGrabaciones)
	{
		i = grabaciones[numSong+1].ini;
		j = grabaciones[numSong].ini;
		while (i < audioBlock.size){
			audioBlock.buffer[j]=audioBlock.buffer[i];
			i++;
			j++;
		}
		i=numSong+1;
		j=numSong;
		
		while(i < numGrabaciones){
			if(j != numSong) grabaciones[j].ini=grabaciones[j-1].fin+1;
			aux = grabaciones[i].fin-grabaciones[i].ini;
			grabaciones[j].fin = grabaciones[j].ini+aux;
			i++;
			j++;
		}
	}
	numGrabaciones--;
	audioBlock.size = auxSize;
	audioBlock.full=FALSE;
}

void esperar(void){
	state = waiting;
	rtc_open( isr_rtc, 127 );
	print_menu();

	keypad_open( isr_keypad );
	while(key!=1 && key!=2 && key!=0 && key!=3);//key > 3
	keypad_close();
	rtc_close();
	clean();
	//Cosa rara para seleccionar el state.
	state = key;
}

void init_program(void){
	sys_init();
	timers_init();
	pbs_init();
	lcd_init();
	uda1341ts_init();
	iis_init( IIS_POLLING );
	rtc_init();

	lcd_clear();
	lcd_on();

	audioBlock.buffer  = BUFFER;
	audioBlock.maxsize = MAXBYTES;
	audioBlock.size    = 0;
	audioBlock.index   = 0;
	audioBlock.full    = FALSE;
	audioBlock.played  = FALSE;

	rtc_time_t rtc_time;

	rtc_time.sec =0; 			/* segundo (0-59)*/
	rtc_time.min =10;  			/* minuto (0-59) */
	rtc_time.hour =19; 			/* hora (0-23) */
	rtc_time.mday =12;			/* día del mes (1-31) */
	rtc_time.wday =6;			/* día de la semana (1-7) comenzado por el domingo */
	rtc_time.mon =1; 			/* mes (1-12) */
	rtc_time.year=18;			//(0-99)

	rtc_puttime( &rtc_time );

	time.numSecs = 0;
	time.updated = FALSE;
	//Inicializo en waiting.
	key=5;
	numGrabaciones = 0;
	numSong=MAX_GRABS+1;
	goOut=0;
	irAtras=FALSE;

	lcd_draw_box(0, 0, 310, 230, BLACK, 10);
}

void print_menu(void){
	//Cajas

	lcd_draw_box( 35,35,55,55, BLACK, 1 );
	lcd_draw_box( 35,57,55,75, BLACK, 1 );
	lcd_draw_box( 35,77,55,95, BLACK, 1 );


	lcd_putint( 40, 39, BLACK, 1 );
	lcd_puts( 60, 40, BLACK, "Grabar" );

	lcd_putint( 40, 59, BLACK, 2 );
	lcd_puts( 60, 60, BLACK, "Reproducir" );

	lcd_putint( 40, 79, BLACK, 3 );
	lcd_puts( 60, 80, BLACK, "Eliminar" );

	lcd_putint( 40, 99, BLACK, 0 );
	lcd_puts( 60, 100, BLACK, "Salir" );
}

void clean(void){
	lcd_draw_hline( 11, 300, 11, WHITE, 200 );
}

void errores_keypad(uint8 scancode){

	clean();
	switch(scancode){
		case KEYPAD_FAILURE:
			lcd_puts( 30, 100, BLACK, "-> Error KEYPAD_TIMEOUT" );
			break;
		case KEYPAD_TIMEOUT:
			lcd_puts( 30, 100, BLACK, "-> Error KEYPAD_FAILURE" );
			break;
		default:
			lcd_puts( 30, 100, BLACK, "-> Error tecla pulsada incorrecta" );
			break;
	}
}

void menu_reproducir(void){
	lcd_puts( 11, 30, BLACK, "-> Elige grabacion,7 salir" );
	int i;
	for(i=0; i < numGrabaciones ; i++){

		lcd_puts( 60, 50 + (i*20), BLACK, "Grabacion " );
		lcd_putint( 150, 50 + (i*20), BLACK, i );
	}

	key=8;
	keypad_open( isr_keypad );
	while(key >= numGrabaciones && key !=7);
	keypad_close();
	numSong = key;

}

void menu_modo_reproduccion(void){
	lcd_puts( 11, 30, BLACK, "-> Elige modo de reproduccion" );
	key=8;

	lcd_puts( 60, 50, BLACK, "Reproduccion normal,con retroceso(7) " );
	lcd_putint(50, 50, BLACK, 0 );

	lcd_puts( 60, 70, BLACK, "Reproduccion doble velocidad " );
	lcd_putint( 50, 70, BLACK, 1 );

	lcd_puts( 60, 90, BLACK, "Reproduccion lenta 1/2 " );
	lcd_putint( 50, 90, BLACK, 2 );

	lcd_puts( 60, 110, BLACK, "Reproduccion inversa " );
	lcd_putint( 50, 110, BLACK, 3 );

	keypad_open( isr_keypad );
	while(key >=4 && key !=7);
	keypad_close();
	clean();
	audioBlock.index=grabaciones[numSong].ini;
		switch(key){
			case 0:
				reproducir(timer0_playing);
				break;
			case 1:
				reproducir(timer0_doubleSpeed);
				break;
			case 2:
				reproducir(timer0_halfSpeed);
				break;
			case 3:
				audioBlock.index=grabaciones[numSong].fin;
				reproducir(timer0_Inverso);
				break;
			default:
				break;
		}
}



