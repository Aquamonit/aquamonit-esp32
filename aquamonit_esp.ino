#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

// ==== CONFIGURACIÓN WiFi ====
const char* WIFI_SSID = "DARIEN";
const char* WIFI_PASSWORD = "66864558";

// ==== CONFIGURACIÓN AWS IoT ====
const char* AWS_ENDPOINT = "a340n6n8bwxje6-ats.iot.us-east-1.amazonaws.com"; //endpoint MQTT
const int AWS_PORT = 8883;
const char* MQTT_TOPIC = "ShowerTopic/Shower01";

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
MIIDWTCCAkGgAwIBAgIUeYBZ3be0lBkfLcVVoEkgpk7sf+cwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MTExMjE2MDcx
OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMYborOkIJki7vMo5FR2
e6zlfVTKuljP3T9HM/zxcHM0RVDGWKjhkSZk7zwVjHTQv21nMrvbcq4qb5078AQ6
a7bn8zQVPgEITWDCbCIiWFyYYc5+WYh+FYDCpDE0YBgOE2vD25Ak6e3r6U98bn8T
wnBKkmbtOC3QUsL+e5krdajSQqEKWcMsB/EBMuvyiZy1N4C+Ga+iOKlCukPVrT7g
8ShG3t2zrAoCW41D17p0bNE9B3mZ8CNSFm+DUeagGijQfeB/M8CnbSMRTWtU/29a
zdPtMnqufXbLL2i7OWi4UMyDuwr/A6G8cNIKndOe8074HYKQGBOWEb3Ho4oCbAkj
nmcCAwEAAaNgMF4wHwYDVR0jBBgwFoAUW5gWeDEMgKk7EaKR9T1bv958msswHQYD
VR0OBBYEFLOrsgUIKTwnSdIlOGBgnqphjr5NMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCS0z3xutoJ0xEqHuFJieVzVEtJ
IgOD176ybdBkS5dt+sv7WU4Y5P87SPOtWwgH/OCvPoK0Lr8ZNLclevz2qZqlQw/Z
uCEoLvlUMDBePBt+rnOVqxfMTOzsdOXYwgzMnpRZoETyhQXfdFasIXktWrIQjoeB
Y2uOFj6TLAbf5Wao0lulyCB28VcOA6wm4bUDrjhP2E21FhR77ee2l1A8OsT2PPa+
F+4VscC7JnMousIvbuCqk45eIvE5pzqqfC+P7Ei7CIfp2AKeB9xht9K6A6pUHOD0
NroiDWX0h8vbP4Lct05H9M53ee5KUUF196cMwG51UPLlCknYX2hB8R7ZU5Js
-----END CERTIFICATE-----
)KEY";

// Llave privada del Thing
static const char AWS_PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAxhuis6QgmSLu8yjkVHZ7rOV9VMq6WM/dP0cz/PFwczRFUMZY
qOGRJmTvPBWMdNC/bWcyu9tyripvnTvwBDprtufzNBU+AQhNYMJsIiJYXJhhzn5Z
iH4VgMKkMTRgGA4Ta8PbkCTp7evpT3xufxPCcEqSZu04LdBSwv57mSt1qNJCoQpZ
wywH8QEy6/KJnLU3gL4Zr6I4qUK6Q9WtPuDxKEbe3bOsCgJbjUPXunRs0T0HeZnw
I1IWb4NR5qAaKNB94H8zwKdtIxFNa1T/b1rN0+0yeq59dssvaLs5aLhQzIO7Cv8D
obxw0gqd057zTvgdgpAYE5YRvcejigJsCSOeZwIDAQABAoIBAARzGnRue0z1O+i6
QmajQqPfg7Ej3IuiH5YMQ16rDZuI0QW/br5cUqZKHhV7Zmw5RBNw/fK6OxkuWtNt
RTbTGRhCnXPOBlZ/mT1TIJqjqthpk8jPjbNDIbiCpq10P7RKnKUPAR97tMVjRxpB
8TlpeDSNeDxXl6tvwmweyyflkf74r9RycqdihlxVQQdD2HGyy/kz5SqGdF7igljL
c5Stp4T77KfKjG+WRHgLTwNTE9mWE2FA6Y8Rl3mqocS8pY4+n6WWlCORqrtWj3iU
Bnopv9iUwStDWlVAs+HgT/EUeGqBRq7AUB14v56PqmxMJCFPFs98xUe49vItL9gq
Wj/gyAECgYEA/x1cuvVXTomu+FiKpi4w9qCjwMX2oJnTM36DliRr1J48z+tAoFia
BhxbzDn9IDDKa5xcDYAKTkVwgV86ekAnq390u1NlZdj//E89GFoL0Rm5EYRJ1ygI
sJZxgdaFGFcScjZnvMDgmaoGfsWslf4MAVhDsBr6F/zwCJ9OWA7Q29ECgYEAxsuh
QUSVHcaxS+B4onexLhfHJxm8e049Oayo+0Ey/Xi+pI/OIvlJBQn9FBZdAhOXT3WJ
6v0q0VY6kKN/qSUmZ+hOWA2jtx1SODdPuBgNPxrLegC8uEoO0dnoAnWbTuXYCNNf
0mTh6V0Cjw+Wds2Ub+QcRcxkjhOaCUpSREkZvLcCgYEAtFoSo00soqYtXv5hTmgb
T09zVKXe29zDdYT8C5nF7XLMdQTQFEtu3+EOeIuKSkaT442Pei/JmVn31k33LlPK
d7BMWt3zMhQ9rCZsf5mw/usbbanC9eskcYp5xhtczj9IGBwkLEmCvr93hgnDH+qQ
nCbE+2um4Gx9WOseXJxkxPECgYEAjECMAz08qyZMXcj/szgYw/k4o2hv5tq7LV6Y
VbOZIuU2M2nVuC0CDFEQwURJ2DY0MXYumKCGvexFI1iGr7IrF3H5G/x7faxtHoY/
cxVc+yGPXwL4Ilj1fa5sFpgcu5xDP27JkuljzT22mXIY6LZ/vDVGkSPz6Z0Atv89
bTnZs58CgYEAl8CF9q1ig5b0aa53I5EeAKW9jdAnwQoS4TnjGy+6twM6f0OUgKvu
Ko5ztFMo5ciGHueVqBApvm3fi0EXH9RjD/322Qutwo6/+QewpZPcalpf/2oirIRX
LT0oplzdNhTK/JTgBOTJeXkkv083dkNdyZb8HTyNc+IN49QFct39mpk=
-----END RSA PRIVATE KEY-----
)KEY";

// ==== VARIABLES ====
float flowRate = 0.0;  // si luego se conectas el sensor, aquí se asigna la lectura real

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
  // Simular un flujo entre 5 y 12 L/min
  flowRate = random(500, 1200) / 100.0;

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
