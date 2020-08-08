#ifndef _C_TOF_H
#define _C_TOF_H


#include <Arduino.h>
#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>
#include <Wire.h>
#include "CFlash.h"
#include "configs.h"


namespace ele400{
//*********************************************************************************************************//
    class ToF
    {
    private:
        SFEVL53L1X    _device;
        //CFlashToF     _flash;
        int           _ID;
        uint16_t      _roix,_roiy,_roic;
        uint16_t      _next_roi;
        int           _next_ID;
        uint16_t      _timingBudget;
        uint16_t      _intermesureT;
        bool          _distanceShort;
        uint16_t      _threshold;
        bool          _ready;
        bool          _started;

    public:
        ToF(int shutdownPin         , int interruptPin      ,
            int deviceID            , uint16_t timingBudget ,
            uint16_t intermesurement, bool distanceShort    ,
            uint16_t    roix        , uint16_t roiy         ,
            uint16_t    roic        , uint16_t thresh);
        //


        //getsetters
        inline int         ID()                 {return _ID;};
        inline uint16_t    getIntermesurementT(){return _intermesureT; };
        inline uint16_t    getTimingBudget()    {return _timingBudget; };
        inline bool        getDistanceShort()   {return _distanceShort;};
        inline bool        isReady()            {return _ready;};
        inline bool        isStarted()          {return _started;};

        bool        getProperty(propertyID _id, uint16_t& dest);
        bool        isCalibrationValid(uint8_t _id);
        void        setTimingBudget(uint16_t value);
        void        setDistanceShort(bool isShort);
        void        setIntermesurementT(uint16_t value);
        void        setProperty(propertyID _ID,uint16_t value,bool setInFlash);
        uint16_t    getDistance();
        void save();
        void init(uint8_t I2CAddr);
        void start();
        void stop();
        void toggle();        
        int  getObjectCloserThanThreshold();
    
        void doOffsetCalibration();
        void doThresholdCalibration();
    };
};
//*********************************************************************************************************//



#endif
