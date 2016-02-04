// $Id: ErrorEventData.cc,v 1.6 2010/05/17 15:59:10 mommsen Exp $
/// @file: ErrorEventData.cc

#include "EventFilter/StorageManager/src/ChainData.h"

#include "IOPool/Streamer/interface/FRDEventMessage.h"

namespace stor
{

  namespace detail
  {

    ErrorEventMsgData::ErrorEventMsgData(toolbox::mem::Reference* pRef) :
      ChainData(I2O_SM_ERROR, Header::ERROR_EVENT),
      _headerFieldsCached(false)
    {
      addFirstFragment(pRef);
      parseI2OHeader();
    }

    inline size_t ErrorEventMsgData::do_i2oFrameSize() const
    {
      return sizeof(I2O_SM_DATA_MESSAGE_FRAME);
    }

    unsigned long ErrorEventMsgData::do_headerSize() const
    {
      if ( !headerOkay() )
      {
        return 0;
      }

      if (! _headerFieldsCached) {cacheHeaderFields();}
      return _headerSize;
    }

    unsigned char* ErrorEventMsgData::do_headerLocation() const
    {
      if ( !headerOkay() )
      {
        return 0;
      }

      if (! _headerFieldsCached) {cacheHeaderFields();}
      return _headerLocation;
    }

    inline unsigned char*
    ErrorEventMsgData::do_fragmentLocation(unsigned char* dataLoc) const
    {
      if ( parsable() )
      {
        I2O_SM_DATA_MESSAGE_FRAME *smMsg =
          (I2O_SM_DATA_MESSAGE_FRAME*) dataLoc;
        return (unsigned char*) smMsg->dataPtr();
      }
      else
      {
        return dataLoc;
      }
    }

    void
    ErrorEventMsgData::do_assertRunNumber(uint32_t runNumber)
    {
      if ( headerOkay() && do_runNumber() != runNumber )
      {
        std::ostringstream errorMsg;
        errorMsg << "Run number " << do_runNumber() 
          << " of error event " << do_eventNumber() <<
          " received from " << hltURL() << 
          " (FU process id " << fuProcessId() << ")" <<
          " does not match the run number " << runNumber << 
          " used to configure the StorageManager." <<
          " Enforce usage of configured run number.";
        _runNumber = runNumber;
        XCEPT_RAISE(stor::exception::RunNumberMismatch, errorMsg.str());
      }
    }

    uint32_t ErrorEventMsgData::do_runNumber() const
    {
      if ( !headerOkay() )
      {
        std::stringstream msg;
        msg << "A run number can not be determined from a ";
        msg << "faulty or incomplete ErrorEvent message.";
        XCEPT_RAISE(stor::exception::IncompleteEventMessage, msg.str());
      }
      
      if (! _headerFieldsCached) {cacheHeaderFields();}
      return _runNumber;
    }

    uint32_t ErrorEventMsgData::do_lumiSection() const
    {
      if ( !headerOkay() )
      {
        std::stringstream msg;
        msg << "A luminosity section can not be determined from a ";
        msg << "faulty or incomplete ErrorEvent message.";
        XCEPT_RAISE(stor::exception::IncompleteEventMessage, msg.str());
      }
      
      if (! _headerFieldsCached) {cacheHeaderFields();}
      return _lumiSection;
    }

    uint32_t ErrorEventMsgData::do_eventNumber() const
    {
      if ( !headerOkay() )
      {
        std::stringstream msg;
        msg << "An event number can not be determined from a ";
        msg << "faulty or incomplete ErrorEvent message.";
        XCEPT_RAISE(stor::exception::IncompleteEventMessage, msg.str());
      }
      
      if (! _headerFieldsCached) {cacheHeaderFields();}
      return _eventNumber;
    }

    inline void ErrorEventMsgData::parseI2OHeader()
    {
      if ( parsable() )
      {
        I2O_SM_DATA_MESSAGE_FRAME *smMsg =
          (I2O_SM_DATA_MESSAGE_FRAME*) _ref->getDataLocation();
        _fragKey.code_ = _messageCode;
        _fragKey.run_ = smMsg->runID;
        _fragKey.event_ = smMsg->eventID;
        _fragKey.secondaryId_ = smMsg->outModID;
        _fragKey.originatorPid_ = smMsg->fuProcID;
        _fragKey.originatorGuid_ = smMsg->fuGUID;
        _rbBufferId = smMsg->rbBufferID;
        _hltLocalId = smMsg->hltLocalId;
        _hltInstance = smMsg->hltInstance;
        _hltTid = smMsg->hltTid;
        _fuProcessId = smMsg->fuProcID;
        _fuGuid = smMsg->fuGUID;
      }
    }

    void ErrorEventMsgData::cacheHeaderFields() const
    {
      unsigned char* firstFragLoc = dataLocation(0);
      unsigned long firstFragSize = dataSize(0);
      bool useFirstFrag = false;

      // if there is only one fragment, use it
      if (_fragmentCount == 1)
      {
        useFirstFrag = true;
      }
      // otherwise, check if the first fragment is large enough to hold
      // the full Event message header  (FRD events have fixed header
      // size, so the check is easy)
      else if (firstFragSize > sizeof(FRDEventHeader_V2))
      {
        useFirstFrag = true;
      }

      boost::shared_ptr<FRDEventMsgView> msgView;
      if (useFirstFrag)
      {
        msgView.reset(new FRDEventMsgView(firstFragLoc));
      }
      else
      {
        copyFragmentsIntoBuffer(_headerCopy);
        msgView.reset(new FRDEventMsgView(&_headerCopy[0]));
      }

      _headerSize = sizeof(FRDEventHeader_V2);
      _headerLocation = msgView->startAddress();

      _runNumber = msgView->run();
      _lumiSection = msgView->lumi();
      _eventNumber = msgView->event();

      _headerFieldsCached = true;
    }

  } // namespace detail

} // namespace stor


/// emacs configuration
/// Local Variables: -
/// mode: c++ -
/// c-basic-offset: 2 -
/// indent-tabs-mode: nil -
/// End: -
