PROCESSOR    18F4620

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
CONFIG OSC = HSPLL      ; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF      ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF       ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = ON        ; Power-up Timer Enable bit (PWRT enabled)
CONFIG BOREN = OFF      ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
CONFIG BORV = 3         ; Brown Out Reset Voltage bits (Minimum setting)
; CONFIG2H
CONFIG WDT = OFF        ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG PBADEN = OFF     ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
CONFIG LPT1OSC = OFF    ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON       ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF        ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF      ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; GLOBAL SYMBOLS
; You need to add your variables here if you want to debug them.
GLOBAL var1
GLOBAL var2
GLOBAL var3
GLOBAL result
GLOBAL counter_4
GLOBAL counter_5
GLOBAL counter_6
GLOBAL temp1
GLOBAL temp2
GLOBAL flag
GLOBAL ra_value
GLOBAL last_b
GLOBAL curr_b
GLOBAL flag_speed
GLOBAL last_b_speed
GLOBAL curr_b_speed
GLOBAL curr_b_reset
GLOBAL last_b_reset
GLOBAL bar_length
    
; Define space for the variables in RAM
PSECT udata_acs
var1:
    DS 1
var2:
    DS 1 
var3:
    DS 1
result: 
    DS 1
counter_4:
    DS 1
counter_5:
    DS 1
counter_6:
    DS 1
temp1:
    DS 1
temp2:
    DS 1
flag:
    DS 1
ra_value:
    DS 1
last_b:
    DS 1
curr_b:
    DS 1
flag_speed:
    DS 1
last_b_speed:
    DS 1
curr_b_speed:
    DS 1
curr_b_reset:
    DS 1
last_b_reset:
    DS 1
bar_length:
    DS 1

PSECT resetVec,class=CODE,reloc=2
resetVec:
    goto       main

PSECT CODE


 
main:  
 
    clrf var1 ; var1 = 0
    clrf var2
    clrf var3
    clrf flag ; to check if paused
    clrf flag_speed ; to check the speed
    clrf ra_value
    clrf last_b
    clrf curr_b
    clrf last_b_speed
    clrf curr_b_speed
    clrf curr_b_reset
    clrf last_b_reset
    movlw 8
    movwf bar_length
   clrf var3
  clrf PORTA ;
  clrf LATA;
  clrf TRISA
  setf TRISB
  
  
  movlw 0b111
  movwf LATA
  
  
  call waste_time_1s
  clrf LATA
  
  movlw 0b011
  movwf LATA
  
  
  main_loop:
    call check_button
    call check_speed
    call check_bar_length
    call update_metronome
  goto main_loop
  
  
check_speed:
    movff curr_b_speed, last_b_speed
    movff PORTB, curr_b_speed
    btfsc last_b_speed, 1 ; check RB1
    call rb1_pressed ; we will execute this if RB1 = 1
    ;if it is not loop
    return
rb1_pressed:
    btfss curr_b_speed, 1
    call rb1_released
    return
rb1_released:
    btg flag_speed, 0 ; flag_speed = 0 when slow, 1 when fast
    ;call update_metronome
   ; setf flag_speed
    return
  
check_bar_length:
    movff curr_b_reset, last_b_reset
    movff PORTB, curr_b_reset
    btfsc last_b_reset, 2 ; check RB2
    call rb2_pressed ; we will execute this if RB2 = 1
    btfsc last_b_reset, 3 ; check RB3
    call rb3_pressed
    btfsc last_b_reset, 4 ; check RB4
    call rb4_pressed
    ;if it is not loop
    return
rb2_pressed:
    btfss curr_b_reset, 2
    call rb2_released
    return
rb2_released:
    movlw 8
    movwf bar_length
    return
  
rb3_pressed:
    btfss curr_b_reset, 3
    call rb3_released
    return
rb3_released:
    decf bar_length
    decf bar_length
    return
    
rb4_pressed:
    btfss curr_b_reset, 4
    call rb4_released
    return
rb4_released:
    incf bar_length
    incf bar_length
    return
    
    
update_metronome:
    btfsc flag, 0
    return
    movlw 4
    btfsc flag_speed, 0
    movlw 8
    addwf var1 ; var1 += .
    bc var1_overflown ; if var1 is overflown
    return ; otherwise return
var1_overflown:
    movlw 2
    addwf var2
    ;incf var2
    bc var2_overflown
    return
var2_overflown:
    
    btg LATA,0

    bcf LATA, 1
    
    
    movf bar_length, W
    incf var3
    
    cpfslt var3
    call var3_overflown
    return
var3_overflown:
    bsf LATA, 1
    clrf var3
    return
    
    
check_button:
    btfss flag, 0
    call check_pause
    btfsc flag, 0
    call check_resume
    return
  
check_pause:
    movff curr_b, last_b
    movff PORTB, curr_b
    btfsc last_b, 0 ; check RB0
    call rb0_pressed ; we will execute this if RB0 = 1
    ;if it is not loop
    return
rb0_pressed:
    btfss curr_b, 0
    call rb0_released
    return
rb0_released:
    movff LATA, ra_value ; AYARLA
    movlw 0b100
    movwf LATA
    setf flag ; flag = 1 when paused
    return
    
check_resume:
    movff curr_b, last_b
    movff PORTB, curr_b
    btfsc last_b, 0
    call rb0_pressed_for_resume ; we will execute this if RB0 = 1
    ;if it is not loop
    return
rb0_pressed_for_resume:
    btfss curr_b, 0
    call rb0_released_resume
    return
rb0_released_resume:
    movff ra_value, LATA ; ayarla bunu
    clrf flag ; flag = 0 when resumed
    return
    
  
 waste_time_1s:
    movlw 5
    
    movwf counter_4
    do_while_counter_4_not_zero:
        do_while_counter_5_not_zero:
            do_while_counter_6_not_zero:
                decf counter_6
                bnz do_while_counter_6_not_zero
            end_do_while_counter_6_not_zero:
            decf counter_5
            bnz do_while_counter_5_not_zero
        end_do_while_counter_5_not_zero:
        decf counter_4
        bnz do_while_counter_4_not_zero
    end_do_while_counter_4_not_zero:
    return
    


end resetVec
