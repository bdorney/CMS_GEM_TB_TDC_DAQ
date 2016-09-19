/******************************************************************************/
/*                                                                            */
/*              utility for input/output control on TERM-100                  */
/*                                                                            */
/*                               C.A.E.N                                      */
/*                                                                            */
/*                 by Carlo Tintori     Viareggio, 07/97                      */
/*                                                                            */    
/******************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <ctype.h>

#include "conio.h"


/******************************************************************************/
/* This file contains the following procedures and functions:                 */
/* -------------------------------------------------------------------------- */
/* print_msg        print a message on the screen                             */
/* print_value      print a message containing a value on the screen          */
/* header_screen    clear the screen and print a title                        */
/* get_str          get e string from the std input                           */
/* get_char         get a char value from the standard input                  */
/* get_short        get a short value from the standard input                 */
/* get_long         get a long value from the standard input                  */
/* view_buffer      memory buffer viewer                                      */
/* init_variable    initialize a variable reading a ini_file                  */
/******************************************************************************/



/******************************************************************************/
/*                             PRINT_MSG                                      */
/*----------------------------------------------------------------------------*/
/* parameters:     xpos, ypos ->   position on the screen                     */ 
/*                 style      ->   text style :  NORMAL, BOLD, UNDERLINE,     */
/*                                               BLINKING, REVERSE            */
/*                 msg        ->   message text                               */
/*----------------------------------------------------------------------------*/
/* print a messege on the screen                                              */
/******************************************************************************/
#ifdef ANSI_C
 void print_msg(uint xpos, uint ypos, uint style, char *msg)
#else
 void print_msg(xpos, ypos, style, msg)
 uint xpos, ypos, style;
 char *msg;
#endif
  {

  check_init();
  gotoxy(xpos, ypos);            /* set the cursor position */
  if(style>0)
    text_style(style);
    
  printf("%s",msg);              /* print the message */
  fflush(stdout);                /* empty the output buffer */

  if(style>0)                    /* restore normal text style */
    text_style(NORMAL);
  }



/******************************************************************************/
/*                             PRINT_VALUE                                    */
/*----------------------------------------------------------------------------*/
/* parameters:     xpos, ypos ->   position on the screen                     */ 
/*                 style      ->   text style :  NORMAL, BOLD, UNDERLINE,     */
/*                                               BLINKING, REVERSE            */
/*                 msgf       ->   message + value format                     */
/*                 value      ->   numerical value (long integer)             */
/*----------------------------------------------------------------------------*/
/* print a message containing a numerical value on the screen                 */
/******************************************************************************/
#ifdef ANSI_C
 void print_value(uint xpos, uint ypos, uint style, char *msgf, long value)
#else
 void print_value(xpos, ypos, style, msgf, value)
 uint xpos, ypos, style;
 char *msgf;
 long value;
#endif
  {
  char msg[80];

  sprintf(msg,msgf,value);
  print_msg(xpos, ypos, style, msg);
  }


/******************************************************************************/
/*                              HEADER_SCREEN                                 */
/*----------------------------------------------------------------------------*/
/* parameters:     headmsg ->   header message                                */ 
/*----------------------------------------------------------------------------*/
/* clear the screen and display a header message on the first line            */
/******************************************************************************/
#ifdef ANSI_C
 void header_screen(char *headmsg)
#else
 void header_screen(headmsg)
 char *headmsg;
#endif
  {
  char blanks[81];
  
  clrscr();
  memset(blanks,BLANK,80);  /* blank line */
  blanks[80]='\0';
  print_msg(1,1,REVERSE,blanks);  /* print the reversed blank line */
  print_msg((80-strlen(headmsg))/2,1,REVERSE,headmsg); /* print the centered message */
  }


/******************************************************************************/
/*                             GET_STR                                        */
/*----------------------------------------------------------------------------*/
/* parameters:     xpos, ypos ->   prompt position on the screen              */
/*                 prompt     ->   prompt text                                */
/*                 str        ->   input string                               */
/* return:         lenght of the string                                       */        
/*----------------------------------------------------------------------------*/
/* display a prompt on the screen and get a string                            */
/******************************************************************************/
#ifdef ANSI_C
 int get_str(uint xpos, uint ypos, char *prompt, char *str)
#else
 int get_str(xpos, ypos, prompt, str)
 uint xpos, ypos;
 char *prompt, *str;
#endif
  {
  char blanks[81];
  uint i;

  check_init();
  gotoxy(xpos,ypos);
  show_curs();                /* make the cursor visible */
  printf("%s",prompt);        /* print the prompt */
  scanf("%s",str);            /* get the string */

  memset(blanks,BLANK,strlen(prompt)+strlen(str));
  blanks[strlen(prompt)+strlen(str)] = '\0';
  gotoxy(xpos,ypos);
  printf(blanks);             /* erase prompt and echo */
  if(curs_invis)
    hide_curs();              /* restore cursor visibility */
  fflush(stdout);             /* empty the output buffer */
  if(str[0] == ESC)           /* if ESC has been pressed -> return empty string */
    str[0] = '\0';
  return(strlen(str));
  }


/******************************************************************************/
/*                    GET_CHAR, GET_SHORT, GET_LONG                           */
/*----------------------------------------------------------------------------*/
/* parameters:     xpos, ypos ->   prompt position on the screen              */
/*                 prompt     ->   prompt text containing value format        */
/*                 var        ->   pointer to the variable to get             */
/*----------------------------------------------------------------------------*/
/* display a prompt on the screen and get a value (char, short or long)       */
/******************************************************************************/
#ifdef ANSI_C
 void get_long(uint xpos, uint ypos, char *prompt, long *var)
#else
 void get_long(xpos, ypos, prompt, var)
 uint xpos, ypos;
 char *prompt;
 long *var;
#endif
  {
  uint i;
  char format[80], strval[80], prompt1[80];

  /* find the var format in the prompt string */
  for(i=0; (i<strlen(prompt)) && (prompt[i]!='%'); i++);
  if(i<strlen(prompt))
    strcpy(format,&prompt[i]);  /* get the value format from the prompt */
  else
    {
    print_err("get_value error: the prompt must contain the value format.");
    return;
    }
  strcpy(prompt1,prompt);   /* copy the prompt */
  prompt1[i]='\0'; /* trunc the prompt */

  /* get the string representing the value */
  if(!get_str(xpos,ypos,prompt1,strval))
    return;
  if(strval[0] == '^')  /* force value format */
    {
    sprintf(format,"%%%c",strval[1]);
    if(toupper(format[1])=='H')
      format[1]='x';
    sscanf(&strval[2],format,var);  /* get the value */
    }
  else
    sscanf(strval,format,var);  /* get the value */
  }

#ifdef ANSI_C
 void get_short(uint xpos, uint ypos, char *prompt, short *var)
#else
 void get_short(xpos, ypos, prompt, var)
 uint xpos, ypos;
 char *prompt;
 short *var;
#endif
  {
  uint i;
  ulong lvar;
  
  get_long(xpos,ypos,prompt,&lvar);
  *var = (ushort)lvar;
  }

#ifdef ANSI_C
 void get_char(uint xpos, uint ypos, char *prompt, char *var)
#else
 void get_char(xpos, ypos, prompt, var)
 uint xpos, ypos;
 char *prompt;
 char *var;
#endif
  {
  uint i;
  ulong lvar;
  
  get_long(xpos,ypos,prompt,&lvar);
  *var = (uchar)lvar;
  }



/******************************************************************************/
/*                             VIEW_BUFFER                                    */
/*----------------------------------------------------------------------------*/
/* parameters:     buffer     ->   pointer to the buffer (ulong *)            */
/*                 nlw        ->   number of lword to view                    */
/*----------------------------------------------------------------------------*/
/* display the content of a memory buffer                                     */
/******************************************************************************/
#ifdef ANSI_C
 void view_buffer(ulong *buffer, uint nlw)
#else
 void view_buffer(buffer, nlw)
 ulong *buffer;
 uint nlw;
#endif
  {
  ushort i,j,line, page=0, gotow, dtsize = 4, ndata;
  ulong *d32;
  ushort *d16;
  uchar *d8;
  char msg[80];
  char key = 0;
  FILE *fsave;

  d32 = buffer;
  d16 = (ushort *)buffer;
  d8  = (uchar *)buffer;

  header_screen("VIEW BUFFER");  /* clear the screen and print the header */
  print_msg(1,3,REVERSE,"  Num.  Addr     Hex         Dec         ");

  /* begin of loop. Exit if 'Q' is pressed  */
  while(key != 'Q')
    {
    ndata = nlw * 4 / dtsize;
    /* write a page */
    for(line=0,i=page*16;(line<16) && (i<ndata);line++,i++)
      {
      if(dtsize == 4)        /* 32 bit */
        sprintf(msg,"%5d   %04X     %08X    %-10d ",i,i*4,d32[i],d32[i]); 
      else if(dtsize == 2)   /* 16 bit */
        sprintf(msg,"%5d   %04X     %04X        %-6d     ",i,i*2,d16[i],d16[i]);
      else                   /* 8 bit */
        sprintf(msg,"%5d   %04X     %02X          %-4d       ",i,i,d8[i],d8[i]);
      print_msg(1,line+5,NORMAL,msg);
      }

    /* print the line menu */
    sprintf(msg,"[Q] Quit  [D] Data_size  [S] Save  [G] Goto");
    if(page!=0)
      strcat(msg,"  [P] Previous");
    if(i!=ndata)
      strcat(msg,"  [N] Next");
    print_msg(2,22,NORMAL,msg);

    /* execute the line menu */
    key=toupper(key_pressed());
    clear_line(22);
    if((key=='N') && (i<ndata))  /* next page */
      page++;
    else if((key=='P') && (page>0))  /* previous page */
      page--;
    else if (key=='D')  /* change data size (8,16,32) */
      {
      dtsize = dtsize * 2; 
      if(dtsize > 4) dtsize = 1;
      page = 0;
      }
    else if(key=='G')  /* goto data */
      {
      get_short(2,22,"Insert data number (dec) : %d",&gotow);
      if(gotow>ndata)
        print_msg(2,22,NORMAL," Invalid data number ");
      else
        page=gotow/16;
      }
    else if(key=='S')  /* save buffer to file */
      {
      get_str(2,22," File Name : ",msg);
      if((fsave=fopen(msg,"w")) == NULL)
        print_msg(2,22,NORMAL," Can't open file ");
      else
        {
        for(j=0;j<ndata;j++)
          fprintf(fsave,"%4X\t%8X\t%d\n",j,buffer[j],buffer[j]);
        fclose(fsave);
        }
      }
    }
  }


/******************************************************************************/
/*                             INIT_VARIABLE                                  */
/*----------------------------------------------------------------------------*/
/* parameters:     ini_file   ->   init file name                             */
/*                 var_name   ->   name string of the variable                */
/*                 var        ->   pointer to the variable (long)             */
/*----------------------------------------------------------------------------*/
/* return:         1          ->   variable initialized                       */
/*                 0          ->   variable not initialized                   */
/*----------------------------------------------------------------------------*/
/* Search the line 'VAR_NAME = init_value' in the inifile and, if found,      */
/* assign the value init_value to the variable var                            */
/******************************************************************************/
#ifdef ANSI_C
 short init_variable(char *ini_file, char *var_name, ulong *var)
#else
 short init_variable(ini_file, var_name, var)
 char *ini_file, *var_name;
 ulong *var;
#endif
  {
  FILE *fini;
  char line[200];
  ushort init=0, pos, found;

  if( (fini=fopen(ini_file,"r")) != NULL)  /* open the ini file */
    {
    while(!feof(fini) && !init)
      {
      fgets(line,100,fini);  /* read a line from the file */
      if(line[0] != '"')  /* not a comment */
        {
        /* search the var_name in the line string */
        for(pos=0, found=0; pos<strlen(line) && !found; pos++)
          if(strncmp(line,var_name,strlen(var_name)) == 0)
            found = 1;
        pos=pos+strlen(var_name);
        if(found)
          {
          while(!isdigit(line[pos]))  /* skipp spaces and '=' */
            pos++;
          if(strncmp(&line[pos],"0x",2) == 0)
            sscanf(&line[pos+2],"%x",var); /* Hex value */
          else
            sscanf(&line[pos],"%d",var); /* Dec value */
          init = 1;
          }
        }
      }
    fclose(fini);
    }

  if(init)
    return(1);
  else
    return(0);
  }



