/*
 * dht.c
 *
 *  Created on: 26.09.2018
 *      Author: HorochovPL
 */

#include <stdint.h>
//#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../avrElapsedTime/elapsedTime.h"
#include "dht.h"

/* * * * * * * * * * * * * * * * * * * * *
 *										 *
 *	 		I N I T 					 *
 * 										 *
 * * * * * * * * * * * * * * * * * * * * */
#define pin PINC
#define ddr DDRC
#define port PORTC
#define mask (1<<PC5)
#define isPinHi	(pin&mask)

#define timeLoBitMax 30					//max '0' time && min '1' time. Between 30 and 70
#define timeStartSig 20					//from datasheet
#define timeTransmMax (timeStartSig+10)	//timeStartSig+4 plus safety margin




void dht_init(void)//pinreg * dht)
{
	ddr|=mask;
	port|=mask;
}


/*
 * Starts transmission with appriopriate timing
 * Then waits for rising edge - start of data bit,
 * wait lil' bit after '0' bit passed and check data line voltage.
 * If high - set bit. And wait for another rising edge to repeat
 *
 * result is 4-byte array:
 * 8bit integral + 8bit decimal (probably empty) hummidity,
 * 8bit integral + 8bit decimal temperature.
 */
err_t dht_getRaw(uint8_t* result)//, pinreg * dht)
{
	uint8_t time=timer_GetMs();	//will store info about up to 255 ms
#define processTimeout  if( (uint8_t) (timer_GetMs()-time) >timeTransmMax) \
		return _dht_timeuot;

	//initiate trans.
	//mcu side
	port&=~mask;
	_delay_ms(timeStartSig);
	port|=mask;
	//dht side
	ddr&=~mask;
	while(isPinHi) processTimeout;	//wait till falling edge for 20-40us
	while(!isPinHi) processTimeout;	//wait till rising edge for 80us
	while(isPinHi) processTimeout;	//wait till falling edge for 80us

	//read time!
	uint8_t buf[5]={};
	for (uint8_t i = 0; i<40; ++i) {

		//low - announcement of next bit
		while(!isPinHi) processTimeout;	//wait till high
		//high for ~30us - low, 70us high
		_delay_us(timeLoBitMax);
		if(isPinHi)		//if high detected, set bit and wait till low state
		{
			buf[i>>3]|=(1<<(7-(i&7)));
			while(isPinHi) processTimeout;	//falling edge for 80us
		}
	}
	ddr|=mask;

	//check if everything went ok and copy results
	uint8_t chcksum=0;
	for (uint8_t i = 0; i < 4; ++i) {
		*(result+i) = buf[i];
		chcksum+=  buf[i];
	}

	if(chcksum != buf[4])
		return _dht_badSum;
	return _dht_ok;
#undef processTimeout
}

/*
 * Returns hummidity and temperature with 1 deg. accuracy
 *
 */
err_t dht_get(uint8_t *hummid ,int8_t *temp)
{
	uint8_t buf[4], errval;
	errval=dht_getRaw(buf);

	if(hummid)
		*hummid=buf[0];

	if(temp)
		*temp=buf[2]+(buf[3]<5?0:1);

	return errval;
}
