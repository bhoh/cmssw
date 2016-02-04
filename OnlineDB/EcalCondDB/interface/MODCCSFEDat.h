#ifndef MODCCSFEDAT_H
#define MODCCSFEDAT_H

#include <map>
#include <stdexcept>

#include "OnlineDB/EcalCondDB/interface/IDataItem.h"
#include "OnlineDB/EcalCondDB/interface/MODRunIOV.h"
#include "OnlineDB/EcalCondDB/interface/EcalLogicID.h"

class MODCCSFEDat : public IDataItem {
 public:
  friend class EcalCondDBInterface;
  MODCCSFEDat();
  ~MODCCSFEDat();

  // User data methods
  inline std::string getTable() { return "OD_CCS_FE_DAT"; }

  inline void setWord(int x) { m_word = x; }
  inline int getWord() const { return m_word; }

 private:
  void prepareWrite() 
    throw(std::runtime_error);

  void writeDB(const EcalLogicID* ecid, const MODCCSFEDat* item, MODRunIOV* iov )
    throw(std::runtime_error);

  void writeArrayDB(const std::map< EcalLogicID, MODCCSFEDat >* data, MODRunIOV* iov)
  throw(std::runtime_error);



  void fetchData(std::map< EcalLogicID, MODCCSFEDat >* fillMap, MODRunIOV* iov)
     throw(std::runtime_error);

  // User data
  int m_word;

};

#endif
