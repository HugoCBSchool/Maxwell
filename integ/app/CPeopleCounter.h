#ifndef _C_PEOPLECOUNTER_H
#define _C_PEOPLECOUNTER_H

#include "ToF.h"
#include <Arduino.h>
#include "configs.h"

namespace ele400
{
    class PeopleCounter
    {

    private:
        ToF             _inward;
        int             _count;
        int             _eventqueue[4];
        eventStruct     _lastEvent;
        int             _eventCount;
        uint8_t         _inPrevious;
        uint8_t         _outPrevious;

        uint32_t        _maxCount;

    public:
        PeopleCounter();
        
        void        init();
        void        reset();
        eventType   checkForEvent();
        void        setMaxCount(uint32_t maxCount);

        inline  ToF*            getInward()      {return &_inward;};
        inline  int             getCount()       {return _count;};
        inline  eventStruct*    getLastEvent()   {return &_lastEvent;};
        inline  bool            isFull()         {return _count>=_maxCount;};

    private:
        void setCount(int value);
        bool processEvent(uint8_t event, uint8_t eventValue);
    };
};

#endif