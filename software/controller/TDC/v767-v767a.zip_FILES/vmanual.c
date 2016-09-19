/*******************************************************************************/
/*                                                                             */
/*                        VME MANUAL CONTROLLER                                */
/*                                                                             */
/*                               C.A.E.N                                       */
/*                                                                             */
/*                 by Carlo Tintori     Viareggio, 09/97                       */
/*                                                                             */    
/*******************************************************************************/

#include <stdio.h>
#include <ctype.h>

#include "vmanual.h"

#include "conio.h"
#include "tutil.h"
#include "menu.h"
#include "vmeio.h"


/*******************************************************************************/
/*                             GLOBAL VARIABLES                                */
/*******************************************************************************/
char man_msg[100];   /* temp string buffer */

ushort init_vmanual = 0;       /* if = 0 -> initialize the menu */
ushort man_oper=0;             /* vme operation */
ulong man_addr = 0xee000000;   /* address */
ulong man_data;                /* data */
ushort man_vector = 0xBB;      /* interrupt vector */
ushort man_level = 1;          /* interrupt level */
ushort man_am = A32;           /* addressing mode */
ushort man_dtsize = D16;       /* data format */
ulong man_basadd = 0xee000000; /* base address */
ushort man_blts = 256;         /* block size for blt */
ushort man_ncyc = 1;           /* number of cycles */
ushort man_autoinc = 0;        /* auto increment address */
ulong *man_buff;               /* memory buffer for blt */

/* usefull names */
char *man_onoff[]      = { "[OFF]", "[ON] " };
char *man_oper_str[]   = { "READ",
                           "WRITE",
                           "BLOCK TRANSFER READ",
                           "BLOCK TRANSFER WRITE",
                           "WAIT INTERRUPT"};



/*******************************************************************************/
/*                             FUNCTIONS DECLARATION                           */
/*******************************************************************************/
void man_read(), man_write(), man_rblt(), man_wblt(), man_int(), set_m_add(), 
     set_m_ds(), set_m_am(), set_m_ba(), set_m_blts(), auto_inc(), num_mcyc(),
     view_bltdt(), exec_man();


/*******************************************************************************/
/*                              MENU DEFINITION                                */
/*******************************************************************************/
MENU_ENTRY manc_m_def[]  = { " R - READ"                   , man_read   , 'R',
                             " W - WRITE"                  , man_write  , 'W',
                             " B - BLOCK TRANSFER READ"    , man_rblt   , 'B',
                             " T - BLOCK TRANSFER WRITE"   , man_wblt   , 'T',
                             " I - CHECK INTERRUPT"        , man_int    , 'I',
                             " L - REPEAT LAST OPERATION"  , exec_man   , 'L',
                             " 1 - ADDRESS"                , set_m_add  , '1',
                             " 2 - BASE ADDRESS"           , set_m_ba   , '2',
                             " 3 - DATA FORMAT"            , set_m_ds   , '3',
                             " 4 - ADDRESSING MODE"        , set_m_am   , '4',
                             " 5 - BLOCK TRANSFER SIZE"    , set_m_blts , '5',
                             " 6 - AUTO INCREMENT ADDRESS" , auto_inc   , '6',
                             " 7 - NUMBER OF CYCLES"       , num_mcyc   , '7',
                             " 8 - VIEW BLT DATA"          , view_bltdt , '8',
                             " 0 - QUIT MANUAL CONTROLLER" , quit_menu  , '0',
                             EOM };
MENU *manc_menu;



/*-----------------------------------------------------------------------------*/
/* print the screen for manual controller */
void manual_screen()
  {
  header_screen("VME MANUAL CONTROLLER");  /* print the header on the screen */

  display_menu(1,YM,manc_menu);  /* display the manual controller menu */

  /* print current settings */
  print_value(36,YM+6,NORMAL,"[%08X]          ",man_addr);  /* vme address */
  print_value(36,YM+7,NORMAL,"[%08X]",man_basadd);  /* base address */
  sprintf(man_msg,"[D%d] ",8*man_dtsize);  /* data size */
  print_msg(36,YM+8,NORMAL,man_msg);
  if(man_am == A32)  /* address modifier */
    print_value(36,YM+9,NORMAL,"[A32]");
  else
    print_value(36,YM+9,NORMAL,"[A24]");
  print_value(36,YM+10,NORMAL,"[%d]      ",man_blts);  /* blt size */
  print_msg(36,YM+11,NORMAL,man_onoff[man_autoinc]);  /* autoincr. address */
  if(man_ncyc > 0)
    print_value(36,YM+12,NORMAL,"[%d]        ",man_ncyc);
  else
    print_msg(36,YM+12,NORMAL,"[infinite]  ");

  }


/*-----------------------------------------------------------------------------*/
/* execute read cycle(s) */
void man_read()
  {
  man_oper = MAN_READ;
  if(man_dtsize == D64)
    print_msg(2,YD+2,NORMAL,"Can't execute a Read Cycle in D64");
  else
    exec_man(); 
  }

/*-----------------------------------------------------------------------------*/
/* execute write cycle(s) */
void man_write()
  {
  man_oper = MAN_WRITE;
  if(man_dtsize == D64)
    print_msg(2,YD+2,NORMAL,"Can't execute a Write Cycle in D64");
  else
    {
    get_long(2,YD+3,"Write Data [hex] : %x",&man_data);  /* get data to write */
    exec_man();
    }
  }

/*-----------------------------------------------------------------------------*/
/* execute block transfer read */
void man_rblt()
  {
  man_oper = MAN_RBLT;
  if(man_dtsize == D8)
    print_msg(2,YD+2,NORMAL,"Can't execute a Block Transfer in D8");
  else if(man_dtsize == D16)
    print_msg(2,YD+2,NORMAL,"Can't execute a Block Transfer in D16");
  else
    exec_man();
  }

/*-----------------------------------------------------------------------------*/
/* execute block transfer write */
void man_wblt()
  {
  ushort i;

  man_oper = MAN_WBLT;
  if(man_dtsize == D8)
    print_msg(2,YD+2,NORMAL,"Can't execute a Block Transfer in D8");
  else if(man_dtsize == D16)
    print_msg(2,YD+2,NORMAL,"Can't execute a Block Transfer in D16");
  else
    {
    get_long(2,YD+3,"Write Data [hex] : %x",&man_data);  /* get data to write */
    for(i=0; i<(man_blts/4); i++)  /* fill the data buffer */
      man_buff[i] = man_data;
    exec_man();
    }
  }

/*-----------------------------------------------------------------------------*/
/* execute interrupt check */
void man_int()
  {
  man_oper = MAN_INT;
  get_short(2,YD+3,"Interrupt Vector [hex] : %x",&man_vector);  /* get the int vect */
  get_short(2,YD+3,"Interrupt Level [hex] : %x",&man_level);  /* get the int level */
  exec_man();
  }


/*-----------------------------------------------------------------------------*/
/* get vme address (offset) and add the base address */
void set_m_add()
  {
  get_long(2,YD+3,"VME Address (only Offset) [hex] : %x",&man_addr);
  man_addr = man_addr + man_basadd;
  print_value(36,YM+6,NORMAL,"[%08X]   ",man_addr);  /* update the screen */
  }

/*-----------------------------------------------------------------------------*/
/* get base address and add it to the vme address */
void set_m_ba()
  {
  man_addr = man_addr - man_basadd;  /* subract the old base address */
  get_long(2,YD+3,"Manual Base address [hex] : %x",&man_basadd);
  print_value(36,YM+7,NORMAL,"[%08X]   ",man_basadd);  /* update the screen */
  man_addr = man_addr + man_basadd;
  print_value(36,YM+6,NORMAL,"[%08X]   ",man_addr);  /* update the screen */
  }
     
/*-----------------------------------------------------------------------------*/
/* toggle data size D8 - D16 - D32 - D64 */
void set_m_ds()
  {
  man_dtsize = man_dtsize * 2;
  if(man_dtsize > D64) man_dtsize = D8;
  sprintf(man_msg,"[D%d] ",8*man_dtsize);
  print_msg(36,YM+8,NORMAL,man_msg);  /* update the screen */
  }

/*-----------------------------------------------------------------------------*/
/* get the addressing mode */
void set_m_am()
  {
  if(man_am == A32)
    {
    man_am = A24;
    print_value(36,YM+9,NORMAL,"[A24]  ");  /* update the screen */
    }
  else
    {
    man_am = A32;
    print_value(36,YM+9,NORMAL,"[A32]  ");  /* update the screen */
    }
  }

/*-----------------------------------------------------------------------------*/
/* get the block transfer size */
void set_m_blts()
  {
  get_short(2,YD+3,"Block transfer size (bytes) [dec] : %d",&man_blts);
  print_value(36,YM+10,NORMAL,"[%d]      ",man_blts);  /* update the screen */
  }

/*-----------------------------------------------------------------------------*/
/* toggle addr. autoinc. option ON - OFF */
void auto_inc()
  {
  man_autoinc = !man_autoinc;
  print_msg(36,YM+11,NORMAL,man_onoff[man_autoinc]);  /* update the screen */
  }

/*-----------------------------------------------------------------------------*/
/* get the number of cycles to execute (0 -> infinite loop) */
void num_mcyc()
  {
  get_short(2,YD+3,"Number of cycles to repeat (0 = infinite) [dec] : %d",&man_ncyc);
  if(man_ncyc > 0)  /* update the screen */
    print_value(36,YM+12,NORMAL,"[%d]        ",man_ncyc);
  else
    print_msg(36,YM+12,NORMAL,"[infinite]  ");
  }

/*-----------------------------------------------------------------------------*/
/* view data in the blt buffer */
void view_bltdt()
  {
  view_buffer(man_buff,man_blts/4);
  manual_screen();
  }

/*-----------------------------------------------------------------------------*/
/* execute vme cycle(s) */
void exec_man()
  {
  ushort i;
  ushort intok = 0;
  ulong old_data;   /* old read data */

  print_msg(2,YD+3,NORMAL,"Running ...       Press a key to stop.");
  clear_line(YD);
  sprintf(man_msg,"%s CYCLE ",man_oper_str[man_oper]);
  print_msg(2,YD,NORMAL,man_msg);
  clear_line(YD+1);  /* delete old data on the screen */
  clear_line(YD+2);
  /* exec loop */
  for(i=0; ((man_ncyc==0) || (i<man_ncyc)) & !kbhit() ; i++)
    {

    switch(man_oper)
      {
      case MAN_WRITE :     /* write cycle */
        if(!vme_write_dt(man_addr,&man_data,man_am,man_dtsize))
          print_msg(2,YD+1,NORMAL,"Bus error!                 ");
        else
          print_msg(2,YD+1,NORMAL,"Cycle(s) completed normally");
        break;
      
      case MAN_READ :    /* read cycles */
        if(!vme_read_dt(man_addr,&man_data,man_am,man_dtsize))
          print_msg(2,YD+1,NORMAL,"Bus error!                 ");
        else
          print_msg(2,YD+1,NORMAL,"Cycle(s) completed normally");
        /* update read data on the screen */
        if((i==0) || (old_data != man_data))  /* data changed */
          if(man_dtsize == D8)
            print_value(2,YD+2,NORMAL,"Data read : %02X ",man_data);
          else if(man_dtsize == D16) 
            print_value(2,YD+2,NORMAL,"Data read : %04X ",man_data);
          else 
            print_value(2,YD+2,NORMAL,"Data read : %08X ",man_data);
        old_data = man_data;
        break;

      case MAN_RBLT :  /* block transfer read cycle */
        for(i=0;i<(man_blts/4);i++)
          man_buff[i]=0;
        if(!vme_read_blk(man_addr,man_buff,man_blts,man_dtsize))
          print_msg(2,YD+1,NORMAL,"Bus error!                 ");
        else
          print_msg(2,YD+1,NORMAL,"Cycle(s) completed normally");
        break;

      case MAN_WBLT :  /* block transfer read cycle */
        if(!vme_write_blk(man_addr,man_buff,man_blts,man_dtsize))
          print_msg(2,YD+1,NORMAL,"Bus error!                 ");
        else
          print_msg(2,YD+1,NORMAL,"Cycle(s) completed normally");
        break;

      case MAN_INT :
        if(!vme_enable_int(man_vector,man_level))
          print_msg(2,YD+2,NORMAL,"Can't check interrupt      ");
        else
          {
          print_msg(2,YD+3,NORMAL,"Waiting for interrupt ... Press a key to stop.");
          while(!intok && !kbhit())   /* check int loop */
            intok = vme_check_int(man_level);
          if(intok)
            print_value(2,YD+2,NORMAL,"Interrupt request active on IRQ%d",man_level);
          else
            print_msg(2,YD+2,NORMAL,"No interrupt request");
          }
        break;
      }

    if(man_autoinc)
      {
      man_addr = man_addr + man_dtsize;  /* increment address (+1 or +2 or +4) */
      print_value(36,YM+10,NORMAL,"[%08X]   ",man_addr);  /* update the screen */
      }
    }
  clear_line(YD+3);  /* delete running message */
  }




/*******************************************************************************/
/*                          CALL FOR MANUAL CONTROLLER                         */
/*******************************************************************************/

/*-----------------------------------------------------------------------------*/
void manual_controller()
  {

  if(!init_vmanual)  /* first call */
    {
    manc_menu = create_menu(manc_m_def);  /* initialize the menu */  
    init_vmanual = 1;
    }

  /* allocate 2048 lword for the software buffer containing data for blt */
  /* NOTE: 4096 bytes are necessary for the pointer allignement */
  man_buff = (ulong *)malloc(4096);
  if (man_buff == NULL)
    {
    print_err(" Can't allocate memory for blt buffer. ");
    exit(-1);
    }
  /* allign the pointer to 2k */
  if ((unsigned)man_buff & 0x3FF)
    {
    unsigned temp = (unsigned)man_buff;	
    temp = 0x400 - (temp & 0x3FF) + temp ;
    (unsigned)man_buff = temp ;
    }

  manual_screen(); /* print the screen */

  /* select & exec loop */
  while(!exit_menu(manc_menu))
    if(query_menu(manc_menu))
      exec_menu(manc_menu);

  free(man_buff);  /* release the memory buffer */
  }



