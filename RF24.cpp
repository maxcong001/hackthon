/*
	TMRh20 2014 - Optimized RF24 Library Fork
*/

/**
 * Example using Dynamic Payloads
 *
 * This is an example of how to use payloads of a varying (dynamic) size.
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <RF24/RF24.h>
#define on_off_pin 17

using namespace std;
RF24 radio(22,0); // CE GPIO, CSN SPI-BUS

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

#define MAX_BODY_LEN 24
#define RF24_MESSAGE_HEADER_LEN (sizeof(RF24_MESSAGE) - sizeof(int))

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


RF24_RETURN_TYPE send_rf24_message(char* message, unsigned char len)
{
	radio.write( message, len );
	return RF24_SUCCESS;
}
RF24_RETURN_TYPE set_pin(unsigned char pin_num, bool level)
{
	digitalWrite(pin_num, level);
	return RF24_SUCCESS;
}

RF24_RETURN_TYPE handle_set_message(unsigned char protol, unsigned mask, char* message, unsigned char len)
{
	printf("enter handle_set_message ");
	// just for test
	if ( protol == one_pin)
	{
		if ( (mask >> 4) == 1)
		{
			pinMode(on_off_pin,OUTPUT);
			if (message[0] == 1)
			{
				set_pin(on_off_pin, HIGH);
				printf("set pin%d to 1\n",on_off_pin);
			}
			else
			{
				set_pin(on_off_pin,LOW);
				printf("set pin%d to 0\n", on_off_pin);
			}
		}
	}
	return RF24_SUCCESS;
	
}
RF24_RETURN_TYPE handle_get_message(unsigned char protol, unsigned mask, char* message, unsigned char len)
{
	//to be done
}
RF24_RETURN_TYPE handle_return_message(unsigned char protol, unsigned mask, char* message, unsigned char len)
{
	//to be done
}
RF24_RETURN_TYPE hanlde_RF24_mesage(char* message, unsigned char len)
{
	char m_message[MAX_BODY_LEN];
	RF24_MESSAGE* rf24_message = (RF24_MESSAGE*)message;
	unsigned char m_type =  (rf24_message->message_type)&0x0f;
	unsigned char m_proto = ((rf24_message->message_type)&0xf0)>>4;
	unsigned char m_len = rf24_message->message_body_len;
	if ( m_len > MAX_BODY_LEN)
	{
		printf("message len is larger than %d", MAX_BODY_LEN);
		return RF24_MESSAGE_LEN_ERROR;
	}
	unsigned short int m_id = rf24_message->message_id;
	int m_pin_mask = rf24_message->pin_mask;
	memcpy(m_message, &(rf24_message->message_body), m_len);
	m_message[m_len] = 0;
	printf("message type is %d\n", m_type);
	switch(m_type){
		case message_set:
			handle_set_message(m_proto, m_pin_mask, m_message, m_len);
			break;
		case message_get:
			handle_get_message(m_proto, m_pin_mask, m_message, m_len);
			break;
		case message_return:
			handle_return_message(m_proto, m_pin_mask, m_message, m_len);
			break;

		default:
			printf("unknown message type");

	}
	return RF24_SUCCESS;

}
RF24_RETURN_TYPE send_RF24_mesage(char* message, unsigned char len, unsigned char m_proto, unsigned char m_type, unsigned int m_pin_mask)
{
	if ( len > MAX_BODY_LEN)
	{
		printf("message len is larger than %d", MAX_BODY_LEN);
		return RF24_MESSAGE_LEN_ERROR;
	}
	char message_buf[32];
	RF24_MESSAGE* rf24_message = (RF24_MESSAGE*)message_buf;
	rf24_message->message_type = (m_type&0x0f)|((m_proto<<4)&0xf0);
	rf24_message->message_body_len = len;
	rf24_message->pin_mask = m_pin_mask;
	memcpy((void*)&(rf24_message->message_body), (void*)message, (int)len);
	send_rf24_message(message_buf, RF24_MESSAGE_HEADER_LEN + len);
	return RF24_SUCCESS;
}
RF24_RETURN_TYPE send_RF24_one_pin(char on_off, char pin_num)
{
	if ( pin_num > 32)
	{
		printf("pin number is out of range!");
		return RF24_PIN_NUM_ERROR;
	}
	char tmp_message = on_off;
	unsigned int pin_mask = 1<<pin_num;

	send_RF24_mesage(&tmp_message, sizeof(char), one_pin, message_set, pin_mask);
	return RF24_SUCCESS;
}


//
// Hardware configuration
// Configure the appropriate pins for your connections

/****************** Raspberry Pi ***********************/

//RF24 radio(22,0); // CE GPIO, CSN SPI-BUS

int interruptPin = 23; // GPIO pin for interrupts

/**************************************************************/

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };



const int min_payload_size = 4;
const int max_payload_size = 32;
const int payload_size_increments_by = 1;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char
bool role_ping_out = 1, role_pong_back = 0;
bool role = 0;

void intHandler(){
  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      uint8_t len=0;

      while (radio.available())
      {
        // Fetch the payload, and see if this was the last one.
	len = radio.getDynamicPayloadSize();
	radio.read( receive_payload, len );

	// Put a zero at the end for easy printing
	receive_payload[len] = 0;

	// Spew it
	printf("Got payload size=%i value=%s, %d \n\r",len,receive_payload,receive_payload[len-1]);
	hanlde_RF24_mesage(receive_payload, len);
      }

      radio.startListening();
    }
  }
}


int main(int argc, char** argv){

  int flag;
  // Print preamble:
  cout << "RF24/examples/pingpair_dyn/\n";

  // Setup and configure rf radio
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setRetries(5,15);
  radio.printDetails();

  pinMode(4,OUTPUT);
  
/********* Role chooser ***********/

  printf("\n ************ Role Setup ***********\n");
  string input = "";
  char myChar = {0};
  cout << "Choose a role: Enter 0 for receiver, 1 for transmitter (CTRL+C to exit) \n>";
  getline(cin,input);

  if(input.length() == 1) {
	myChar = input[0];
	if(myChar == '0'){
		cout << "Role: Pong Back, awaiting transmission " << endl << endl;
	}else{  cout << "Role: Ping Out, starting transmission " << endl << endl;
		role = role_ping_out;
	}
  }
/***********************************/

    if ( role == role_ping_out )    {
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
    } else {
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
      radio.startListening();
    }
    attachInterrupt(interruptPin, INT_EDGE_FALLING, intHandler); //Attach interrupt to bcm pin 23

// forever loop
	while (1)
	{

if (role == role_ping_out)
  {
    // The payload will always be the same, what will change is how much of it we send.
    static char send_payload[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ789012";

    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    if (flag == 0)
    {
       flag = 1;
       send_RF24_one_pin(1,4);
    }
    else
    {
	flag = 0;
	send_RF24_one_pin(0,4);
    }

    printf("send out RF message\n");
    fflush(stdout);
    // Try again 1s later
    delay(2000);
  }


}
}


