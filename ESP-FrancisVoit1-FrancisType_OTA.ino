/*
   voiture1 Francis Francis type

   1 micro servo pour la direction (plage droite 1150, gauche 1720)
   1 pont H pour la commande de la propulsion.

   Mise à jour OTA

   Gestion des vitesses:
       0 = arret
       1 à 3 vitesse marche avant ou arriere

*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>                 // OTA
#include <Servo.h>

/*
   Your WiFi config here
*/
IPAddress    apIP(44, 44, 44, 1);      // Définition de l'adresse IP statique.
const char *ssid = "Francis-Voit1";     // Nom du reseau wifi (*** A modifier ***)
const char *password = "12345678";      // mot de passe       (*** A modifier ***)
ESP8266WebServer server(80);            // Port du serveur

int choix = 0;                          // à l'arret pour commencer

int BoiteVitesse = 1;
int valeurAcceleration[4] = { 0 , 450 , 700 , 1024 };  // arret, 1ere, 2eme, 3eme

const int PwmAvant    = D7;             // vers pin D2 - jaune
const int pinServo    = D6;             // servo direction
const int PwmArriere  = D5;             // vers pin D5 - blanc / rouge

const int minServo    = 1150;           //  valeur mini droite 
const int maxServo    = 1720;           //  valeur maxi gauche
const int droitServo  = 1430;           //  valeur tout droit
int valeurDirection   = droitServo;     //  valeur en cours...

Servo ServoVolant;                      // Moteur volant

bool debug = true;

String getPage() {
  String page = "<html lang=fr-FR><head>";
  page += "<title>Commande voiture 1</title>";        // (*** A modifier ***)
  page += "<style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  page += "</head><body><h1>Francis - commande voiture</h1>";
  if (choix == 0) {
    page += "<h3>Arret (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else if (choix == 1) {
    page += "<h3>Avance (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else if (choix == 2) {
    page += "<h3>Recule (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else if (choix == 3) {
    page += "<h3>Gauche (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else if (choix == 4) {
    page += "<h3>Droite (vitesse "; page += BoiteVitesse; page += ")</h3>";
  } else {
    page += "<h3>Commande invalide</h3>";
  }
  page += "<br> <br>";
  page += "<a href=\"/?Avance=1\"style=\" position : absolute; font-size : 26px; background-color: green; color: white; padding: 14px; text-decoration: none;margin-left: 20px; border-radius: 40px\">  Avance</a>";
  page += "<a href=\"/?Vitesse3=1\"style=\" position : absolute; font-size : 26px; background-color: blue; color: white; padding: 14px; text-decoration: none; margin-left: 260px; border-radius: 40px\">V:3</a>";
  page += "<br> <br> <br> <br> <br>";

  page += "<a href=\"/?Arret=1\" style=\" position : absolute; font-size : 26px; background-color: red; color: white; padding: 14px; text-decoration: none;margin-left: 35px; border-radius: 40px\">Arret</a>";
  page += "<a href=\"/?Vitesse2=1\"style=\" position : absolute; font-size : 26px;background-color: blue; color: white; padding: 14px; text-decoration: none;margin-left:260px; border-radius: 40px\">V:2</a>";
  page += "<br> <br> <br> <br> <br>";

  page += "<a href=\"/?Recule=1\" style=\" position : absolute; font-size : 26px; background-color: green; color: white; padding: 14px; text-decoration: none;margin-left: 25px; border-radius: 40px\">Recule</a>";
  page += "<a href=\"/?Vitesse1=1\" style=\" position : absolute; font-size : 26px; background-color: blue; color: white; padding: 14px; text-decoration: none;margin-left:260px; border-radius: 40px\">V:1</a>";

  page += "<a href=\"/?TGauche=1\" style=\" position : absolute; font-size : 26px; background-color: black; color: white; padding: 14px; text-decoration: none;margin-left:400px; border-radius: 40px\">\\\\</a>";
  page += "<a href=\"/?Gauche=1\" style=\" position : absolute; font-size : 26px; background-color: black; color: white; padding: 14px; text-decoration: none;margin-left:500px; border-radius: 40px\">\\</a>";
  page += "<a href=\"/?Droit=1\" style=\" position : absolute; font-size : 26px; background-color: black; color: white; padding: 14px; text-decoration: none;margin-left:600px; border-radius: 40px\">||</a>";
  page += "<a href=\"/?Droite=1\" style=\" position : absolute; font-size : 26px; background-color: black; color: white; padding: 14px; text-decoration: none;margin-left:700px; border-radius: 40px\">/</a>";
  page += "<a href=\"/?TDroite=1\" style=\" position : absolute; font-size : 26px; background-color: black; color: white; padding: 14px; text-decoration: none;margin-left:800px; border-radius: 40px\">//</a>";

  //page += "<br> <br> <br>  <br> <br>";

  /*
    page += "<form action=\"/curseur\" method=\"post\"> <input type=\"range\" name=\"curseur\" id=\"curseur\" style=\" position : absolute; margin-left: 220px;\" name=\"curseur\" min=\"1150\" max=\"1720\" value=\"";
    page += valeurDirection;
    page += "\"oninput=\"result4.value=parseInt(curseur.value)\"/> <output name=\"result4\">";
    page += "</form>";
  */
  page += "</body></html>";

  return page;
}

void handleRoot() {
  if (debug) 
    Serial.println("Entree handleRoot");

  int V1 = server.arg("Vitesse1").toInt();
  if (1 == V1) {
    BoiteVitesse = 1;
  }

  int V2 = server.arg("Vitesse2").toInt();
  if (1 == V2) {
    BoiteVitesse = 2;
  }

  int V3 = server.arg("Vitesse3").toInt();
  if (1 == V3) {
    BoiteVitesse = 3;
  }

  int S1 =  server.arg("Avance").toInt();
  if (1 == S1) {
    choix = 1;
  }
  int S2 =  server.arg("Recule").toInt();
  if (1 == S2) {
    choix = 2;
  }

  int S0 =  server.arg("Arret").toInt();
  if (1 == S0) {
    choix = 0;
  }

  int D1 = server.arg("TGauche").toInt();
  if (1 == D1) {
    valeurDirection = maxServo;
  }

  int D2 = server.arg("Gauche").toInt();
  if (1 == D2) {
    valeurDirection += 80;
  }
  int D3 = server.arg("Droit").toInt();
  if (1 == D3) {
    valeurDirection = droitServo;
  }
  int D4 = server.arg("Droite").toInt();
  if (1 == D4) {
    valeurDirection += -80;
  }
  int D5 = server.arg("TDroite").toInt();
  if (1 == D5) {
    valeurDirection = minServo;
  }

  gestionDirection(valeurDirection);
  commandeVoiture(choix);
  server.send ( 200, "text/html", getPage() );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void arret() {
  Serial.println (" ||||   On s'arrete  |||");

  analogWrite (PwmAvant,   0);
  analogWrite (PwmArriere, 0);
}

void commandeVoiture (int action) {

  switch (action) {
    case 1:
      avance();
      break;

    case 2:
      recule();
      break;

    default:
      arret();
  }
}

void avance() {
  Serial.println (" ==<   On avance  <<<");

  analogWrite (PwmAvant, valeurAcceleration[BoiteVitesse]);
  analogWrite (PwmArriere, 0);
}

void recule() {
  // on recule
  Serial.println (" ==<   On recule  <<<");

  analogWrite (PwmAvant, 0);
  analogWrite (PwmArriere, valeurAcceleration[BoiteVitesse]);

}


// Gestion de la direction
//
// idata = 512 droit => 1435 aprox.
// idata < 512 on tourne a Gauche max 1720
// idata > 512 on tourne à droite min 1150
//
void gestionDirection (int data) {

  int valServo = data;

  // pour eviter de casser la direction ....
  if (valServo < minServo) {
    valServo = minServo;
  } else if (valServo > maxServo) {
    valServo = maxServo;
  }

  if (debug) {
    Serial.print ("    Direction data: ");
    Serial.println (data);

    Serial.print ("    Direction : ");
    Serial.println (valServo);
  }

  ServoVolant.writeMicroseconds(valServo);
}



void setup()
{
  pinMode(pinServo,   OUTPUT);                     // Pour la direction
  pinMode(PwmAvant,   OUTPUT);
  pinMode(PwmArriere, OUTPUT);

  Serial.begin(115200);
  Serial.println(" ");
  Serial.println(" ");

  ServoVolant.attach(pinServo);                    // Relie le ServoDroit a la pin

  // declaration du wifi:
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  server.on ( "/", handleRoot );                   // Page web
  server.on ( "/inline", []() {                    // Etat du serveur
    server.send ( 200, "text/plain", "Commande Voiture OK" );
  } );
  server.onNotFound (handleNotFound);

  // initialisation de l'OTA
  ArduinoOTA.setPort(8266);                        // Port par defaut
  ArduinoOTA.setHostname(ssid);
  ArduinoOTA.setPassword("87654321");              // un peu de sécurité :-)

  ArduinoOTA.onStart([]() {                         // arrêt avant de la mise à jour
    arret();
  });

  ArduinoOTA.begin();

  server.begin();

  if (debug)
    Serial.println ("      >>>>>>>>>>>>  fin du 7up  <<<<<<<<<<<<<<<<<<");

  delay (100);

}

void loop()
{
  ArduinoOTA.handle();                           // OTA

  if ( 0 == WiFi.softAPgetStationNum()) {
    arret ();
    if (debug)
      Serial.println ("Pas de console de commande trouvée");
    delay(1000);
  } else {
    server.handleClient();
  }
}
