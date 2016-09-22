#ifndef __IDAQVMEINTERFACE_H_
#define __IDAQVMEINTERFACE_H_

//C++ Includes
#include <memory>

//Framework Includes
#include "IDaqVmeTypes.h"
//#include "CAENVMElib.h"

//ROOT Includes

class IDaqVmeInterface {
public:
    IDaqVmeInterface( IDaqVmeModuleType aModule = idmV2718, short aLink = 0, short aBoardNum = 0, long aBaseAddress = 0 );
    virtual ~IDaqVmeInterface();
    
    //Base Address
    long GetBaseAddress(){ return m_baseAddr; };
    void SetBaseAddress( long aBaseAddress ){ m_baseAddr = aBaseAddress; };
    //Reset I/F
    void Reset();
    
    //Link
    void setBoardNumber( short aBoardNum = 0 ){ m_sNumBoard = aBoardNum; return; };
    void setLinkNumber( short aLink = 0) { m_sNumLink = aLink; return; };
    void setModuleType( IDaqVmeModuleType aModule = idmV2718 ){ m_vmeType = aModule; return; };
    
    // Read functions
    virtual void Read( uint32_t aAddress, uint32_t &aData);
    virtual void Read( uint32_t aAddress, uint16_t &aData);
    /*template<typename RegSize_t>
    void Read( uint32_t aAddress, RegSize_t &aData){
        m_vmeStatus = (IDaqVmeCode)CAENVME_ReadCycle( m_lHandle, (unsigned long)aAddress, &aData, cvA32_U_DATA, cvD16);
        return;
    };*/
    
    virtual void ReadBLT( uint32_t aAddress, uint16_t &aData, const uint16_t nData, uint16_t &nDataRead);
    virtual void ReadBLT( uint32_t aAddress, uint32_t &aData, const uint16_t nData, uint16_t &nDataRead);
    virtual void ReadBLT( uint32_t aAddress, uint64_t &aData, const uint16_t nData, uint16_t &nDataRead);
    /*template<typename RegSize_t>
    void ReadBLT( uint32_t aAddress, RegSize_t &aData, const uint16_t nData, uint16_t &nDataRead){
        int nbread( 0 );
        m_vmeStatus = (IDaqVmeCode)CAENVME_BLTReadCycle( m_lHandle, ( unsigned long )aAddress, &aData, nData * 8, cvA32_U_MBLT, cvD64,&nbread );
        nDataRead = nbread/ sizeof( aData );
        return;
    };*/
    
    // Write functions
    virtual void Write( uint32_t aAddress, const uint32_t &aData);
    virtual void Write( uint32_t aAddress, const uint16_t &aData);
    /*template<typename RegSize_t>
    void Write( uint32_t aAddress, const RegSize_t &aData){
        m_vmeStatus = ( IDaqVmeCode ) CAENVME_WriteCycle( m_lHandle, ( unsigned long ) aAddress, ( void* ) &aData, cvA32_U_DATA, cvD16 );
        return;
    };*/
    
    virtual void WriteA24(uint32_t, const uint16_t&);
    //Status
    virtual IDaqVmeCode GetStatus(){ return m_vmeStatus; };
    virtual void ResetStatus(){ m_vmeStatus = IDaqSuccess; };
    
    //Connection
    virtual void Connect();
    virtual void Disconnect();
    
    //Registers
    virtual void CheckRegister( CVRegisters reg );
    
    virtual void SetOutputLevel( int aOutput, IDaqSwitch sw );
    virtual void CheckOuputConf( int aOutput );
    virtual void SetOutputSource( int aOutput, CVIOSources s);
    
    virtual void SetScalerConf();
    virtual void ResetScaler();
    virtual void CheckScaler();
    virtual unsigned short GetScaler(){ return m_usScaler; };
    virtual void EnableScaler();
    virtual void DisableScaler();
    
    virtual uint16_t GetInputLevel( int aInput ) { return (m_uiInputLevel.get())[ aInput ]; }
    virtual void CheckInputLevel();
    virtual void SetInputConf( int aInput, bool aPolarity );
    virtual void GetInputConf( int aInput, int &aPol, int &aLedPol ){ aPol = (m_cvInIOPol.get())[ aInput ]; aLedPol = (m_cvInLedPol.get())[ aInput ]; };
    virtual void CheckInputConf( int aInput = 0 );
    
    
private:
    IDaqVmeModuleType m_vmeType;
    short m_sNumLink;
    short m_sNumBoard;
    long  m_baseAddr;
    long m_lHandle;
    bool m_bisConnected;
    IDaqVmeCode m_vmeStatus;
    
    /*uint16_t       inputLevel[ 3 ];
    uint16_t       internalReg[ 44 ];
    CVIOPolarity   outPol[ 5 ];
    CVLEDPolarity  outLedPol[ 5 ];
    CVIOSources    outSource[ 5 ];
    CVIOPolarity   inPol[ 2 ];
    CVLEDPolarity  inLedPol[ 2 ];*/
    std::shared_ptr<uint16_t>       m_uiInputLevel;
    std::shared_ptr<uint16_t>       m_RegInternal;
    std::shared_ptr<CVIOPolarity>   m_cvOutIOPol;
    std::shared_ptr<CVLEDPolarity>  m_cvOutLedPol;
    std::shared_ptr<CVIOSources>    m_cvOutSource;
    std::shared_ptr<CVIOPolarity>   m_cvInIOPol;
    std::shared_ptr<CVLEDPolarity>  m_cvInLedPol;
    unsigned short m_usScaler;
};


#endif
