/*******************************************************************************/
/*                                                                             */
/*                          V767 header file                                   */
/*                                                                             */
/*                               C.A.E.N                                       */
/*                                                                             */
/*                 by Carlo Tintori     Viareggio, 07/97                       */
/*                                                                             */    
/*******************************************************************************/

/*******************************************************************************/
/* This file contains the register memory map, the register bit                */
/* definition, the opcode mnemonic names, some constant definition and the     */
/* declaration of the following utility procedures:                            */
/*                                                                             */
/* init_v767          initialize the V767 variables                            */
/* enable_mcst        enable multicast for write operations                    */
/* disable_mcst       disable multicast for write operations                   */
/* read_reg           read a register of the board                             */
/* write_reg          write a register of the board                            */
/* read_setup         read a program setup word                                */
/* write_setup        write a program setup word                               */
/* write_testword     write a memory test word                                 */
/* write_testevent    write a memory test event                                */
/* read_dataword      read a single word from the output buffer of the board   */
/* read_datablock     read a block of words from the output buffer of the board*/
/* read_chain_blk     read a block of words with chained block transfer        */
/* fill_buffer        read n words from output and put them in a local buffer  */
/* alloc_buffer       allocate memory for local buffer                         */
/* read_tdc_idcode    read tdc id code and write it into a formatted string    */
/* read_tdc_idcode    read tdc error code and write it into a formatted string */
/*******************************************************************************/


#ifndef __V767_H
#define __V767_H


/*******************************************************************************/
/*                                 short names                                 */
/*******************************************************************************/

#ifndef ulong
#define ulong unsigned long 
#endif
#ifndef uint
#define uint unsigned int 
#endif
#ifndef ushort
#define ushort unsigned short
#endif
#ifndef uchar
#define uchar unsigned char
#endif



/*******************************************************************************/
/*                           GLOBAL VARIABLES                                  */
/*******************************************************************************/
/* vme addressing variables */
extern ushort geo_addr;        /* geographical address */
extern ulong base_addr;        /* base address */ 
extern ulong sds_addr;         /* sparse data scan address (cblt-mcst) */
extern ushort blk_size;        /* block size for block tranfer */
extern ushort blk_dmode;       /* data size (D32 or D16) for block tranfer */
extern ushort addr_mode;       /* addressing mode (A24 or A32) */
extern ushort num_channels;    /* number of channels (64/128) */


/* usefull string definitions */
extern char *onoff[];
extern char *endis[];
extern char *pos_str[];
extern char *acqm_str[];
extern char *drdym_str[];
extern char *term_str[];
extern char *rost_str[];
extern char *edge_str[];

/*******************************************************************************/
/*                             some definitions                                */
/*******************************************************************************/

#define BA_32                0     /* addressing with Base Address in A32 */
#define BA_24                1     /* addressing with Base Address in A24 */
#define GEO_24               2     /* addressing with Geo Address in A24 */

#define STOP_MATCH_MODE      0     /* acquisition modes */
#define START_MATCH_MODE     1
#define START_GATING_MODE    2
#define CONT_STORE_MODE      3

#define INACTIVE_BOARD       0     /* MCST/CBLT settings */
#define LAST_BOARD           1
#define FIRST_BOARD          2
#define ACTIVE_BOARD         3

#define DETECT_RISE          1     /* edge detection */
#define DETECT_FALL          2
#define DETECT_BOTH          3

#define TIMEOUT_ERROR       -1

#define DATA                 0     /* data type */
#define HEADER               2
#define EOB                  1
#define NOT_VALID            3

/* software memory buffer (bytes) for data read out */
#define BUFF_SIZE            2048  

#define TIMEOUT      1000000     /* timeout for setup handshake */  
/* -----------  !!!!!!!!!  WARNING  !!!!!!!!!!  -------------- */
/* increase this parameter if your CPU is too fast !!!!        */
/* (see WRITE_SETUP and READ_SETUP procedures)                 */
/* ----------------------------------------------------------- */

#define TDC0            0          /* tdc numbers */
#define TDC1            1
#define TDC2            2
#define TDC3            3

#define TDC_IDCODE      0x00DAC08F

#define LOCK_ERR        0x01      /* error mask */
#define HIT_ERR         0x02
#define EV_OVFL_ERR     0x04
#define TRG_OVFL_ERR    0x08
#define SER_TRG_ERR     0x10



/********************************************************************************/
/*                        SETUP OPCODE DEFINITIONS                              */
/********************************************************************************/

#define EN_MEM_TEST        0x0100  /* enable memory test */
#define DIS_MEM_TEST       0x0200  /* disable memory test */
#define READ_MEM_TEST      0x0300  /* read memory test on/off */

#define STOP_MATCH         0x1000  /* set stop trigger matching */
#define START_MATCH        0x1100  /* set start trigger matching */
#define START_GAT          0x1200  /* set start gating */
#define CONT_STO           0x1300  /* set continuous storage */
#define READ_ACQ_MODE      0x1400  /* read acquisition mode */
#define LOAD_DEFAULT       0x1500  /* set default configuration */
#define SAVE_CONFIG        0x1600  /* save configuration */
#define LOAD_CONFIG        0x1700  /* load configuration */
#define EN_AUTO_LOAD       0x1800  /* enable auto load user config */
#define DIS_AUTO_LOAD      0x1900  /* disable auto load user config */
#define READ_AUTO_LOAD     0x1A00  /* read auto load */

#define EN_CHANNEL(nn)     (0x2000+(nn)) /* enable channel nn */
#define DIS_CHANNEL(nn)    (0x2100+(nn)) /* disable channel nn */
#define READ_STAT_CH(nn)   (0x2200+(nn)) /* read status channel nn */
#define EN_ALL_CH          0x2300        /* enable all channels */
#define DIS_ALL_CH         0x2400        /* disable all channels */
#define WRITE_EN_PATTERN   0x2500        /* write enable patterns */
#define READ_EN_PATTERN    0x2600        /* read enable patterns */

#define SET_WIN_WIDTH      0x3000  /* set window width */
#define READ_WIN_WIDTH     0x3100  /* read window width */
#define SET_WIN_OFFS       0x3200  /* set window offset */
#define READ_WIN_OFFS      0x3300  /* read window offset */
#define SET_TRG_LAT        0x3400  /* set trigger latency */
#define READ_TRG_LAT       0x3500  /* read trigger latency */
#define EN_SUB_TRG         0x3600  /* enable subtraction of trigger time */
#define DIS_SUB_TRG        0x3700  /* disable subtraction of trigger time */
#define EN_OVL_TRG         0x3800  /* enable overlapping triggers */
#define DIS_OVL_TRG        0x3900  /* disable overlapping triggers */
#define READ_TRG_CONF      0x3A00  /* read trigger configuration */ 

#define EN_RO_START        0x4000  /* enable read out of 1 start time */
#define EN_RO_ALL_START    0x4100  /* enable read out of 4 start time */
#define DIS_RO_START       0x4200  /* disable readout of start time */
#define EN_SUB_START       0x4300  /* enable subtraction of start time */
#define DIS_SUB_START      0x4400  /* disable subtraction of start time */
#define EN_EMPTY_START     0x4500  /* enable empty start */
#define DIS_EMPTY_START    0x4600  /* disable empty start */
#define READ_START_CONF    0x4700  /* read start configuration */

#define SET_ADJUST_CH(nn)  (0x5000+(nn))  /* set channel nn adjust */
#define READ_ADJUST_CH(nn) (0x5100+(nn))  /* read chennel nn adjust */
#define SET_GLOB_OFFS      0x5200         /* set global offset */
#define READ_GLOB_OFFS     0x5300         /* read global offset */
#define EN_ALL_ADJUST      0x5400         /* enable all adjusts */
#define DIS_ALL_ADJUST     0x5500         /* disable all adjusts */
#define RESET_ADJUST       0x5600         /* set all adjusts to 0 */
#define READ_EN_ADJUST     0x5700         /* read adjust enable/disable */

#define RISE_ALL           0x6000  /* only rising edge all channels */
#define FALL_ALL           0x6100  /* only falling edge odd all channels */
#define ODDR_EVENF         0x6200  /* rise edge odd ch / fall edge even ch */
#define ODDF_EVENR         0x6300  /* fall edge odd ch / rise edge even ch */
#define RISE_START         0x6400  /* rise edge start */
#define FALL_START         0x6500  /* fall edge start */
#define BOTH_ALL           0x6600  /* both edge all channels and start */
#define READ_DETECTION     0x6700  /* read edge detection configuration */

#define DR_EV_READY        0x7000  /* set D.R. = event ready */
#define DR_ALMOST_FULL     0x7100  /* set D.R. = buffer almost full */
#define DR_NOT_EMPTY       0x7200  /* set D.R. = buffer not empty */
#define READ_DR_MODE       0x7300  /* read data ready mode */
#define SET_ALM_FULL       0x7400  /* set almost full level */
#define READ_ALM_FULL      0x7500  /* read almost full level */

#define READ_TDC_ERR(c)    (0x8000+(c)) /* read error code of TDC c */
#define READ_TDC_ID(c)     (0x8100+(c)) /* read ID code of TDC c */
#define WRITE_SETUP(c)     (0x8200+(c)) /* write JTAG setup register of TDCs */
#define JTAG_SAMPLE(c)     (0x8300+(c)) /* BST sample of TDC c */
#define JTAG_EXTEST(c)     (0x8400+(c)) /* BST extest of TDC c */

#define SET_REJ_OFFS       0x9000  /* set reject offset */
#define READ_REJ_OFFS      0x9100  /* read reject offset */
#define EN_AUTO_REJ        0x9200  /* enable automatic reject */
#define DIS_AUTO_REJ       0x9300  /* disable automatic reject */
#define READ_EN_REJ        0x9400  /* read en/dis automatic reject */

#define SET_BACK_WIN       0xA000  /* set look back window */
#define READ_BACK_WIN      0xA100  /* read look back window */
#define SET_AHEAD_WIN      0xA200  /* set look ahead window */
#define READ_AHEAD_WIN     0xA300  /* read look ahead window */

#define SET_DLL_CURR       0xB000  /* set DLL current */
#define READ_DLL_CURR      0xB100  /* read DLL current */
#define RESET_DLL          0xB200  /* reset DLL */
#define EN_DB_SYNC         0xB300  /* enable double syncronizer */
#define DIS_DB_SYNC        0xB400  /* disable double syncronizer */
#define EN_DB_PRIOR        0xB500  /* enable double hit priority */
#define DIS_DB_PRIOR       0xB600  /* disable double hit priority */
#define READ_ADV_CONF      0xB700  /* read advanced configuration */
#define SET_ERR_MASK       0xB800  /* set error mask */
#define READ_ERR_MASK      0xB900  /* read error mask */




/********************************************************************************/
/*                              REGISTERS ADDRESS                               */
/********************************************************************************/

#define INT_LEVEL       0x0A    /* IRQ number                                   */
#define INT_VECTOR      0x0C    /* interrupt vector                             */
#define STATUS1         0x0E    /* term, busy, dtready                          */
#define STATUS2         0x48    /* fifo flags, tdc error                        */
#define CONTROL1        0x10    /* berr enable, panres prog, sel_addr, blt_stop */                                       
#define CONTROL2        0x4A    /* acq. mode, dtready mode, test on/off         */
#define MCST_CTRL       0x20    /* first/last board                             */
#define RESET           0x18    /* reset software                               */
#define BAR             0x04    /* base address register (GEO)                  */
#define ADERH           0x12    /* Address decoder register (high)              */
#define ADERL           0x14    /* Address decoder register (low)               */
#define BIT_SET         0x06    /* sw reset on                                  */
#define BIT_CLEAR       0x08    /* sw reset off                                 */
#define MCST_ADDRESS    0x16    /* MCST/CBLT base address                       */
#define DATA_BUFFER     0x00    /* data                                         */
#define CLEAR           0x54    /* clear data (reset fifo,tdc)                  */
#define EVENT_CNT       0x4C    /* event counter                                */
#define CLEAR_EV_CNT    0x4E    /* reset event counter                          */
#define SW_TRIG         0x5A    /* trigger software                             */
#define SETUP_HS        0x50    /* setup handshake register                     */
#define SETUP_BUFFER    0x52    /* setup OPCODE and OPERANDS                    */
#define TESTWORD_H      0x56    /* memory test (16 bit high)                    */
#define TESTWORD_L      0x58    /* memory test (16 bit low)                     */
#define CONF_ROM(r)     (0x1000 + r)  /* configuration ROM                      */

/* Conf ROM fields */
#define SERNUM_H        0x0F02
#define SERNUM_L        0x0F06



/*******************************************************************************/
/*                         MACROS FOR BIT OPERATIONS                           */
/*******************************************************************************/
#define TEST_BIT(b,r)   ( ((r) & (1<<(b))) >> (b) )
#define SET_BIT(b,r)    r = (r) | (1<<(b))
#define CLEAR_BIT(b,r)  r = (r) & (~(1<<(b)))
#define TOGGLE_BIT(b,r) r = (r) ^ (1<<(b))


/********************************************************************************/
/*                          BIT REGISTER DEFINITION                             */
/********************************************************************************/

/*-------------------------------- CONTROL1 ------------------------------------*/
#define BERR_ENABLE      5  /* bus error enable              */
#define PANRES_MODE      4  /* front panel reset mode        */
#define BLT_STOP         2  /* stop blt32 and blt64 with EOB */

/*-------------------------------- SDS_CTRL ------------------------------------*/
#define BOARD_POS(r)     ((r) & 0x0003)

/*------------------------- BIT SET, BIT CLEAR ---------------------------------*/
#define BERR_FLAG        3  /* 1 if a BERR has been generated*/
#define SEL_ADDRESS      4  /* address relocation            */
#define SW_RESET         7  /* software reset                */

/*--------------------------------- STATUS1 ------------------------------------*/
#define LOCAL_DRDY       0  /* local drdy                    */
#define GLOBAL_DRDY      1  /* global drdy                   */
#define LOCAL_BUSY       2  /* local busy                    */
#define GLOBAL_BUSY      3  /* global busy                   */
#define TERM_ON          6  /* term on                       */
#define TERM_OFF         7  /* term off                      */
#define TERM_STATUS(r)   (((r) & 0x00C0) >> 6)      

/*-------------------------------- CONTROL2 ------------------------------------*/
#define ACQ_MODE0        0  /* acquisition mode              */
#define ACQ_MODE1        1
#define ACQ_MODE(r)      ((r) & 0x0003)
#define DRDY_MODE0       2  /* data ready mode               */
#define DRDY_MODE1       3
#define DRDY_MODE(r)     (((r) & 0x000C) >> 2)
#define MEM_TEST         4  /* memory test on/off            */

/*--------------------------------- STATUS2 ------------------------------------*/
#define BUF_EMPTY        0  /* output buffer empty           */  
#define BUF_FULL         1  /* output buffer full            */
#define BUF_ALM_FULL     2  /* output buffer almost full     */
#define TDC_ERROR        3  /* OR tdc error                  */
#define TDC0_ERROR       12 /* tdc0 error                    */
#define TDC1_ERROR       13 /* tdc1 error                    */
#define TDC2_ERROR       14 /* tdc2 error                    */
#define TDC3_ERROR       15 /* tdc3 error                    */

/*----------------------------- SETUP HANDSHAKE --------------------------------*/
#define READ_OK           0     /* read operation complete    */  
#define WRITE_OK          1     /* write operation complete   */             

/*------------------------------- DATA BUFFER ----------------------------------*/
#define TIME(r)           (((r)&0x000FFFFF) )
#define EDGE(r)           (((r)&0x00100000) >> 20)
#define DATA_TYPE(r)      (((r)&0x00600000) >> 21)
#define START(r)          (((r)&0x00800000) >> 23)
#define CHANNEL(r)        (((r)&0x7F000000) >> 24)
#define TDC(r)            (((r)&0x70000000) >> 29)
#define EVENT_NUM(r)      (((r)&0x0000FFFF) )
#define WORD_NUM(r)       (((r)&0x0000FFFF) )
#define GEO_B(r)          (((r)&0xF8000000) >> 27)
#define STAT_B(r)         (((r)&0x07000000) >> 24)



/*******************************************************************************/
/*                 V767 UTILITY FUNCTION DECLARATION                           */
/*******************************************************************************/
/* the following procedures are contained in the file v767util.c               */
/*******************************************************************************/
void enable_mcst();
void disable_mcst();
ulong *alloc_buffer();
ushort fill_buffer();
/* etc. etc.....*/
#endif
