#ifndef _C_HANDDETECTOR_H
#define _C_HANDDETECTOR_H

#include "ToF.h"
#include "configs.h"


namespace ele400
{
//-------------------------------
    class HandDetector
    {

    private:
        uint8_t         _previous;
        ToF             _device;
        eventStruct     _currentState;
        eventStruct     _previousState;


    public://methods
        HandDetector();

        inline  eventStruct* getCurrentState()   {return &_currentState;};
        inline  eventStruct* getPreviousState()  {return &_previousState;};
        inline  ToF*         getDevice()         {return &_device;};
        inline  bool         isReady()           {return _device.isReady();};
                void         init();
                void         reset();
                eventType    checkForEvent();


    private://methods
        bool    processEvent(uint8_t event);

    };
};


#endif
