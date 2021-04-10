#include <stdint.h>
#include <stdio.h>
#include <msp430.h>
/*
 * main.c
 */
unsigned int positionData;
unsigned int positionDataOld;
unsigned int voltage;
char str[10];
unsigned int Wheel_getValue(void);
void UartWriteNum(unsigned int x);			//按高八位、第八位传送16位数
void UartWriteStr(char *write);				//传送字符串

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;				// Stop watchdog timer

    //测试AD采集频率
    P7DIR |= BIT0 + BIT1;
    P7OUT &= ~(BIT0 + BIT1);

    // Configure Wheel ports
    P8DIR |= BIT0;							// Enable wheel
    P8OUT |= BIT0;

    //初始化AD
    ADC12CTL0 = ADC12SHT02 + ADC12ON;		// Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP;					// Use sampling timer
    ADC12MCTL0 = ADC12INCH_5;				// Use A5 (wheel) as input
    ADC12CTL0 |= ADC12ENC;					// Enable conversions
    P6DIR &= ~BIT5;							// A5 ADC input
    P6SEL |= BIT5;							// P6.5 ADC option select (A5)

    //初始化Timer
    TA0CCTL0 = CCIE;						// CCR0 interrupt enabled
    TA0CCR0 = 436;							// 中断频率75Hz（即AD采样频率）
    TA0CTL = TASSEL_1 + MC_1 + TACLR;		// ACLK, upmode, clear TAR

    //初始化UART
    P4SEL |= BIT4+BIT5;						// P4.4,5 = USCI_A1 TXD/RXD
    UCA1CTL1 |= UCSWRST;					// **Put state machine in reset**
    UCA1CTL1 |= UCSSEL_1;					// CLK = ACLK
    UCA1BR0 = 0x03;							// 32kHz/9600=3.41 (see User's Guide)
    UCA1BR1 = 0x00;
    UCA1MCTL = UCBRS_3+UCBRF_0;				// Modulation UCBRSx=3, UCBRFx=0
    UCA1CTL1 &= ~UCSWRST;					// **Initialize USCI state machine**

    __bis_SR_register(GIE);					// LPM0, ADC12_ISR will force exit
    // 主循环
    while (1);

}

// Timer0 A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
	P7OUT ^= BIT0;
    voltage = Wheel_getValue()*3.3*1000/4096 + 0.5;		//通过齿轮电位计采样得到需设置的对比度值
    sprintf(str,"%dmV \0",voltage);
    UartWriteStr(str);
    //UartWriteNum(voltage);
}

unsigned int Wheel_getValue(void)
{
    //measure ADC value
    ADC12IE = BIT0;                                    // Enable interrupt
    ADC12CTL0 |= ADC12SC;                              // Start sampling/conversion
    __bis_SR_register(LPM0_bits + GIE);                // LPM0, ADC12_ISR will force exit
    ADC12IE = 0x00;                                    // Disable interrupt

    //remove fluctuations(简单的平滑滤波，可以删去)
    if (positionData > positionDataOld)
        if ((positionData - positionDataOld) > 10)
            positionDataOld = positionData;				//use new data if change is beyond
														// fluctuation threshold
        else
            positionData = positionDataOld;				//use old data if change is not beyond
														// fluctuation threshold
    else
		if ((positionDataOld - positionData) > 10)
			positionDataOld = positionData;				//use new data if change is beyond
														// fluctuation threshold
		else
			positionData = positionDataOld;				//use old data if change is not beyond
														// fluctuation threshold

    return positionData;
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch (__even_in_range(ADC12IV, ADC12IV_ADC12IFG15))
    {
        // Vector  ADC12IV_ADC12IFG0: ADC12IFG0:
        case  ADC12IV_ADC12IFG0:
        	P7OUT ^= BIT1;								//测试AD采集频率
            positionData = ADC12MEM0;					// ADC12MEM = A0 > 0.5AVcc?
            __bic_SR_register_on_exit(LPM0_bits);		// Exit active CPU
            break;

        default:
            break;
    }
}

void UartWriteNum(unsigned int x)  //传送 高八位、低八位
{

        while (!(UCA1IFG&UCTXIFG));// USCI_A0 TX buffer ready?
        UCA1TXBUF = x>>8;     // TX -> RXed character

        while (!(UCA1IFG&UCTXIFG));// USCI_A0 TX buffer ready?
        UCA1TXBUF = x & 0xFF;     // TX -> RXed character
}

void UartWriteStr(char *write)
{
    while(*write!='\0')
    {
        while (!(UCA1IFG&UCTXIFG));// USCI_A0 TX buffer ready?
        UCA1TXBUF = *write;     // TX -> RXed character
        write++;
    }

}
