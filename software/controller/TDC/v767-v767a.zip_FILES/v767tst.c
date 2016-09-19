/*******************************************************************************/
/*                                                                             */
/*                          V767 test program                                  */
/*                                                                             */
/*                               C.A.E.N                                       */
/*                                                                             */
/*                 by Carlo Tintori     Viareggio, 07/97                       */
/*                                                                             */    
/*******************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <math.h> 

#include "conio.h"
#include "vmeio.h"
#include "tutil.h"
#include "vmanual.h"
#include "menu.h"

#include "v767menu.h"
#include "v767.h"


/*******************************************************************************/
/*                             GLOBAL VARIABLES                                */
/*******************************************************************************/

/* registers */
ushort control1, control2, status1, status2, setup_hs, mcst_ctrl;

/* board setup variables */
ushort int_vect;              /* interrupt vector */
ushort int_lev;               /* interrupt level */
ushort acq_mode;              /* acquisition mode */
ushort drdy_mode;             /* data ready mode */
ushort almful_lev;            /* almost full level */
ushort enword[8];             /* eneble pattern for channels */
ushort trgwin_width;          /* trigger window width */
short  trgwin_offs;           /* trigger window offset */
ushort trg_latency;           /* trigger latency */
ushort trg_conf;              /* trigger conf. bit (sub trg, ovl trg) */
ushort str_conf;              /* start conf. bit (rdout str, sub str, empty str) */
ushort ch_adjust[128];        /* individual channels adjust */
short  global_offset;         /* global time offset (coarse counter) */
ushort adj_enable;            /* individual adjust enable */
ushort odd_detect;            /* odd channel edge detection */
ushort even_detect;           /* even channel edge detection */
ushort start_detect;          /* start edge detection */
ushort en_autoload;           /* enable auto load user setup */
ushort back_win;              /* look back window */
ushort ahead_win;             /* look ahead window */
ushort adv_conf;              /* advanced settings conf */
ushort rej_offset;            /* automatic reject offset */
ushort en_autorej;            /* enable auto reject */
ushort dll_curr;              /* dll current */ 


ulong *swbuffer;     /* software memory buffer for block transfer */

char msg[100];       /* string temp buffer */


/*******************************************************************************/
/*                                 BOARD SELECT                                */
/*******************************************************************************/
void board_select()
  {
  /* print the screen for board select menu */
  header_screen("V767 BOARD SELECT");
  display_menu(1,4,sel_menu);
  /* print the current values */
  print_value(20,4,NORMAL,"[%08X]",base_addr);
  print_value(20,5,NORMAL,"[%02X]",geo_addr);
  print_value(20,6,NORMAL,"[%08X]",sds_addr);

  /* select & exec loop */
  while(!exit_menu(sel_menu))
    if(query_menu(sel_menu))
        exec_menu(sel_menu);
  }

/*-----------------------------------------------------------------------------*/
void get_base_addr()
  {
  get_long(1,22," New base address [hex 0-FFFFFFFF] : %x",&base_addr);
  print_value(20,4,NORMAL,"[%08X]",base_addr); /* update value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void get_geo_addr()
  {
  get_short(1,22," New GEO address [hex 0-1F] : %x",&geo_addr);
  print_value(20,5,NORMAL,"[%02X]",geo_addr); /* update value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void get_sds_addr()
  {
  get_long(1,22," New SDS address [hex 0-FFFFFFFF] : %x",&sds_addr);
  print_value(20,6,NORMAL,"[%08X]",sds_addr); /* update value on the screen */
  }



/*******************************************************************************/
/*                                 BOARD STATUS                                */
/*******************************************************************************/
brd_stat_screen()
  {
  /* print all the status and control settings on the screen */

  ushort tdcerr0,tdcerr1,tdcerr2,tdcerr3;
  ulong tdcid0,tdcid1,tdcid2,tdcid3,tmp;
  char term[10];
  ushort i;

  header_screen("V767 BOARD STATUS & CONTROL"); 

  /* check terminations */
  sprintf(msg," Termination: %s",term_str[TERM_STATUS(status1)]);
  print_msg(1,3,NORMAL,msg);

  /* check memory test bit */
  sprintf(msg," Memory test: %s",onoff[TEST_BIT(MEM_TEST,control2)]);
  print_msg(1,4,NORMAL,msg);

  /* data buffer status */
  if(TEST_BIT(BUF_EMPTY,status2))
    sprintf(msg," Data Buffer: EMPTY");
  else if(TEST_BIT(BUF_FULL,status2))
    sprintf(msg," Data Buffer: FULL");
  else if(TEST_BIT(BUF_ALM_FULL,status2))
    sprintf(msg," Data Buffer: MORE THAN %d DATA",almful_lev);
  else
    sprintf(msg," Data Buffer: LESS THAN %d DATA",almful_lev);
  print_msg(1,5,NORMAL,msg);

  /* busy & data ready */
  sprintf(msg," Local Busy:       %s   Global Busy:       %s",
    onoff[TEST_BIT(LOCAL_BUSY,status1)], onoff[TEST_BIT(GLOBAL_BUSY,status1)] );
  print_msg(1,6,NORMAL,msg);
  sprintf(msg," Local Data Ready: %s   Global Data Ready: %s",
    onoff[TEST_BIT(LOCAL_DRDY,status1)], onoff[TEST_BIT(GLOBAL_DRDY,status1)] );
  print_msg(1,7,NORMAL,msg);

  /* serial number */
  print_value(1,8,NORMAL," Serial Number = %d",read_sernum());

  /* global tdc error */
  if(!TEST_BIT(TDC_ERROR,status2)) print_msg(1,9,NORMAL," No TDC error occurred");
  else print_msg(1,9,NORMAL," A TDC error is occurred");

  display_menu(2,12,bset_menu);
  /* print current values */
  print_value(36,15,NORMAL,"[%d]     ",int_vect);
  print_value(36,16,NORMAL,"[%d]     ",int_lev);
  print_value(36,17,NORMAL,"[%d]     ",almful_lev);
  print_msg(36,18,NORMAL,pos_str[BOARD_POS(mcst_ctrl)]);
  print_msg(36,19,NORMAL,onoff[TEST_BIT(BERR_ENABLE,control1)]);
  }

/*-----------------------------------------------------------------------------*/
void board_status()
  {
  ushort st1,st2;

  /* read current status and settings from the board registers */
  control1   = read_reg(CONTROL1);
  control2   = read_reg(CONTROL2);
  mcst_ctrl  = read_reg(MCST_CTRL);
  status1    = read_reg(STATUS1) & 0x00CF;
  status2    = read_reg(STATUS2) & 0x000F;
  int_vect   = read_reg(INT_VECTOR) & 0x000F;
  int_lev    = read_reg(INT_LEVEL) & 0x0007; 
  write_setup(READ_ALM_FULL); 
  almful_lev = read_setup(); 

  brd_stat_screen();  /* print the screen */
  
  /* select & exec loop */
  while(!exit_menu(bset_menu))
    {
    if(query_menu(bset_menu))
      exec_menu(bset_menu);
      /* check if status is changed */
      st1 = read_reg(STATUS1) & 0x00CF;
      st2 = read_reg(STATUS2) & 0x000F;
      /* if status is changed -> update the screen */
      if( (st1 != status1) || (st2 != status2) )
        {
        status1 = st1; /* store new status */
        status2 = st2;
        brd_stat_screen();
        }
    }

  }

/*-----------------------------------------------------------------------------*/
void tdc_err()
  {
  /* print tdc error codes on the screen */

  ushort i;

  for(i=12;i<24;i++)
    clear_line(i);   /* delete the menu */
  print_msg(1,12,NORMAL," TDC error codes:");
  /* read tdc error codes */
  for(i=0;i<(num_channels/32);i++)
    {
    read_tdc_error(i,msg);
    print_msg(4,13+i,NORMAL,msg);
    }
  print_msg(1,17,NORMAL," Press a key to continue");
  key_pressed();  /* wait a key */
  brd_stat_screen();     /* restore the screen */
  }

/*-----------------------------------------------------------------------------*/
void tdc_id()
  {
  /* print tdc id codes on the screen */

  ushort i;

  for(i=12;i<24;i++)
    clear_line(i);   /* delete the menu */
  print_msg(1,12,NORMAL," TDC id codes:");
  for(i=0;i<(num_channels/32);i++)
    {
    read_tdc_idcode(i,msg);
    print_msg(4,13+i,NORMAL,msg);
    }
  print_msg(1,17,NORMAL," Press a key to continue");
  key_pressed();  /* wait a key */
  brd_stat_screen();     /* restore the screen */
  }

/*-----------------------------------------------------------------------------*/
void read_conf_rom()
  {
  }

/*-----------------------------------------------------------------------------*/
void get_int_vect()
  {
  get_short(1,22," New interrupt vector [dec 0-255] : %d",&int_vect);
  write_reg(INT_VECTOR,int_vect);
  print_value(36,15,NORMAL,"[%d]     ",int_vect); /* update value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void get_int_lev()
  {
  get_short(1,22," New interrupt level [dec 0-7] : %d",&int_lev);
  write_reg(INT_LEVEL,int_lev);
  print_value(36,16,NORMAL,"[%d]     ",int_lev); /* update value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void get_almf_lev()
  {
  get_short(1,22," New almost full level [dec 1-32767] : %d",&almful_lev);
  write_setup(SET_ALM_FULL);
  write_setup(almful_lev);
  print_value(36,17,NORMAL,"[%d]     ",almful_lev); /* update value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void set_brd_pos()
  {
  /* toggle the board position */
  /* ->  inactive -> first -> active -> last ->  */
  /* <-----------------------------------------  */

  mcst_ctrl = (mcst_ctrl + 1) & 0x03;
  write_reg(MCST_CTRL,mcst_ctrl);
  write_reg(MCST_CTRL,mcst_ctrl);   /* write the new control1 register */
  print_msg(36,18,NORMAL,pos_str[BOARD_POS(mcst_ctrl)]); /* update value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void set_berren()
  {
  /* toggle berr enable on/off */
  TOGGLE_BIT(BERR_ENABLE,control1);
  write_reg(CONTROL1,control1);   /* write the new control1 register */
  print_msg(36,19,NORMAL,onoff[TEST_BIT(BERR_ENABLE,control1)]); /* update value on the screen */
  }



/*******************************************************************************/
/*                               ACQUISITION SETUP                             */
/*******************************************************************************/
void acq_setup_screen()
  {
  ushort i;

  /* print the screen for acq. setup */
  header_screen("ACQUISITION SETUP");
  display_menu(1,4,acq_menu);
  /* print current settings */
  print_msg(30,4,NORMAL,acqm_str[acq_mode]);
  print_msg(30,5,NORMAL,drdym_str[drdy_mode]);
  }

/*-----------------------------------------------------------------------------*/
void acq_setup()
  {
  ushort i;

  /* read current settings from the board */
  write_setup(READ_ACQ_MODE);
  acq_mode = read_setup() & 0x03;
  write_setup(READ_DR_MODE);
  drdy_mode = read_setup() & 0x03;

  acq_setup_screen(); /* print the screen */

  /* select & exec loop */
  while(!exit_menu(acq_menu))
    if(query_menu(acq_menu))
        {
        exec_menu(acq_menu);
        acq_setup_screen(); /* update the screen */
        }
  }

/*-----------------------------------------------------------------------------*/
void set_acq_mode()
  {
  /* toggle the acquisition mode */
  /* ->  stop_match -> start_match -> start_gating -> cont_store ->  */
  /* <-------------------------------------------------------------  */
  acq_mode = (acq_mode + 1) % 4;
  write_setup(STOP_MATCH + (acq_mode << 8));   /* write the acq mode */
  }

/*-----------------------------------------------------------------------------*/
void set_dtrdy_mode()
  {
  /* toggle the data ready mode */
  /* -> ev_ready -> buffer almost full -> datum ready ->  */
  /* <--------------------------------------------------  */
  drdy_mode = (drdy_mode + 1) % 3;
  write_setup(DR_EV_READY + (drdy_mode << 8));   /* write the acq mode */
  }


/*******************************************************************************/
/*                             CHANNELS ENABLE                                 */
/*******************************************************************************/
void ch_enable_screen()
  {
  ushort i;

  /* print the screen for ch. enable */
  header_screen("CHANNELS ENABLE"); 
  /* print the current settings for each tdc */
  for(i=0; i<(num_channels/16); i++)
    {
    sprintf(msg," ch [%d..%d] ", i*16,i*16+15);
    print_msg(1,4+i,NORMAL,msg);
    sprintf(msg," (TDC%d) : [ %04X ]",i/2,enword[i]);
    print_msg(15,4+i,NORMAL,msg);
    }
  display_menu(1,14,chen_menu);
  }

void ch_enable()
  {
  ushort i;

  /* read current settings */
  write_setup(READ_EN_PATTERN);
  for(i=0;i<(num_channels/16);i++)
    enword[i]=read_setup();

  ch_enable_screen(); /* print the screen */

  /* select & exec loop */
  while(!exit_menu(chen_menu))
    if(query_menu(chen_menu))
        {
        exec_menu(chen_menu);
        ch_enable_screen(); /* restore the screen */
        }
  }

/*-----------------------------------------------------------------------------*/
void ch1_en()
  {
  ushort ch;

  get_short(1,22," Number of channel to enable [dec 0:127] : %d",&ch);
  enword[ch/16] = enword[ch/16] | (1<<(ch%16));
  write_setup(EN_CHANNEL(ch)); /* enable the channel */
  }

/*-----------------------------------------------------------------------------*/
void ch1_dis()
  {
  ushort ch;

  get_short(1,22," Number of channel to disable [dec 0:127] : %d",&ch);
  enword[ch/16] = enword[ch/16] & ~(1<<(ch%16));
  write_setup(DIS_CHANNEL(ch)); /* disable the channel */
  }

/*-----------------------------------------------------------------------------*/
void chall_en()
  {
  ushort i;

  for(i=0;i<8;i++)
    enword[i]=0xFFFF;
  write_setup(EN_ALL_CH); /* enable all the channels */
  }

/*-----------------------------------------------------------------------------*/
void chall_dis()
  {
  ushort i;

  for(i=0;i<8;i++)
    enword[i]=0x0000;
  write_setup(DIS_ALL_CH); /* disable all the channels */
  }

/*-----------------------------------------------------------------------------*/
void ch16_endis()
  {
  ushort i,enw;

  write_setup(WRITE_EN_PATTERN);
  for(i=0; i<(num_channels/16); i++)
    {
    sprintf(msg," Enable pattern for ch[%d:%d] ",i*16,i*16+15);
    print_msg(1,22,NORMAL,msg); 
    get_short(36,22," [hex 0:FFFF] : %x",&enw);
    enword[i] = enw;
    write_setup(enw);
    clear_line(22);
    }

  }


/*******************************************************************************/
/*                                TRIGGER SETTINGS                             */
/*******************************************************************************/
void trg_sets()
  {
  header_screen("TRIGGER SETTINGS");

  /* read current settings */
  write_setup(READ_WIN_WIDTH);
  trgwin_width = read_setup();
  write_setup(READ_WIN_OFFS);
  trgwin_offs = read_setup();
  write_setup(READ_TRG_LAT);
  trg_latency = read_setup();
  write_setup(READ_TRG_CONF);
  trg_conf = read_setup();

  display_menu(1,4,trgs_menu);
  /* print the current values */
  print_value(32,4,NORMAL,"[%d]        ",trgwin_width);
  print_value(32,5,NORMAL,"[%d]        ",trgwin_offs);
  print_value(32,6,NORMAL,"[%d]        ",trg_latency);
  print_msg(32,7,NORMAL,endis[TEST_BIT(0,trg_conf)]);
  print_msg(32,8,NORMAL,endis[TEST_BIT(1,trg_conf)]);

  /* select & exec loop */
  while(!exit_menu(trgs_menu))
    if(query_menu(trgs_menu))
      exec_menu(trgs_menu);
  }

/*-----------------------------------------------------------------------------*/
void get_twin_width()
  {
  get_short(1,22," New trg window width [dec 1-65535] : %d",&trgwin_width);
  write_setup(SET_WIN_WIDTH); /* write the new win width */
  write_setup(trgwin_width);
  print_value(32,4,NORMAL,"[%d]        ",trgwin_width); /* update the value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void get_twin_offs()
  {
  get_short(1,22," New trg window offset [dec -30000:30000] : %d",&trgwin_offs);
  write_setup(SET_WIN_OFFS); /* write the new win offset */
  write_setup(trgwin_offs);
  print_value(32,5,NORMAL,"[%d]        ",trgwin_offs); /* update the value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void get_trg_lat()
  {
  get_short(1,22," New trigger latency [dec 0-65535] : %d",&trg_latency);
  write_setup(SET_TRG_LAT); /* write the new trigger latency */
  write_setup(trg_latency);
  print_value(32,6,NORMAL,"[%d]        ",trg_latency); /* update the value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void en_sub_trg()
  {
  TOGGLE_BIT(0,trg_conf);  /* toggle sub. trg. time tag option */
  if(trg_conf & 1)
    write_setup(EN_SUB_TRG); /* enable sub trg time */
  else
    write_setup(DIS_SUB_TRG); /* disable sub trg time */
  print_msg(32,7,NORMAL,endis[trg_conf & 1]); /* update the screen */
  }

/*-----------------------------------------------------------------------------*/
void en_ovl_trg()
  {
  TOGGLE_BIT(1,trg_conf);  /* toggle ovl. trg. option */
  if(trg_conf & 2)
    write_setup(EN_OVL_TRG); /* enable overlapp. trg */
  else
    write_setup(DIS_OVL_TRG); /* disable overlapp. trg */
  print_msg(32,8,NORMAL,endis[(trg_conf & 2)>>1]); /* update the screen */
  }



/*******************************************************************************/
/*                                START SETTINGS                               */
/*******************************************************************************/
void start_sets()
  {
  header_screen("START SETTINGS");

  /* read the current settings */
  write_setup(READ_START_CONF);
  str_conf = read_setup();

  display_menu(1,4,strs_menu);

  /* print the current settings */
  print_msg(32,4,NORMAL,rost_str[str_conf & 0x03]);
  print_msg(32,5,NORMAL,endis[TEST_BIT(2,str_conf)]);
  print_msg(32,6,NORMAL,endis[TEST_BIT(3,str_conf)]);

  /* select & exec loop */
  while(!exit_menu(strs_menu))
    if(query_menu(strs_menu))
      exec_menu(strs_menu);
  }

/*-----------------------------------------------------------------------------*/
void en_ro_str()
  {
  ushort ro_mode;
  
  ro_mode = str_conf & 0x03;
  ro_mode = (ro_mode + 1) % 3;
  str_conf = str_conf & 0xFFFC | ro_mode;
  if(ro_mode == 0)
    write_setup(DIS_RO_START); /* disable read out of start */
  if(ro_mode == 1)
    write_setup(EN_RO_START);  /* enable read out of start */
  if(ro_mode == 2)
    write_setup(EN_RO_ALL_START);  /* enable read out of start */
  print_msg(32,4,NORMAL,rost_str[str_conf & 0x03]);
  }

/*-----------------------------------------------------------------------------*/
void en_sub_str()
  {
  TOGGLE_BIT(2,str_conf);  /* toggle sub. start time option */
  if(TEST_BIT(2,str_conf))
    write_setup(EN_SUB_START); /* enable sub start time */
  else
    {
    write_setup(DIS_SUB_START); /* disable sub start time */
    /* when sub start is disabled -> read out of start must be disabled */
    write_setup(DIS_RO_START);
    str_conf = str_conf & 0xFFFC;
    print_msg(32,4,NORMAL,rost_str[str_conf & 0x03]);
    }
  print_msg(32,5,NORMAL,endis[TEST_BIT(2,str_conf)]); /* update the screen */
  }

/*-----------------------------------------------------------------------------*/
void en_empty_str()
  {
  TOGGLE_BIT(3,str_conf);  /* toggle empty start option */
  if(TEST_BIT(3,str_conf))
    {
    write_setup(EN_EMPTY_START); /* enable empty start */
    /* when empty start is enabled -> read out of start must enbled for all TDCs */
    write_setup(EN_RO_ALL_START);
    str_conf = (str_conf & 0xFFFC) | 0x0002;
    print_msg(32,4,NORMAL,rost_str[str_conf & 0x03]);
    }
  else
    write_setup(DIS_EMPTY_START); /* disable empty start */
  print_msg(32,6,NORMAL,endis[TEST_BIT(3,str_conf)]); /* update the screen */
  }


/*******************************************************************************/
/*                                ADJUST SETTINGS                              */
/*******************************************************************************/
void adj_sets_screen()
  {
  ushort i,j;

  /* print the screen for adjust settings */
  header_screen("CHANNEL ADJUST");

  /* read current settings */
  write_setup(READ_EN_ADJUST);
  adj_enable = read_setup();
  write_setup(READ_GLOB_OFFS);
  global_offset = read_setup();

  /* read adjust and write them on the screen */
  if(adj_enable) /* adjust enabled */
    {
    for(i=0; i<num_channels; i++)
      {
      write_setup(READ_ADJUST_CH(i));
      ch_adjust[i] = read_setup();
      }

    /* print the current channel adjust values */
    for(i=0; i<(num_channels/16); i++)
      {
      sprintf(msg," Ch[%d..%d]  ",i*16,i*16+15);
      print_msg(1,3+i,NORMAL,msg);
      for(j=0; j<16; j++)
        print_value(16+j*4,3+i,NORMAL,"%-3d",ch_adjust[i*16+j]);
      }

    }
  else   /* adjust not enabled */
    print_msg(1,4,NORMAL," Individual adjust not enabled");
   
  display_menu(1,12,adj_menu);

  /* print global offset and indiv adjust option */
  print_value(26,14,NORMAL,"[%d]        ",global_offset);
  print_msg(26,13,NORMAL,endis[adj_enable]);
  }

/*-----------------------------------------------------------------------------*/
void adjust_sets()
  {
  adj_sets_screen(); /* print the screen */
  
  /* select & exec loop */
  while(!exit_menu(adj_menu))
    if(query_menu(adj_menu))
      exec_menu(adj_menu);
  }

/*-----------------------------------------------------------------------------*/
void get_ch_adj()
  {
  ushort ch;

  if(adj_enable) /* adjust enabled */
    {
    get_short(1,22," Number of channel [dec 0-127] : %d",&ch);
    get_short(1,22," Adjust value [dec 0-127] : %d",&ch_adjust[ch]);
    print_msg(1,22,NORMAL," Adjust setting is in progress ...");
    write_setup(SET_ADJUST_CH(ch)); /* write the new channel adjust */
    write_setup(ch_adjust[ch]);
    clear_line(22);
    print_value(16+(ch%16)*4,3+ch/16,NORMAL,"%-3d",ch_adjust[ch]); /* update the screen */
    }
  else /* adjust not enabled */
    {
    print_msg(1,22,NORMAL," Individual adjust must be enabled.");
    delay(1000);
    clear_line(22);
    }
  }

/*-----------------------------------------------------------------------------*/
void en_ind_adj()
  {
  adj_enable = !adj_enable; /* toggle enable adj. option */
  if(adj_enable)
    write_setup(EN_ALL_ADJUST); /* enable adjust */
  else
    write_setup(DIS_ALL_ADJUST); /* disable adjust */
  adj_sets_screen(); /* restore the screen */
  }

/*-----------------------------------------------------------------------------*/
void get_glob_offs()
  {
  get_short(1,22," Global offset [dec 0-65535] : %d",&global_offset);
  write_setup(SET_GLOB_OFFS); /* write the new global offset */
  write_setup(global_offset);
  print_value(26,14,NORMAL,"[%d]       ",global_offset); /* update the screen */
  }


/*******************************************************************************/
/*                               EDGE DETECT                                   */
/*******************************************************************************/
void display_detect()
  {
  /* read detection config and display it on the screen */
  write_setup(READ_DETECTION);
  odd_detect = read_setup() % 3;
  even_detect = read_setup() % 3;
  start_detect = read_setup() % 3;

  print_msg(20,4,NORMAL,edge_str[odd_detect]);
  print_msg(20,5,NORMAL,edge_str[even_detect]);
  print_msg(20,6,NORMAL,edge_str[start_detect]);
  }


void edge_detect()
  {
  header_screen("EDGE DETECTION");

  /* read current settings */
  print_msg(2,4,NORMAL,"ODD CHANNELS   : ");
  print_msg(2,5,NORMAL,"EVEN CHANNELS  : ");
  print_msg(2,6,NORMAL,"START          : ");

  display_detect();
  display_menu(1,8,edge_menu);

  /* select & exec loop */
  while(!exit_menu(edge_menu))
    if(query_menu(edge_menu))
      exec_menu(edge_menu);
  }

/*-----------------------------------------------------------------------------*/
void rise_all()
  {
  write_setup(RISE_ALL);
  odd_detect = DETECT_RISE;
  even_detect = DETECT_RISE;
  start_detect = DETECT_RISE;
  print_msg(20,4,NORMAL,edge_str[odd_detect]);
  print_msg(20,5,NORMAL,edge_str[even_detect]);
  print_msg(20,6,NORMAL,edge_str[start_detect]);
  }

/*-----------------------------------------------------------------------------*/
void fall_all()
  {
  write_setup(FALL_ALL);
  display_detect();
  }

/*-----------------------------------------------------------------------------*/
void both_all()
  {
  write_setup(BOTH_ALL);
  display_detect();
  }

/*-----------------------------------------------------------------------------*/
void oddr_evenf()
  {
  write_setup(ODDR_EVENF);
  display_detect();
  }

/*-----------------------------------------------------------------------------*/
void oddf_evenr()
  {
  write_setup(ODDF_EVENR);
  display_detect();
  }

/*-----------------------------------------------------------------------------*/
void rise_start()
  {
  write_setup(RISE_START);
  display_detect();
  }

/*-----------------------------------------------------------------------------*/
void fall_start()
  {
  write_setup(FALL_START);
  display_detect();
  }


/*******************************************************************************/
/*                                ADVANCED SETTINGS                            */
/*******************************************************************************/
void advanced_sets()
  {
  header_screen("ADVANCED SETTINGS");

  /* read the current settings */
  write_setup(READ_BACK_WIN);
  back_win = read_setup();
  write_setup(READ_AHEAD_WIN);
  ahead_win = read_setup();
  write_setup(READ_ADV_CONF);
  adv_conf = read_setup();
  write_setup(READ_REJ_OFFS);
  rej_offset = read_setup();
  write_setup(READ_EN_REJ);
  en_autorej = read_setup();
  write_setup(READ_DLL_CURR);
  dll_curr = read_setup();

  display_menu(1,4,advs_menu);

  /* print the current settings */
  print_value(32,4,NORMAL,"[%d]      ",back_win);
  print_value(32,5,NORMAL,"[%d]      ",ahead_win);
  print_value(32,6,NORMAL,"[%d]      ",rej_offset);
  print_msg(32,7,NORMAL,endis[en_autorej]);
  print_value(32,8,NORMAL,"[%d]      ",dll_curr);
  print_msg(32,10,NORMAL,endis[TEST_BIT(0,adv_conf)]);
  print_msg(32,11,NORMAL,endis[TEST_BIT(1,adv_conf)]);


  /* select & exec loop */
  while(!exit_menu(advs_menu))
    if(query_menu(advs_menu))
      exec_menu(advs_menu);

  }

/*-----------------------------------------------------------------------------*/
void set_lbwin()
  {
  get_short(1,22," New looking back window [dec 0-255] : %d",&back_win);
  write_setup(SET_BACK_WIN); /* write the new looking back window */
  write_setup(back_win);
  print_value(32,4,NORMAL,"[%d]        ",back_win); /* update the value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void set_lawin()
  {
  get_short(1,22," New looking ahead window [dec 0-255] : %d",&ahead_win);
  write_setup(SET_AHEAD_WIN); /* write the new looking ahead window */
  write_setup(ahead_win);
  print_value(32,5,NORMAL,"[%d]        ",back_win); /* update the value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void set_rejmar()
  {
  get_short(1,22," New reject offset [dec 0-65535] : %d",&rej_offset);
  write_setup(SET_REJ_OFFS); /* write the new reject margin */
  write_setup(rej_offset);
  print_value(32,6,NORMAL,"[%d]        ",rej_offset); /* update the value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void en_auto_rej()
  {
  en_autorej = !en_autorej;  /* toggle autorej option */
  if(en_autorej) write_setup(EN_AUTO_REJ);
  else write_setup(DIS_AUTO_REJ);
  print_msg(32,7,NORMAL,endis[en_autorej]);
  }

/*-----------------------------------------------------------------------------*/
void set_dllc() 
  {
  get_short(1,22," New dll current [dec 0-31] : %d",&dll_curr);
  write_setup(SET_DLL_CURR); /* write the new dll current */
  write_setup(dll_curr);
  print_value(32,8,NORMAL,"[%d]        ",dll_curr); /* update the value on the screen */
  }

/*-----------------------------------------------------------------------------*/
void reset_dll()
  {
  write_setup(RESET_DLL);
  print_msg(2,22,NORMAL,"DLL reset executed.");
  delay(800);
  clear_line(22);
  }

/*-----------------------------------------------------------------------------*/
void db_syncro()
  {
  TOGGLE_BIT(0,adv_conf);  /* toggle double syncrinize option */
  if(TEST_BIT(0,adv_conf)) write_setup(EN_DB_SYNC);
  else write_setup(DIS_DB_SYNC);
  print_msg(32,10,NORMAL,endis[TEST_BIT(0,adv_conf)]);
  }

/*-----------------------------------------------------------------------------*/
void db_hprio()
  {
  TOGGLE_BIT(1,adv_conf);  /* toggle double hit priority queue option */
  if(TEST_BIT(1,adv_conf)) write_setup(EN_DB_PRIOR);
  else write_setup(DIS_DB_PRIOR);
  print_msg(32,11,NORMAL,endis[TEST_BIT(1,adv_conf)]);
  }


/*******************************************************************************/
/*                                SAVE/LOAD SETUP                              */
/*******************************************************************************/
void saveload_setup()
  {
  header_screen("SAVE/LOAD SETUP");

  write_setup(READ_AUTO_LOAD);
  en_autoload = read_setup() & 1;
  display_menu(1,4,save_menu);
  print_msg(40,7,NORMAL,endis[en_autoload]);

  /* select & exec loop */
  while(!exit_menu(save_menu))
    if(query_menu(save_menu))
      exec_menu(save_menu);

  }

/*-----------------------------------------------------------------------------*/
void save_user()
  {
  write_setup(SAVE_CONFIG);
  print_msg(1,22,NORMAL," User configuration saved. Press a key.");
  key_pressed();
  clear_line(22);
  }

/*-----------------------------------------------------------------------------*/
void load_user()
  {
  write_setup(LOAD_CONFIG);
  print_msg(1,22,NORMAL," User configuration loaded. Press a key.");
  key_pressed();
  clear_line(22);
  }

/*-----------------------------------------------------------------------------*/
void load_default()
  {
  write_setup(LOAD_DEFAULT);
  print_msg(1,22,NORMAL," Default configuration loaded. Press a key.");
  key_pressed();
  clear_line(22);
  }

/*-----------------------------------------------------------------------------*/
void auto_load()
  {
  en_autoload = !en_autoload;
  if(en_autoload)
    write_setup(EN_AUTO_LOAD);
  else
    write_setup(DIS_AUTO_LOAD);
  print_msg(40,7,NORMAL,endis[en_autoload]);
  }



/*******************************************************************************/
/*                                  CLEAR_DATA                                 */
/*******************************************************************************/
void clear_data()
  {
  print_msg(2,22,NORMAL,"Data cleared.");
  clear_module();
  clear_line(22);
  }


/*******************************************************************************/
/*                                 READOUT DATA                                */
/*******************************************************************************/
void readout_data()
  {
  ushort i, line=0, nd=0, nword=0, curr=0, quit=0;
  ushort free = 0; /* 0 -> stop at the end of page; 1 -> write data without stopping  */
  ulong data,vme_addr;      /* pointer to the data buffer */
  char key=0;      /* key pressed */
  char dt_str[80];
  static char *edge_str[] = {"FALL", "RISE" };


  header_screen("OUTPUT DATA");  

  print_msg(2,22,NORMAL,"[Q] quit   [F] free mode   [P] pause mode");

  /* begin of loop that reads data from the output buffer of the board and writes */
  /* them on the screen. The loop stops when the key 'Q' is pressed.              */
  while( 1 )
    {
    if(curr == nword)  /* swbuffer is empty */
      {
      if(TEST_BIT(BUF_EMPTY,read_reg(STATUS2)))  /* data buffer of the board is empty */
        {
        print_msg(1,20,NORMAL," Output buffer is empty. Waiting for data ... ");
        while(TEST_BIT(BUF_EMPTY,read_reg(STATUS2)))
          {
          key = toupper(kbhit());
          if(key == 'Q')  return;
          if(key == 'F')  free = 1;
          if(key == 'P')  free = 0;
          }
        clear_line(20);
        }
      nword = fill_buffer(swbuffer,blk_size);
      curr = 0;
      }

    if(line == 0)
      for(i=0; i<16; i++)  /* clear old data */
        clear_line(i + 3);


    /* compose the data string */
    data = swbuffer[curr];
    if(DATA_TYPE(data) == DATA)
      if(START(data))
        sprintf(dt_str," - START (tdc %d)      : edge = %s   time = %-10d ", 
          TDC(data), edge_str[EDGE(data)], TIME(data));
      else
        sprintf(dt_str," - HIT  (channel %3d) : edge = %s   time = %-10d ",
          CHANNEL(data), edge_str[EDGE(data)], TIME(data));
    else if(DATA_TYPE(data) == HEADER)
      sprintf(dt_str,  "HEADER  (board %-2d)    : Event n. %-5d ",
        GEO_B(data),EVENT_NUM(data));
    else
      sprintf(dt_str,  "EOB     (board %-2d)    : %d words read.   Status = %X ",
        GEO_B(data),WORD_NUM(data),STAT_B(data));

    print_value(2,line+3,NORMAL,"%d",nd);
    print_msg(6,line+3,NORMAL,dt_str);

    nd++; curr++; line++;
    if(line == 16)  /* next page */
      {
      if(!free) /* if pause mode -> wait a key */
        {
        print_msg(2,20,NORMAL,"Press a key to continue ...");
        key = toupper(key_pressed());
        if( key == 'Q' ) return;
        if( key == 'F' ) free = 1;
        }
      clear_line(20);
      line = 0;
      }

    key = toupper(kbhit());
    if( key == 'F' ) free = 1;  /* set free run mode */
    if( key == 'P' ) free = 0;  /* set pause mode */
    if( key == 'Q' ) return;  /* abandon readout data procedure */
    }
  }

  

/*******************************************************************************/
/*                                 BOARD TEST                                  */
/*******************************************************************************/
void memory_test()
  {
  header_screen("MEMORY TEST");
  write_setup(READ_MEM_TEST);
  if(!read_setup())
    write_setup(EN_MEM_TEST);

  display_menu(1,4,memt_menu);

  /* select & exec loop */
  while(!exit_menu(memt_menu))
    if(query_menu(memt_menu))
      exec_menu(memt_menu);

  }


/*-----------------------------------------------------------------------------*/
void mtst_write1()
  {
  ulong testw;

  get_long(1,22," Memory test word [hex 0-FFFFFFFF] : %x",&testw);
  write_testword(testw);
  }

/*-----------------------------------------------------------------------------*/
void mtst_writen()
  {
  ulong testw,nw;

  get_long(1,22," Number of words for sequence 0,1,2,3 ... : %d",&nw);
  print_msg(1,22,NORMAL," Running ... ");
  for(testw=0;testw<nw;testw++)
    write_testword(testw);
  clear_line(22);
  }

/*-----------------------------------------------------------------------------*/
void mtst_writev()
  {
  ulong testw;
  ushort i,nev,nw;


  get_short(1,22," Number of events to write (dec) : %d",&nev);
  get_short(1,22," Number of words for each event (dec) : %d",&nw);
  print_msg(1,22,NORMAL," Running ... ");
  for(i=0; i<nev; i++)
    write_testevent(i,nw);
  clear_line(22);    
  }

/*-----------------------------------------------------------------------------*/
void mtst_read1()
  {
  ulong testw;

  read_dataword(&testw);
  print_value(1,16,NORMAL," Word read : hex %08X",testw);
  }


/*-----------------------------------------------------------------------------*/
void abandon_test()
  {
  /*  write_setup(DIS_MEM_TEST); disable memory test */
  control2 = control2 & 0xFFEF;
  write_reg(CONTROL2,control2);
  }


/*******************************************************************************/
/*                                 STATISTICS                                  */
/*******************************************************************************/
#define sq(x)  ((x) * (x))

void statistics()
  {

  ushort quit=0, waitstart=1;
  ushort i,j,scnt=0,key,nword=0,curr=0,tdc=0;
  ushort nsample = 100;
  ushort update = 100;
  ulong dt;
  ulong sum[128];
  double mean[128];
  double dev[128];

  write_setup(CONT_STO);

  header_screen("MEAN & DEVIATION");
  print_msg(2,22,NORMAL,"[Q] quit  [S] samples [R] reset  [T] tdc ");
  print_msg(2,3,NORMAL," Ch.  Mean   Std.Dev");
  print_msg(42,3,NORMAL," Ch.  Mean   Std.Dev");
  for(i=0; i<32; i++)
    print_value(2+40*(i/16),5+i%16,NORMAL,"%3d   ---    --- ",i);

  for(i=0;i<128;i++)
    {
    sum[i] = 0;
    mean[i] = 0;
    }

  while(!quit)
    {
    if(curr == nword)
      {
      curr = 0;
      while((nword=fill_buffer(swbuffer,blk_size)) == 0)
        if( toupper(kbhit()) == 'Q' ) return;
      }

    dt = swbuffer[curr++];
    if(START(dt))
      {
      scnt++;
      waitstart = 0;
      }
    else
      if(!waitstart)
        {
        if(mean[CHANNEL(dt)] == 0) mean[CHANNEL(dt)] = TIME(dt);
        sum[CHANNEL(dt)] += TIME(dt);
        dev[CHANNEL(dt)] += sq( (double)(TIME(dt) - mean[CHANNEL(dt)]) );
        }

    if(scnt == nsample)
      {
      scnt = 0;
      for(j=tdc*32; j<((tdc+1)*32); j++)
        if(sum[j] > 0)
          {
          mean[j] = 1.0*sum[j]/nsample;
          dev[j] = sqrt(1.0*dev[j]/nsample);
          if(dev[j] > 99) dev[j]=99;
          sprintf(msg,"%5.2f   %2.4f",mean[j],dev[j]);
          sum[j] = 0; dev[j] = 0;
          print_msg(8+40*(j/16),5+j%16,NORMAL,msg);          
          }
      }
    key = toupper(kbhit());
    if( key == 'Q' ) return;
    if( key == 'S' )
      {
      get_short(2,22,"Number of samples [dec] : %d", &nsample);
      print_msg(2,22,NORMAL,"[Q] quit  [S] samples [R] reset  [T] tdc ");
      write_reg(CLEAR,0);  /* clear data */
      waitstart=1;
      }
    if( key == 'R' )
      {
      write_reg(CLEAR,0);  /* clear data */
      waitstart=1;
      }
    if( key == 'T' )
      tdc = (tdc++) & 0x03 ;

    }

  }


/*******************************************************************************/
/*                                RESET MODULE                                 */
/*******************************************************************************/
void reset_mod()
  {
  write_reg(RESET,0);  /* reset module */
  print_msg(2,22,NORMAL,"Module reset executed.");
  delay(800);
  clear_line(22);
  }


/*******************************************************************************/
/*                                   MAIN                                      */
/*******************************************************************************/
void main()
  {
  
  init_term();     /* initialize the terminal */
  init_vme();      /* initialize the bus error handler */
  init_v767();
  swbuffer = alloc_buffer();   /* initialize the software memory data buffer */

  /* initialize all the menus */  
  main_menu  = create_menu(main_m_def);
  sel_menu   = create_menu(sel_m_def);
  bset_menu  = create_menu(bset_m_def);
  acq_menu   = create_menu(acq_m_def);
  chen_menu  = create_menu(chen_m_def);
  trgs_menu  = create_menu(trgs_m_def);
  strs_menu  = create_menu(strs_m_def);
  adj_menu   = create_menu(adj_m_def);
  edge_menu  = create_menu(edge_m_def);
  advs_menu  = create_menu(advs_m_def);
  save_menu  = create_menu(save_m_def);
  memt_menu  = create_menu(memt_m_def);

  hide_curs(); 

  header_screen("V767 TEST PROGRAM - MAIN MENU");
  display_menu(1,4,main_menu);
  
  /* main menu */
  while(!exit_menu(main_menu))
    if (query_menu(main_menu))
      {    
      exec_menu(main_menu);
      /* restore the screen */
      header_screen("V767 TEST PROGRAM - MAIN MENU");
      display_menu(1,4,main_menu);
      }

  show_curs();
  clrscr();
  free(swbuffer);
  deinit_vme();
  }

