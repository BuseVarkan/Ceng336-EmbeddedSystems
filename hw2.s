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
GLOBAL state
GLOBAL display_1
GLOBAL display_2
GLOBAL display_3
GLOBAL display_4
GLOBAL speed
GLOBAL bar_length
GLOBAL current_beat
GLOBAL w_tmp
GLOBAL s_tmp
GLOBAL last_portb 
GLOBAL current_portb 
GLOBAL display_select    
GLOBAL value_to_write
    

; Define space for the variables in RAM
PSECT udata_acs
state:
    DS 1 
display_1:
    DS 1 
display_2:
    DS 1 
display_3:
    DS 1 
display_4:
    DS 1 
speed:
    DS 1 
bar_length:
    DS 1 
current_beat:
    DS 1 
w_tmp:
    DS 1 
s_tmp:
    DS 1 
last_portb:
    DS 1 
current_portb:
    DS 1 
display_select:
    DS 1 
value_to_write:
    DS 1 


PSECT CODE
org 0x0000
    goto main


org 0x0008
interrupt_service_routine:
    btfsc INTCON, INTCON_TMR0IF_POSITION
    bra timer0_interrupt
    
    btfsc PIR1, PIR1_TMR1IF_POSITION
    bra timer1_interrupt
    
    btfsc PIR2, PIR2_TMR3IF_POSITION
    bra timer3_interrupt
    
    btfsc INTCON, INTCON_RBIF_POSITION
    bra portb_interrupt

    retfie  1                 ; No match

; Jump tables for initializing timer0 and converting number to 7 segment    

start_timer0:
    rlncf WREG, 1, 0
    rlncf WREG, 1, 0
    addwf PCL
    nop
    nop
    goto ms1000
    goto ms900
    goto ms800
    goto ms700
    goto ms600
    goto ms500
    goto ms400
    goto ms300
    goto ms200
    return
    
get_7_segment_value:
    rlncf WREG, 1, 0
    rlncf WREG, 1, 0
    addwf PCL
    nop
    nop
    goto num1
    goto num2
    goto num3
    goto num4
    goto num5
    goto num6
    goto num7
    goto num8
    goto num9    
    return
    
; Selecting display and writing the 7 segment value
multiplexer:
    rlncf WREG, 1, 0
    rlncf WREG, 1, 0
    addwf PCL
    nop
    nop
    goto mult1
    goto mult2
    goto mult3
    goto mult4
    return
    
    
main:
    clrf state
    clrf display_1
    clrf display_2
    clrf display_3
    clrf display_4
    clrf last_portb
    clrf current_portb
    clrf display_select
    clrf T0CON
    clrf T1CON
    clrf T3CON
    
    movlw 6
    movwf speed
    
    movlw 4
    movwf bar_length

    movff bar_length, current_beat
    
    clrf TRISA
    clrf TRISD
    clrf TRISC
    setf TRISB
    
    movf PORTB  
    nop
    
    ; enable interrupts
    movlw 0b11101000
    movwf INTCON
    
    movlw 0b00000001 
    movwf PIE1
   
    movlw 0b00000010 
    movwf PIE2
    
    call pause
    
    call initialize_timer3
    call refresh_display
    movlw 10110001B
    movwf T3CON
    
    bra main_loop     
  
    ; every task is completed in interrupt service routines so main loop is empty    
main_loop:
    goto main_loop

        
pause:
    clrf state
    clrf display_2
    
    movlw 0b1110011
    movwf display_1
    
    clrf display_3
    
    movf speed, W
    call get_7_segment_value
    movwf display_4
    
    bcf T0CON, 7
    
    return
    
start_metronome:
    movff bar_length, current_beat
    
    clrf display_1

    movf bar_length, W
    call get_7_segment_value
    movwf display_4
    
    movlw 0b1000000
    movwf display_3
    
    call beat_once
    movf speed, W
    call start_timer0
    
    return

    ; increase current beat, start timer1 and change display values    
beat_once:    
    movf current_beat, W
    subwf bar_length, W
    tstfsz WREG
    bnz not_exceed_bar
	clrf current_beat
	bsf LATC, 1

    not_exceed_bar:   
    
    incf current_beat
    bsf LATC, 0
    call start_timer1

    movf current_beat, W
    call get_7_segment_value
    movwf display_2

    return
    
timer0_interrupt:
    bcf INTCON, INTCON_TMR0IF_POSITION
    movff STATUS, s_tmp
    movwf w_tmp
   
    call beat_once
    movf speed, W
    call start_timer0
    
    movf w_tmp, W
    movff s_tmp, STATUS       
    retfie

    
timer1_interrupt:
    bcf PIR1, PIR1_TMR1IF_POSITION
    movff STATUS, s_tmp
    movwf w_tmp
        
    movlw 11110001B
    movwf T1CON

    ; turn off leds
    bcf LATC, 0
    bcf LATC, 1    
    
    movf w_tmp, W
    movff s_tmp, STATUS       
    retfie
    
timer3_interrupt:
    bcf PIR2, PIR2_TMR3IF_POSITION
    movff STATUS, s_tmp
    movwf w_tmp
    
    call refresh_display
    
    ; write initial value
    movlw 0xFE
    movwf TMR3H
    movlw 0x0C
    movwf TMR3L
    
    movf w_tmp, W
    movff s_tmp, STATUS   
    retfie

portb_interrupt:
    movff PORTB, current_portb
    bcf INTCON, INTCON_RBIF_POSITION
    movff STATUS, s_tmp
    movwf w_tmp
    
    rb4:
	btfss last_portb, 4
	goto rb5
	btfsc current_portb, 4
	goto rb5

    rb4_pressed: 
	tstfsz state
	bra rb4_running
	
	setf state
	call start_metronome
	
	bra rb5
	rb4_running:
	    clrf state
	    call pause
    
    rb5:
	btfsc current_portb, 5
	goto rb6
	btfss last_portb, 5
	goto rb6

    rb5_pressed:    
	tstfsz state
	bra rb5_running
	
	movlw 9
	cpfseq speed
	incf speed	
	
	movf speed, W
	call get_7_segment_value
	movwf display_4
	
	bra rb6
	rb5_running:
	    movlw 8
	    cpfseq bar_length
	    incf bar_length	
	    
	    movf bar_length, W
	    call get_7_segment_value
	    movwf display_4
       
    rb6:
	btfsc current_portb, 6
	goto rb7
	btfss last_portb, 6
	goto rb7

    rb6_pressed:    
	tstfsz state
	bra rb6_running
	
	movlw 1
	cpfseq speed
	decf speed	
	
	movf speed, W
	call get_7_segment_value
	movwf display_4	
	
	bra rb7
	rb6_running:
	    movlw 2
	    cpfseq bar_length
	    decf bar_length
	    
	    movf bar_length, W
	    call get_7_segment_value
	    movwf display_4
        
    rb7:
	btfsc current_portb, 7
	goto check_buttons_finished
	btfss last_portb, 7
	goto check_buttons_finished

    rb7_pressed:    
	tstfsz state
	bra rb7_running
	movlw 6
	movwf speed
	
	movf speed, W
	call get_7_segment_value
	movwf display_4
	
	bra check_buttons_finished
	rb7_running:
	    movlw 4
	    movwf bar_length

	    movf bar_length, W
	    call get_7_segment_value
	    movwf display_4
    
    check_buttons_finished:
    
    movff current_portb, last_portb
    
    movf w_tmp, W
    movff s_tmp, STATUS   
    retfie

start_timer1:
    movlw 0xE7
    movwf TMR1H
    movlw 0x96
    movwf TMR1L
    movlw 11111001B
    movwf T1CON
    return

initialize_timer3:
    movlw 0xFE
    movwf TMR3H
    movlw 0x0C
    movwf TMR3L
    return
    
refresh_display:   
    incf display_select
    
    movf display_select, W
    call multiplexer
    
    movwf LATA
    
    movff value_to_write, LATD    
    
    movlw 4
    cpfslt display_select
    clrf display_select
    return
    
    
ms1000:
    movlw 0xC2
    movwf TMR0H
    movlw 0xF7
    movwf TMR0L
    movlw 10010101B
    movwf T0CON
    return
ms900:
    movlw 0x92
    movwf TMR0H
    movlw 0x23
    movwf TMR0L
    movlw 10010100B
    movwf T0CON
    return
ms800:
    movlw 0xF3
    movwf TMR0H
    movlw 0xCB
    movwf TMR0L
    movlw 10010111B
    movwf T0CON
    return
ms700:
    movlw 0xAA
    movwf TMR0H
    movlw 0x8D
    movwf TMR0L
    movlw 10010100B
    movwf T0CON
    return
ms600:
    movlw 0xDB
    movwf TMR0H
    movlw 0x61
    movwf TMR0L
    movlw 10010101B
    movwf T0CON
    return
ms500:
    movlw 0xC2
    movwf TMR0H
    movlw 0xF7
    movwf TMR0L
    movlw 10010100B
    movwf T0CON
    return
ms400:
    movlw 0xF3
    movwf TMR0H
    movlw 0xCB
    movwf TMR0L
    movlw 10010110B
    movwf T0CON
    return
ms300:
    movlw 0xDB
    movwf TMR0H
    movlw 0x61
    movwf TMR0L
    movlw 10010100B
    movwf T0CON
    return
ms200:
    movlw 0xF3
    movwf TMR0H
    movlw 0xCB
    movwf TMR0L
    movlw 10010101B
    movwf T0CON
    return

    
num1:
    movlw 0b0000110
    return
num2:
    movlw 0b1011011
    return 
num3:
    movlw 0b1001111
    return
num4: 
    movlw 0b1100110
    return
num5:    
    movlw 0b1101101
    return    
num6:    
    movlw 0b1111101
    return    
num7:    
    movlw 0b0000111
    return    
num8:    
    movlw 0b1111111
    return    
num9:  
    movlw 0b1101111
    return

    
mult1:    
    movff display_1, value_to_write
    movlw 0b0001
    return    
mult2:
    movff display_2, value_to_write
    movlw 0b0010
    return    
mult3:    
    movff display_3, value_to_write
    movlw 0b0100
    return    
mult4:    
    movff display_4, value_to_write
    movlw 0b1000
    return    
    
    