
#include "CHardwareImpl.h"

namespace ele400
{
    CHardwareImpl::CHardwareImpl()
    :   _inDistroCount(0)
    ,   _inCount(0)
    ,   _pplCounter()
    ,   _distributor()
    ,   _isEnabled(false)
    {}


    void CHardwareImpl::init()
    {
        _pplCounter.init();
        _distributor.init();
    }

    void CHardwareImpl::doOffsetCalibration(uint8_t device)
    {
        switch(device)
        {
            case ID_OUT:
                _pplCounter.getInward()->doOffsetCalibration();
                break;
            case ID_IN:
                _pplCounter.getInward()->doOffsetCalibration();
                break;
            case ID_HAND:
                _distributor.getSensor()->getDevice()->doOffsetCalibration();
                break;
        }
    }
    void CHardwareImpl::doDistanceCalibration(uint8_t device)
    {
        switch(device)
        {
            case ID_OUT:
                _pplCounter.getInward()->doThresholdCalibration();
                break;
            case ID_IN:
                _pplCounter.getInward()->doThresholdCalibration();
                break;
            case ID_HAND:
                _distributor.getSensor()->getDevice()->doThresholdCalibration();
                break;
        }
    }
    bool CHardwareImpl::doHandCycle()
    {
        return _isEnabled?_distributor.checkForEvent():false;
    }
    bool CHardwareImpl::doPplCycle()
    {
        //if an event occured
        if(!_isEnabled) return false;
        
        if(_pplCounter.checkForEvent())
        {
            //increment ingress count only if event is an entry
            _inCount+=( (_pplCounter.getLastEvent()->eType==eventType::ENTRY)
                        ?   1
                        :   0   );

            //increment clean ingress only if
            //  event is an entry and
            //  last distributor event is a distribution and
            //  the time between distribution and crossing is below 3 seconds
            _inDistroCount+=(   (_pplCounter.getLastEvent()->eType==eventType::ENTRY)
                                ?   (_distributor.getLastEvent()->eType==eventType::DISTRIBUTION)
                                    &&
                                    (       (_pplCounter.getLastEvent()->eventTime)
                                        -   (_distributor.getLastEvent()->eventTime)
                                        <   MAX_TIME_HAND2IN)
                                    ?   1
                                    :   0
                                :   0);
            return true;
        }
        return false;
    }
    String& CHardwareImpl::jsonPayload()
    {
        String payload = F("{\"occupancy\": {occupancy},\"usages\": {usages},\"inevents\": {inevents},\"incleanevents\": {incleanevents}}");
        payload.replace(F("{occupancy}"),String(getTotalInside()));
        payload.replace(F("{usages}"),String(getRemainingUsages()));
        payload.replace(F("{inevents}"),String(getTotalIngress()));
        payload.replace(F("{incleanevents}"),String(getCleanIngress()));
        return payload;
    }
};
