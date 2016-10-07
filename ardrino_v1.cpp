
/** RF24Mesh_Example.ino by TMRh20

   This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
   master node.
   The nodes will refresh their network address as soon as a single write fails. This allows the
   nodes to change position in relation to each other and the master node.
*/


#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
//#include <printf.h>


/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**
   User Configuration: nodeID - A unique identifier for each radio. Allows addressing
   to change dynamically with physical changes to the mesh.

   In this example, configuration takes place below, prior to uploading the sketch to the device
   A unique value from 1-255 must be configured for each node.
   This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.

 **/
#define nodeID 1
#define RF_SUCCESS 1
#define RF_FAIL 2
#define RETRY_MAX 9

uint32_t displayTimer = 0;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};


struct rf_payload{
    char protocol;
    char len;
    char data;
};
enum rf_protocol{
    rf_one_wire_onoff,
    rf_onoff,
    rf_one_wire_analog,
    rf_one_wire,
    rf_i2c,
    rf_spi,
    rf_serial    
};
struct one_wire_onoff_s{
    char pin;
    char data;
    char reserved;
};

void setup() {

  Serial.begin(115200);
  //printf_begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
  
//  attachInterrupt(1, interrupt_1, RISING); 
//  attachInterrupt(0, interrupt_0, RISING);  

}

void loop() {
  mesh.update();
  
  while (network.available()) {
    
    RF24NetworkHeader header;
    network.peek(header);    
    
    rf_payload payload_buf;
    char message_buf[32 - sizeof(RF24NetworkHeader)];
    
    switch(header.type){
      // Display the incoming millis() values from the sensor nodes
      case 'S': 
        network.read(header,message_buf,32 - sizeof(RF24NetworkHeader)); 
        //Serial.println(dat); 
        handle_S_message(message_buf, reinterpret_cast<rf_payload*>(message_buf)->len);
        break;
      case 'P':
        network.read(header,message_buf,32 - sizeof(RF24NetworkHeader)); 
        //Serial.println(dat); 
        handle_P_message(message_buf, reinterpret_cast<rf_payload*>(message_buf)->len);
        break;
      case 'G':
        network.read(header,message_buf,32 - sizeof(RF24NetworkHeader)); 
        //Serial.println(dat); 
        handle_G_message(message_buf, reinterpret_cast<rf_payload*>(message_buf)->len);
        break;         
      default: 
        network.read(header,0,0); 
        Serial.println(header.type);
        break;
    }
    Serial.print("Received packet #");
  }
}

void send_message(char* payload, char type, char len)
{
  for (int i = 0; i < RETRY_MAX; i++)
  {
      if (!mesh.write(payload, type, len))   
      {
        // If a write fails, check connectivity to the mesh network
        if ( ! mesh.checkConnection() ) {
          //refresh the network address
          Serial.println("Renewing Address");
          mesh.renewAddress();
        } 
        else 
        {
          Serial.println("Send fail, Test OK");
        }
      } 
      else 
      {
        Serial.print("Send OK: "); Serial.println(displayTimer);
        break;
      }       
  }
}

void handle_S_message(char *payload_p, char len)
{
  rf_payload* rf_payload_p = NULL;
  rf_payload_p = reinterpret_cast<rf_payload*>(payload_p);
  switch(rf_payload_p -> protocol){
    case rf_one_wire_onoff:
      one_wire_onoff_s tmp_one_wire_onoff_s;
      memcpy(&tmp_one_wire_onoff_s, &(rf_payload_p->data), sizeof(tmp_one_wire_onoff_s));
      if ( tmp_one_wire_onoff_s.pin == 2)
      {
        attachInterrupt(0, interrupt_0, RISING);
      }
      else if ( tmp_one_wire_onoff_s.pin == 3)
      {// to do:four type here
        attachInterrupt(1, interrupt_0, RISING);
      }
      else
      {
        Serial.println("wrong pin!");
      }
      send_message(payload_p, 'S', len);
      break;
    default:
      Serial.println("unknow protocol!");
      break;
  }
}
void handle_G_message(char *payload_p, char len)
{
  rf_payload* rf_payload_p = NULL;
  rf_payload_p = reinterpret_cast<rf_payload*>(payload_p);
  switch(rf_payload_p -> protocol){
    case rf_one_wire_onoff:
      one_wire_onoff_s tmp_one_wire_onoff_s;
      memcpy(&tmp_one_wire_onoff_s, &(rf_payload_p->data), sizeof(tmp_one_wire_onoff_s));
      tmp_one_wire_onoff_s.data = digitalRead(tmp_one_wire_onoff_s.pin);
      
      memcpy(&(rf_payload_p->data), &tmp_one_wire_onoff_s, sizeof(tmp_one_wire_onoff_s));   

      send_message( payload_p, 'G', len); 
      break;
      
    default:
      Serial.println("unknow protocol!");
      break;       
  }
}
void handle_P_message(char *payload_p, char len)
{
  rf_payload* rf_payload_p = NULL;
  rf_payload_p = reinterpret_cast<rf_payload*>(payload_p);
  switch(rf_payload_p -> protocol){
    case rf_one_wire_onoff:
      one_wire_onoff_s tmp_one_wire_onoff_s;
      memcpy(&tmp_one_wire_onoff_s, &(rf_payload_p->data), sizeof(tmp_one_wire_onoff_s));
      digitalWrite(tmp_one_wire_onoff_s.pin,tmp_one_wire_onoff_s.data);
      
      send_message(payload_p, 'P', len);    
      
      break;
    default:
      Serial.println("unknow protocol!");
      break;
  }
}
void interrupt_0(void)
{
  one_wire_onoff_s payload_buf;
  payload_buf.pin = 2;
  payload_buf.data = 1;
  
  char buf[32];//Max size is 32.
  rf_payload tx_payload;
  tx_payload.protocol = rf_one_wire_onoff;
  tx_payload.len = sizeof(rf_payload) + sizeof(one_wire_onoff_s) - 1;
  //tx_payload.data = 1;
  memcpy(buf, &tx_payload, sizeof(rf_payload));
  memcpy(buf + sizeof(rf_payload) - 1, &payload_buf, sizeof(one_wire_onoff_s));

  send_message(buf, 'R', tx_payload.len);
}
void interrupt_1(void)
{
  one_wire_onoff_s payload_buf;
  payload_buf.pin = 3;
  payload_buf.data = 1;
  
  char buf[32];//Max size is 32.
  rf_payload tx_payload;
  tx_payload.protocol = rf_one_wire_onoff;
  tx_payload.len = sizeof(rf_payload) + sizeof(one_wire_onoff_s) - 1;
  //tx_payload.data = 1;
  memcpy(buf, &tx_payload, sizeof(rf_payload));
  memcpy(buf + sizeof(one_wire_onoff_s) - 1, &payload_buf, sizeof(one_wire_onoff_s));
  send_message(buf, 'R', tx_payload.len);
}





