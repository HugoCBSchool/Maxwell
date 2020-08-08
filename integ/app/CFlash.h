#ifndef _C_FLASH_H
#define _C_FLASH_H

#include "FlashStorage.h"
#include "configs.h"



namespace ele400
{
    typedef struct
    {
        uint16_t        threshold;
        uint16_t        offset;
        uint16_t        XTalk;
    } ToFCfg;

    typedef struct
    {
        ToFCfg     sToFCfg;
        uint8_t    valid;
    } FlashContentTofCfg;

};

namespace ele400
{
    class CFlashToF
    {

    public:
        CFlashToF(FlashStorageClass<FlashContentTofCfg>& flash);
        
        void init();
        void commit();

    //getsetters
        uint16_t    getProperty(propertyID _id);
        void        setProperty(propertyID _id, uint16_t value);

        void setValidity(uint8_t validityMask,bool value);
        bool checkValidity(uint8_t validityMask,bool value);


    private://utils
        uint16_t* property(propertyID _id);
        ToFCfg*   read();


    private :         
        bool                _initialized;
        FlashContentTofCfg  _content;
        bool                _dirty;

        FlashStorageClass<FlashContentTofCfg>& h_flash_storage;

    };

}; // namespace ele400

#endif
