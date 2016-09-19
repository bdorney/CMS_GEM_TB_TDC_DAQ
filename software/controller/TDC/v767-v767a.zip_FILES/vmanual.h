/*******************************************************************************/
/*                                                                             */
/*                        VME MANUAL CONTROLLER                                */
/*                             header file                                     */
/*                                                                             */
/*                               C.A.E.N                                       */
/*                                                                             */
/*                 by Carlo Tintori     Viareggio, 09/97                       */
/*                                                                             */    
/*******************************************************************************/


#ifndef __VMANUAL_H
#define __VMANUAL_H


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


#define MAN_READ    0
#define MAN_WRITE   1
#define MAN_RBLT    2
#define MAN_WBLT    3
#define MAN_INT     4

#define YM          3   /* y coordinate for menu */
#define YD          20  /* y coordinate for display message */

/*******************************************************************************/
/*                             manual controller call                          */
/*******************************************************************************/
void manual_controller();

#endif


