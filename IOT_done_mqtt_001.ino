#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 4      // Chân kết nối DHT11
#define DHTTYPE DHT11
#define LED_PIN 2     // Chân GPIO nối với đèn LED
#define LED_PIN2 15   // Chân GPIO nối với LED thứ hai
#define LED_PIN3 16   // Chân GPIO nối với LED thứ ba
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Phòng Vip";
const char* password = "83868386";
const char* mqttServer = "192.168.0.107";  // Địa chỉ IP của Mosquitto broker
const int mqttPort = 1883;
const char* mqttUser = "anhlong";  
const char* mqttPassword = "123";  
const char* topic = "sensors/data";
const char* ledTopic = "sensors/led";  // Topic điều khiển đèn LED
const char* ledTopic2 = "sensors/fan";  // Topic điều khiển LED thứ hai
const char* ledTopic3 = "sensors/pump";  // Topic điều khiển LED thứ ba

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  if (String(topic) == "sensors/led") {
    if (msg == "ON") {
      digitalWrite(LED_PIN, LOW);  // Bật LED (LOW = ON)
      Serial.println("Đèn LED đã bật ngay lập tức");
    } else if (msg == "OFF") {
      digitalWrite(LED_PIN, HIGH);  // Tắt LED (HIGH = OFF)
      Serial.println("Đèn LED đã tắt ngay lập tức");
    }
  } else if (String(topic) == "sensors/fan") {
    if (msg == "ON") {
      digitalWrite(LED_PIN2, LOW);  // Bật LED thứ hai (LOW = ON)
      Serial.println("Đèn LED 2 đã bật ngay lập tức");
    } else if (msg == "OFF") {
      digitalWrite(LED_PIN2, HIGH);  // Tắt LED thứ hai (HIGH = OFF)
      Serial.println("Đèn LED 2 đã tắt ngay lập tức");
    }
  } else if (String(topic) == "sensors/pump") {
    if (msg == "ON") {
      digitalWrite(LED_PIN3, LOW);  // Bật LED thứ ba (LOW = ON)
      Serial.println("Đèn LED 3 đã bật ngay lập tức");
    } else if (msg == "OFF") {
      digitalWrite(LED_PIN3, HIGH);  // Tắt LED thứ ba (HIGH = OFF)
      Serial.println("Đèn LED 3 đã tắt ngay lập tức");
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // Đảm bảo đèn LED tắt khi khởi động
  digitalWrite(LED_PIN2, HIGH); // Tắt LED 2 khi khởi động
  digitalWrite(LED_PIN3, HIGH); // Tắt LED 3 khi khởi động

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Đang kết nối WiFi...");
  }
  Serial.println("Đã kết nối WiFi");

  // Cài đặt MQTT Broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  reconnectMQTT();
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Đang kết nối MQTT...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Đã kết nối MQTT");
      client.subscribe(ledTopic);   // Đăng ký topic điều khiển đèn LED
      client.subscribe(ledTopic2);  // Đăng ký topic điều khiển LED thứ hai
      client.subscribe(ledTopic3);  // Đăng ký topic điều khiển LED thứ ba
    } else {
      Serial.print("Lỗi kết nối MQTT, mã lỗi: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

unsigned long lastSensorPublish = 0;
const unsigned long sensorInterval = 5000;  // Gửi dữ liệu cảm biến mỗi 7 giây

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Kiểm tra thời gian và gửi dữ liệu cảm biến nếu đến thời điểm
  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorPublish >= sensorInterval) {
    lastSensorPublish = currentMillis;

    // Đọc dữ liệu cảm biến
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int lightValue = analogRead(34);

    if (!isnan(temperature) && !isnan(humidity)) {
      String payload = "{\"temperature\":" + String(temperature) +
                       ", \"humidity\":" + String(humidity) +
                       ", \"light\":" + String(lightValue) + "}";
      client.publish("sensors/data", payload.c_str());
      Serial.println("Đã gửi dữ liệu: " + payload);
    } else {
      Serial.println("Lỗi khi đọc cảm biến DHT");
    }
  }
}
