/******************************************************************************/
/*                                                                            */
/*                             MENU LIBRARY                                   */
/*                                                                            */
/*                               C.A.E.N                                      */
/*                                                                            */
/*                 by Carlo Tintori     Viareggio, 07/97                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* This file contains the following procedures and functions:                 */
/*                                                                            */
/* create_menu        create a menu                                           */
/* display_menu       display a menu on the screen                            */
/* query_menu         read the keyboard and check the bindkeys                */
/* exec_menu          execute the entry corresponding to the last key pressed */
/* quit_menu          abandon the current menu                                */
/******************************************************************************/

#ifndef __MENU_H
#define __MENU_H


/*******************************************************************************/
/*                             menu type definition                            */
/*******************************************************************************/

#define EOM       ""   /* end of menu */

typedef struct
  {
  uint *variable;
  uint max,min;
  } VAR_LINK;
  

/* menu entry type */
typedef struct
  {
  char *text;        /* entry display text   */
  void (*exec)();    /* procedure to execute */
  char bindkey;      /* key assignement      */
  }  MENU_ENTRY;


/* menu descriptor type */
typedef struct
  {
  MENU_ENTRY  *entry;       /* menu entries */
  uint        numentry;     /* number of entries */
  uint        xpos, ypos;   /* position on the screen */
  int         lastkey;      /* entry of the last bindkey pressed */
  uint        exit_cond;    /* exit condition */
  }  MENU;




/*******************************************************************************/
/*                             FUNCTIONS DECLARATION                           */
/*******************************************************************************/

/*******************************************************************************/
/*                              CREATE_MENU                                    */
/*-----------------------------------------------------------------------------*/
/* parameters:     entry      ->   menu entries definition                     */
/* return          menu       ->   menu pointer                                */
/*-----------------------------------------------------------------------------*/
/* initialize a menu.                                                          */
/* example :            MENU_ENTRY mydef[] = { " 1 - ENTRY1 ", proc1, '1',     */
/*                                             " 2 - ENTRY2 ", proc2, '2',     */
/*                                             " 3 - ENTRY3 ", proc3, '3' }    */
/*                      MENU *mymenu;                                          */
/*                      mymenu = create_menu(mydef);                           */
/*******************************************************************************/
#ifdef ANSI_C
 MENU *create_menu(MENU_ENTRY entry[]);
#else
 MENU *create_menu();
#endif


/*******************************************************************************/
/*                           DISPLAY_MENU                                      */
/*-----------------------------------------------------------------------------*/
/* parameters:     menu       ->   menu to display                             */
/*                 xpos, ypos ->   menu origin on the screen                   */ 
/*                 spacing    ->   space between menu lines                    */
/*-----------------------------------------------------------------------------*/
/* display a menu on the screen                                                */
/*******************************************************************************/
#ifdef ANSI_C
 void display_menu(uint xpos, uint ypos, MENU *menu);
#else
 void display_menu();
#endif


/*******************************************************************************/
/*                             QUERY_MENU                                      */
/*-----------------------------------------------------------------------------*/
/* parameters:     menu       ->   menu to query                               */
/*-----------------------------------------------------------------------------*/
/* if a menu bindkey is pressed, then return the bindkey pressed else return 0 */
/*******************************************************************************/
#ifdef ANSI_C
 int query_menu(MENU *menu);
#else
 int query_menu();
#endif


/*******************************************************************************/
/*                              EXEC_MENU                                      */
/*-----------------------------------------------------------------------------*/
/* parameters:     menu       ->   menu to exec                                */
/*-----------------------------------------------------------------------------*/
/* execute the procedure corresponding to the last key pressed                 */
/*******************************************************************************/
#ifdef ANSI_C
 void exec_menu(MENU *menu);
#else
 void exec_menu();
#endif


void get_value();
void quit_menu();


#endif
