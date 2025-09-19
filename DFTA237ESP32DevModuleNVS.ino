/* 
  ESP32 – Connexion Wi-Fi + affichage OLED 0.91"
  Vidéo YouTube : https://youtu.be/dLGMKs4d2Co
  Lien GitHub : https://github.com/claudiusmarius/ESP32-Saisir-et-sauvegarder-vos-identifiants-Wi-Fi-via-OLED-et-boutons-DFTA237
  ------------------------------------------------
  - Permet de saisir SSID et mot de passe via 3 boutons et un écran OLED
  - Sauvegarde les identifiants dans la NVS (Preferences)
  - Affiche "GOOD" ou "NO GOOD" selon la connexion Wi-Fi
  - Récupère l’heure via NTP, applique le fuseau (UTC+1) + heure d’été/hiver
  - Un appui long (>2s) sur OK efface les identifiants et relance la configuration
*/

#include <WiFi.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ---------- Paramètres de l’écran OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1        // Pas de reset matériel
#define OLED_ADDR 0x3C       // Adresse I2C de l’écran
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- Définition des boutons ----------
const int BTN_UP   = 26;     // Rouge : incrémente le caractère
const int BTN_NEXT = 27;     // Jaune : change de ligne (minuscules, majuscules, chiffres…)
const int BTN_OK   = 25;     // Vert : valide ou efface (si appui long)

// ---------- Variables globales ----------
Preferences prefs;   // Stockage non volatil (NVS)
const char* CHAR_LINES[] = {  // 4 « bandes » de caractères disponibles
  "abcdefghijklmnopqrstuvwxyz",     // minuscules
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ",     // majuscules
  "0123456789",                     // chiffres
  " !@#$_-.,:/+*()?=<"              // symboles ; "<" = retour arrière
};
const int NUM_LINES = 4;

String ssid = "";             // SSID saisi ou stocké
String password = "";         // mot de passe saisi ou stocké
String tempInput = "";        // tampon pour la saisie en cours
int lineIndex = 0;            // ligne actuelle dans CHAR_LINES
int charIndex = 0;            // position du caractère courant

bool okbuttonPressed = false; // sert à détecter la durée d’appui sur OK
unsigned long okpressStart = 0;
bool inConfig = false;        // indique si on est en mode configuration

// ---------- Client NTP ----------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
// Décalage = 0 car on applique nous-même UTC+1 et DST

// =====================================================
// FONCTIONS UTILITAIRES
// =====================================================

// Lecture d’un bouton : retourne true si appuyé (LOW car PULLUP actif)
bool readBtn(int pin) { return digitalRead(pin) == LOW; }

// Attend que le bouton soit relâché (évite répétitions)
bool waitRelease(int pin) { while(readBtn(pin)) delay(10); return true; }

// =====================================================
// AFFICHAGE DE LA SAISIE SUR OLED
// =====================================================
void showOLEDInput() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  // Affiche le texte en cours + curseur
  display.println("Saisie: " + tempInput + "_");
  display.setCursor(0,16);
  display.print("Type:");
  switch(lineIndex){
    case 0: display.print("Min "); break;
    case 1: display.print("Maj "); break;
    case 2: display.print("Num "); break;
    case 3: display.print("Sym "); break;
  }
  display.print(" Char:");
  display.print(CHAR_LINES[lineIndex][charIndex]);
  display.display();
}

// =====================================================
// MODE CONFIGURATION : saisie SSID + mot de passe
// =====================================================
void enterConfigMode() {
  tempInput = "";
  lineIndex = 0;
  charIndex = 0;

  // --- 1) Saisie du SSID ---
  while(ssid == "") {
    showOLEDInput();

    // Bouton UP : avance dans les caractères
    if(readBtn(BTN_UP)){
      charIndex++;
      if(charIndex >= strlen(CHAR_LINES[lineIndex])) charIndex = 0;
      waitRelease(BTN_UP);
    }

    // Bouton NEXT : change de ligne (majuscules, chiffres…)
    if(readBtn(BTN_NEXT)){
      lineIndex++;
      if(lineIndex >= NUM_LINES) lineIndex = 0;
      charIndex = 0;
      waitRelease(BTN_NEXT);
    }

    // Bouton OK : ajoute le caractère ou valide (appui long)
    if(readBtn(BTN_OK)){
      unsigned long start = millis();
      while(readBtn(BTN_OK));              // attend le relâchement
      unsigned long duration = millis()-start;

      if(duration > 1000){                 // >1s = fin de saisie
        ssid = tempInput;
        tempInput = "";
        break;
      } else {                             // sinon ajoute ou efface
        char c = CHAR_LINES[lineIndex][charIndex];
        if(c=='<'){ if(tempInput.length()>0) tempInput.remove(tempInput.length()-1); }
        else tempInput += c;
      }
    }
  }

  // --- 2) Saisie du mot de passe ---
  while(password == "") {
    showOLEDInput();
    // même logique que pour le SSID
    if(readBtn(BTN_UP)){
      charIndex++;
      if(charIndex >= strlen(CHAR_LINES[lineIndex])) charIndex = 0;
      waitRelease(BTN_UP);
    }
    if(readBtn(BTN_NEXT)){
      lineIndex++;
      if(lineIndex >= NUM_LINES) lineIndex = 0;
      charIndex = 0;
      waitRelease(BTN_NEXT);
    }
    if(readBtn(BTN_OK)){
      unsigned long start = millis();
      while(readBtn(BTN_OK));
      unsigned long duration = millis()-start;
      if(duration > 1000){
        password = tempInput;
        tempInput = "";
        break;
      } else {
        char c = CHAR_LINES[lineIndex][charIndex];
        if(c=='<'){ if(tempInput.length()>0) tempInput.remove(tempInput.length()-1); }
        else tempInput += c;
      }
    }
  }

  // Sauvegarde en mémoire non volatile
  prefs.putString("ssid", ssid);
  prefs.putString("pass", password);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Identifiants sauvegardes !");
  display.display();
  delay(1000);
}

// =====================================================
// Connexion Wi-Fi avec délai maximum
// =====================================================
bool connectWiFi(unsigned long timeout=30000){
  WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long start = millis();
  while(WiFi.status() != WL_CONNECTED && millis()-start < timeout){
    delay(500);
  }
  return WiFi.status() == WL_CONNECTED;
}

// =====================================================
// AFFICHAGE ÉTAT WIFI
// =====================================================
void showWiFiStatus() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  if(WiFi.status() == WL_CONNECTED){
    display.println("GOOD !");
    display.println("");
    //display.print("IP: ");
    //display.println(WiFi.localIP().toString());
    display.print("IP: "); display.println(String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + ".xx.xx"); 
    // Affiche l'IP tronquée (ex : 192.168.xx.xx)

  } else {
    display.println("NO GOOD !");
  }
  display.display();
}

// =====================================================
// INVITATION : APPUI LONG POUR CONFIG
// =====================================================
void showAppuiLongInvitation(int durationSec){
  unsigned long startTime = millis();
  while(millis()-startTime < (unsigned long)durationSec*1000){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);

    // Clignote toutes les 500 ms
    if(((millis()/500)%2)==0)
      display.println("Appui long OK pour config");

    // Barre de progression (visualisation du compte à rebours)
    int elapsed = millis()-startTime;
    int barWidth = map(elapsed,0,durationSec*1000,0,SCREEN_WIDTH-20);
    display.fillRect(0,20,barWidth,5,SSD1306_WHITE);

    // Affiche les secondes restantes
    int secondsLeft = durationSec - (elapsed/1000);
    display.setCursor(SCREEN_WIDTH-15,19);
    display.print(secondsLeft);

    display.display();
    delay(50);

    // Si appui long détecté → efface prefs et lance config
    if(readBtn(BTN_OK)){
      unsigned long pressStart = millis();
      while(readBtn(BTN_OK));
      if(millis()-pressStart > 2000){
        inConfig = true;
        prefs.clear();
        ssid = "";
        password = "";
        enterConfigMode();
        break;
      }
    }
  }
}

// =====================================================
// GESTION HEURE LOCALE (DST Europe)
// =====================================================

// Détermine si une date est en heure d’été
bool isDST(int y, int m, int d, int w){ 
  // Europe : dernier dimanche de mars à octobre
  if(m<3 || m>10) return false;
  if(m>3 && m<10) return true;
  int lastSunday = d - w;
  if(m==3) return d >= lastSunday;
  if(m==10) return d < lastSunday;
  return false;
}

// Retourne +3600 si heure d’été active
int getUTCDSTOffset(unsigned long epoch){
  time_t t = epoch;
  struct tm * now = gmtime(&t);
  return isDST(now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_wday) ? 3600 : 0;
}

// Convertit en heure locale lisible HH:MM:SS
String getLocalTimeString(){
  timeClient.update();
  unsigned long epoch = timeClient.getEpochTime();
  int offset = 3600 + getUTCDSTOffset(epoch); // UTC+1 de base + DST éventuel
  time_t local = epoch + offset;
  struct tm * t = gmtime(&local);
  char buf[9];
  sprintf(buf,"%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
  return String(buf);
}

// =====================================================
// SETUP : initialisation globale
// =====================================================
void setup() {
  // Déclaration des boutons en entrée avec pullup interne
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);

  // OLED
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();

  // Lecture des identifiants en mémoire
  prefs.begin("wifi", false);
  ssid = prefs.getString("ssid","");
  password = prefs.getString("pass","");

  // Affiche message d’invitation (permet appui long pour reset)
  showAppuiLongInvitation(12);

  // Connexion Wi-Fi si on a des identifiants
  bool wifiOK = false;
  if(ssid != "" && password != "") wifiOK = connectWiFi();

  // Affiche GOOD / NO GOOD
  showWiFiStatus();

  // Si connecté → démarrage NTP et affichage de l’heure
  if(wifiOK){
    timeClient.begin();
    timeClient.update();
    delay(20000); // Pause 20 s avant affichage de l’heure
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(17,13);
    display.println(getLocalTimeString());
    display.display();
  }
}

// =====================================================
// LOOP : rafraîchissement permanent
// =====================================================
void loop() {
  // Vérifie si OK est maintenu >2s → reset config
  if(readBtn(BTN_OK)){
    if(!okbuttonPressed) okpressStart = millis();
    okbuttonPressed = true;
    if(millis()-okpressStart > 2000 && !inConfig){
      inConfig = true;
      prefs.clear();
      ssid = "";
      password = "";
      enterConfigMode();
      WiFi.begin(ssid.c_str(), password.c_str());
      while(WiFi.status() != WL_CONNECTED){ delay(500); }
      timeClient.begin();
      timeClient.update();
      inConfig = false;
    }
  } else okbuttonPressed = false;

  // Rafraîchit l’affichage : IP ou heure locale
  if(WiFi.status() != WL_CONNECTED){
    showWiFiStatus();
  } else {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(17,13);
    display.println(getLocalTimeString());
    display.display();
  }

  delay(1000); // mise à jour toutes les secondes
}
