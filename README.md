# HydroponieNode1
Arduino with capteur to can network

Noeud CAN a base d'arduino nano et de différents capteurs:
- Capteur de temperature DS18B20
- Capteur TDS
- Capteur ultrason JSN SR04
- Capteur de présence d'eau

## Message CAN
Toutes les 5 secondes les mesures sont réalisées par et envoyée dans deux messages CAN:
* Message 1
    * ID : 100
    * Data0 : presence d'eau (0 ou 1)
    * Data1 : Température float 1
    * Data2 : Température float 2
    * Data3 : Température float 3
    * Data4 : Température float 4
* Message 2
    * ID : 200
    * Data0 : Distance Sonar float 1
    * Data1 : Distance Sonar float 2
    * Data2 : Distance Sonar float 3
    * Data3 : Distance Sonar float 4
    * Data4 : TDS float 1
    * Data5 : TDS float 2
    * Data6 : TDS float 3
    * Data7 : TDS float 4
