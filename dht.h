/*
 * dht.h
 *
 *  Created on: 26.09.2018
 *      Author: HorochovPL
 */

#ifndef DHT11_DHT_H_
#define DHT11_DHT_H_

#ifndef _err_t
		typedef uint8_t err_t;
	#define _err_t
#endif

enum errorDht
{
	_dht_ok=0,
	_dht_timeuot,
	_dht_badSum,	//when checksum went wrong
};

void dht_init(void);
err_t dht_getRaw(uint8_t* result);
err_t dht_get(uint8_t *hummid ,int8_t *temp);

#endif /* DHT11_DHT_H_ */
