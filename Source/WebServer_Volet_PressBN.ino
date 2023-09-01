    #include <ESP8266WiFi.h>
    #include <WiFiClient.h>
    #include <ESP8266WebServer.h>
    #include <ESP8266mDNS.h>
    #include <SoftwareSerial.h>
    
    static const String monSSID = "MON-SSID";
    static const String monPass = "MON-PASSWORD-WIFI";
    static const String HostName = "WeVolet";
    
    ESP8266WebServer server(80);

    static const int BN_Up   = D3; // Digital pin connected to BN UP.
    static const int BN_Stop = D2; // Digital pin connected to BN STOP.
    static const int BN_Down = D1; // Digital pin connected to BN DOWN.

    static int EtatBN_UP;
    static int EtatBN_Stop;
    static int EtatBN_Down;

    static const int RelayH = D5;     // Digital pin connected to Relay
    static const int RelayB = D6;     // Digital pin connected to Relay

    static const int DelayVolet = 25; // 25 secondes entre chaque action.
    static int Timer;
    static int VoletStatus; // 5 = Indéterminé, 31 = En monté, 32 = En descente, 1 = monté, 2 = descendu.

    static void CommandVolet(int i)
    {
     if (VoletStatus < 30) // Volet en mouvement on ne fait rien.
     {
      if (i == 1) // Demande d'ouverture.
      {
        VoletStatus = 31; // Volet en mouvement.
        pinMode(RelayH , OUTPUT);
        pinMode(RelayB , INPUT);
        Timer = DelayVolet;
      }
      else if (i == 2) // Demande de fermeture.
           {
             VoletStatus = 32; // Volet en mouvement.
             pinMode(RelayH , INPUT);
             pinMode(RelayB , OUTPUT);
             Timer = DelayVolet;
           }
     }
    }
    
    static void StopVolet()
    {
       if (VoletStatus == 31) VoletStatus = 1;
       if (VoletStatus == 32) VoletStatus = 2;
       pinMode(RelayH , INPUT);
       pinMode(RelayB , INPUT);
       Timer = 0;
    }
    
    void setup() {
      Serial.begin(115200);
      delay(2000);

      IPAddress LAN_ADDR(192,168,0,7);
      IPAddress LAN_GW(192,168,0,1);
      IPAddress LAN_SUBNET(255,255,255,0);
      IPAddress LAN_DNS(192,168,0,1);
      WiFi.mode(WIFI_STA);
      WiFi.config(LAN_ADDR,LAN_GW,LAN_SUBNET,LAN_DNS);
      WiFi.setHostname(HostName.c_str()); // Placer le Nom d'hote après WiFi.Config().
      WiFi.begin(monSSID, monPass);
      Serial.print("Etablissement de la connection WIFI");
      while (WiFi.status() != WL_CONNECTED) 
      {
        delay(500);
        Serial.print(".");
      }
      Serial.println("");    
      Serial.print("Connection etablie - Adresse IP : ");
      Serial.println(WiFi.localIP());
      WiFi.setAutoReconnect(true); // Permet la reconnection automatique après une perte du WIFI.
      WiFi.persistent(true);

      // Circuit Ouvert.
      Serial.println("Opening Circuit...");
      pinMode(RelayH , INPUT);
      pinMode(RelayB , INPUT);
      VoletStatus = 5; // 5 -> Etat des volets indéterminé.
      Timer = 0; // Rien à faire au démarrage.

      // Initialisation des boutons.
      pinMode(BN_Up,INPUT_PULLUP);   // Dans ce mode bouton non pressé alors EtatBN == 1. Si bouton pressé alors EtatBN == 0.
      pinMode(BN_Stop,INPUT_PULLUP); // Dans ce mode bouton non pressé alors EtatBN == 1. Si bouton pressé alors EtatBN == 0.
      pinMode(BN_Down,INPUT_PULLUP); // Dans ce mode bouton non pressé alors EtatBN == 1. Si bouton pressé alors EtatBN == 0.
      EtatBN_UP   = 1;
      EtatBN_Stop = 1;
      EtatBN_Down = 1;
      
      // On pose une fonction handle pour l'appel de l'URL / ...
      server.on("/", HTTP_GET,handleRoot);
      // On pose une fonction handle pour l'appel de l'URI /Command ...
      server.on("/Command", HTTP_GET, handleCommand);
      // Enfin on pose une fonction handle pour une URI / [pas de handle correspondant] ...
      server.onNotFound(handleNotFound);
      server.begin();
    }

    void handleCommand()
    {
     Serial.println("handleCommand...");
    /*
     String message = "handleCommand Called\n";
     message += "URI: ";
     message += server.uri();
     message += "\nMethod: ";
     message += (server.method() == HTTP_GET)?"GET":"POST";
     message += "\nArguments: ";
     message += server.args();
     message += "\n";
     for (uint8_t i=0; i<server.args(); i++){
       message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
     }
     */
     String INDEX_HTML = "";

     INDEX_HTML +="<!DOCTYPE HTML>";
     INDEX_HTML +="<html>";
     INDEX_HTML +="<head>";
     INDEX_HTML +="<meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate' />";
     INDEX_HTML +="<meta http-equiv='Pragma' content='no-cache' />";
     INDEX_HTML +="<meta http-equiv='Expires' content='0' />";
     INDEX_HTML +="</head>";
     INDEX_HTML +="<body style='text-align:center;'>";

     // Test l'existance de la valeur csvalue1 dans la requète http://URI/Command?csvalue1=XX 
     if (server.hasArg("VoletStop"))
     {
      StopVolet();
      Serial.println("Volet Stop.");
     }
     else
     {     
      if (server.hasArg("Volet"))
      {
       String RelayValue1 = server.arg("Volet");
       int RelayValue = RelayValue1.toInt();
       if (RelayValue == 1)
       {
        Serial.println("Ouverture Volet");
        CommandVolet(1); 
       }
       else
       {
        Serial.println("Fermeture Volet");
        CommandVolet(2);
       }
      }
     }
     INDEX_HTML +="<div class='Volet'>"; INDEX_HTML += String(VoletStatus); INDEX_HTML +="</div><br/>";
     INDEX_HTML +="<div class='EtatVoletBin'>0</div><br/>"; 
     INDEX_HTML +="</body>";
     INDEX_HTML +="</html>"; 
     server.send(200, "text/html", INDEX_HTML);
     Serial.println("Page Web Envoyee.");
    }

    void handleRoot()
    {
      Serial.println("Creation Page Web De Resultat...");
      String INDEX_HTML = "";
      INDEX_HTML +="<!DOCTYPE HTML>";
      INDEX_HTML +="<html>";
      INDEX_HTML +="<head>";
      INDEX_HTML +="<meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate' />";
      INDEX_HTML +="<meta http-equiv='Pragma' content='no-cache' />";
      INDEX_HTML +="<meta http-equiv='Expires' content='0' />";
      INDEX_HTML +="</head>";
      INDEX_HTML +="<body style='text-align:center;'>";
      INDEX_HTML +="<div class='Volet'>"; INDEX_HTML += String(VoletStatus); INDEX_HTML +="</div><br/>"; 
      INDEX_HTML +="<div class='EtatVoletBin'>0</div><br/>"; 
      INDEX_HTML +="</body>";
      INDEX_HTML +="</html>"; 
      Serial.println("Envoie De La Page Web...");
      server.send(200, "text/html", INDEX_HTML);
      Serial.println("Page Web Envoyee.");
    }

    void handleNotFound()
    {
      String message = "File Not Found\n\n";
      message += "URI: ";
      message += server.uri();
      message += "\nMethod: ";
      message += (server.method() == HTTP_GET)?"GET":"POST";
      message += "\nArguments: ";
      message += server.args();
      message += "\n";
      for (uint8_t i=0; i<server.args(); i++){
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
      }
      server.send(404, "text/plain", message);
    }

    void loop() 
    {
      EtatBN_UP   = digitalRead(BN_Up);
      EtatBN_Stop = digitalRead(BN_Stop);
      EtatBN_Down = digitalRead(BN_Down);
      
      if (EtatBN_UP == 0 && EtatBN_Down == 1 && EtatBN_Stop == 1) // BN Up pressed...
      {
       Serial.println("BN Up Pressed...");
       Timer = 0; // Reset Action.
       CommandVolet(1);
      }

      if (EtatBN_UP == 1 && EtatBN_Down == 0 && EtatBN_Stop == 1) // BN Down pressed...
      {
       Serial.println("BN Down Pressed...");
       Timer = 0; // Reset Action.
       CommandVolet(2);
      }

      if (EtatBN_UP == 1 && EtatBN_Down == 1 && EtatBN_Stop == 0) // BN Stop pressed...
      {
       Serial.println("BN Stop Pressed...");
       StopVolet();
      }
          
      if (Timer > 0)
      {
       delay(1000);
       Timer--;
       Serial.println("Timer = "+String(Timer)+".");
      }
      else
      {
       StopVolet();
      }
      server.handleClient();
    }