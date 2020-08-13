//INCLUDE LIBRERIES
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "erbium.h"
#include "dev/leds.h"
#include "er-coap-13.h"

//DEFINITION OF THE RESOURCES
#define REST_RES_BLUE 1//Represents the air conditioning
#define REST_RES_RED 1//Represents heating
#define REST_RES_GREEN 1//Represents the ventilation
#define REST_RES_TOGGLE 1
#define REST_RES_TEMP 1 //Represents the temperature

//GLOBAL VARIABLES
bool blue = false;//Boolean variable, if it is false the Air conditioning is OFF, ON if it is true
bool red = false;//Boolean variable, if it is false the Heating is OFF, ON if it is true
bool green = false;//Boolean variable, if it is false the Ventilation is OFF, ON if it is true
int seconds = 0;//Time in seconds 
int temp = 10;//Intitialize the temperature to 10

#if REST_RES_BLUE
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
/* Requesting the Air conditioning status using GET */
RESOURCE(AirCond, METHOD_GET, "services/AirCond", "title=\"AIR CONDITIONING unit\";rt=\"AirCond\"");
/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void AirCond_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", blue);
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
}
#endif
#if REST_RES_RED
/* Requesting the Heating status using GET */
RESOURCE(Heating, METHOD_GET, "services/Heating", "title=\"HEATING unit\";rt=\"Heating\"");

void Heating_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", red);
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
}
#endif
#if REST_RES_GREEN
/* Requesting the Ventilation status using GET */
RESOURCE(Ventilation, METHOD_GET, "services/Ventilation", "title=\"VENTILATION unit\";rt=\"Ventilation\"");

void Ventilation_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", green);
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
}
#endif
#if REST_RES_TEMP
/*
 * Example for a periodic resource.
 * It takes an additional period parameter, which defines the interval to call [name]_periodic_handler().
 * A default post_handler takes care of subscriptions by managing a list of subscribers to notify.
 */
PERIODIC_RESOURCE(temperature, METHOD_GET, "sensors/temperature", "title=\"TEMPERATURE\"", 5*CLOCK_SECOND);
//The mote sense the temperature every 5 seconds
void temperature_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", temp);
	REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
}
/*
 * Additionally, a handler function named [resource name]_handler must be implemented for each PERIODIC_RESOURCE.
 * It will be called by the REST manager process with the defined period.
 */
void temperature_periodic_handler(resource_t *r){
	static char content[4];//Number of simulated motes
	
	/* Build notification. */
	coap_packet_t notification[1];/* This way the packet can be treated as pointer as usual. */
	coap_init_message(notification, COAP_TYPE_NON, REST.status.OK, 0);
	coap_set_payload(notification, content, snprintf(content, sizeof(content),"%d", temp));
	
	/* Notify the registered observers with the given message type, observe option, and payload. */
	REST.notify_subscribers(r, temp, notification);
	
	/* Changing the temperature value after 20 seconds (4 because it has a period of 5 seconds)*/
	seconds++;
	if (seconds == 4){
		if (green == true){
			if(blue == true){
				temp+= -2;//If the ventilation and air conditioning are ON the temperature decreases 2C degrees
			}else{
			   if (red == true){ 
				temp+= 2;//If the ventilation and heating are ON the temperature increases 2C degrees
			   }
			};   
		}else{
			if(blue == true){
				temp+= -1;//If the ventilation is OFF and air conditioning are ON the temperature decreases 1C degrees
			}else{
			   if (red == true){ 
				temp+= 1;//If the ventilation is OFF and heating are ON the temperature increases 1C degrees
			   }
			};		
		};
		seconds = 0;
	}	
}
#endif

#if REST_RES_TOGGLE
/* A simple actuator example. Toggles the leds */
/* Actuator for the Air Conditioning using Post Method */
RESOURCE(toggleAirCond, METHOD_POST, "actuators/toggleAirCond", "title=\"Toggle Air Conditioning\";rt=\"AirCond\"");

void toggleAirCond_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    if (blue == true){
    	blue = false;
    	leds_toggle(LEDS_BLUE);
    }else{	
    	if (red == true){
    		REST.set_response_status(response, REST.status.BAD_REQUEST);
    	}else{
    		blue = true;
    		leds_toggle(LEDS_BLUE);
    	};
    };
}
#endif

#if REST_RES_TOGGLE
/* Actuator for the Heating using Post Method */
RESOURCE(toggleHeating, METHOD_POST, "actuators/toggleHeating", "title=\"Toggle Heating\";rt=\"Heating\"");

void toggleHeating_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    if (red == true){
    	red = false;
    	leds_toggle(LEDS_RED);
    }else{	
    	if (blue == true){
    		REST.set_response_status(response, REST.status.BAD_REQUEST);
    	}else{
    		red = true;
    		leds_toggle(LEDS_RED);
    	};
    };
}
#endif

#if REST_RES_TOGGLE
/* Actuator for the Ventilation using Post Method */
RESOURCE(toggleVentilation, METHOD_POST, "actuators/toggleVentilation", "title=\"Toggle Ventilation\";rt=\"Ventilation\"");

void toggleVentilation_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
    if (green == true)
    	green = false;
    else
    	green = true;
    leds_toggle(LEDS_GREEN);
}
#endif
PROCESS(sensormote, "Sensor Mote");
AUTOSTART_PROCESSES(&sensormote);

PROCESS_THREAD(sensormote, ev, data){
	PROCESS_BEGIN();
	rest_init_engine();
	temp = random_rand()%21 + 10;
	#if REST_RES_BLUE
	  rest_activate_resource(&resource_AirCond);
	#endif
	#if REST_RES_RED
	  rest_activate_resource(&resource_Heating);
	#endif
	#if REST_RES_GREEN
	  rest_activate_resource(&resource_Ventilation);
	#endif
	#if REST_RES_TEMP
	  rest_activate_periodic_resource(&periodic_resource_temperature);
	#endif
	#if REST_RES_TOGGLE
	  rest_activate_resource(&resource_toggleAirCond);
	  rest_activate_resource(&resource_toggleHeating);
	  rest_activate_resource(&resource_toggleVentilation);
	#endif
	/* Define application-specific events here. */
	while(1){
	    PROCESS_WAIT_EVENT();
	}
	PROCESS_END();
}
