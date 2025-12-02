#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <DHT.h>

// ⚠️ --- 1. WIFI AND API CONFIGURATION --- ⚠️
// YOU MUST CHANGE THESE FOUR LINES BELOW!
const char* ssid = "vivo T4x 5G";         
const char* password = "kingofhel"; 

const char* weatherApiKey = "c7c8bb9dcd6613c7d55296e9247ae783"; 
const char* city = "Phagwara"; // Set your desired city for weather tracking

// --- Hardware Configuration ---
#define DHTPIN 4
#define DHTTYPE DHT11

// Define the serial port for the DFPlayer Mini (UART1 on ESP32)
// TX1=GPIO17 (DFPlayer RX), RX1=GPIO16 (DFPlayer TX)
HardwareSerial mySerial(1);
DFRobotDFPlayerMini player;
DHT dht(DHTPIN, DHTTYPE);

// Define API URL structure
String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&appid=" + String(weatherApiKey) + "&units=metric";

// Variable to track the last played track
int lastTrack = 0;

// --- Function Prototypes ---
void connectWiFi();
void updateMusicBasedOnWeather();


void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Starting API Weather Music Player Setup ---");
  
  // --- 1. WiFi Connection ---
  connectWiFi();

  // --- 2. Initialize DHT Sensor ---
  dht.begin();
  Serial.println("✅ DHT Sensor initialized!");
  delay(1000);

  // --- 3. Initialize DFPlayer Mini ---
  Serial.println("Starting DFPlayer initialization...");
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  
  if (!player.begin(mySerial)) {
    Serial.println("❌ DFPlayer NOT detected! Check wiring / SD card / power.");
    Serial.println("System Halted.");
    while (true);
  }

  Serial.println("✅ DFPlayer detected!");
  player.volume(25); // Set volume level (0-30)
  delay(1000);
  
  Serial.println("\n-------------------------------------");
  Serial.println("✅ ALL HARDWARE READY. Starting Weather Fetch.");
  Serial.println("-------------------------------------");
}


void loop() {
  // --- Read Local Sensor Data (DHT) ---
  float hum = dht.readHumidity();
  float temp = dht.readTemperature(); 

  // Check if the DHT reading was successful
  if (isnan(hum) || isnan(temp)) {
    Serial.println("\n❌ FAILED TO READ LOCAL SENSOR DATA!");
  } else {
    Serial.println("\n--- Local Sensor Readings (DHT) ---");
    Serial.print("🌡️ Local Temperature: ");
    Serial.print(temp);
    Serial.println(" °C");
    
    Serial.print("💧 Local Humidity: ");
    Serial.print(hum);
    Serial.println(" %");
  }

  // --- Get Real-Time Weather and Update Music ---
  if (WiFi.status() == WL_CONNECTED) {
    updateMusicBasedOnWeather();
  } else {
    Serial.println("⚠️ WiFi Disconnected. Reconnecting...");
    connectWiFi(); 
  }

  // Wait 30 seconds before checking the weather and sensor again
  delay(30000); 
}

// ------------------------------------------------------------------
// --- Helper Functions ---
// ------------------------------------------------------------------

void connectWiFi() {
  Serial.print("Attempting to connect to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int timeoutCounter = 0;
  int maxTries = 40; // 20 seconds timeout
  
  while (WiFi.status() != WL_CONNECTED && timeoutCounter < maxTries) {
    delay(500);
    Serial.print(".");
    timeoutCounter++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected Successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
    Serial.print("WiFi Status Code (Check Credentials/2.4GHz): ");
    Serial.println(WiFi.status()); 
  }
}

void updateMusicBasedOnWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot fetch weather: WiFi is disconnected.");
    return;
  }

  HTTPClient http;
  http.begin(weatherUrl);
  int httpCode = http.GET();

  Serial.print("\nAttempting to fetch weather data for: ");
  Serial.println(city);

  if (httpCode == HTTP_CODE_OK) { // HTTP 200: Success
    String payload = http.getString();
    
    // Use ArduinoJson to parse the response
    StaticJsonDocument<512> doc; 
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print(F("❌ JSON parsing failed: "));
      Serial.println(error.f_str());
      return;
    }
    
    // --- EXTRACT API DATA: Temp and Humidity ---
    float apiTemp = doc["main"]["temp"];
    float apiHumidity = doc["main"]["humidity"];

    // Extract weather details
    String mainWeather = doc["weather"][0]["main"].as<String>();
    String description = doc["weather"][0]["description"].as<String>();
    
    Serial.println("\n-------------------------------------");
    Serial.println("--- Real-Time Weather Data ---");
    Serial.print("City: ");
    Serial.println(city);
    
    // Print the extracted API Temperature and Humidity
    Serial.print("🌡️ API Temperature: ");
    Serial.print(apiTemp);
    Serial.println(" °C");
    
    Serial.print("💧 API Humidity: ");
    Serial.print(apiHumidity);
    Serial.println(" %");
    
    Serial.print("Condition: ");
    Serial.print(mainWeather);
    Serial.print(" (");
    Serial.print(description);
    Serial.println(")");

    int track = 0;
    String weatherStatus = "";
    
    // --- Mapping Weather to Tracks ---
    // Track 1: Rainy/Storms (0001.mp3)
    if (mainWeather == "Rain" || mainWeather == "Drizzle" || mainWeather == "Thunderstorm" || mainWeather == "Snow") {
      track = 1; 
      weatherStatus = "Rainy Weather / Storms";
    }
    // Track 2: Cloudy/Normal (0002.mp3)
    else if (mainWeather == "Clouds" || mainWeather == "Mist" || mainWeather == "Fog" || mainWeather == "Haze") {
      track = 2; 
      weatherStatus = "Cloudy/Normal Weather";
    }
    // Track 3: Sunny/Clear (0003.mp3)
    else if (mainWeather == "Clear") {
      track = 3; 
      weatherStatus = "Sunny/Clear Weather";
    }
    else {
      Serial.print("⚠️ Unknown weather condition (");
      Serial.print(mainWeather);
      Serial.println(") received. Retaining previous track.");
      track = lastTrack > 0 ? lastTrack : 2; 
    }

    // --- Play Track if Condition Changed ---
    if (track != lastTrack) {
      Serial.print("▶ Changing Track to: ");
      Serial.println(track);
      player.play(track); 
      lastTrack = track;
    }
    
    Serial.print("🎵 Current Track: ");
    Serial.print(track);
    Serial.print(" (");
    Serial.print(track < 10 ? "000" : ""); 
    Serial.print(track);
    Serial.println(".mp3)");
    
    Serial.print("☀️ Music Theme: ");
    Serial.println(weatherStatus);
    Serial.println("-------------------------------------");

  } // <-- THIS IS THE FIXED CLOSING BRACE
  else {
      // HTTP Code > 0 but not 200 (e.g., 401 for unauthorized key, 404 for wrong city)
      Serial.print("❌ HTTP Error: Received non-OK code (");
      Serial.print(httpCode);
      Serial.println("). Check API Key (401) or City Name (404).");
  }

  http.end();
}
