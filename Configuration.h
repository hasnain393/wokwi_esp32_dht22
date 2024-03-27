
#include <pgmspace.h>
 
#define SECRET
#define My_THING_NAME   "ESP32_DHT11"                         
 
const char NETWORK_USER  [] = "Wokwi-GUEST";               
const char NETWORK_SECRET  [] = "";           
const char AWS_CORE_THING[] = "a1v3e7acmb8s7z-ats.iot.us-east-1.amazonaws.com";       

static const char AWS_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";
 

static const char AWS_THING_CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)KEY";
 

static const char AWS_SECRET_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
)KEY";


