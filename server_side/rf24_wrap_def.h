/*
 * rf24_wrap_def.h
 *
 *  Created on: 2016Äê9ÔÂ17ÈÕ
 *      Author: mcong
 */

#define on_off_pin 17
#define MAX_BODY_LEN 24
#define RF24_MESSAGE_HEADER_LEN (sizeof(RF24_MESSAGE) - sizeof(int))
enum MESSAGE_TYPE{
	message_get,
	message_set,
	message_return,
	message_max
};
enum MESSAGE_BUS{
	one_pin,
	spi_bus,
	i2c_bus,

};

struct RF24_MESSAGE{
	// xxxx(message type) xxxx(message bus type)
	unsigned char message_type;
	unsigned char message_body_len;
	// this is used for check response, did not add this function as time limit
	unsigned short int message_id;
	unsigned int pin_mask;
	// sizeof header is 8, we got 32byte max, so the message body should less than 24 byte.
	// sizeof the whole struct should be 8 + message_body_len
	char message_body;
};
enum RF24_RETURN_TYPE{
	RF24_ERROR,
	RF24_SUCCESS,
	RF24_MESSAGE_LEN_ERROR,
	RF24_PIN_NUM_ERROR,
	RF23_ERROR_MAX
};


