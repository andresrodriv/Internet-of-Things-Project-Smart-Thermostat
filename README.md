# Internet-of-Things-Project-Smart-Thermostat
The topology used in this project consists in the creation of 5 sky motes; The first one, rpl-border-router, included in the examples folder of Contiki 2.7, is already working for our purposes. The rest of the motes are defined as sensor-mote, this mote is created using two main methods, GET and POST, for each resource that I use in this application. The resources are defined as BLUE, RED, GREEN, TOOGLE and TEMP, corresponding to the air conditioning, the heating, the ventilation, the actuators and temperature systems, respectively. The GET method is implemented four times, one per each service (AirCond, Heating and Ventilation), and one for the temperature sensor, in the temperature one, it is defined as a PERIODIC_RESOURCE and configured in order to have the temperature value every 20 seconds. The temperature handler function is implemented according to the requirements, taking in to account the value of the Boolean variables for the three main services and according to this increase or decrease the temperature value. The POST method is implemented three times, one per each actuator (toogleAirCond, toogleHeating and toggleVentilation), in the functions of the AirCond and the Heating system, the implemented algorithm is implemented in order to make them mutually exclusive. In the main process the temp value is initialized with a random value between 10 and 30.