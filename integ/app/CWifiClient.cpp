#include "CWifiClient.h"
#include "secrets.h"

namespace ele400
{
    // MQTT publish topics
    static const char PROGMEM IOT_EVENT_TOPIC[] = "devices/{device_id}/messages/events/";
    static const char PROGMEM IOT_DIRECT_METHOD_RESPONSE_TOPIC[] = "$iothub/methods/res/{status}/?$rid={request_id}";

    // MQTT subscribe topics
    static const char PROGMEM IOT_C2D_TOPIC[] = "devices/{device_id}/messages/devicebound/#";
    static const char PROGMEM IOT_DIRECT_MESSAGE_TOPIC[] = "$iothub/methods/POST/#";

    CWiFiClient::CWiFiClient(   CHardwareImpl& impl)
    :   _ssid(SECRET_SSID)
    ,   _pass(SECRET_PASS)
    ,   _broker(SECRET_BROKER)
    ,   _deviceId(SECRET_DEVICE_ID)
    ,   _lastMillis(0)
    ,   _wifiClient()
    ,   _sslClient(_wifiClient)
    ,   _mqttClient(_sslClient)
    ,   _impl(&impl)
    ,   _enableUpdates(false)
    {
        //none for now
    }

    void CWiFiClient::init( void            (*onMsgCallback)(int)   ,
                            unsigned long   (*getTimeCallback)()    )
    {
        if (!ECCX08.begin())
        {
            Serial.println("No ECCX08 present!");
            while (1);
        }
 
        // reconstruct the self signed cert
        ECCX08SelfSignedCert.beginReconstruction(0, 8);
        ECCX08SelfSignedCert.setCommonName(ECCX08.serialNumber());
        ECCX08SelfSignedCert.endReconstruction();

        // Set a callback to get the current time
        // used to validate the servers certificate
        ArduinoBearSSL.onGetTime(getTimeCallback);

        // Set the ECCX08 slot to use for the private key
        // and the accompanying public certificate for it
        _sslClient.setEccSlot(0, ECCX08SelfSignedCert.bytes(), ECCX08SelfSignedCert.length());

        // Set the client id used for MQTT as the device id
        _mqttClient.setId(_deviceId);

        // Set the username to "<broker>/<device id>/api-version=2018-06-30" and empty password
        String username;

        username += _broker;
        username += "/";
        username += _deviceId;
        username += "/api-version=2018-06-30";

        _mqttClient.setUsernamePassword(username, "");

        // Set the message callback, this function is
        // called when the MQTTClient receives a message
        _mqttClient.onMessage(onMsgCallback);
    }
    bool CWiFiClient::loop(bool eventOccured)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            connectWiFi();
        }

        if (!_mqttClient.connected())
        {
            // MQTT client is disconnected, connect
            connectMQTT();
        }

        // poll for new MQTT messages and send keep alives
        _mqttClient.poll();

        // publish a message roughly every 5 seconds.
        if (    (millis() - _lastMillis > TIME_BETWEEN_UPDATES) &&  
                _enableUpdates                                  &&
                eventOccured                                    )
        {
            _lastMillis = millis();
            _enableUpdates=false;
            publishUpdate();
            _impl->resetInCounts();
            return false;
        }
        return eventOccured;
    }
    void CWiFiClient::connectMQTT()
    {
        Serial.print("Attempting to MQTT broker: ");
        Serial.print(_broker);
        Serial.println(" ");

        while (!_mqttClient.connect(_broker.c_str(), 8883))
        {
            // failed, retry
            Serial.print(".");
            Serial.println(_mqttClient.connectError());
            delay(5000);
        }
        Serial.println();

        Serial.println("You're connected to the MQTT broker");
        Serial.println();

        // subscribe to a topic
        _mqttClient.subscribe("devices/" + _deviceId + "/messages/devicebound/#");
        _mqttClient.subscribe("$iothub/methods/POST/#");
    };
    void CWiFiClient::connectWiFi()
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.print(_ssid);
        Serial.print(" ");

        while (WiFi.begin(_ssid.c_str(),_pass.c_str()) != WL_CONNECTED)
        {
            // failed, retry
            Serial.print(".");
            delay(5000);
        }
        Serial.println();

        Serial.println("You're connected to the network");
        Serial.println();
    } 
    void CWiFiClient::publishUpdate()
    {
        if (_impl)
        {
            Serial.println("Publishing update");
            Serial.println("topic:");
            Serial.println("devices/" + _deviceId + "/messages/events/");
            Serial.println("payload:");
            Serial.print(_impl->jsonPayload());
            // send message, the Print interface can be used to set the message contents
            _mqttClient.beginMessage("devices/" + _deviceId + "/messages/events/");
            _mqttClient.print(_impl->jsonPayload());
            _mqttClient.endMessage();
        }
        else
        {
            Serial.println("invalid pointer to implementation. cannot retrieve payload.");
        }
    }
    void CWiFiClient::onMessageReceived(int messageSize)
    {
        String topicStr = _mqttClient.messageTopic();
        topicStr.toUpperCase();
        String payloadStr=_mqttClient.readString();


        if (topicStr.startsWith(F("$IOTHUB/METHODS/POST/")))
        { // direct method callback
            handleDirectMethod(topicStr,payloadStr);
        }
        else if (topicStr.indexOf(F("/MESSAGES/DEVICEBOUND/")) > -1)
        { // cloud to device message
            handleCloud2DeviceMessage(topicStr,payloadStr);
        }
        else
        { // unknown message
            Serial.println("unknown Message:");
            Serial.println("topic:");
            Serial.print(topicStr);
            Serial.println();
            Serial.println("body:");
            Serial.print(payloadStr);
            Serial.println();
        }

    }
    void CWiFiClient::handleDirectMethod(const String& topic, const String& payload)
    {
        String msgId = topic.substring(topic.indexOf("$RID=") + 5);
        String methodName = topic.substring(topic.indexOf(F("$IOTHUB/METHODS/POST/")) + 21, topic.indexOf("/?$"));
        methodName.toUpperCase();
        Serial.println("Direct method called:");
        Serial.print(methodName);
        Serial.println();
        
        String responseTopic = (String)IOT_DIRECT_METHOD_RESPONSE_TOPIC;
        responseTopic.replace(F("{request_id}"), msgId);
        responseTopic.replace(F("{status}"), F("200")); //OK

        if (strcmp(methodName.c_str(), "REFILL") == 0)
        {
            // acknowledge receipt of the command
            _mqttClient.beginMessage(responseTopic);
            _mqttClient.print("");
            _mqttClient.endMessage();
            //take action
            if(_impl) _impl->refill();
        }
        else if(strcmp(methodName.c_str(),"ENABLE"))
        {
            // acknowledge receipt of the command
            _mqttClient.beginMessage(responseTopic);
            _mqttClient.print("");
            _mqttClient.endMessage();
            //take action
            if(_impl) _impl->enable();
            _enableUpdates=true;
        }
        //add other functions here
    }
    void CWiFiClient::handleCloud2DeviceMessage(const String& topic, const String& payload)
    {
        Serial.print(payload);
    }
}; // namespace ele400