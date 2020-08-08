#include "CFlash.h"

namespace ele400
{
    CFlashToF::CFlashToF(FlashStorageClass<FlashContentTofCfg>& flash) 
    :   _initialized(false) 
    ,   _dirty(false)
    ,   h_flash_storage(flash)
    {}

    void CFlashToF::init()
    {
      Serial.println("reading flash");
        _content = h_flash_storage.read();
        Serial.println("reading flash done");
        if (!checkValidity(THRESH, true))
            _content.sToFCfg.threshold = 0;
        if (!checkValidity(OFFSET, true))
            _content.sToFCfg.offset = 0;
        if (!checkValidity(XTALK, true))
            _content.sToFCfg.XTalk = 0;
        _initialized = true;
    }
    void CFlashToF::commit()
    {
        if (!_initialized)
            init();
        if (_dirty)
        {
            h_flash_storage.write(_content);
        }
        _dirty = false;
    }
    ToFCfg *CFlashToF::read()
    {
        if (!_initialized)
            init();
        return (&_content.sToFCfg);
    }
    
    void CFlashToF::setValidity(uint8_t validityMask,bool value)
    {
        _dirty=(checkValidity(validityMask,value)?true:_dirty);

        _content.valid= (   value
                            ?   (_content.valid|validityMask)
                            :   _content.valid&(~validityMask)  );
    }
    bool CFlashToF::checkValidity(uint8_t validityMask,bool valid)
    {
        if (!_initialized)
            init();

        return 
        (    
            (   
                (   
                    valid
                    ?   _content.valid
                    :   ~_content.valid 
                ) 
                & 
                validityMask 
            ) 
            == 
            validityMask 
        );
    }
    

    uint16_t CFlashToF::getProperty(propertyID _id)
    {
        switch (_id)
        {
        case THRESH:
            return _content.sToFCfg.threshold;
        case OFFSET:
            return _content.sToFCfg.offset;
        case XTALK:
            return _content.sToFCfg.XTalk;
        }
    }
    uint16_t* CFlashToF::property(propertyID _id)
    {
        switch (_id)
        {
        case THRESH:
            return &_content.sToFCfg.threshold;
        case OFFSET:
            return &_content.sToFCfg.offset;
        case XTALK:
            return &_content.sToFCfg.XTalk;
        }
    }
    void CFlashToF::setProperty(propertyID _id,uint16_t value)
    {
        _dirty=(    (value!=getProperty(_id))
                    ?   true
                    :   _dirty                  );

        *property(_id)=value;
        setValidity(_id,true);
    }
    


}; // namespace ele400
