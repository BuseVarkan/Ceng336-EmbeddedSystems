#include "common.h"

/**********************************************************************
 * --------------------- COUNTER & ALARM DEFINITION -------------------
 **********************************************************************/
Counter Counter_list[] = 
  {
   /*******************************************************************
    * -------------------------- First counter ------------------------
    *******************************************************************/
   {
     {
       200,                                /* maxAllowedValue        */
        10,                                /* ticksPerBase           */
       100                                 /* minCycle               */
     },
     0,                                    /* CounterValue           */
     0                                     /* Nbr of Tick for 1 CPT  */
   }
  };

Counter Counter_kernel = 
  {
    {
      65535,                              /* maxAllowedValue        */
          1,                              /* ticksPerBase           */
          0                               /* minCycle               */
    },
    0,                                    /* CounterValue           */
    0                                     /* Nbr of Tick for 1 CPT  */
  };

AlarmObject Alarm_list[] = 
  {
   /*******************************************************************
    * -------------------------- First task ---------------------------
    *******************************************************************/
   {
     OFF,                                  /* State                   */
     0,                                    /* AlarmValue              */
     0,                                    /* Cycle                   */
     &Counter_kernel,                      /* ptrCounter              */
     SEND_MES_TASK_ID,                     /* TaskID2Activate         */
     SEND_MES_EVENT,                       /* EventToPost             */
     0                                     /* CallBack                */
   },
    {
     OFF,                                  /* State                   */
     0,                                    /* AlarmValue              */
     0,                                    /* Cycle                   */
     &Counter_kernel,                      /* ptrCounter              */
     LCD_TASK_ID,                          /* TaskID2Activate         */
     LCD_EVENT,                            /* EventToPost             */
     0                                     /* CallBack                */
   }
};

#define _ALARMNUMBER_          sizeof(Alarm_list)/sizeof(AlarmObject)
#define _COUNTERNUMBER_        sizeof(Counter_list)/sizeof(Counter)
unsigned char ALARMNUMBER    = _ALARMNUMBER_;
unsigned char COUNTERNUMBER  = _COUNTERNUMBER_;
unsigned long global_counter = 0;

/**********************************************************************
 * --------------------- COUNTER & ALARM DEFINITION -------------------
 **********************************************************************/
Resource Resource_list[] = 
  {
   {
      10,                                /* priority           */
       0,                                /* Task prio          */
       0,                                /* lock               */
   }
  };
  
#define _RESOURCENUMBER_       sizeof(Resource_list)/sizeof(Resource)
unsigned char RESOURCENUMBER = _RESOURCENUMBER_;

/**********************************************************************
 * ----------------------- TASK & STACK DEFINITION --------------------
 **********************************************************************/
#define DEFAULT_STACK_SIZE      256
DeclareTask(EVAL_REC_MES_TASK);
DeclareTask(SEND_MES_TASK);
DeclareTask(LCD);
DeclareTask(NEW_ING_CUS_TASK);
DeclareTask(SOLVE_HASH_TASK);

// to avoid any C18 map error : regroup the stacks into blocks
// of 256 bytes (except the last one).
#pragma		udata      STACK_A   
volatile unsigned char stack_eval_rec_mes_task[DEFAULT_STACK_SIZE];
#pragma		udata      STACK_B   
volatile unsigned char stack_send_mes_task[DEFAULT_STACK_SIZE];
#pragma		udata      STACK_C   
volatile unsigned char stack_lcd[DEFAULT_STACK_SIZE];
#pragma		udata      STACK_D   
volatile unsigned char stack_new_ing_cus[DEFAULT_STACK_SIZE];
#pragma		udata      STACK_E   
volatile unsigned char stack_solve_hash_task[DEFAULT_STACK_SIZE];
#pragma		udata

/**********************************************************************
 * ---------------------- TASK DESCRIPTOR SECTION ---------------------
 **********************************************************************/
#pragma		romdata		DESC_ROM
const rom unsigned int descromarea;
/**********************************************************************
 * -----------------------------  task 0 ------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_task0 = {
	EVAL_REC_MES_TASK_PRIO,           /* prioinit from 0 to 15       */
	stack_eval_rec_mes_task,          /* stack address (16 bits)     */
	EVAL_REC_MES_TASK,                /* start address (16 bits)     */
	READY,                            /* state at init phase         */
	EVAL_REC_MES_TASK_ID,             /* id_tsk from 0 to 15         */
	sizeof(stack_eval_rec_mes_task)   /* stack size    (16 bits)     */
};

/**********************************************************************
 * -----------------------------  task 1 ------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_task1 = {
	SEND_MES_TASK_PRIO,               /* prioinit from 0 to 15       */
	stack_send_mes_task,              /* stack address (16 bits)     */
	SEND_MES_TASK,                    /* start address (16 bits)     */
	READY,                            /* state at init phase         */
	SEND_MES_TASK_ID,                 /* id_tsk from 0 to 15         */
	sizeof(stack_send_mes_task)       /* stack size    (16 bits)     */
};

/**********************************************************************
 * -----------------------------  LCD task ------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_lcd_task = {
	LCD_TASK_PRIO,                    /* prioinit from 0 to 15       */
	stack_lcd,                        /* stack address (16 bits)     */
	LCD,                            /* start address (16 bits)     */
	READY,                            /* state at init phase         */
	LCD_TASK_ID,                      /* id_tsk from 0 to 15         */
	sizeof(stack_lcd)            /* stack size    (16 bits)     */
};

/**********************************************************************
 * ------------------------NEW_ING_CUS_TASK ------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_new_ing_cus_task = {
	NEW_ING_CUS_TASK_PRIO,                    /* prioinit from 0 to 15       */
	stack_new_ing_cus,                        /* stack address (16 bits)     */
	NEW_ING_CUS_TASK,                   /* start address (16 bits)     */
	READY,                            /* state at init phase         */
	NEW_ING_CUS_TASK_ID,                  /* id_tsk from 0 to 15         */
	sizeof(stack_new_ing_cus)              /* stack size    (16 bits)     */
};

/**********************************************************************
 * ------------------------SOLVE_HASH_TASK ------------------------------
 **********************************************************************/
rom_desc_tsk rom_desc_stack_solve_hash_task = {
	SOLVE_HASH_TASK_PRIO,                    /* prioinit from 0 to 15       */
	stack_solve_hash_task,                        /* stack address (16 bits)     */
	SOLVE_HASH_TASK,                   /* start address (16 bits)     */
	READY,                            /* state at init phase         */
	SOLVE_HASH_TASK_ID,                  /* id_tsk from 0 to 15         */
	sizeof(stack_solve_hash_task)              /* stack size    (16 bits)     */
};


/**********************************************************************
 * --------------------- END TASK DESCRIPTOR SECTION ------------------
 **********************************************************************/
rom_desc_tsk end = {
	0x00,                              /* prioinit from 0 to 15       */
	0x0000,                            /* stack address (16 bits)     */
	0x0000,                            /* start address (16 bits)     */
	0x00,                              /* state at init phase         */
	0x00,                              /* id_tsk from 0 to 15         */
	0x0000                             /* stack size    (16 bits)     */
};

volatile rom unsigned int * taskdesc_addr = (&(descromarea)+1);
	
/* End of File : taskdesc.c */
