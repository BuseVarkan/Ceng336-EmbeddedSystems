/*
 * File: Main.c
 */

#include "Pragmas.h"
#include "the3.h"
#include "ADC.h"
#include "LCD.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

void __interrupt(high_priority) FNC();
void movePlayer(byte player, byte direction, bool ai);
void updateLCDCell(byte x, byte y);
byte getPlayerInCoordinates(byte x, byte y);


volatile char CONVERT=0;
bool is_active = false;
byte current_portb = 0;
byte last_portb = 0;
bool is_waiting_bounce = false;
byte selected_player = 0;
byte player_positions[4][2] = {{3, 2}, {3, 3}, {14, 2}, {14, 3}}; // p0, p1, p2, p3
byte frisbee_position[2] = {9, 2};
byte target_position[2] = {0, 0};
byte timer_1_counter = 0;
byte speed = 4;
unsigned short number_of_steps = 0;
byte frisbee_current_step = 0;
byte score_A=0;
byte score_B=0;
byte counter_for_timer2 = 0;

bool THROW_FRISBEE = false;


void moveEverything() {
    unsigned short convertion = 0;
    byte frisbee_old_x = frisbee_position[0];
    byte frisbee_old_y = frisbee_position[1];
    byte winner_player;
    
    frisbee_position[0] = frisbee_steps[frisbee_current_step][0];
    frisbee_position[1] = frisbee_steps[frisbee_current_step][1];
    
    frisbee_current_step++;
    
    updateLCDCell(frisbee_old_x, frisbee_old_y);
    updateLCDCell(frisbee_position[0], frisbee_position[1]);
    
    if (number_of_steps == frisbee_current_step) {
        is_active = false;
        frisbee_current_step = 0;
        updateLCDCell(target_position[0], target_position[1]);
        winner_player = getPlayerInCoordinates(target_position[0], target_position[1]);
        
        if (winner_player == 0 || winner_player == 1) {
            score_A++;
        } else if (winner_player == 2 || winner_player == 3) {
            score_B++;
        }
        
        return;
    }
    
    for (int i = 0; i < 4; i++) {
        if (i == selected_player) continue;
        movePlayer(i, random_generator(9), true);
    }
    
    convertion = readADCChannel(0);
    if (convertion <= 255) {
        speed = 4;
    } else if (convertion <= 511) {
        speed = 8;
    } else if (convertion <= 767) {
        speed = 12;
    } else if (convertion <= 1023) {
        speed = 16;
    }  
}


void sevenSegmentDisplay(byte score, byte part) { // part0 A, part1 line, part2 B
    if(part==0) LATA = 0b00001000;
    else if(part==1){
        LATA = 0b00010000; 
        LATD = 0b1000000;
        return;
    }
    else LATA = 0b00100000;
    
    if(score==0) LATD = 0b0111111;
    else if(score==1) LATD = 0b0000110;
    else if(score==2) LATD = 0b1011011;
    else if(score==3) LATD = 0b1001111;
    else if(score==4) LATD = 0b1100110;
    else if(score==5) LATD = 0b1101101;
}


void startTimer0() {
    is_waiting_bounce = true;
    TMR0H = 0b11110100;
    TMR0L = 0b10001110;
    T0CON = 0b10010111;
}


void timer0Interrupt() {
    INTCONbits.TMR0IF = 0;
    is_waiting_bounce = false;
    TMR0H = 0b11110100;
    TMR0L = 0b10001110;
    T0CONbits.TMR0ON = 0;
}


void startTimer1() {
    TMR1H = 0b10000101;
    TMR1L = 0b11101110;
    T1CON = 0b11111101;
}


void timer1Interrupt() {
    PIR1bits.TMR1IF = 0;
    if (!is_active) return;
       
    TMR1H = 0b10000101;
    TMR1L = 0b11101110;

    timer_1_counter++;
    LCDGoto(target_position[0], target_position[1]);
    if (timer_1_counter & 1) {
        LCDDat(' ');
    } else {
        LCDDat(7);
    }
    if (timer_1_counter % speed == 0) {
        timer_1_counter = 0;
        moveEverything();
    }
    
}


void startTimer2() {
    PR2 = 0b11001111;
    T2CON = 0b01111111;
}

void timer2Interrupt() {
    INTCONbits.TMR0IF = 0;
    startTimer2();
    
    if(counter_for_timer2 == 0){
        sevenSegmentDisplay(score_A, 0);
        counter_for_timer2 ++ ;
    }
    else if(counter_for_timer2 == 1) {
        sevenSegmentDisplay(0, 1);
        counter_for_timer2 ++ ;
    }
        
    else {
        sevenSegmentDisplay(score_B, 2);
        counter_for_timer2 = 0;
    }
    
}


byte getPlayerInCoordinates(byte x, byte y) {
    byte i = 0;
    for (; i < 4; i++) {
        if (player_positions[i][0] == x && player_positions[i][1] == y) {
            return i;
        }
    }
    return i;
}

void updateLCDCell(byte x, byte y) {
    byte datum = 0;
    bool frisbee = (x == frisbee_position[0] && y == frisbee_position[1]);
    byte player = getPlayerInCoordinates(x, y);
    
    if (!frisbee && player == 4) {
        datum = ' ';
    } else if (frisbee && player == 4) {
        datum = 6;
    } else {
        if (player >= 2) {
            datum++;
        } 
        if (player == selected_player) {
            datum += 2;
        } 
        if (frisbee) {
            datum += 2;
            if (player != selected_player) {
                datum += 2;
            }
        }        
    }
    LCDGoto(x, y);
    LCDDat(datum);
    
}

void movePlayer(byte player, byte direction, bool ai) {
    byte old_x = player_positions[player][0];
    byte old_y = player_positions[player][1];
    byte new_x = old_x;
    byte new_y = old_y;
    
    if (direction == 1) {
        if (player_positions[player][1] == 1) return;
        new_y--; 
    } else if (direction == 2) {
        if (player_positions[player][0] == 16) return;
        new_x++;         
    } else if (direction == 3) {
        if (player_positions[player][1] == 4) return;
        new_y++;         
    } else if (direction == 4) {
        if (player_positions[player][0] == 1) return;
        new_x--;     
    } else if (direction == 5) {
        if (player_positions[player][1] == 1 || player_positions[player][0] == 16) return;
        new_y--;
        new_x++;
    } else if (direction == 6) {
        if (player_positions[player][1] == 4 || player_positions[player][0] == 16) return;
        new_y++;
        new_x++;     
    } else if (direction == 7) {
        if (player_positions[player][1] == 4 || player_positions[player][0] == 1) return;
        new_y++;
        new_x--;     
    } else if (direction == 8) {
        if (player_positions[player][1] == 1 || player_positions[player][0] == 1) return;
        new_y--;
        new_x--;     
    } else if (direction == 9) {
        return;
    }    
    
    if (ai && new_x == target_position[0] && new_y == target_position[1]) return;
    
    if (getPlayerInCoordinates(new_x, new_y) != 4) return;
    
    player_positions[player][0] = new_x;
    player_positions[player][1] = new_y;
    updateLCDCell(old_x, old_y);
    updateLCDCell(new_x, new_y);
}


void moveSelectedPlayer(byte direction) {
    movePlayer(selected_player, direction, false);
}

void portbInterrupt() {
    last_portb = current_portb;
    current_portb = PORTB;
    INTCONbits.RBIF = 0;
    
    if((current_portb & 1 << 4) && !(last_portb & 1 << 4)){
        moveSelectedPlayer(1);
    }
    if((current_portb & 1 << 5) && !(last_portb & 1 << 5)){
        moveSelectedPlayer(2);
    }
    if((current_portb & 1 << 6) && !(last_portb & 1 << 6)){
        moveSelectedPlayer(3);
    }
    if((current_portb & 1 << 7) && !(last_portb & 1 << 7)){
        moveSelectedPlayer(4);
    }
}


void RB0Interrupt() {
    INTCONbits.INT0IF = 0;
    if (is_waiting_bounce || is_active) return;

    if (player_positions[selected_player][0] == frisbee_position[0] && player_positions[selected_player][1] == frisbee_position[1]) {
    
        number_of_steps = compute_frisbee_target_and_route(frisbee_position[0], frisbee_position[1]);
        target_position[0] = frisbee_steps[number_of_steps-1][0];
        target_position[1] = frisbee_steps[number_of_steps-1][1];

        LCDGoto(target_position[0], target_position[1]);
        LCDDat(7);
    
        is_active = true;
        moveEverything();
        startTimer1();
    }    
    startTimer0();
}


void RB1Interrupt() {
    INTCON3bits.INT1IF = 0;
    if (is_waiting_bounce) return;
    
    byte old_selected_player = selected_player;    
    selected_player++;
    if (selected_player == 4) selected_player = 0;
    
    updateLCDCell(player_positions[old_selected_player][0], player_positions[old_selected_player][1]);
    updateLCDCell(player_positions[selected_player][0], player_positions[selected_player][1]);
    
    startTimer0();
    
}

void __interrupt(high_priority) FNC()
{
    if(INTCONbits.INT0IF)
    {
        RB0Interrupt();
    }
    else if(INTCON3bits.INT1IF)
    {
        RB1Interrupt();
    }   
    else if(INTCONbits.RBIF)
    {
        portbInterrupt();
    }
    else if(INTCONbits.TMR0IF)
    {
        timer0Interrupt();
    }
    else if(PIR1bits.TMR1IF)
    {
        timer1Interrupt();
    }
    else if(PIR1bits.TMR2IF)
    {
        timer2Interrupt();
    }

    
}


void initialGameConfig() {
    LCDAddSpecialCharacter(0, teamA_player);
    LCDAddSpecialCharacter(1, teamB_player);
    LCDAddSpecialCharacter(2, selected_teamA_player);
    LCDAddSpecialCharacter(3, selected_teamB_player);
    LCDAddSpecialCharacter(4, selected_teamA_player_with_frisbee);
    LCDAddSpecialCharacter(5, selected_teamB_player_with_frisbee);
    LCDAddSpecialCharacter(6, frisbee);
    LCDAddSpecialCharacter(7, frisbee_target);
    
    
    LCDGoto(3, 3);
    LCDDat(0);
    LCDGoto(14, 2);
    LCDDat(1);
    LCDGoto(14, 3);
    LCDDat(1);  
    LCDGoto(3, 2);
    LCDDat(2);
    LCDGoto(9, 2);
    LCDDat(6);
    
}

void initInterrupts(){
    current_portb = PORTB;
    
    INTCON = 0b11111000;
    INTCON3bits.INT1IE = 1;
    PIE1bits.TMR1IE = 1;  
    PIE1bits.TMR2IE = 1;  
}

void initTimer3() {
    T3CON = 0b10000001;
}

void main(void) { 
    
    initADC();
    
    InitLCD();
    
    initialGameConfig();
    
    initInterrupts();
    
    initTimer3();
    
    startTimer2();
            
    TRISB = 255;
    TRISA = 0;
    
    while(1){}
     
    return;
}