#ifndef __IDAQVMETYPES_H_
#define __IDAQVMETYPES_H_

#include "CAEN/CAENVMElib.h"
#include "CAEN/CAENVMEtypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//using namespace std;

typedef signed char 	        int8_t ;
typedef unsigned char 	        uint8_t ;
typedef signed short int        int16_t ;
typedef unsigned short int      uint16_t ;
typedef signed int              int32_t;
typedef unsigned int            uint32_t ;
//typedef signed long long int 	int64_t ;
//typedef unsigned long long int 	uint64_t ;

template <typename DataType> 
class DataWord {
	public:
        DataWord( DataType d = 0 ) { data = d; }
        DataWord( const DataWord& dw ) { data = dw.GetData(); }

        virtual ~DataWord(){}

        DataType GetData() const { return data; }
        void SetData( DataType d ){ data = d; }
  
  protected:
        DataType data;
};

template < typename DataType > std::istream &operator>>( std::istream &s, DataWord< DataType > &dw );

typedef enum IDaqVmeModuleType{
	idmGenericModule	= 0x0,
  	idmV2718					= 0x1,
 	idmV1718					= 0x2,
  	idmV787						= 0x3,
  	idmV792						= 0x4,
  	idmV792N					= 0x5,
	idmV775						= 0x6
} IDaqVmeModuleType;

typedef enum IDaqVmeModuleKind {
	idmkUnknown = 0x0,
	idmkVmeInterface = 0x1,
	idmkAdc = 0x2,
	idmkTdc = 0x3
} IDaqVmeModuleKind;

//typedef enum  MD_VmeCode {
typedef enum {
        IDaqSuccess       =  0,           /* Operation completed successfully             */
        IDaqBusError      = -1,           /* VME bus error during the cycle               */
        IDaqCommError     = -2,           /* Communication error                          */
        IDaqGenericError  = -3,           /* Unspecified error                            */
        IDaqInvalidParam  = -4            /* Invalid parameter                            */
} IDaqVmeCode ;

typedef enum IDaqSwitch {
	IDaqDisable   = 0,         
  	IDaqEnable    = 1,
  	IDaqToggle    = 2,
	IDaqUndefined = 3
} IDaqSwitch;


/* =================================================== V792 ====================================================================*/

typedef enum  V792AcqMode {
  V792_ALL                 = 0x00, // no overflow suppression, nor zero suppression 
  V792_OVERFLOWSUP         = 0x01, // overflow suppression, no  zero suppression
  V792_ZEROSUP             = 0x02, // zero suppression, no overflow suppression
  V792_OVERFLOWSUP_ZEROSUP = 0x03  // overflow suppression and zero suppression

} V792AcqMode ;

typedef enum  V792ReadoutMode {
  V792_D32                = 0x00, /* Reading out single 32-bits words
                                   untill Filler is received                 */
  V792_D32_DR             = 0x01, /* If DREADY, reading out single 32-bits words
				   until Filler is received                  */
  V792_D32_ER             = 0x02, /* If EVRDY,
        			   reading out EVENT TRIGGER times 
				   34 single 32-bits words
				   (make sens only in Acq Mode  V792_ALL)    */
  V792_BLT32_BERR           = 0x04, /* Reading out blocks of 32-bits words
                                   until BERR is received                    */
  V792_BLT32_BERR_DR        = 0x05, /* If DREADY,
				   reading out blocks of 32-bits words
				   until BERR is received                    */
  V792_BLT32_BERR_ER        = 0x06, /* If EVRDY,
				   reading out blocks of 32-bits words
				   until BERR is received                    */
  V792_D32_EVTCOUNT       = 0x10  /* Read out EVENTCOUNTER_L
				   then reading out EVENTCOUNTER_L times
				   34 single 32-bits words
				   (make sense only in Acq Mode  V792_ALL)    */
} V792ReadoutMode ;

typedef enum  V792Type {
  V792_AA  = 0,
  V792_AC  = 1,
  V792_NA  = 2,
  V792_NC  = 3,

} V792Type ;

typedef enum  V792BitSet1Mask {
  V792_BS1_BerrFlag  = 0x0008,
  V792_BS1_SelAddr   = 0x0010,
  V792_BS1_SoftReset = 0x0080
} V792BitSet1Mask;

typedef enum  V792BitSet2Mask {
  V792_BS2_TestMem      = 0x0001,
  V792_BS2_OffLine      = 0x0002,
  V792_BS2_ClearData    = 0x0004,
  V792_BS2_OverRange    = 0x0008,
  V792_BS2_LowThreshold = 0x0010,
  V792_BS2_TestAcq      = 0x0040,
  V792_BS2_SlideScale   = 0x0080,
  V792_BS2_StepTh       = 0x0100,
  V792_BS2_AutoIncr     = 0x0800,
  V792_BS2_EmptyEvent   = 0x1000,
  V792_BS2_SlideSub     = 0x2000,
  V792_BS2_AllTrg       = 0x4000
} V792BitSet2Mask;

typedef enum  V792Status1Mask {
  V792_Status1_DReady       = 0x0001,
  V792_Status1_GlobalDReady = 0x0002,
  V792_Status1_Busy         = 0x0004,
  V792_Status1_GlobalBusy   = 0x0008,
  V792_Status1_Amnesia      = 0x0010,
  V792_Status1_Purged       = 0x0020,
  V792_Status1_TermOn       = 0x0040,
  V792_Status1_TermOff      = 0x0080,
  V792_Status1_Evrdy        = 0x0100
} V792Status1Mask;

typedef enum  V792Status2Mask {
  V792_Status2_BufferEmpty  = 0x0002,
  V792_Status2_BufferFull   = 0x0004,
  V792_Status2_DSel0        = 0x0010,
  V792_Status2_DSel1        = 0x0020,
  V792_Status2_CSel0        = 0x0040,
  V792_Status2_CSel1        = 0x0080
} V792Status2Mask;

typedef enum  V792ControlMask {
  V792_Control_BlkEnd        = 0x0004,
  V792_Control_ProgReset     = 0x0010,
  V792_Control_BerrEnable    = 0x0020,
  V792_Control_Align64       = 0x0040,
} V792ControlMask;


/*
typedef enum  V792EventCounter {
  V792_AllEvents      = 0,
  V792_AcceptedEvents = 1
} V792EventCounter;
*/

#define V792_DATAWORDSIZE              4

/* Register addresses */
#define V792_OUTPUTBUFFER              0x0000
#define V792_OUTPUTBUFFERLAST          0x07FC
#define V792_FIRMWAREREVISION          0x1000
#define V792_GEOADRESSE                0x1002
#define V792_BITSET1                   0x1006
#define V792_BITCLEAR1                 0x1008
#define V792_INTERRUPTLEVEL            0x100A
#define V792_INTERRUPTVECTOR           0x100c
#define V792_STATUS1                   0x100E
#define V792_CONTROL                   0x1010
#define V792_SINGLESHOTRESET           0x1016
#define V792_EVENTTRIGGER              0x1020
#define V792_STATUS2                   0x1022
#define V792_EVENTCOUNTER_L            0x1024
#define V792_EVENTCOUNTER_H            0x1026
#define V792_INCREMENTEVENT            0x1028
#define V792_INCREMENTOFFSET           0x102A
#define V792_FASTCLEARWINDOW           0x102E
#define V792_BITSET2                   0x1032
#define V792_BITCLEAR2                 0x1034
#define V792_CRATESELECT               0x103C
#define V792_EVENTCOUNTERRESET         0x1040
#define V792_IPED                      0x1060
#define V792_SWCOMM                    0x1068
#define V792_THRESHOLDS                0x1080

typedef enum DWV792MeasurementStatus {
  DWV792_Normal          = 0x0,
  DWV792_OverFlow        = 0x1,
  DWV792_UnderThreshold  = 0x2
} DWV792MeasurementStatus;

typedef enum DWV792DataType {
    DWV792_Measurement     = 0x0,
    DWV792_Myst            = 0x1,
    DWV792_GlobalHeader    = 0x2,
    DWV792_GlobalTrailer   = 0x4,
    DWV792_DataBlockLimit  = 0x5,
    DWV792_Filler          = 0x6,
    DWV792_InvalidData     = 0x7
} DWV792DataType;

/* =================================================== V755 ====================================================================*/

#define V775_DATAWORDSIZE			4

#define V775_OUTPUTBUFFER			0x0000
#define V775_OUTPUTBUFFER_LAST      0x0FFC
#define V775_GEO_ADDRESS			0x1002
#define V775_BIT_SET1				0x1006
#define V775_BIT_CLEAR1				0x1008
#define V775_STATUS_REGISTER1       0x100E
#define V775_CONTROL_REGISTER1      0x1010
#define V775_STATUS_REGISTER2		0x1022
#define V775_EVENT_COUNTER_LOW		0x1024
#define V775_EVENT_COUNTER_HIGH		0x1026
#define V775_INCREMENT_EVENT		0x1028
#define V775_INCREMENT_OFFSET		0x102A
#define V775_BIT_SET2				0x1032
#define V775_BIT_CLEAR2				0x1034
#define V775_EVENT_COUNTER_RESET	0x1040
#define V775_FULL_SCALE_RANGE		0x1060
#define V775_THRESHOLDS				0x1080
#define V775_THRESHOLDS_END			0x10BE


typedef enum  V775BitSet1Mask {
	V775_BS1_BerrFlag  = 0x0008,
	V775_BS1_SelAddr   = 0x0010,
	V775_BS1_SoftReset = 0x0080
} V775BitSet1Mask;

typedef enum  V775BitSet2Mask {
	V775_BS2_TestMem      = 0x0001,
	V775_BS2_OffLine      = 0x0002,
	V775_BS2_ClearData    = 0x0004,
	V775_BS2_OverRange    = 0x0008,
	V775_BS2_LowThreshold = 0x0010,
	V775_BS2_ValidControl = 0x0020,
	V775_BS2_TestAcq      = 0x0040,
	V775_BS2_SlideEnable  = 0x0080,
	V775_BS2_StepTh       = 0x0100,
	V775_BS2_StartStop    = 0x0400,
	V775_BS2_AutoIncr     = 0x0800,
	V775_BS2_EmptyProg    = 0x1000,
	V775_BS2_SlideSub     = 0x2000,
	V775_BS2_AllTrg       = 0x4000
} V775BitSet2Mask;

typedef enum  V775Status1Mask {
	V775_Status1_DReady       = 0x0001,
	V775_Status1_GlobalDReady = 0x0002,
	V775_Status1_Busy         = 0x0004,
	V775_Status1_GlobalBusy   = 0x0008,
	V775_Status1_Amnesia      = 0x0010,
	V775_Status1_Purged       = 0x0020,
	V775_Status1_TermOn       = 0x0040,
	V775_Status1_TermOff      = 0x0080,
	V775_Status1_Evrdy        = 0x0100
} V775Status1Mask;

typedef enum  V775Status2Mask {
	V775_Status2_BufferEmpty  = 0x0002,
	V775_Status2_BufferFull   = 0x0004,
	V775_Status2_DSel0        = 0x0010,
	V775_Status2_DSel1        = 0x0020,
	V775_Status2_CSel0        = 0x0040,
	V775_Status2_CSel1        = 0x0080
} V775Status2Mask;

typedef enum  V775ControlMask {
	V775_Control_BlkEnd        = 0x0004,
	V775_Control_ProgReset     = 0x0010,
	V775_Control_BerrEnable    = 0x0020,
	V775_Control_Align64       = 0x0040,
} V775ControlMask;

typedef enum V775DataStructure {
	V775_Data_Overflow        = 0x1000,
	V775_Data_UnderThreshold  = 0x2000,
	V775_Data_Valid           = 0x4000,
	V775_Data_Header          = 0x2000000,
	V775_Data_Footer          = 0x4000000
} V775DataStructure;

typedef enum  V755AcqMode {
	V775_ALL                 = 0x00, // no overflow suppression, nor zero suppression 
	V775_OVERFLOWSUP         = 0x01, // overflow suppression, no  zero suppression
	V775_ZEROSUP             = 0x02, // zero suppression, no overflow suppression
	V775_OVERFLOWSUP_ZEROSUP = 0x03  // overflow suppression and zero suppression
} V775AcqMode;

typedef enum  V775ReadoutMode {
	V775_D32                = 0x00, /* Reading out single 32-bits words
	untill Filler is received                 */
	V775_D32_DR             = 0x01, /* If DREADY, reading out single 32-bits words
	until Filler is received                  */
	V775_D32_ER             = 0x02, /* If EVRDY,
	reading out EVENT TRIGGER times 
	34 single 32-bits words
	(make sens only in Acq Mode  V775_ALL)    */
	V775_BLT32_BERR           = 0x04, /* Reading out blocks of 32-bits words
	until BERR is received                    */
	V775_BLT32_BERR_DR        = 0x05, /* If DREADY,
	reading out blocks of 32-bits words
	until BERR is received                    */
	V775_BLT32_BERR_ER        = 0x06, /* If EVRDY,
	reading out blocks of 32-bits words
	until BERR is received                    */
	V775_D32_EVTCOUNT       = 0x10  /* Read out EVENTCOUNTER_L
			then reading out EVENTCOUNTER_L times
			34 single 32-bits words
			(make sense only in Acq Mode  V775_ALL)    */
} V775ReadoutMode ;

typedef enum DWV775DataType {
	DWV775_EventDatum     = 0x0,
	DWV775_EventHeader    = 0x2,
	DWV775_EventTrailer   = 0x4,
	DWV775_DataFiller     = 0x6,
	DWV775_InvalidData    = 0x7
} DWV775DataType;

typedef enum DWV775MeasurementStatus {
	DWMSV775_Normal          = 0x0,
	DWMSV775_OverFlow        = 0x1,
	DWMSV775_UnderThreshold  = 0x2
} DWV775MeasurementStatus;

/* =================================================== V513 ====================================================================*/

#define V513_DATAWORDSIZE                       2
#define V513_VERSION_AND_SERIES					0xFE
#define V513_MANIFACTURER_AND_MODULE_TYPE		0xFC
#define V513_FIXED_CODE							0xFA
//============= 0x50 - 0xF8 RESERVED
#define V513_CLEAR_INPUT_REGISTER				0x48
#define V513_INITIALIZE_STATUS_REGISTERS		0x46
#define V513_CLEAR_STROBE_BIT					0x44
#define V513_MODULE_RESET						0x42
#define V513_CLEAR_VME_INTERRUPT				0x40
//============= 0x30 - 0x3E RESERVED
#define V513_CHANNEL15_STATUS_REGISTER			0x2E
#define V513_CHANNEL14_STATUS_REGISTER			0x2C
#define V513_CHANNEL13_STATUS_REGISTER			0x2A
#define V513_CHANNEL12_STATUS_REGISTER			0x28
#define V513_CHANNEL11_STATUS_REGISTER			0x26
#define V513_CHANNEL10_STATUS_REGISTER			0x24
#define V513_CHANNEL09_STATUS_REGISTER			0x24
#define V513_CHANNEL08_STATUS_REGISTER			0x20
#define V513_CHANNEL07_STATUS_REGISTER			0x1E
#define V513_CHANNEL06_STATUS_REGISTER			0x1C
#define V513_CHANNEL05_STATUS_REGISTER			0x1A
#define V513_CHANNEL04_STATUS_REGISTER			0x18
#define V513_CHANNEL03_STATUS_REGISTER			0x16
#define V513_CHANNEL02_STATUS_REGISTER			0x14
#define V513_CHANNEL01_STATUS_REGISTER			0x12
#define V513_CHANNEL00_STATUS_REGISTER			0x10
#define V513_CHANNEL_STATUS_REG_START			0x10
//============= 0x0E - 0x0A RESERVED
#define V513_INT_MASK_REGISTER					0x08
#define V513_STROBE_REGISTER					0x06
#define V513_INPUT_REGISTER						0x04
#define V513_OUTPUT_REGISTER					0x04
#define V513_INTERRUPT_LEVEL_REGISTER			0x02
#define V513_INTERRUPT_VECTOR_REGISTER			0x00

typedef enum  V513ChannelDirection {
	V513_Output        = 0x0,
	V513_Input         = 0x1
}V513ChannelDirection;

typedef enum  V513ChannelPolarity {
	V513_Negative        = 0x0,
	V513_Positive        = 0x2
}V513ChannelPolarity;

typedef enum  V513ChannelInputMode {
	V513_Glitched        = 0x0,
	V513_Normal          = 0x4
}V513ChannelInputMode;

typedef enum  V513ChannelTransferMode {
	V513_Transparent        = 0x0,
	V513_ExtStrobe          = 0x8
}V513ChannelTransferMode;


/* =================================================== General DAQ ====================================================================*/
#define DAQ_DATAWORDSIZE     4
#define DAQ_TIMESTAMPSIZE    12
#define DAQ_ENDOFFILESIZE    8

typedef enum DWMetaDAQDataType {
    DW_StartOfEvent     = 0x1,
    DW_StartOfRun       = 0x2,
    DW_RunInfo          = 0x3,
    DW_StartOfTimeStamp = 0x4,
    DW_Filler           = 0x6,
    DW_StartOfDataBlock = 0x7,
    DW_EndOfFile        = 0x8,
    DW_EndOfEvent       = 0x9,
    DW_InvalidData      = 0xF
} DWMetaDAQDataType;


typedef enum DWMetaDAQSync {
  DW_SyncStartOfEvent = 0x11,
  DW_SyncEndOfFile     = 0x12
} DWMetaDAQDataSync;



#endif
