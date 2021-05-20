#define TEMPERATURE_THRESHOLD 20 // *C
#define HUMIDITY_THRESHOLD    40 // 0-100%
#define MOISTURE_THRESHOLD    30 // 0-100%
#define CO2_THRESHOLD       1000 // ppm

#define DHT_TYPE DHT11

const float v400 = 4.535;   // 400 ppm
const float v40000 = 3.206; // 40k ppm

const uint32_t LAMP_PERIOD = 12 * 60 * 60 * 1000; // 12 h
const int SEND_PERIOD      =       2 * 60 * 1000  // 2 min