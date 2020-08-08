#ifndef _C_WIFICLIENT_H
#define _C_WIFICLIENT_H

#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <utility/ECCX08SelfSignedCert.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000
#include "CHardwareImpl.h"
#include "secrets.h"
#include "configs.h"

namespace ele400
{
    class CWiFiClient
    {
    private:
        String          _ssid;
        String          _pass;
        String          _broker;
        String          _deviceId;
        CHardwareImpl*  _impl;

        WiFiClient      _wifiClient;
        BearSSLClient   _sslClient;
        MqttClient      _mqttClient;

        unsigned long   _lastMillis;
        bool            _enableUpdates;
    
    public:
        CWiFiClient(    CHardwareImpl& impl );

        void init(  void                (*onMsgCallback)    (int)   ,
                    unsigned long       (*getTimeCallback)  ()      );
        
        void connectMQTT();
        void connectWiFi();
        bool loop(bool eventOccured);
        void onMessageReceived(int messageSize);

        inline void enableUpdates(bool enable){_enableUpdates=enable;};   
        
    private:
        void publishUpdate();
        void handleDirectMethod(const String& topic,const String& payload);
        void handleCloud2DeviceMessage(const String& topic,const String& payload);
    };

}; // namespace ele400

#endif
