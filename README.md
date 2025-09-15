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
