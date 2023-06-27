#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/
char k = 0;
char l = 0;
extern char ingredients[4];
char ingredients_status[4];
extern char customers[3][5];
char ingredient_indices[2];
char index;
char found_in_index = -1;
extern unsigned char cook_message_buffer[3][3];
extern unsigned char cook_message_ready_count;
char ingredient_count;
char toss_index;
extern unsigned char toss_message_buffer;
extern unsigned char toss_message_ready;
char food_judge;
extern unsigned char slow_cook_message_buffer[2];
extern unsigned char slow_cook_message_ready;
char food_for_judge;
char current_food_judge;


/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/
char find_ingredient(char ingredient);
void mark_needed(char ingredient);

/**********************************************************************
 * --------------------NEW_ING_CUS_TASK -------------------------------
 *
 * 
 * 
 *
 **********************************************************************/

TASK(NEW_ING_CUS_TASK) 
{
	while(1) {
        WaitEvent(NEW_ING_CUS_EVENT);
        ClearEvent(NEW_ING_CUS_EVENT);
        
        k = 0;
        ingredients_status[0] = AVAILABLE;
        ingredients_status[1] = AVAILABLE;
        ingredients_status[2] = AVAILABLE;
        ingredients_status[3] = AVAILABLE;
        
        food_judge = 0;
        for (; k < 3; k++) {
            if (customers[k][1] == 'F') {
                food_judge = customers[k][0];
            }
            
            
            if (customers[k][0] && !customers[k][4] && customers[k][3] > 1) {
                ingredient_indices[0] = 'N';
                ingredient_indices[1] = 'N';                
                index = find_ingredient(customers[k][1]);
                if (index == -1) {
                    if (customers[k][2] != 'N') {
                        mark_needed(customers[k][2]);
                    }
                    continue;
                }
                ingredient_indices[0] = index;
                if (customers[k][2] != 'N') {
                    index = find_ingredient(customers[k][2]);
                    if (index == -1) {
                        mark_needed(customers[k][1]);                        
                        continue;
                    }
                    ingredient_indices[1] = index;    
                }
                cook_message_buffer[cook_message_ready_count][0] = customers[k][0];
                cook_message_buffer[cook_message_ready_count][1] = ingredient_indices[0];
                cook_message_buffer[cook_message_ready_count][2] = ingredient_indices[1];
                ++cook_message_ready_count;
                customers[k][4] = 1;
                
                if (ingredients_status[ingredient_indices[0]] == NEEDED) {
                    ingredients_status[ingredient_indices[0]] = COOKING;
                    mark_needed(ingredients[ingredient_indices[0]]); 
                } else {
                    ingredients_status[ingredient_indices[0]] = COOKING;
                }
                
                if (ingredient_indices[1] != 'N') {
                    if (ingredients_status[ingredient_indices[1]] == NEEDED) {
                        ingredients_status[ingredient_indices[1]] = COOKING;
                        mark_needed(ingredients[ingredient_indices[1]]); 
                    } else {
                        ingredients_status[ingredient_indices[1]] = COOKING;
                    }
                }
            }
        }
        
        if (food_judge && current_food_judge != food_judge) {
            current_food_judge = food_judge;
            food_for_judge = -1;
            food_for_judge = find_ingredient('P');
            if (food_for_judge == -1) {
                food_for_judge = find_ingredient('B');
            }
            if (food_for_judge == -1) {
                food_for_judge = find_ingredient('M');
            }
            
            slow_cook_message_buffer[0] = food_judge;
            slow_cook_message_buffer[1] = food_for_judge;            
            slow_cook_message_ready = 1;
        }
        
        
        k = 0;
        ingredient_count = 0;
        for (; k < 4; k++) {
            if (ingredients[k] != 'N') {
                ingredient_count++;
            }
        }
        if (ingredient_count == 4) {
            toss_index = -1;
            k = 0;
            for (; k < 4; k++) {
                if (ingredients_status[k] == AVAILABLE) {
                    toss_index = k;
                }
            }
            if (toss_index == -1) {
                k = 0;
                for (; k < 4; k++) {
                    if (ingredients_status[k] != COOKING) {
                        toss_index = k;
                    }
                }                
            }
            if (toss_index != -1) {
                toss_message_buffer = toss_index;
                toss_message_ready = 1;
            }
        }
        
       
	}
	TerminateTask();
}

char find_ingredient(char ingredient) {
    found_in_index = -1;
    l = 0;
    
    for (; l < 4; ++l) {
        if (ingredient == ingredients[l] && ingredients_status[l] != COOKING) {
            found_in_index = l;
            break;
        }
    }
    return found_in_index;
}

void mark_needed(char ingredient) {
    l = 0;
    
    for (; l < 4; ++l) {
        if (ingredient == ingredients[l] && ingredients_status[l] == AVAILABLE) {
            ingredients_status[l] = NEEDED;
            return;
        }
    }
    
}

/* End of File : new_ing_cus_task.c */