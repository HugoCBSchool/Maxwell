#ifndef _C_PWM_H
#define _C_PWM_H

#include <Arduino.h>
#include "CHandDetector.h"
#include "configs.h"

namespace ele400
{

    class distributor
    {
    private:
        int             _usages;
        unsigned long   _timeDistributionStart;
        HandDetector    _handDetector;
        bool            _distributing;
        eventStruct     _lastEvent;

    public:
        distributor();

        void init();
        bool checkForEvent();

        inline void             refill()            {_usages=NB_USAGE_BEFORE_REFILL;};
        inline HandDetector*    getSensor()         {return &_handDetector;};
        inline eventStruct*     getLastEvent()      {return &_lastEvent;};
        inline int              getRemainingUsages(){return _usages;};
    };
}; // namespace ele400

#endif
