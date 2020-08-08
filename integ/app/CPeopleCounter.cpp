#include "CPeopleCounter.h"

#define THRESH_PPL 800
namespace ele400
{
//*********************************************

    PeopleCounter::PeopleCounter()
    :   _inward(    SHUTDOWN_PIN_IN     , INTERRUPT_PIN_IN  , 
                    ID_IN               , TIMING_BUDGET_PPL ,
                    INTERMESURET_PPL    , DISTMODE_PPL      , 
                    ROIX_PPL            , ROIY_PPL          ,
                    ROIC_IN             , THRESH_PPL      )
    ,   _count(0)
    ,   _eventCount(1)
    ,   _maxCount(DEFAULT_MAX_COUNT)
    ,   _inPrevious(NOBODY)
    ,   _outPrevious(NOBODY)
    {
        for(int i=0;i<4;i++){ _eventqueue[i]=0;}
        _lastEvent.eType=eventType::NONE;
        _lastEvent.eventTime=0;
        //todo
    }
//*********************************************

//*********************************************
    eventType PeopleCounter::checkForEvent()
    {
        int         status1=-1;
        int         status2=-1;                    //return statusses of sensor scans
        eventType   returnValue=eventType::NONE;  //for return value
        
        
        while(status1<0)
        {

            status1=_inward.getObjectCloserThanThreshold();
            if(status1==-3)
            {
                Serial.println("inward sensor not calibrated. Blocking");
                while(1){}
            }
        }
        _inward.toggle();
        if(processEvent(status1,_inward.ID())) returnValue=_lastEvent.eType;
        while(status1<0)
        {

            status1=_inward.getObjectCloserThanThreshold();
            if(status1==-3)
            {
                Serial.println("inward sensor not calibrated. Blocking");
                while(1){}
            }
        }
        _inward.toggle();
        if(processEvent(status1,_inward.ID())) returnValue=_lastEvent.eType;        
        
        return returnValue;
    }
//*********************************************
    bool PeopleCounter::processEvent(uint8_t event,uint8_t eventValue)
    {
        int AllZonesCurrentStatus = 0;
        int AnEventHasOccured = 0;


        // left zone
        if (eventValue == _inward.ID())
        {
            if (event != _inPrevious)
            {
                // event in left zone has occured
                AnEventHasOccured = 1;

                if (event == SOMEONE)
                {
                    AllZonesCurrentStatus += 1;
                }
                // need to check right zone as well ...
                if (_outPrevious == SOMEONE)
                {
                    // event in left zone has occured
                    AllZonesCurrentStatus += 2;
                }
                // remember for next time
                _inPrevious = event;
            }
        }
        // right zone
        else
        {
            if (event != _outPrevious)
            {
                // event in left zone has occured
                AnEventHasOccured = 1;
                if (event == SOMEONE)
                {
                    AllZonesCurrentStatus += 2;
                }
                // need to left right zone as well ...
                if (_inPrevious == SOMEONE)
                {
                    // event in left zone has occured
                    AllZonesCurrentStatus += 1;
                }
                // remember for next time
                _outPrevious = event;
            }
        }

        // if an event has occured
        if (AnEventHasOccured)
        {
            if (_eventCount < 4)
            {
                _eventCount++;
            }

            // if nobody anywhere lets check if an exit or entry has happened
            if ((_inPrevious == NOBODY) && (_outPrevious == NOBODY))
            {
                // check exit or entry only if eventqueueFillingSize is 4 (for example 0 1 3 2) and last event is 0 (nobobdy anywhere)
                if (_eventCount == 4)
                {
                    // check exit or entry. no need to check eventqueue[0] == 0 , it is always the case

                    if ((_eventqueue[1] == 1) && (_eventqueue[2] == 3) && (_eventqueue[3] == 2))
                    {
                        // This an entry
                        setCount(_count+1);
                        _lastEvent.eType=eventType::ENTRY;
                    }
                    else if ((_eventqueue[1] == 2) && (_eventqueue[2] == 3) && (_eventqueue[3] == 1))
                    {
                        // This an exit
                        setCount(_count-1);
                        _lastEvent.eType=eventType::EXIT;
                    }
                }
                else
                {
                    _lastEvent.eType = eventType::NONE;
                }
                
                _eventCount = 1;
            }
            else
            {
                // update eventqueue
                // example of eventqueue update
                // 0
                // 0 1
                // 0 1 3
                // 0 1 3 1
                // 0 1 3 3
                // 0 1 3 2 ==> if next is 0 : check if exit
                _eventqueue[_count - 1] = AllZonesCurrentStatus;
                _lastEvent.eType=eventType::NONE;
            }
            _lastEvent.eventTime=millis();
        }

        return AnEventHasOccured;
    }
//*********************************************
    void PeopleCounter::reset()
    {
        _count=0;
        _eventCount=0;
        for(int i=0;i<4;i++)
        {
            _eventqueue[i]=0;
        }
        _lastEvent.eType=eventType::NONE;
        _lastEvent.eventTime=0;
    }
//*********************************************
    void PeopleCounter::init()
    {
        _inward.init(I2C_IN);
        pinMode(GREEN_LED_PIN,OUTPUT);
        digitalWrite(GREEN_LED_PIN,0);
        pinMode(RED_LED_PIN,OUTPUT);
        digitalWrite(RED_LED_PIN,0);
    }
//*********************************************
    void PeopleCounter::setCount(int value)
    {
        if(value>0) 
        {
            _count=value;
        }
        else
        {
            _count=0;
        }
        digitalWrite(RED_LED_PIN, _count>= _maxCount ? 1 : 0);
        digitalWrite(GREEN_LED_PIN, _count >= _maxCount ? 0 : 1);
    }
}; // namespace ele400
