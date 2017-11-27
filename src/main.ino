// ESP8266 NodeMCU Telegram Bot code

// TODO: Parametrizar la placa para la WeMos
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// SSID y password,
// y bot 
#include "config.h"


WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

int bot_mtbs = 500; //mean time between scan messages
long bot_lasttime;   //last time messages' scan has been done


WiFiUDP ntpUDP;

NTPClient timeClient(
  ntpUDP, 
  "europe.pool.ntp.org", 
  3600, // Time offset
  60000 // Time interval
);

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Invitado";
    Serial.print("Respondiendo a ");
    Serial.println(from_name);

    if (text == "/time") {
      String formatedTime = timeClient.getFormattedTime();
      Serial.println(formatedTime);
      bot.sendMessage(chat_id, formatedTime, "");
    }

    if (text == "/update") {
      timeClient.update();
      Serial.println(timeClient.getFormattedTime());
    }


    if (text == "/start") {
      String welcome = "Bot telegram con NTP, " + from_name + ".\n";
      welcome += "/time : to switch the Led OFF\n";
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

  Serial.print("NTP.");
  timeClient.begin();
  Serial.println(".");

}

long lastNtpPoll = 0, now;

#define INTERVAL (15*60*1000)

void loop() {
  long m0, m1;
  char buff[100];
  now = millis(); 

  if ((now - lastNtpPoll)>INTERVAL) {
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
    lastNtpPoll = now;
  }

  if (now > (bot_lasttime + bot_mtbs))  {
    m0 = millis();
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    m1 = millis();
    while(numNewMessages) {
      // Calculo del tiempo de atención
      dtostrf((m1 - m0) / 1000.0, 6, 4, buff);
      Serial.printf("getUpdates tardo %s seg\n", buff);
  
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }

}

