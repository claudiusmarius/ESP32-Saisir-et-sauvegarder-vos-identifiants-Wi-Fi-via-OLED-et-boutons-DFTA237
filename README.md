# ESP32-Saisir-et-sauvegarder-vos-identifiants-Wi-Fi-via-OLED-et-boutons-DFTA237
New concept

# ESP32 Wi-Fi Config via OLED & Buttons (with NVS)

Ce projet permet de configurer un ESP32 sans liaison série :
- Entrer le SSID et le mot de passe Wi-Fi via un écran OLED et trois boutons.
- Sauvegarder les identifiants en **NVS** (non-volatile storage).
- Afficher l’état de connexion (GOOD / NO GOOD), l’adresse IP (complète ou tronquée) et l’heure locale via NTP avec gestion automatique de l’heure d’été/hiver.

## Matériel requis
- ESP32 (classique, WROOM ou WROVER)
- Écran OLED SSD1306 (0.91" ou équivalent)
- 3 boutons poussoirs
- Alimentation 5 V (via Vin ou USB)

## Fonctionnalités
- Interface utilisateur simple (OLED + boutons).
- Stockage persistant dans NVS.
- Détection d’appui long pour effacer et reconfigurer les identifiants.
- Affichage de l’heure locale avec prise en compte du DST (Europe).

## Utilisation
1. Compiler et téléverser le sketch sur l’ESP32.
2. Au démarrage, maintenir le bouton **OK** pendant l’invitation pour entrer les identifiants.
3. Valider chaque champ par un appui long.
4. Une fois configuré, l’ESP32 se reconnecte automatiquement après un redémarrage.

---
💡 Ce projet est **Open Source** : vous êtes libre de l’utiliser, le modifier et le partager, tant que vous citez la source.


# ESP32 – Saisir et sauvegarder vos identifiants Wi-Fi via OLED et boutons (DFTA237)

> Nouveau concept : configuration Wi-Fi d’un ESP32 **sans liaison série**, uniquement avec 3 boutons et un mini écran OLED.

---

## 📝 Présentation

Ce projet montre comment permettre à un utilisateur d’entrer ses identifiants Wi-Fi (SSID et mot de passe) **sans modifier le code** et sans passer par l’IDE Arduino.  
Les données sont saisies via trois boutons poussoirs et affichées sur un écran OLED 0.91".  
Elles sont ensuite sauvegardées dans la mémoire **NVS** de l’ESP32 (stockage non volatile).

Après redémarrage :

- L’appareil affiche un message invitant l’utilisateur à faire un **appui long sur le bouton OK** (pendant 12 s) pour reconfigurer les identifiants.  
- Si aucun appui n’est effectué, l’ESP32 tente de se connecter au Wi-Fi.  
- Une fois connecté, il affiche :
  - l’état de connexion (**GOOD / NO GOOD**),
  - l’adresse IP (complète ou tronquée),
  - l’heure locale récupérée via NTP, avec gestion automatique du passage **heure d’été / heure d’hiver** (Europe).

> 💡 L’exemple fourni affiche simplement l’heure, mais vous pouvez adapter le code pour vos propres projets (station météo, domotique, horloge murale, etc.).

---

## 🛠️ Matériel requis

- ESP32 (DevKit, WROOM ou WROVER)  
- Écran OLED SSD1306 (0.91", I²C, adresse par défaut `0x3C`)  
- 3 boutons poussoirs NO (tous reliés au GND pour le commun)  
- Alimentation 5 V (via USB ou broche `VIN`)  

---

## 📁 Fichiers du dépôt

| Fichier | Description |
|---------|-------------|
| `ESP32_WiFi_OLED.ino` | Sketch Arduino principal |
| `schema.png` *(optionnel)* | Schéma de câblage / breadboard |
| `gerber.zip` *(optionnel)* | Fichiers de fabrication PCB |
| `README.md` | Ce document |

> Tu peux ajouter librement d’autres documents (photos, notes, etc.).

---

## 🚀 Utilisation

1. **Téléversement du code**  
   - Ouvre le fichier `ESP32_WiFi_OLED.ino` dans l’IDE Arduino.  
   - Sélectionne la carte `ESP32 Dev Module`.  
   - Installe les bibliothèques nécessaires :  
     - `WiFi.h` (inclus dans l’ESP32 core)  
     - `Preferences.h` (inclus)  
     - `Wire.h` (I²C, inclus)  
     - [`Adafruit_GFX`](https://github.com/adafruit/Adafruit-GFX-Library)  
     - [`Adafruit_SSD1306`](https://github.com/adafruit/Adafruit_SSD1306)  
     - [`NTPClient`](https://github.com/arduino-libraries/NTPClient)  
   - Compile et téléverse.

2. **Première configuration**  
   - Au démarrage, un message t’invite à faire un **appui long sur le bouton OK**.  
   - Sélectionne les caractères de ton SSID à l’aide des boutons, puis valide par un appui long.  
   - Recommence pour le mot de passe.  
   - Les identifiants sont stockés en NVS.

3. **Connexion & affichage**  
   - Au redémarrage, si les identifiants sont présents, l’ESP32 se connecte automatiquement.  
   - L’écran affiche l’état (**GOOD / NO GOOD**), l’adresse IP, puis l’heure locale.

---

## 💡 Astuces

- Tu peux choisir d’afficher **l’adresse IP complète** ou **tronquée** (par exemple se terminant par `.xx.xx`).  
  → Dans le code, deux lignes sont prévues : commente celle que tu ne veux pas utiliser (`//`).  
- Le régulateur interne permet d’alimenter l’ESP32 directement en **5 V** sur la broche `VIN` (mais ne jamais injecter plus de ~6 V).  
- Pour des projets définitifs, ajoute un **condensateur** de filtrage entre `VIN` et `GND`.

---

## 📸 Démonstration

Dans la vidéo associée :

1. Démarrage sans saisie (connexion automatique).  
2. Exemple de saisie volontairement erronée (pour cacher mes identifiants).  
3. Validation et affichage de l’heure.

---

## 📜 Licence

Ce projet est **Open Source** sous licence [MIT](https://opensource.org/licenses/MIT).  
Vous êtes libre de l’utiliser, de le modifier et de le partager, à condition de citer la source.

---

### ✨ Auteur

Projet développé par **Claude Dufourmont (DFTA237)**.  
N’hésitez pas à laisser un ⭐ sur le dépôt si ce travail vous est utile !


