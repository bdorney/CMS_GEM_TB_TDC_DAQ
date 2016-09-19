/*******************************************************************************/
/*                                                                             */
/*                        V767 utility procedures                              */
/*                                                                             */
/*                               C.A.E.N                                       */
/*                                                                             */
/*                 by Carlo Tintori     Viareggio, 07/97                       */
/*                                                                             */    
/*******************************************************************************/

/*******************************************************************************/
/* This file contains the following procedures and functions:                  */
/*                                                                             */
/* init_v767          initialize the V767 variables                            */
/* enable_mcst        enable multicast for write operations                    */
/* disable_mcst       disable multicast for write operations                   */
/* read_reg           read a register of the board                             */
/* write_reg          write a register of the board                            */
/* set_bitreg         set a bit of a register                                  */
/* clear_bitreg       clear a bit of a register                                */
/* toggle_bitreg      toggle a bit of a register                               */
/* test_bitreg        test a bit of a register                                 */
/* read_setup         read a program setup word                                */
/* write_setup        write a program setup word                               */
/* reset_module       SW reset of the module                                   */
/* clear_module       SW clear of the output buffer and readout controller     */
/* write_testword     write a memory test word                                 */
/* write_testevent    write a memory test event                                */
/* read_dataword      read a single word from the output buffer of the board   */
/* read_datablock     read a block of words from the output buffer of the board*/
/* read_chain_blk     read a block of words with chained block transfer        */
/* fill_buffer        read n words from output and put them in a local buffer  */
/* alloc_buffer       allocate memory for local buffer                         */
/* read_tdc_idcode    read tdc id code and write it into a formatted string    */
/* read_tdc_error     read tdc error code and write it into a formatted string */
/*******************************************************************************/

#include <stdio.h>
#include <strings.h>

#include "conio.h"
#include "tutil.h"
#include "vmeio.h"
#include "v767.h"


/*******************************************************************************/
/*                           GLOBAL VARIABLES                                  */
/*******************************************************************************/

/* vme addressing variables */
ushort geo_addr;                  /* geographical address */
ulong  base_addr;                 /* base address */
ulong  sds_addr;                  /* mcst/cblt register */
ushort mcst_on;                   /* mcst = 1 -> multicast access */
ushort blk_size;                  /* block size for block tranfer */
ushort blk_dmode;                 /* data size (D32 or D64) for block tranfer */
ushort addr_mode;                 /* addressing mode (BA_32, BA_24, GEO_24) */
ushort num_channels;              /* number of channels (64/128) */


/* usefull strings */
char *onoff[]      = { "[OFF]", "[ON] " };
char *endis[]      = { "[DISABLED]", "[ENABLED] "}; 
char *pos_str[]    = { "[INACTIVE]",             /* board position strings */
                       "[FIRST]   ",
                       "[LAST]    ",
                       "[ACTIVE]  " };
char *acqm_str[]   = { "[STOP TRG MATCHING] ",   /* acquisition mode strings */
                       "[START TRG MATCHING]",
                       "[START GATING]      ",
                       "[CONTINUOUS STORAGE]" };
char *drdym_str[]  = { "[AT LEAST ONE EVENT]",   /* data ready mode strings */
                       "[BUFFER ALMOST FULL]",
                       "[AT LEAST ONE DATUM]",};
char *term_str[]   = { "[TERM ERROR]",           /* termination strings */
                       "[TERM ON]   ",
                       "[TERM OFF]  ",
                       "[TERM ERROR]" };
char *rost_str[]   = { "[DO NOT READ]        ",  /* read out of start */
                       "[READ FROM FIRST TDC]",
                       "[READ FROM ALL TDC]  ",};
char *edge_str[]   = { "",                       /* edge detection */
                       "[RISE]",
                       "[FALL]",
                       "[BOTH]",};



/*******************************************************************************/
/*                               INIT_V767                                     */
/*-----------------------------------------------------------------------------*/
/* Read the V767.INI file and initialize the global variables                  */
/*******************************************************************************/
 void init_v767()
  {
  FILE *ini_file;
  char fline[100];
  ulong tmp;

  /* module base address */
  if(init_variable("v767.ini","BASE_ADDRESS",&tmp))  base_addr=tmp;
  else  base_addr=0xEE000000;

  /* module geo address */
  if(init_variable("v767.ini","SLOT_NUMBER",&tmp))  geo_addr=(ushort)tmp;
  else geo_addr=10;

  /* addressing mode */
  if(init_variable("v767.ini","ADDR_MODE",&tmp))    addr_mode=(ushort)tmp;
  else addr_mode=BA_32;

  /* mcst/cblt address */
  if(init_variable("v767.ini","MCST/CBLT_ADDRESS",&tmp))  sds_addr=tmp;
  else  sds_addr=0xAA000000;

  /* num of bytes for blt cycles */
  if(init_variable("v767.ini","BLOCK_SIZE",&tmp))   blk_size=(ushort)tmp;
  else blk_size=256;

  /* number of channels (64 or 128) */
  if(init_variable("v767.ini","NUM_CHANNELS",&tmp)) num_channels=(ushort)tmp;
  else num_channels=128;

  mcst_on = 0;
  }


/*******************************************************************************/
/*                             ENABLE_MCST                                     */
/*-----------------------------------------------------------------------------*/
/* enable Multi Cast mode                                                      */
/*******************************************************************************/
 void enable_mcst()
  {
  mcst_on = 1;
  }

/*******************************************************************************/
/*                             DISABLE_MCST                                    */
/*-----------------------------------------------------------------------------*/
/* disable Multi Cast mode                                                     */
/*******************************************************************************/
 void disable_mcst()
  {
  mcst_on = 0;
  }

/*******************************************************************************/
/*                              READ_REG                                       */
/*-----------------------------------------------------------------------------*/
/* parameters:     reg_addr->   register address                               */ 
/* return:         register content                                            */
/*-----------------------------------------------------------------------------*/
/* read the content of a 16bit register of the board                           */
/*******************************************************************************/
#ifdef ANSI_C
 ushort read_reg(ushort reg_addr)
#else
 ushort read_reg(reg_addr)
 ushort reg_addr;
#endif
  {
  ulong data,vme_addr;
  ushort a_mode;
  char msg[100];

  vme_addr = base_addr+reg_addr;
  if(addr_mode == GEO_24)
    vme_addr = (ulong)geo_addr<<19+reg_addr;

  a_mode = A24;
  if(addr_mode == BA_32)
    a_mode = A32;

  if(!vme_read_dt(vme_addr, &data, a_mode, D16))
    {
    sprintf(msg,"Bus error reading a register at address %X",vme_addr);
    print_err(msg);
    exit(-1);
    }

  return((ushort)data);
  }


/*******************************************************************************/
/*                             WRITE_REG                                       */
/*-----------------------------------------------------------------------------*/
/* parameters:     reg_addr->   register address                               */ 
/*                 data    ->   new register content                           */
/*-----------------------------------------------------------------------------*/
/* write the new content into a 16bit register of the board                    */
/*******************************************************************************/
#ifdef ANSI_C
 void write_reg(ushort reg_addr, ushort data)
#else
 void write_reg(reg_addr, data)
 ushort reg_addr, data;
#endif
  {
  ulong data32;  /* data on 32 bit */
  ulong vme_addr;
  ushort a_mode;
  char msg[100];


  data32 = (ulong)data;

  if(mcst_on)
    {
    vme_addr = sds_addr+reg_addr;
    a_mode = A32;
    }
  else
    {
    vme_addr = base_addr+reg_addr;
    if(addr_mode == GEO_24)
      vme_addr = (ulong)geo_addr<<19+reg_addr;
    a_mode = A24;
    if(addr_mode == BA_32)
      a_mode = A32;
    }

  if(!vme_write_dt(vme_addr, &data32, a_mode, D16))
    {
    sprintf(msg,"Bus error writing a register at address %X",vme_addr);
    print_err(msg);
    exit(-1);
    }
  }


/*******************************************************************************/
/*        SET_REGBIT, CLEAR_REGBIT, TOGGLE_REGBIT, TEST_REGBIT                 */
/*-----------------------------------------------------------------------------*/
/* parameters:     address ->   register address                               */ 
/*                 bit     ->   bit number                                     */
/*-----------------------------------------------------------------------------*/
/* set, clear, toggle or test a bit of a register                              */
/*******************************************************************************/
#ifdef ANSI_C
 void set_regbit(ushort reg_addr, ushort bit)
#else
 void set_regbit(reg_addr, bit)
 ushort reg_addr, bit;
#endif
  {
  ushort reg;

  reg = read_reg(reg_addr);
  SET_BIT(bit,reg);
  write_reg(reg_addr,reg);
  }

#ifdef ANSI_C
 void clear_regbit(ushort reg_addr, ushort bit)
#else
 void clear_regbit(reg_addr, bit)
 ushort reg_addr, bit;
#endif
  {
  ushort reg;

  reg = read_reg(reg_addr);
  CLEAR_BIT(bit,reg);
  write_reg(reg_addr,reg);
  }

#ifdef ANSI_C
 void toggle_regbit(ushort reg_addr, ushort bit)
#else
 void toggle_regbit(reg_addr, bit)
 ushort reg_addr, bit;
#endif
  {
  ushort reg;

  reg = read_reg(reg_addr);
  TOGGLE_BIT(bit,reg);
  write_reg(reg_addr,reg);
  }

#ifdef ANSI_C
 ushort test_regbit(ushort reg_addr, ushort bit)
#else
 ushort test_regbit(reg_addr, bit)
 ushort reg_addr, bit;
#endif
  {
  ushort reg;

  reg = read_reg(reg_addr);
  return(TEST_BIT(bit,reg));
  }


/*******************************************************************************/
/*                               READ_SETUP                                    */
/*-----------------------------------------------------------------------------*/
/* return:      setup word                                                     */
/*-----------------------------------------------------------------------------*/
/* read a 16bit setup word (operand) from the SETUP_BUFFER                     */
/*******************************************************************************/
 ushort read_setup()
  {
  ushort i, data;

  /* checks if datum is ready */
  if(!TEST_BIT(READ_OK,read_reg(SETUP_HS)))
    {
    print_err("Unexpected read command during setup");
    exit(-1);
    }
   
  data = read_reg(SETUP_BUFFER);
  /* wait until the micro is ready for a new read or write command */
  for(i=0; (i<TIMEOUT) && (read_reg(SETUP_HS)==0); i++);

  if(i==TIMEOUT)      /* Setup timeout */
    {
    print_err("Timeout error during TDC read setup");
    exit(-1);
    }
  else
    return( data );
  }


/*******************************************************************************/
/*                              WRITE_SETUP                                    */
/*-----------------------------------------------------------------------------*/
/* parameters:     data    ->   setup word                                     */
/*-----------------------------------------------------------------------------*/
/* write a 16bit setup word (opcode or operand) into the SETUP_BUFFER          */
/*******************************************************************************/
#ifdef ANSI_C
 void write_setup(ushort data)
#else
 void write_setup(data)
 ushort data;
#endif
  {
  ushort i;

  /* checks if the board is ready to get the datum */
  if(!TEST_BIT(WRITE_OK,read_reg(SETUP_HS)))
    {
    print_err("Unexpected write command during setup");
    exit(-1);
    }
   
  write_reg(SETUP_BUFFER,data);
  /* wait until the micro is ready for a new read or write command */
  for(i=0; (i<TIMEOUT) && (read_reg(SETUP_HS)==0); i++);

  if(i==TIMEOUT)      /* Setup timeout */
    {
    print_err("Timeout error during TDC write setup");
    exit(-1);
    }
  }


/*******************************************************************************/
/*                              RESET_MODULE                                   */
/*-----------------------------------------------------------------------------*/
/* Software reset of the module                                                */
/*******************************************************************************/
void reset_module()
  {
  write_reg(RESET,0);  /* SW reset */
  delay(500);  /* wait 500 ms */
  }


/*******************************************************************************/
/*                              CLEAR_MODULE                                   */
/*-----------------------------------------------------------------------------*/
/* Software clear of output buffer and readout controller                      */
/*******************************************************************************/
void clear_module()
  {
  write_reg(CLEAR,0);  /* SW clear */
  delay(500);  /* wait 500 ms */
  }


/*******************************************************************************/
/*                              WRITE_TESTWORD                                 */
/*-----------------------------------------------------------------------------*/
/* parameters:     data    ->   memory test word                               */
/*-----------------------------------------------------------------------------*/
/* write a 32bit memory test word                                              */
/*******************************************************************************/
#ifdef ANSI_C
 void write_testword(ulong data)
#else
 void write_testword(data)
 ulong data;
#endif
  {
  write_reg(TESTWORD_L,(ushort)(data & 0xFFFF)); /* low */
  write_reg(TESTWORD_H,(ushort)(data >> 16));    /* high */
  }



/*******************************************************************************/
/*                              WRITE_TESTEVENT                                */
/*-----------------------------------------------------------------------------*/
/* parameters:     ev_num  ->   event number                                   */
/*                 nword   ->   number of word                                 */
/*-----------------------------------------------------------------------------*/
/* write the following test event with n words                                 */
/*   HEADER                                                                    */
/*   START at time FFFF                                                        */
/*   HIT on channel 0 at time 1                                                */
/*   HIT on channel 1 at time 2                                                */
/*   ....                                                                      */
/*   EOB                                                                       */
/*******************************************************************************/
#ifdef ANSI_C
 void write_testevent(ushort ev_num,ushort nword)
#else
 void write_testevent(ev_num, nword)
 ushort ev_num, nword;
#endif
  {
  ushort i;

  write_testword(0x00400000 | ev_num);
  for(i=0;i<nword;i++)
    if(i==0)
      write_testword(0x0080FFFF);
    else
      write_testword((((i-1)&0x7F)<<24) | i);
  write_testword(0x00200000 | nword);
  }


/*******************************************************************************/
/*                              READ_DATAWORD                                  */
/*-----------------------------------------------------------------------------*/
/* parameters:     data    ->   read data                                      */
/* return:         1 -> ok                                                     */
/*                 0 -> buffer empty                                           */
/*-----------------------------------------------------------------------------*/
/* read a single 32bit data word from the DATA_BUFFER                          */
/*******************************************************************************/
#ifdef ANSI_C
 short read_dataword(ulong *data)
#else
 short read_dataword(data)
 ulong *data;
#endif
  {
  ulong vme_addr;

  vme_addr = base_addr + DATA_BUFFER;  /* data buffer vme address */
  vme_read_dt(vme_addr, data, addr_mode, D32);   /* read data */
  if(DATA_TYPE(*data)==NOT_VALID)
    return(0);  /* empty */
  else
    return(1);   /* ok */
  }


/*******************************************************************************/
/*                              READ_DATABLOCK                                 */
/*-----------------------------------------------------------------------------*/
/* parameters:     buffer    ->   pointer to the memory buffer                 */
/*                 blk_dmode ->   data format (D32 or D64)                     */
/* return:         1 -> ok                                                     */
/*                 0 -> bus error during BLT                                   */
/*-----------------------------------------------------------------------------*/
/* read a block of 32bit data words from the DATA_BUFFER of the selected board */
/* with a normal block transfer                                                */
/*******************************************************************************/
#ifdef ANSI_C
 short read_datablock(ulong *buffer, ushort blk_dmode)
#else
 short read_datablock(buffer, blk_dmode)
 ulong *buffer;
 ushort blk_dmode;
#endif
  {
  ulong vme_addr;

  vme_addr = base_addr + DATA_BUFFER;  /* data buffer vme address */
  return(vme_read_blk(vme_addr, buffer, blk_size, blk_dmode));   /* read data */
  }


/*******************************************************************************/
/*                              READ_CHAIN_BLK                                 */
/*-----------------------------------------------------------------------------*/
/* parameters:     buffer    ->   pointer to the memory buffer                 */
/* return:         1 -> ok                                                     */
/*                 0 -> bus error during BLT                                   */
/*-----------------------------------------------------------------------------*/
/* read a block of 32bit data words with chained block tranfer                 */
/*******************************************************************************/
#ifdef ANSI_C
 short read_chain_blk(ulong *buffer)
#else
 short read_chain_blk(buffer)
 ulong *buffer;
#endif
  {
  ulong vme_addr;

  vme_addr = sds_addr + DATA_BUFFER;   /* data buffer vme address */
  return(vme_read_blk(vme_addr, buffer, blk_size, D32));   /* read data */
  }


/*******************************************************************************/
/*                                FILL_BUFFER                                  */
/*-----------------------------------------------------------------------------*/
/* parameters:  buffer    ->   pointer to the memory buffer                    */
/*              num_data  ->   number of data to write into the buffer         */
/* return:      number of data read from the board and written into the buffer */
/*-----------------------------------------------------------------------------*/
/* read num_data words from the data buffer and write them into the memory     */
/* buffer. If the data buffer of the board contains less than num_data words   */
/* the procedure exits and return the number of words read                     */
/*******************************************************************************/
#ifdef ANSI_C
 ushort fill_buffer(ulong *buffer, ushort num_data)
#else
 ushort fill_buffer(buffer, num_data)
 ulong *buffer;
 ushort num_data;
#endif
  {
  ulong vme_addr,data;
  ushort i=0, empty=0;

  vme_addr = base_addr + DATA_BUFFER;  /* data buffer vme address */

  while( (i<num_data) && !empty)
    {
    vme_read_dt(vme_addr, &data, addr_mode, D32);   /* read data */
    if(DATA_TYPE(data) != NOT_VALID)
      buffer[i++] = data;
    else
      empty=1;
    }

  return(i);
  }


/*******************************************************************************/
/*                               ALLOC_BUFFER                                  */
/*-----------------------------------------------------------------------------*/
/* return:       swbuffer    ->   pointer to the memory buffer                 */
/*-----------------------------------------------------------------------------*/
/* allocate memory for the software data buffer                                */
/*******************************************************************************/
 ulong *alloc_buffer()
  {
  ulong *swbuffer,*temp;

  /* allocate BUFF_SIZE lword for the software buffer containing data read out from */
  /* the output buffer of the board                                                 */
  /* NOTE: 2 * BUFF_SIZE bytes are necessary for the pointer allignement            */
  swbuffer = (ulong *)malloc(BUFF_SIZE*2);
  if (swbuffer == NULL)
    {
    print_err(" Can't allocate memory for data read out. ");
    exit(-1);
    }
  /* allign the pointer to 2k */
  if ((unsigned)swbuffer & 0x3FF)
    {
    unsigned temp = (unsigned)swbuffer;	
    temp = 0x400 - (temp & 0x3FF) + temp ;
    (unsigned)swbuffer = temp ;
    }

  return(swbuffer);
  }


/*******************************************************************************/
/*                             READ_SERNUM                                     */
/*-----------------------------------------------------------------------------*/
/* return:         board serial number                                         */
/*-----------------------------------------------------------------------------*/
/* read the board serial number from the CONF_ROM                              */
/*******************************************************************************/
ushort read_sernum()
  {
  ushort ser_num;

  /* read the serial number from CONF ROM */
  ser_num = read_reg(CONF_ROM(SERNUM_L)) & 0x00FF;
  ser_num = ser_num + (CONF_ROM(SERNUM_H) << 8);
  return(ser_num);
  }


/*******************************************************************************/
/*                             READ_TDC_IDCODE                                 */
/*-----------------------------------------------------------------------------*/
/* parameters:     tdc    ->  tdc to read                                      */
/*                 idcode ->  idcode string (lenght = 41)                      */
/*-----------------------------------------------------------------------------*/
/* read the TDC id_code and put it into the following string idcode:           */
/*  "ES2 code: XXX - TDC code: XXXX - Vers: X"                                 */
/*******************************************************************************/
#ifdef ANSI_C
 void read_tdc_idcode(ushort tdc, char *idcode)
#else
 void read_tdc_idcode(tdc, idcode)
 ushort tdc;
 char *idcode;
#endif
  {
  ushort hid,lid;
  
  /* read tdc id code */
  write_setup(READ_TDC_ID(tdc));
  lid=read_setup();   /* first word  */
  hid=read_setup();   /* second word */
  /* print the idcode string */
  sprintf(idcode,"ES2 code: %03X - TDC code: %04X - Vers: %1X",
     (lid>>1) & 0x07FF, (lid>>12) | ((hid & 0x0FFF) << 4), hid>>12 );
  }


/*******************************************************************************/
/*                             READ_TDC_ERROR                                  */
/*-----------------------------------------------------------------------------*/
/* parameters:     tdc    ->  tdc to read                                      */
/*                 errstr ->  error string (lenght = 65)                       */
/* return:         error code                                                  */
/*-----------------------------------------------------------------------------*/
/* read the TDC error and put it into the string errstr                        */
/*******************************************************************************/
#ifdef ANSI_C
 short read_tdc_error(ushort tdc, char *errstr)
#else
 short read_tdc_error(tdc, errstr)
 ushort tdc;
 char *errstr;
#endif
  {
  ushort err;

  if(tdc>3) return(-1);
  /* read tdc error codes */
  write_setup(READ_TDC_ERR(tdc));
  err = read_setup() & 0x1F;

  /* print the error string */
  sprintf(errstr,"TDC %d : ",tdc);
  if(!err)
    strcat(errstr,"OK");
  else
    {
    if(err & LOCK_ERR)     strcat(errstr," NOT_LOCKED");
    if(err & HIT_ERR)      strcat(errstr," HIT_ERROR");
    if(err & EV_OVFL_ERR)  strcat(errstr," EVENT_OVFL");
    if(err & TRG_OVFL_ERR) strcat(errstr," TRIGGER_OVFL");
    if(err & SER_TRG_ERR)  strcat(errstr," SERIAL_ERR"); 
    }
  return (err);
  }



