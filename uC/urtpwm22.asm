
 errorlevel -306 ;supress warning, use plus to put back
 ERRORLEVEL -302 ;remove message about using proper bank


 LIST p=16F628          ;tell assembler what chip we are using
 include "P16F628.inc"  ;include the defaults for the chip
 
 __config b'11110100101011' ;20MHz external EC ;!MCLR reset

 cblock 0x20   ;start of general purpose registers 
   var1
   var2
   var3
   var4
   var5
   cnt1
 endc

 #define nbby d'2'

reset   org	0x00	;reset vector address	
	goto	main
	nop
	nop
	nop
	org	0x04	;interrupts go to this location 
	goto	irrpt 

main
	
   movlw  0x07
   movwf  CMCON			;turn comparators off (make it like a 16F84)


 bsf 	STATUS,RP0   ;switch from bank0 to bank1
 movlw  b'00100000'
 movwf  TRISB
 movlw  b'00000110'
 movwf  TRISB
 bcf    STATUS,RP0   ;switch from bank1 to bank0

 clrf INTCON
 bsf INTCON,GIE
 bsf INTCON,PEIE
 bsf 	STATUS,RP0
 clrf PIE1
 bsf PIE1,RCIE
 
 bcf TXSTA,TX9
 bcf TXSTA,SYNC
 bsf TXSTA,BRGH
 movlw d'125'
 movwf SPBRG
 bcf	STATUS,RP0

 bcf RCSTA,RX9
 bsf RCSTA,SPEN
 
 bsf RCSTA,CREN
 bsf 	STATUS,RP0
 bsf TXSTA,TXEN 
 bcf	STATUS,RP0

 movlw 0xFF
 movwf var2

wait
 movf var1,1
 decfsz var2,1
 goto wait

 movlw 3
 movwf cnt1
 movlw var1
 movwf FSR
 bcf STATUS,IRP


  ;-------------
 movlw  0xFF
 movwf  CCP1CON         ;setup PWM mode 

 movlw  0x1F
 movwf  CCPR1L          ;duty cycle

 bsf 	STATUS,RP0
 movlw  0x3F
 movwf  PR2             ;period
 bcf    STATUS,RP0
 movlw  0xFF
 movwf  T2CON           ;prescale, enable timer 2

  ;-------------


 call rreset

loop
  goto loop

rreset 
 movlw 4
 movwf cnt1
 movlw var1
 movwf FSR
 bcf STATUS,IRP
 return

irrpt
 bcf INTCON,GIE
 btfsc PIR1,RCIF
 goto  recv
 clrf  PIR1
 bsf INTCON,GIE
 retfie

recv

 bcf PIR1,RCIF
 movf  RCREG,0
 movwf INDF
 movwf TXREG
 movf  cnt1,0
 movwf TXREG
 
 incf FSR,1  

 decf cnt1,1
 btfsc STATUS,Z
 goto rcv2

rcvend
 bsf INTCON,GIE
 retfie


rcv2

 call rreset

 movf   var1,0
 bsf 	STATUS,RP0
 movwf  PR2             ;period
 bcf    STATUS,RP0 

 
 movf var2,0
 andlw 0x03     
 iorlw b'01111100'
 movwf T2CON

 movf var3,0            ;duty cycle
 movwf CCPR1L


 movlw b'00001111'
 movwf var5
 btfsc var4,0
 bsf   var5,CCP1Y
 btfss var4,0
 bcf   var5,CCP1Y
 btfsc var4,1
 bsf   var5,CCP1X
 btfss var4,1
 bcf   var5,CCP1X
 movf  var5,0
 movwf CCP1CON

 bsf INTCON,GIE
 retfie

 end