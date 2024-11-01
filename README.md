# HydroponieNode1
Noeud CAN controlé par un arduino nano communiquant les valeurs de différents capteurs:
- Capteur de temperature DS18B20 (valeur de type float)
- Capteur TDS (valeur de type float)
- Capteur ultrason JSN SR04 (valeur de type float)
- Capteur de présence d'eau (valeur de type boolean)

## Les message CAN
Toutes les 5 secondes les mesures sont envoyées dans deux messages CAN:

* Message CAN 1 :
    * ID : 100
    * Data0 : presence d'eau (boolean 0 ou 1)
    * Data1 : Température (float 1/4)
    * Data2 : Température (float 2/4)
    * Data3 : Température (float 3/4)
    * Data4 : Température (float 4/4)
* Message CAN 2 :
    * ID : 200
    * Data0 : Distance Sonar (float 1/4)
    * Data1 : Distance Sonar (float 2/4)
    * Data2 : Distance Sonar (float 3/4)
    * Data3 : Distance Sonar (float 4/4)
    * Data4 : TDS (float 1/4)
    * Data5 : TDS (float 2/4)
    * Data6 : TDS (float 3/4)
    * Data7 : TDS (float 4/4)
