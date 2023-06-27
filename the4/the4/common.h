#ifndef COMMON_H
#define COMMON_H

#include "device.h"

#define _XTAL_FREQ   10000000

/***********************************************************************
 * ------------------------ Timer settings -----------------------------
 **********************************************************************/
#define _10MHZ	63320
#define _16MHZ	61768
#define _20MHZ	60768
#define _32MHZ	57768
#define _40MHZ 	55768

/***********************************************************************
 * ----------------------------- Events --------------------------------
 **********************************************************************/
#define ALARM_EVENT        0x80  // ilk bu gidecek
#define SEND_BYTE_EVENT    0x40
#define LCD_EVENT          0x20
#define REC_MES_EVENT      0x10
#define SEND_MES_EVENT     0x08
#define NEW_ING_CUS_EVENT  0x04
#define SOLVE_HASH_EVENT   0x02


/***********************************************************************
 * ----------------------------- Alarms --------------------------------
 **********************************************************************/
#define SEND_MES_ALARM_ID   0       /* Alarm ID (index) in tascdesc.c */
#define LCD_ALARM_ID        1       /* Alarm ID (index) in tascdesc.c */


/***********************************************************************
 * ----------------------------- Task ID -------------------------------
 **********************************************************************/
/* Info about the tasks:
 * TASK0: USART
 * TASK1: USART
 */
#define SEND_MES_TASK_ID     2
#define LCD_TASK_ID          3
#define EVAL_REC_MES_TASK_ID 4
#define NEW_ING_CUS_TASK_ID  5
#define SOLVE_HASH_TASK_ID   6


/* Priorities of the tasks */
#define SEND_MES_TASK_PRIO      15
#define LCD_TASK_PRIO           8
#define EVAL_REC_MES_TASK_PRIO  9
#define NEW_ING_CUS_TASK_PRIO   14
#define SOLVE_HASH_TASK_PRIO    1

/* Operation Modes of The Cooking Simulator */
#define IDLE      0
#define ACTIVE    1
#define END       2


#define AVAILABLE 0
#define COOKING   1
#define NEEDED    2


#define COLUMN            17              /* Character number per line */
#define LINE              2               /*      Number of line       */


#endif

/* End of File : common.h */
