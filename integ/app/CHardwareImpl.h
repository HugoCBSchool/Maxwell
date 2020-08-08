#ifndef _C_HARDWAREIMPL_H
#define _C_HARDWAREIMPL_H


#include "CPeopleCounter.h"
#include "Cpwm.h"


#define MAX_TIME_HAND2IN 5000

namespace ele400
{
    class CHardwareImpl
    {
    private:
        PeopleCounter   _pplCounter;
        distributor     _distributor;
        
        int             _inCount        ,
                        _inDistroCount  ;
        bool            _isEnabled;

    public:
        CHardwareImpl();
        void init();

        void doOffsetCalibration(uint8_t device);
        void doDistanceCalibration(uint8_t device);
        bool doHandCycle();
        bool doPplCycle();

        String& jsonPayload();

        inline int  getTotalIngress()       {return _inCount;};
        inline int  getCleanIngress()       {return _inDistroCount;};
        inline int  getTotalInside()        {return _pplCounter.getCount();};
        inline int  getRemainingUsages()    {return _distributor.getRemainingUsages()>-1
                                                    ?   _distributor.getRemainingUsages()
                                                    :   0;};
        inline void refill()                {_distributor.refill();};
        inline void resetInCounts()         {_inCount=0;_inDistroCount=0;};
        inline bool isEnabled()             {return _isEnabled;};
        inline void enable()                {_isEnabled=true;};
    };
} // namespace ele400


#endif
