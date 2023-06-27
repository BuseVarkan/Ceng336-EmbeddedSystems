#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/

unsigned char cook_message_buffer[3][3];
unsigned char cook_message_ready_count = 0;
unsigned char slow_cook_message_buffer[2];
unsigned char slow_cook_message_ready = 0;
char hash_reply_message_buffer[18];
unsigned char hash_reply_message_ready = 0;
unsigned char toss_message_buffer;
unsigned char toss_message_ready = 0;

unsigned char command_buffer[18];
unsigned char command_buffer_count = 0;

extern char program_mode;
char u;

/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/


/**********************************************************************
 * ---------------------- SEND_MES_TASK -------------------------------
 *
 * 
 * 
 *
 **********************************************************************/

TASK(SEND_MES_TASK) 
{
	while(1) {
        WaitEvent(SEND_MES_EVENT);
        ClearEvent(SEND_MES_EVENT);
        if (program_mode == END) {
            continue;
        }
        if (slow_cook_message_ready) {
            slow_cook_message_ready = 0;
            
            command_buffer[0] = ':';
            command_buffer[1] = slow_cook_message_buffer[1];            
            command_buffer[2] = slow_cook_message_buffer[0]; 
            command_buffer[3] = 'S';
            TXREG = '$';
            command_buffer_count = 4;
            
        } else if (cook_message_ready_count) {
            cook_message_ready_count--;
            
            command_buffer[0] = ':';
            command_buffer[1] = cook_message_buffer[cook_message_ready_count][2];            
            command_buffer[2] = cook_message_buffer[cook_message_ready_count][1];  
            command_buffer[3] = cook_message_buffer[cook_message_ready_count][0]; 
            command_buffer[4] = 'C';
            TXREG = '$';
            command_buffer_count = 5;
            
        } else if (hash_reply_message_ready) {
            hash_reply_message_ready--;
            command_buffer[0] = ':';
            
            for (u = 1; u < 17; ++u) {
                command_buffer[u] = hash_reply_message_buffer[16 - u];
            }
                  
            command_buffer[17] = 'H';
            TXREG = '$';
            command_buffer_count = 18;
        } else if (toss_message_ready) {
            
            command_buffer[0] = ':';
            command_buffer[1] = toss_message_buffer;
            command_buffer[2] = 'T';       
            TXREG = '$';
            command_buffer_count = 3;
            
            toss_message_ready = 0;
        } else {
            command_buffer[0] = ':';
            command_buffer[1] = 'W';       
            TXREG = '$';
            command_buffer_count = 2;
        }
	}
	TerminateTask();
}

/* End of File : sen_mes_task.c */