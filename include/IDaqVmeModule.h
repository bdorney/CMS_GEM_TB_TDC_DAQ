#ifndef __IDAQVMEMODULE_H_
#define __IDAQVMEMODULE_H_

#include "IDaqVmeTypes.h"
#include "IDaqVmeInterface.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

//using namespace std;

class IDaqVmeVirtualModule{

public:
    IDaqVmeVirtualModule();
    virtual ~IDaqVmeVirtualModule(){
        m_vmeInt.reset();
        m_strName.clear();
    };
    
    virtual void Initialize(){ m_vmeStatus = IDaqGenericError; return; }
    virtual void Arm(){ m_vmeStatus = IDaqGenericError; return; };
    virtual uint16_t Readout(FILE *fp, uint64_t &fs){ return 0; }
    virtual uint16_t Readout( std::vector<uint32_t>& vec_uiInputData ){ return 0; };
    virtual bool IsInitialized(){ return m_bInitStatus; }

    virtual IDaqVmeModuleType GetModuleType(){ return m_vmeType; }
    virtual void SetModuleType( IDaqVmeModuleType vmeInputType ){ m_vmeType = vmeInputType; return;}
    
    virtual uint16_t GetWordSize() { return 0; }
    
    virtual IDaqVmeCode GetStatus(){ return m_vmeStatus; };
    virtual std::string GetName(){ return m_strName; }
    virtual void SetName( std::string strInput ="Unknown Module" ){ m_strName = strInput; return; }
    virtual void Connect( std::shared_ptr<IDaqVmeInterface> inputVmeInt );

    virtual bool IsConnected();

    virtual void SetVerboseMode(bool bInput){ m_bVerboseMode = bInput; return; }

protected:
    IDaqVmeCode m_vmeStatus;
    IDaqVmeModuleKind m_vmeKind;
    IDaqVmeModuleType m_vmeType;
    std::shared_ptr<IDaqVmeInterface> m_vmeInt;
    std::string m_strName;
    bool m_bConnectStatus;
    bool m_bInitStatus;
    bool m_bVerboseMode;
}; //End class IDaqVmeVirtualModule

template <typename AddressType, typename DataType >
class IDaqVmeModule : public IDaqVmeVirtualModule {
public:
    IDaqVmeModule() : m_data(new DataType) {
	m_vmeStatus = IDaqSuccess;
	m_vmeKind = idmkUnknown;
    	m_vmeType = idmGenericModule;

        SetName();

	m_bConnectStatus = false;
	m_bInitStatus = false; 
	m_bVerboseMode = false;
    };
    
    virtual ~IDaqVmeModule(){
        m_vmeInt.reset();
        m_data.reset();
        m_strName.clear();
    };
    
    void SetVmeInterface( std::shared_ptr<IDaqVmeInterface> inputVmeInt ) { m_vmeInt = inputVmeInt; return;}
    
    virtual void SetBaseAddress( AddressType inputAddr ){ m_baseAddr = inputAddr; return;}
    virtual AddressType GetBaseAddress(){ return m_baseAddr; }
    
    //DataType * GetData() {return data;}
    std::shared_ptr<DataType> GetData(){ return m_data; }
    uint16_t   GetDataSize() { return m_bufferSize;}
    uint16_t   GetBufferCapacity() { return m_bufferSize / sizeof( DataType ); }
    uint16_t   GetWordSize() { return sizeof( DataType ); }
    uint16_t   GetNByteRead(){ return m_nWordRead * sizeof( DataType ); }
    uint16_t   GetNWordRead(){ return m_nWordRead; }
    
    DataType GetWord( uint16_t uiWordIdx = 0 ){
        //if ( iw < nWordRead) { return  data[ iw ]; }
        if ( uiWordIdx < m_nWordRead) { return  (*m_data)[ uiWordIdx ]; }
        else return 0;
    };
    
    /*template<typename RegSize_t>
    void CheckRegister( const AddressType &inputAddr, RegSize_t &RegVal ){
        AddressType addr = m_baseAddr + inputAddr;
        RegSize_t tempRegVal;
        m_vmeInt->Read<RegSize_t>( addr, RegVal );
	//m_vmeInt->Read( addr, RegVal );
        m_vmeStatus = m_vmeInt->GetStatus();
        if ( m_vmeStatus == IDaqSuccess ) RegVal = tempRegVal;
        
        return;
    };*/

    void CheckRegister( const AddressType &inputAddr, uint16_t &regval ){
	AddressType addr = m_baseAddr + inputAddr;
	uint16_t tempRegVal;
	m_vmeInt->Read( addr, tempRegVal );
	m_vmeStatus = m_vmeInt->GetStatus();
	if ( m_vmeStatus == IDaqSuccess ){ regval = tempRegVal;}

	return;
     };
    
    void CheckRegister( const AddressType &inputAddr, uint32_t &regval ){
	AddressType addr = m_baseAddr + inputAddr;
	uint32_t tempRegVal;
	m_vmeInt->Read( addr, tempRegVal );
	m_vmeStatus = m_vmeInt->GetStatus();
	if ( m_vmeStatus == IDaqSuccess ){ regval = tempRegVal;}

	return;
     };
    
    /*template<typename RegSize_t>
    void SetRegister( const AddressType &inputAddr, const RegSize_t &RegData, RegSize_t &RegVal ){
        AddressType addr = m_baseAddr + inputAddr;
        m_vmeInt->Write<RegSize_t>( addr, RegData );
        m_vmeStatus = m_vmeInt->GetStatus();
        if ( m_vmeStatus == IDaqSuccess ) RegVal = RegData;
        
        return;
    };*/

    void SetRegister( const AddressType &inputAddr, const uint16_t &RegData, uint16_t &RegVal ){
        AddressType addr = m_baseAddr + inputAddr;
        m_vmeInt->Write( addr, RegData );
        m_vmeStatus = m_vmeInt->GetStatus();
        if ( m_vmeStatus == IDaqSuccess ){ RegVal = RegData ;}

	return;
    };
    
    void SetRegister( const AddressType &inputAddr, const uint32_t &RegData, uint32_t &RegVal ){
        AddressType addr = m_baseAddr + inputAddr;
        m_vmeInt->Write( addr, RegData );
        m_vmeStatus = m_vmeInt->GetStatus();
        if ( m_vmeStatus == IDaqSuccess ){ RegVal = RegData ;}

	return;
    };
    
    virtual void ReadOutputBuffer( const uint16_t &nw, const AddressType &inputAddr = 0 ){
        m_nWordRead = 0;
        m_vmeStatus = IDaqCommError;
        if ( m_vmeInt ) {
            m_vmeInt->ReadBLT( m_baseAddr + inputAddr, (m_data.get())[ 0 ], nw, m_nWordRead );
            //m_vmeInt->ReadBLT<DataType>( m_baseAddr + inputAddr, (m_data.get())[ 0 ], nw, m_nWordRead );
            m_vmeStatus = m_vmeInt->GetStatus();
            //if ( status != IDaqSuccess ) _nWordRead = nw; else _nWordRead = 0;
            //ret = _vmeInt->ReadBLT( _BA + ad, _outputBuffer[0], nw );
            //if ( ret == MDSuccess ) { _nWordRead = nw; }
            //else { _nWordRead = 0; }
        }
        
        return;
    };
    
    void PrintOutputBuffer( std::ostream &os = std::cout ){
        for (uint16_t uiWordIdx = 0; uiWordIdx < m_nWordRead; ++uiWordIdx ) {
            os << std::endl << std::showbase << std::hex << (*m_data)[ uiWordIdx ];
        }
        
        return;
    };
    
    void WriteOutputBuffer( FILE *fp ){
        fwrite( m_data.get(), sizeof( DataType ), m_nWordRead, fp );
        
        return;
    };
    
    virtual void ReadAndWriteOutputBuffer( FILE *fp, uint16_t nw = 1 ){
        ReadOutputBuffer( nw );
        WriteOutputBuffer( fp );
        
        return;
    };
    
    virtual void ReadAndWriteOutputBuffer( std::vector<uint32_t> &vec_uiInputData, uint16_t nw = 1 ){
        ReadOutputBuffer( nw );
        std::cout << "nWordsRead = " << m_nWordRead << std::endl;
        vec_uiInputData.assign( m_data.get(), m_data.get() + m_nWordRead );
        for( int i=0; i < m_nWordRead; ++i ){
            std::cout << "data[ " << i << " ] = " << (m_data.get())[ i ] << std::endl;
        }
        
        return;
    };

protected:    
    AddressType m_baseAddr;
    //DataType *  data;
    std::shared_ptr<DataType> m_data;
    uint16_t    m_bufferSize;       // number of bytes in the buffer
    uint16_t    m_nWordRead;        // number of DataType words read
};

#endif
