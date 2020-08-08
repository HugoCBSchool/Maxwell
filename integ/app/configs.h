#ifndef _CONFIGS_H
#define _CONFIGS_H
#include <Arduino.h>

//pins
    #define PWM_PIN_PUMP            A3
    #define RED_LED_PIN             2
    #define GREEN_LED_PIN           3

    #define SHUTDOWN_PIN_HAND       6
    #define SHUTDOWN_PIN_IN         A2
    #define SHUTDOWN_PIN_OUT        5

    #define INTERRUPT_PIN_HAND      7
    #define INTERRUPT_PIN_IN        A1
    #define INTERRUPT_PIN_OUT       4

//I2C_
    #define I2C_DEFAULT_ADR         0x52
    #define I2C_HAND                0x52
    #define I2C_IN                  0x56
    #define I2C_OUT                 0x54


//tof
    //hardware
        //hand
            #define TIMING_BUDGET_HAND  30
            #define INTERMESURET_HAND   30
            #define DISTMODE_HAND       false
            #define PWM_ANALOG_VALUE    199
            #define ROIX_HAND           16
            #define ROIY_HAND           16
            #define ROIC_HAND           199
        //ppl 
            #define TIMING_BUDGET_PPL   30
            #define INTERMESURET_PPL    30
            #define DISTMODE_PPL        true
            #define ROIX_PPL            8
            #define ROIY_PPL            16
            #define ROIC_IN             167
            #define ROIC_OUT            231
    //implementation
        #define ID_HAND                 3
        #define ID_OUT                  2
        #define ID_IN                   1
        //ppl
            #define NOBODY                  0    
            #define SOMEONE                 1
            #define DEFAULT_MAX_COUNT       30
            #define DEFAULT_THRESHOLD_IN    1000
            #define DEFAULT_THRESHOLD_OUT   1000
        //hand
            #define NB_USAGE_BEFORE_REFILL  45
            #define TIME_FOR_DOSE           300

//wifi related
    #define TIME_BETWEEN_UPDATES            5000



namespace ele400
{
    typedef enum
    {
        NONE,
        ENTRY,
        EXIT,
        HAND,
        NOHAND,
        DISTRIBUTION
    } eventType;

    typedef enum
    {
        THRESH = 0x01,
        OFFSET = 0x02,
        XTALK = 0x04,
    } propertyID;

    namespace calibrationID
    {
        static const uint8_t OFFSET = (propertyID::OFFSET |
                                       propertyID::XTALK    );
        static const uint8_t THRESHOLD = propertyID::THRESH;
    }; // namespace calibrationID

    typedef struct
    {
        eventType eType;
        unsigned long eventTime;
    } eventStruct;
}; // namespace ele400

#endif
