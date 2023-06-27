#include "common.h"
#include "pic_hash_compute.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/
extern char program_mode; 
extern char hashstring[9];
extern char hash_reply_message_buffer[18];
extern unsigned char hash_reply_message_ready;

/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/


/**********************************************************************
 * ---------------------- SOLVE_HASH_TASK -------------------------------
 *
 * 
 * 
 *
 **********************************************************************/

TASK(SOLVE_HASH_TASK) 
{
    char i;
	while(1) {
        WaitEvent(SOLVE_HASH_EVENT);
        ClearEvent(SOLVE_HASH_EVENT);
        if (program_mode == END) {
            continue;
        }
        compute_hash(hashstring, hash_reply_message_buffer);
        hash_reply_message_ready = 1;
	}
	TerminateTask();
}

/* End of File : solve_hash_task.c */