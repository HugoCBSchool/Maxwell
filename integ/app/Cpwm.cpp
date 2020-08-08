#include "Cpwm.h"


namespace ele400
{
    distributor::distributor()
    :   _usages(NB_USAGE_BEFORE_REFILL)
    ,   _timeDistributionStart(0)
    ,   _distributing(false)
    ,   _handDetector()  
    {
        _lastEvent.eType=eventType::NONE;
        _lastEvent.eventTime=0;
    }

    void distributor::init()
    {
        pinMode(PWM_PIN_PUMP,OUTPUT);
        analogWrite(PWM_PIN_PUMP,0);
        _handDetector.init();
    }

    bool distributor::checkForEvent()
    {
        bool AnEventOccured=false;
        unsigned long _time;

        if(!_handDetector.isReady())
        {
            Serial.println("The hand detector is not calibrated. Unable to start distributor. Stalling.");
            while(1){}
        }

        //if an event occured during hand detection
        if(_handDetector.checkForEvent())
        {
            //if it is a hand event
            if( _handDetector.getCurrentState()->eType==eventType::HAND )
            {
                //if we are not distributing
                if(!_distributing)
                {
                    //if not empty
                    if(_usages>0)
                    {
                        //toggle pump to start distribution
                        _timeDistributionStart = millis();
                        _distributing = true;
                        analogWrite(PWM_PIN_PUMP, PWM_ANALOG_VALUE);
                    }
                }
                else
                {
                    //if we are still distributing we shouldnt recieve an event
                    Serial.println("unexpected HAND event during distribution");
                    //we can still process if we need to set off the pump to stop distribution
                    _time=millis();

                    //if we reached the endpoint of distribution sequence
                    if(_time-_timeDistributionStart>=TIME_FOR_DOSE)
                    {
                        //an event occured on class variables
                        AnEventOccured=true;
                        _lastEvent.eType=eventType::DISTRIBUTION;
                        _lastEvent.eventTime=_time;
                        //toggle pump to off and countdown
                        analogWrite(PWM_PIN_PUMP,0);
                        _usages=_usages>0?(_usages-1):0;
                        _distributing=false;
                    }
                }

            }
            else if (_handDetector.getCurrentState()->eType==eventType::NOHAND)
            {//else if the event is of type nohand

                //if we are currently distributing
                if(_distributing)
                {

                    //fetch time
                    _time=millis();
                    //compare to know if we should stop
                    if(_time-_timeDistributionStart>=TIME_FOR_DOSE)
                    {
                        //we should stop
                        AnEventOccured=true;
                        _lastEvent.eType=eventType::DISTRIBUTION;
                        _lastEvent.eventTime=_time;
                        _usages=_usages>0?(_usages-1):0;
                        analogWrite(PWM_PIN_PUMP,0);
                        _distributing=false;
                    }
                }
                else
                {
                    //unexpected state, no actions taken
                    Serial.println("unexpected NOHAND event during standby for distribution");
                }
            }
            else
            {
                //unexpected
                Serial.println("unexpected event type fetched from hand detector after positive check for event call");
            }
        }
        else if(_distributing)
        {//if no event, still look if we need to stop the pump
            _time = millis();

            if (_time - _timeDistributionStart >= TIME_FOR_DOSE)
            {
                AnEventOccured = true;
                _lastEvent.eType = eventType::DISTRIBUTION;
                _lastEvent.eventTime = _time;
                _usages=_usages>0?(_usages-1):0;
                analogWrite(PWM_PIN_PUMP, 0);
                _distributing = false;
            }
        }

        return AnEventOccured;
    }
}; // namespace ele400
