#include "ToF.h"


namespace ele400
{
//************************************************************************
    ToF::ToF(int shutdownPin         , int interruptPin      ,
            int deviceID            , uint16_t timingBudget ,
            uint16_t intermesurement, bool distanceShort    ,
            uint16_t    roix        , uint16_t roiy         ,
            uint16_t    roic        , uint16_t thresh)
        :   _device(Wire,shutdownPin,interruptPin)
        ,   _ID(deviceID)
        ,   _intermesureT(intermesurement)
        ,   _timingBudget(timingBudget)
        ,   _distanceShort(distanceShort)
        ,   _ready(false)
        ,   _roix(roix)
        ,   _roiy(roiy)
        ,   _roic(roic)
        ,   _next_roi(roic==ROIC_IN?ROIC_OUT:ROIC_IN)
        ,   _next_ID(deviceID==ID_IN?ID_OUT:ID_IN)
        ,   _threshold(thresh)
    {
        pinMode(shutdownPin,OUTPUT);
        _device.sensorOff();
    }

    void ToF::init(uint8_t I2CAddr)
    {
        Serial.println("init start");
        _device.sensorOn();
        delay(50);
        while (0!=_device.begin())
        {
            Serial.println("sensor failed to boot");
        };
        _device.sensorOn();
        while (!_device.checkBootState())
        {
          Serial.println("not booting");
            delay(10);
        }
     
        _device.sensorOn();
        _device.setI2CAddress(I2CAddr);
        if (_distanceShort)
        {
            _device.setDistanceModeShort();
        }
        else
        {
            _device.setDistanceModeLong();
        }
        Serial.println("has booted");
        _device.setTimingBudgetInMs(_timingBudget);
        _device.setIntermeasurementPeriod(_intermesureT);
        _device.setROI(_roix,_roiy,_roic);
        Serial.println("init flash");
        //_flash.init();
        Serial.println("init done");
    }
    void ToF::start()
    {
        if(_device.checkBootState()&&!_started)
        {
          //Serial.println("start ranging called");
            _device.startRanging();
        }
        _started=true;
    }
    void ToF::stop()
    {
        if(_started)
        {
          //Serial.println("stop ranging called");
            _device.stopRanging();
            _device.clearInterrupt();
            delay(5);
        }
        _started=false;
    }
    void ToF::save()
    {
        //_flash.commit();
    }
    void ToF::setTimingBudget(uint16_t value)
    {
        bool _init;
        if(value!=_timingBudget)
        {
            _timingBudget=value;
            if(_init=_started) 
                stop();
            _device.setTimingBudgetInMs(value);
            if(_init)
                start();
        }
    }
    void ToF::setDistanceShort(bool isShort)
    {
        bool _init;
        if(isShort!=_distanceShort)
        {
            _distanceShort=isShort;
            if(_init=_started) 
                stop();
            if(isShort)
            {
                _device.setDistanceModeShort();
            }
            else
            {
                _device.setDistanceModeLong();
            }
            if(_init)
                start();
        }
    }
    void ToF::setIntermesurementT(uint16_t value)
    {
        bool _init;
        if(value!=_intermesureT)
        {
            _intermesureT=value;
            if(_init=_started) 
                stop();
            _device.setIntermeasurementPeriod(value);
            if(_init)
                start();
        }
    }
    void ToF::doOffsetCalibration()
    {
        Serial.println();
        Serial.println("*****************************************************************************************************");
        Serial.println("                                    Offset calibration");
        Serial.println("Place a light grey (17 % gray) target at a distance of 140mm in front of the VL53L1X sensor.");
        Serial.println("The calibration will start 5 seconds after a distance below 10 cm was detected for 5 second.");
        Serial.println("Use the resulting offset distance as parameter for the setOffset() function called after begin().");
        Serial.println("-----------------------------------------------------------------------------------------------------");
        Serial.println();

        int tLowDistanceCount = 0;
        _device.startRanging();
        double tempTime = 5000 / _intermesureT;
        while (tLowDistanceCount < tempTime)
        {
            while (!_device.checkForDataReady())
            {
                delay(1);
            }
            if (_device.getDistance() < 100)
            {
                tLowDistanceCount++;
            }
            else
            {
                tLowDistanceCount = 0;
            }
            _device.clearInterrupt();
        }
        _device.stopRanging();

        Serial.println("Distance below 10cm detected for more than 5 second, start offset calibration in 5 seconds");
        
        delay(5000);

        _device.calibrateOffset(140);
        _device.calibrateXTalk(140);

        //_flash.setProperty(propertyID::OFFSET,_device.getOffset());
        //_flash.setProperty(propertyID::XTALK,_device.getXTalk());
        Serial.print("Offset: ");
        Serial.print(_device.getOffset());
        Serial.println();
        Serial.print("XTalk: ");
        Serial.print(_device.getXTalk());
        Serial.println();
        //_ready=isCalibrationValid(calibrationID::THRESHOLD);

        Serial.println("XTalk and Offset calibrated and written to flash. No commitments to flash have been done. Must call save.");
        Serial.println("*****************************************************************************************************");
    }
    void ToF::doThresholdCalibration()
    {
        Serial.println();
        Serial.println("*****************************************************************************************************");
        Serial.println("                                    Threshold calibration");
        Serial.println("Place a light grey (17 % gray) target at the minimum distance where we consider that nobody is in the FOV of the sensor.");
        Serial.println("The calibration will start 5 seconds after a distance below 10 cm was detected for 5 second.");
        Serial.println("-----------------------------------------------------------------------------------------------------");
        Serial.println();

        int tLowDistanceCount = 0;
        stop();
        start();
        double tempTime = 5000 / _intermesureT;
        while (tLowDistanceCount < tempTime)
        {
            while (!_device.checkForDataReady())
            {
                delay(1);
            }
            if (_device.getDistance() < 100)
            {
                tLowDistanceCount++;
            }
            else
            {
                tLowDistanceCount = 0;
            }
            _device.clearInterrupt();
        }
        stop();

        Serial.println("Distance below 10cm detected for more than 5 second, start offset calibration in 5 seconds");
        
        delay(5000);

        start();

        uint16_t mindistance=0;
        uint16_t distance;
        while(tLowDistanceCount<tempTime)
        {
            while (!_device.checkForDataReady())
            {
                delay(1);
            }
            if(_device.getRangeStatus()==0)
            {
                distance=_device.getDistance();
                mindistance=(mindistance=0?distance:(distance<mindistance?distance:mindistance));
                tLowDistanceCount++;
            }
            _device.clearInterrupt();
        }

        Serial.println("threshold calibrated and written to flash. No commitments to flash have been done. Must call save.");
        Serial.println("value obtained:");
        Serial.print(mindistance);
        Serial.println("*****************************************************************************************************");
        stop();
    }
    int ToF::getObjectCloserThanThreshold()
    {
        uint16_t distance;
        uint16_t status;
        if(!isStarted()) 
        {
            start();
            //SSerial.println("threshold measurement operation started");
        }
        if(_device.checkForDataReady())
        {
            //Serial.println("data available, fetching distance");
            distance=_device.getDistance();
            status=_device.getRangeStatus();
            _device.clearInterrupt();

            if(status==0||status==2) return (distance<_threshold)?1:0;

            Serial.print("ranging error: ");
            //Serial.println(status);
            //Serial.print("distance ");Serial.println(distance);
            /*Serial.println(    (   _ID==ID_IN
                                    ?   "SENSOR INWARD"
                                    :   _ID==ID_OUT
                                        ?   "SENSOR OUTWARD"
                                        :   "SENSOR HAND"));*/
            return -2;
        }
        return -1;
    }
    uint16_t ToF::getDistance()
    {
        uint16_t distance;
        int statuss;
        /*if(!isCalibrationValid(calibrationID::OFFSET|calibrationID::THRESHOLD))
        {
            Serial.println("Calibration state invalid. All calibrations must be done to operate in this mode.");
            return -3;
        }*/
        start();
        /*
        while(!_device.checkForDataReady()){
          Serial.println("waiting");
          delay(1000);
         }*/
        
        distance=_device.getDistance();
        statuss=_device.getRangeStatus();
        _device.clearInterrupt();
        stop();
        Serial.println("distance:");
        Serial.print(distance);
        Serial.println();
        Serial.println("status");
        Serial.print(statuss);
        Serial.println();
        delay(500);

        return statuss==0?distance:0;
    }
    void ToF::toggle()
    {
        stop();
        _ID=_next_ID;
        _next_ID=_ID==ID_IN?ID_OUT:ID_IN;
        _roic=_next_roi;
        _next_roi=_roic==ROIC_IN?ROIC_OUT:ROIC_IN;
        _device.setROI(_roix,_roiy,_roic);
    }
};//ele400
