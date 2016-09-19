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

#include <stdio.h>
#include <strings.h>
#include <ctype.h>

#include "conio.h"
#include "tutil.h"
#include "menu.h"


/******************************************************************************/
/*                            GLOBAL VARIABLES                                */
/******************************************************************************/
MENU *active_menu;   /* active menu */


/******************************************************************************/
/*                              CREATE_MENU                                   */
/*----------------------------------------------------------------------------*/
/* parameters:     entry      ->   menu entries definition                    */
/* return          menu       ->   menu pointer                               */
/*----------------------------------------------------------------------------*/
/* initialize a menu.                                                         */
/* example :            MENU_ENTRY mydef[] = { " 1 - ENTRY1 ", proc1, '1',    */
/*                                             " 2 - ENTRY2 ", proc2, '2',    */
/*                                             " 3 - ENTRY3 ", proc3, '3' }   */
/*                      MENU *mymenu;                                         */
/*                      mymenu = create_menu(mydef);                          */
/******************************************************************************/
#ifdef ANSI_C
 MENU *create_menu(MENU_ENTRY entry[])
#else
 MENU *create_menu(entry)
 MENU_ENTRY entry[];
#endif
  {
  uint i;
  MENU *menu;

  /* allocate memory for menu struct */
  if( (menu = (MENU *)malloc(sizeof(MENU))) == 0)
    {
    print_err("Not enough memory for menu allocation");
    exit(-1);
    }
  else
    {
    menu->entry = entry;   /* pointer to the menu entries */
    for(i=0; entry[i].text[0] != '\0'; i++);  /* count the num. of entries */
    if(i>100)
      {
      print_err("To many menu entries");
      exit(-1);
      }
    menu->numentry = i;
    menu->lastkey = -1;
    menu->exit_cond = 0;
    return(menu);
    }
  }


/******************************************************************************/
/*                           DISPLAY_MENU                                     */
/*----------------------------------------------------------------------------*/
/* parameters:     xpos, ypos ->   menu origin on the screen                  */
/*                 menu       ->   menu to display                            */
/*----------------------------------------------------------------------------*/
/* display a menu on the screen                                               */
/******************************************************************************/
#ifdef ANSI_C
 void display_menu(uint xpos, uint ypos, MENU *menu)
#else
 void display_menu(xpos, ypos, menu)
 uint xpos, ypos;
 MENU *menu;
#endif
  {
  uint i;
  char msg[81];

  menu->xpos = xpos;  /* store the menu position on the screen */
  menu->ypos = ypos;
  for(i=0; i < menu->numentry; i++)  /* print menu entries */
    print_msg(xpos,i+ypos,NORMAL,menu->entry[i].text);
  }


/******************************************************************************/
/*                             QUERY_MENU                                     */
/*----------------------------------------------------------------------------*/
/* parameters:     menu       ->   menu to query                              */
/*----------------------------------------------------------------------------*/
/* if a menu bindkey is pressed, then return the bindkey pressed else return 0*/
/******************************************************************************/
#ifdef ANSI_C
 int query_menu(MENU *menu)
#else
 int query_menu(menu)
 MENU *menu;
#endif
  {
  char select;
  uint i=0,match=0;

  active_menu = menu;
  if( ( select=kbhit() ) != 0)         /* read the keyboard without stopping */
    while( (i < menu->numentry) && !match ) 
      {
      if(toupper(select) == toupper(menu->entry[i].bindkey))    /* check the bindkeys matching */
        {
        match = select;    /* -> quit the while loop */
        menu->lastkey = i;       /* entry to execute */
        }
      i++;
      }
  return(match);  /* return the matched bindkey (0 if no match) */
  }


/******************************************************************************/
/*                              EXEC_MENU                                     */
/*----------------------------------------------------------------------------*/
/* parameters:     menu       ->   menu to exec                               */
/*----------------------------------------------------------------------------*/
/* execute the procedure corresponding to the last key pressed                */
/******************************************************************************/
#ifdef ANSI_C
 void exec_menu(MENU *menu)
#else
 void exec_menu(menu)
 MENU *menu;
#endif
  {
  if(menu->lastkey >= 0)  /* if a valid bindkey has been pressed ... */
    (menu->entry[menu->lastkey].exec());    /* execute the procedure */
  menu->lastkey = -1; 
  }


/******************************************************************************/
/*                              EXIT_MENU                                     */
/*----------------------------------------------------------------------------*/
/* return:         exit condition -> 0 : don't exit    1 : exit               */ 
/*----------------------------------------------------------------------------*/
/* check the exit condition a menu                                            */
/******************************************************************************/
#ifdef ANSI_C
 uint exit_menu(MENU *menu)
#else
 uint exit_menu(menu)
 MENU *menu;
#endif
  {
  if(!menu->exit_cond)
    return(0);
  else
    {
    menu->exit_cond = 0;
    return(1);
    }
  }


/******************************************************************************/
/*                              QUIT_MENU                                     */
/*----------------------------------------------------------------------------*/
/* quit the current menu                                                      */
/******************************************************************************/
void quit_menu()
  {
  active_menu->exit_cond = 1;  /* set the exit condition */
  }


