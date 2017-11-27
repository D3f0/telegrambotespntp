// ESP8266 NodeMCU Telegram Bot code

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <NTPClient.h>

// SSID y password,
// y bot 
#include "config.h"


WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

int bot_mtbs = 500; //mean time between scan messages
long bot_lasttime;   //last time messages' scan has been done

const int ledPin = LED_BUILTIN;
int ledStatus = 1;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Invitado";
    Serial.print("Atendiendo a:");
    Serial.println(from_name);

    if (text == "/ledon") {
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
      Serial.print("Prendiendo led para %s");
    }

    if (text == "/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (text == "/status") {
      if(ledStatus){
        bot.sendMessage(chat_id, "Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }

    if (text == "/start") {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Flash Led Bot example.\n\n";
      welcome += "/ledon : to switch the Led ON\n";
      welcome += "/ledoff : to switch the Led OFF\n";
      welcome += "/status : Returns current status of LED\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Wifi: ");

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, LOW); // initialize pin as off
}

void loop() {
  long m0, m1;
  char buff[100];

  if (millis() > bot_lasttime + bot_mtbs)  {
    m0 = millis();
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    m1 = millis();
    while(numNewMessages) {
      // Calculo del tiempo de atención
      dtostrf((m1 - m0) / 1000.0, 6, 4, buff);
      Serial.printf("getUpdates tardo %s seg", buff);
  
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}

