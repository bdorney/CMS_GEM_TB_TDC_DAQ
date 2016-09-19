/******************************************************************************/
/*                                                                            */
/*              utility for input/output control on TERM-100                  */
/*                                                                            */
/*                               C.A.E.N                                      */
/*                                                                            */
/*                 by Carlo Tintori     Viareggio, 07/97                      */
/*                                                                            */    
/******************************************************************************/

/******************************************************************************/
/* This file contains the following procedures and functions declaration:     */
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


#ifndef __TUTIL_H
#define __TUTIL_H


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
 void print_msg(uint, uint, uint, char *);
#else
 void print_msg();
#endif


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
 void print_value(uint, uint, uint, char *, long);
#else
 void print_value();
#endif


/******************************************************************************/
/*                              HEADER_SCREEN                                 */
/*----------------------------------------------------------------------------*/
/* parameters:     headmsg ->   header message                                */ 
/*----------------------------------------------------------------------------*/
/* clear the screen and display a header message on the first line            */
/******************************************************************************/
#ifdef ANSI_C
 void header_screen(char *);
#else
 void header_screen();
#endif


/******************************************************************************/
/*                             WRITE_LOG                                      */
/*----------------------------------------------------------------------------*/
/* parameters:     msg        ->   log message text                           */
/*----------------------------------------------------------------------------*/
/* write a messege in the log file                                            */
/******************************************************************************/
#ifdef ANSI_C
 void write_log(char *);
#else
 void write_log();
#endif


/******************************************************************************/
/*                             PRINT_ERR                                      */
/*----------------------------------------------------------------------------*/
/* parameters:     msg        ->   error message text                         */
/*----------------------------------------------------------------------------*/
/* print a messege error on the screen and in the log file                    */
/******************************************************************************/
#ifdef ANSI_C
 void print_err(char *);
#else
 void print_err();
#endif


/******************************************************************************/
/*                             GET_STR                                        */
/*----------------------------------------------------------------------------*/
/* parameters:     xpos, ypos ->   prompt position on the screen              */
/*                 prompt     ->   prompt text                                */
/*                 str        ->   input string                               */
/*----------------------------------------------------------------------------*/
/* display a prompt on the screen and get a string                            */
/******************************************************************************/
#ifdef ANSI_C
 int get_str(uint, uint, char *, char *);
#else
 int get_str(xpos, ypos, prompt, str);
#endif


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
 void get_long(uint, uint, char *, long *);
#else
 void get_long();
#endif

#ifdef ANSI_C
 void get_short(uint, uint, char *, short *);
#else
 void get_short();
#endif

#ifdef ANSI_C
 void get_char(uint, uint, char *, char *);
#else
 void get_char();
#endif


/******************************************************************************/
/*                             VIEW_BUFFER                                    */
/*----------------------------------------------------------------------------*/
/* parameters:     buffer     ->   pointer to the buffer (ulong *)            */
/*                 nlw        ->   number of lword to view                    */
/*----------------------------------------------------------------------------*/
/* display the content of a memory buffer                                     */
/******************************************************************************/
#ifdef ANSI_C
 void view_buffer(ulong *, uint);
#else
 void view_buffer();
#endif


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
 short init_variable(char *, char *, long *);
#else
 short init_variable();
#endif


#endif

