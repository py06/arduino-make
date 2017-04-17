// 
//    FILE: dht.h
// VERSION: 0.1.00
// PURPOSE: DHT Temperature & Humidity Sensor library for Arduino
//
//     URL: http://arduino.cc/playground/Main/DHTLib
//
// HISTORY:
// see dht.cpp file
//

#ifndef dht_h
#define dht_h

#include <Arduino.h>
#define DHT_LIB_VERSION "0.1.00"

struct dht;

int dht_init(struct dht *dht);
int read11(struct dht *, uint8_t);
int read22(struct dht *, uint8_t);
int dht_set_ops(struct dht *dht, union _dht_ops *ops);

union _dht_ops {
	int (*read11)(struct dht *, uint8_t);
	int (*read22)(struct dht *, uint8_t);
};

struct dht {
	double humidity;
	double temperature;
	union _dht_ops ops;
};

#endif
