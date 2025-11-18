#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

// ==== CONFIGURACIÓN WiFi ====
const char* WIFI_SSID = "SMOVILES";
const char* WIFI_PASSWORD = "roaming-2022";

// ==== CONFIGURACIÓN AWS IoT ====
const char* AWS_ENDPOINT = "a340n6n8bwxje6-ats.iot.us-east-1.amazonaws.com"; //endpoint MQTT
const int AWS_PORT = 8883;
const char* MQTT_TOPIC = "ShowerTopic/shower01";

// ==== CERTIFICADOS ====
// Certificado raíz de Amazon
static const char AWS_ROOT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Certificado del Thing
static const char AWS_CLIENT_CERT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUZ0/vuMTeGmi1ngWtMnqDjSHBP9UwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MTExODE1MDg0
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANBx85pqBwcitsSJuanq
6sMkRARAMM9eWilItNK11f1QBFVzLSGYFPbE4HLEpA+8K6YQhFbMopUEvOP9Yto/
OnJGtfIwc2yUsGGtqDmtwWSiq18gwMyGw2YLRZ2VAHo0Nshv4/6Bx9Yzt2BxTKTz
sjyXSZ3q+qQMpIlvL0Qk1XVeC/A1Dwzz27YVsHI+kMPRDa+HDoq/2rlQY2d19h7d
ep6uCdSP6Fc4D+D3Sdb4ORAyJ4Uv5nWqZySCBMMdMaevjNg2Oanbg6gaFpCwRDrF
m1ulEtEETzPWClUXFhV/fcdpjzuEvoxFPB5jJ6I37qx47G57gD1nLhQfMhLzFFa7
JY0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUCSj/gaM9mI2dJnyudqpFNo0vHz0wHQYD
VR0OBBYEFE7StsLthxM4yWA50Kn/hoILgnyHMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQA6xwee8vZKK+rVKgmV7R+DwyG8
dR6q2ZnEkox7rrcQV2TCGvvr3uV2+NvwItBBYkIHuHCML5hrrNizeMchfa1Em+1Z
VlOC/Rq1NmxpQrxkB1esUuc3ASeQwpk6HP8flBZdZeEN6zj38q6LShj8N0DDxiCA
Dv8ymcwPkX4JbJc+G5euNoXvavOYuMVszJ9hyWF5Y+TxOelkg3zB/mi6i7I9rZts
IEmrXm4A7T59E+B5AP2i2NF48+hR8x0/4r/XDuCwHTXVzcj0619vwQsvcRmMHjoE
jvxaPxYm6ediEH52wdtvRwruWnLMThHXaRwmaa7d2ALfdLVeI9jZa11Elh8s
-----END CERTIFICATE-----
)KEY";

// Llave privada del Thing
static const char AWS_PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA0HHzmmoHByK2xIm5qerqwyREBEAwz15aKUi00rXV/VAEVXMt
IZgU9sTgcsSkD7wrphCEVsyilQS84/1i2j86cka18jBzbJSwYa2oOa3BZKKrXyDA
zIbDZgtFnZUAejQ2yG/j/oHH1jO3YHFMpPOyPJdJner6pAykiW8vRCTVdV4L8DUP
DPPbthWwcj6Qw9ENr4cOir/auVBjZ3X2Ht16nq4J1I/oVzgP4PdJ1vg5EDInhS/m
dapnJIIEwx0xp6+M2DY5qduDqBoWkLBEOsWbW6US0QRPM9YKVRcWFX99x2mPO4S+
jEU8HmMnojfurHjsbnuAPWcuFB8yEvMUVrsljQIDAQABAoIBAHElSawJ3aK77yWj
9MoNID2o09rS1Sva9Wnvc2flGDEWXCPeDyNrIAYLPCJQJxX7eF3+HTfYgHAPUFEZ
W61IIPui8mRydFT+KPf8gZI9hPqRaiCK8NyUUkBz4ef113p+GTaXtdSSsdzN2b5a
AdaQCbHmFSnJQSAHVNDdL3T34X6BAUZVd4dk1bki0SHCk7GHb5icKWtjNadUjS5k
VPrKc+USeJ2lfRqTqL7Bmaez+P3k0/SmoUcG9+SxQXdj9Pa5iXQaqAEh7Zv3eQDf
DAYIAvWllnqCqpCTyhSTrjZTWI5Azs1zswZxBBuj48n2u6aGTyy+jXz+J8kFlgdd
J4vNGOkCgYEA8XkSNPjiKVuSul1BX/1npjulyVA3Gk/1XYc+ZY3+nxEjW6zOvSaT
OUr+9sYeyVnIVIHhAE8Ie8ph7BQiIGzOtsT+sY407D59sQHYjWQ1HJVoQFeD/y92
jyTQeRb5ue1Yc4Oo4IToFlk6GsdnxevIELSujE4rzaU/OyZWz1u/Qn8CgYEA3Pw3
88qqImZPPzh/XEwcOSbLmFWaHavY+5QR1P6LtP+sAFJIo+ZUaumLg5LcG79aOU3t
5NRKgcGoMOp4Hxl4NzLOyDMRYPOzEoZk05ne8RjCq/z/YGMHtfcNhtvEiffKv/4i
TQmQzGNG2MxJjHDTtG00mzGDGn3GkY0Bt0O/efMCgYEA1uwWy3U1/u+VgjCcjm5C
Qc/XOH4f/VhroGpOnB8tILElUmRVwj9CTH02v7gJENfLmYPcGuU9PMkeLPyif0+5
ES33L1vOOPNJ9jEZaIdnSmCk4JfEUW4STUfMEerkLJtW3cGynttMow+ZytnuwFei
w1O+A/1TuX6579Co9XjIm8sCgYEAslBKjcuQsSVA+NCL5Fv9whgN3KPmMd8CvS3j
nayv6qMLlU7QEoC+yW4eB1MvjKxBD1BUj3rFjCJJpv7rYpzYpiG3PVAHy+WZL2+G
WEAm5wCFxOXm403FYQ0yuwkGdBWgYn+cCfGYB3RsLG/bQRmhmkssUbYIBg5LdhMD
uW9PSAUCgYA+lUGrtUkqMNNCMK8aVUWwJ9OSZjYch5wJo+Lh6eQLIvNk8D8Gz/yY
mjqn66icLKH2G0ES7PW3Bld0BcePjGxxZ6nAhfU3lCwS9M7YL8HPBfABLr4nrWVK
F2x4AuP3cLSJVlyX0xzNci08kkzFo/KsBDHiEip0vIHgiWj2r+vK2g==
-----END RSA PRIVATE KEY-----
)KEY";

// ==== VARIABLES ====
float flowRate = 0.0;

// ==== SENSOR DE FLUJO YF-S201 ====
volatile uint32_t pulseCount = 0;
const int FLOW_SENSOR_PIN = 4;   // pin donde conectaste el cable amarillo del YF-S201
const float K_FACTOR = 7.5;       // Pulsos por cada L/min (depende del YF-S201)

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

// ==== CONEXIÓN WiFi ====
void connectWiFi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ==== CONFIGURAR HORA (para timestamp) ====
void setupTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Sincronizando hora");
  while (time(nullptr) < 100000) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n Hora sincronizada.");
}

// ==== CONEXIÓN AWS MQTT ====
void connectAWS() {
  secureClient.setCACert(AWS_ROOT_CA);
  secureClient.setCertificate(AWS_CLIENT_CERT);
  secureClient.setPrivateKey(AWS_PRIVATE_KEY);
  mqttClient.setServer(AWS_ENDPOINT, AWS_PORT);

  Serial.print("Conectando a AWS IoT Core...");
  while (!mqttClient.connect("shower01")) { // ID del cliente MQTT
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\n Conectado a AWS IoT Core!");
}

// ==== OBTENER TIMESTAMP ISO 8601 ====
String getISO8601Timestamp() {
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  char buffer[40];
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int milliseconds = tv.tv_usec / 1000;
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  char isoTime[60];
  snprintf(isoTime, sizeof(isoTime), "%s.%03dZ", buffer, milliseconds);
  return String(isoTime);
}

// ==== PUBLICAR DATOS ====
void publishData() {
  
  // Calcular pulsos del sensor
  noInterrupts();
  uint32_t pulses = pulseCount;
  pulseCount = 0;
  interrupts();

  // Calcular caudal real en L/min
  flowRate = pulses / K_FACTOR;

  String timestamp = getISO8601Timestamp();

  // Estructura de los datos
  String payload = "{";
  payload += "\"thing_name\": \"shower01\",";
  payload += "\"timestamp\": \"" + timestamp + "\",";
  payload += "\"flow_rate\": " + String(flowRate, 2) + ",";
  payload += "\"unit\": {\"flow_rate\": \"L/min\"}";
  payload += "}";

  mqttClient.publish(MQTT_TOPIC, payload.c_str());
  Serial.println("\n Publicado en AWS:");
  Serial.println(payload);
}

void setup() {
  Serial.begin(115200);
  // === CONFIGURAR SENSOR ===
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);

  connectWiFi();
  setupTime();
  connectAWS();
}

void loop() {
  if (!mqttClient.connected()) {
    connectAWS();
  }
  mqttClient.loop();

  publishData();
  delay(5000); // envía cada 5 segundos
}
