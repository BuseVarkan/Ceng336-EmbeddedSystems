#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/
char program_mode = IDLE; 
extern unsigned char message_buffer[21];
extern unsigned char LCDchar[LINE][COLUMN];
unsigned char LCD_initial_value[LINE][COLUMN] = {"MONEY:     00000", "C: 0    ING:NNNN"};
char i = 0;
unsigned short last_money = 0;
unsigned short current_money = 0;
char ingredients[4] = {'N', 'N', 'N', 'N'};
char j = 0;
char customers[3][5] = {{0, 'N', 'N', 0, 0}, {0, 'N', 'N', 0, 0}, {0, 'N', 'N', 0, 0}};
char new_ingredient_or_customer = 0;
char number_of_customers = 0;
char hashstring[9];

/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/

/**********************************************************************
 * ------------------ EVAL_REC_MES_TASK -------------------------------
 *
 * 
 *
 **********************************************************************/
TASK(EVAL_REC_MES_TASK)
{
	while(1) {
        WaitEvent(REC_MES_EVENT);
        ClearEvent(REC_MES_EVENT);
        
        switch (message_buffer[0]) {
            case 'G' :
                i = 0;
                program_mode = ACTIVE;
                while (i < 34) {
                    LCDchar[0][i] = LCD_initial_value[0][i];
                    ++i;  
                } 
                SetRelAlarm(SEND_MES_ALARM_ID, 50, 56);
                break;                
            case 'E' :    
                CancelAlarm(SEND_MES_ALARM_ID);
                program_mode = END;
                break;
            case 'R' :
                current_money = ((unsigned short)message_buffer[17] << 8) | message_buffer[18];
                if (current_money != last_money) {
                    last_money = current_money;
                    
                    LCDchar[0][15] = current_money % 10 + 48;
                    current_money /= 10;
                    LCDchar[0][14] = current_money % 10 + 48;
                    current_money /= 10;
                    LCDchar[0][13] = current_money % 10 + 48;
                    current_money /= 10;
                    LCDchar[0][12] = current_money % 10 + 48;
                    current_money /= 10;
                    LCDchar[0][11] = current_money % 10 + 48;
                }
                
                j = 0;
                for (; j < 4; ++j){
                    if (ingredients[j] != message_buffer[13 + j]) {
                        if (ingredients[j] != message_buffer[13 + j] && (
                            message_buffer[13 + j] == 'P' || 
                            message_buffer[13 + j] == 'B' || 
                            message_buffer[13 + j] == 'M')
                            ) {
                            new_ingredient_or_customer = 1;
                        }
                        ingredients[j] = message_buffer[13 + j];
                        LCDchar[1][12 + j] = ingredients[j];
                    }
                    
                    
                }
                j =  0;
                number_of_customers = 0;                        
                for (; j < 3; ++j){
                    if (customers[j][0]) {
                        ++number_of_customers;
                    }
                    if(customers[j][0] == message_buffer[1 + 4*j]) {
                        continue;
                    }
                    if (message_buffer[1 + 4*j]) {
                        new_ingredient_or_customer = 1;
                    }
                    customers[j][0] = message_buffer[1 + 4*j];
                    customers[j][1] = message_buffer[2 + 4*j];
                    customers[j][2] = message_buffer[3 + 4*j];
                    customers[j][3] = message_buffer[4 + 4*j];
                    customers[j][4] = 0;
                }
                LCDchar[1][3] = 48 + number_of_customers;  // 48 is ASCII 0
                
                if (new_ingredient_or_customer) {
                    new_ingredient_or_customer = 0;
                    SetEvent(NEW_ING_CUS_TASK_ID, NEW_ING_CUS_EVENT);
                }
               
                break;
            case 'H' :
                hashstring[0] = message_buffer[1];
                hashstring[1] = message_buffer[2];
                hashstring[2] = message_buffer[3];
                hashstring[3] = message_buffer[4];
                hashstring[4] = message_buffer[5];
                hashstring[5] = message_buffer[6];
                hashstring[6] = message_buffer[7];
                hashstring[7] = message_buffer[8];
                hashstring[8] = '\0';
                SetEvent(SOLVE_HASH_TASK_ID, SOLVE_HASH_EVENT);
                break;
            default:
                break;
        }
              
	}
	TerminateTask();
}


/* End of File : eval_rec_mes_task.c */
