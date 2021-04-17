#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <AsyncElegantOTA.h>
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "HX711.h"

HX711 scale;
const int LOADCELL_DOUT_PIN = D5;
const int LOADCELL_SCK_PIN = D6;

AsyncWebServer server(80);
DNSServer dns;

char response[1024];

void setup(void)
{
  Serial.begin(115200);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(22.45f);

  AsyncWiFiManager wifiManager(&server, &dns);
  wifiManager.autoConnect("NodeMCU_IOT");

  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", response);
  });
  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
}

void loop()
{
  delay(1000);
  if (scale.wait_ready_timeout(1000))
  {
    long reading = scale.get_units(10);
    StaticJsonDocument<1024> data;
    data["ip"] = WiFi.localIP().toString();

    if (false)
    {
      data["error"] = "true";
      data["message"] = "Error reading sensor data";
      serializeJson(data, response);
      return;
    }
    data["weight_grams"] = reading;
    Serial.println(reading);
    serializeJson(data, response);
  }
  else
  {
    Serial.println("No data");
  }
}