#include "CHandDetector.h"
#define THRESH_HAND 200
namespace ele400
{

    HandDetector::HandDetector()
    :    _previous(NOBODY)
    ,   _device(    SHUTDOWN_PIN_HAND   , INTERRUPT_PIN_HAND , 
                    ID_HAND             , TIMING_BUDGET_HAND ,
                    INTERMESURET_HAND   , DISTMODE_HAND      ,
                    ROIX_HAND           , ROIY_HAND          ,
                    ROIC_HAND           , THRESH_HAND)
    {
        _currentState.eType=eventType::NOHAND;
        _currentState.eventTime=0;
        _previousState.eType=eventType::NOHAND;
        _previousState.eventTime=0;
    }
    void HandDetector::reset()
    {
        _device.stop();
        _previous=NOBODY;
        _currentState.eType=eventType::NOHAND;
        _currentState.eventTime=0;
        _previousState.eType=eventType::NOHAND;
        _previousState.eventTime=0;
    }
    void HandDetector::init()
    {
        _device.init(I2C_HAND);
    }

    eventType   HandDetector::checkForEvent()
    {
        int         status=-1;

        while(status<0)
        {
            status=_device.getObjectCloserThanThreshold();
            if(status==-3)
            {
                Serial.println("hand sensor not calibrated. Blocking");
                while(1){}
            }
        }

        return (    processEvent(status)    ?
                        _currentState.eType :
                       eventType::NONE      );
    }
    bool        HandDetector::processEvent(uint8_t event)
    {
        if (event != _previous)
        {
            _previousState.eventTime=_currentState.eType;
            _currentState.eventTime=millis();
            _previousState.eType=_currentState.eType;
            _currentState.eType=(   (event==1)
                                    ?   eventType::HAND
                                    :   eventType::NOHAND   );

            _previous=event;
            return true;
        }
        _previous=event;
        return false;
    }
};
