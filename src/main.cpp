#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <WiFiGeneric.h>
#include <HTTPClient.h>


HTTPClient http;

struct TaskParameters {
    int id;
};

constexpr int timesToRun = 15;
int timesRan = 0;


TaskHandle_t loopTask;
void taskFunction(void *parameter) {
    auto *params = (TaskParameters *)parameter;
    Serial.println(params->id);
    while (true) {
        if (timesRan == timesToRun) {
            Serial.println("[FREERTOS]: Task deleted!");
            digitalWrite(2, HIGH);
            vTaskDelay(pdMS_TO_TICKS(500));
            digitalWrite(2, LOW);
            vTaskDelete(loopTask);
        } else {
            http.begin(""); //API
            http.addHeader("Content-Type", "text/plain");
            http.setUserAgent("ESP32/1.0");
            http.setTimeout(2000);

            int responseStatus = http.GET();
            if (responseStatus != HTTP_CODE_OK) {
                Serial.printf("[HTTP]: Error while sending request [%d]\n", responseStatus);
            } else {
                timesRan++;
                Serial.printf("[HTTP]: Response txt [%s] [%d / %d]\n", http.getString().c_str(), timesRan, timesToRun);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}


void setup() {
    Serial.begin(9600);
    pinMode(2, OUTPUT);

    WiFi.mode(WIFI_AP_STA);
    WiFi.enableLongRange(true);
    WiFi.begin("s", "p");

    while (WiFiClass::status() != WL_CONNECTED) {
        Serial.println("Connecting..");
        delay(500);
    }
    Serial.println("Connected");

    auto *taskParams = new TaskParameters{};
    taskParams->id = 69;

    xTaskCreate(taskFunction, "Task1", 10000, taskParams, 1, &loopTask);
}

void loop() {
    delay(400);
    Serial.println("TEST!");
}
