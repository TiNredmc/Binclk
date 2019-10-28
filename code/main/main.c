#include <stdint.h>
#include <stm8l.h>
#include <delay.h>
#include <rtc.h>

RTC_InitTypeDef   RTC_InitStr;
RTC_TimeTypeDef   RTC_TimeStr;
RTC_DateTypeDef   RTC_DateStr;

/* init the RTC */
void Calendar_Init(void)// RTC stuffs that I'm not yet understand how to set the time 
{
  RTC_InitStr.RTC_HourFormat = RTC_HourFormat_24;
  RTC_InitStr.RTC_AsynchPrediv = 0x7F;
  RTC_InitStr.RTC_SynchPrediv = 0x00FF;
  RTC_Init(&RTC_InitStr);

  RTC_DateStructInit(&RTC_DateStr);
  RTC_DateStr.RTC_WeekDay = RTC_Weekday_Sunday;
  RTC_DateStr.RTC_Date = 18;
  RTC_DateStr.RTC_Month = RTC_Month_August;
  RTC_DateStr.RTC_Year = 19;
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);

  RTC_TimeStructInit(&RTC_TimeStr);
  RTC_TimeStr.RTC_Hours   = 0;
  RTC_TimeStr.RTC_Minutes = 0;
  RTC_TimeStr.RTC_Seconds = 0;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
	if (RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr) != SUCCESS){
	PA_ODR ^= (1 << 2) | (1 << 3) ;
	delay_ms(1000);
	}
}

/* init the LSI oscillator and set as RTC  default clock */
void initLSI(){
CLK_ICKCR=(1 << CLK_ICKCR_LSION);
while (!(CLK_ICKCR & (1 << CLK_ICKCR_LSIRDY)));  // enable LSI oscillator.  

delay_ms(1000); // wait for the LSI clock to stable 	

CLK_CRTCR = ( ((uint8_t)0x04) | ((uint8_t)0x00) ); //set the RTC clock to LSI clock and set the divider at 1 
CLK_PCKENR2 |= (uint8_t)((uint8_t)1 << ((uint8_t)0x12 & (uint8_t)0x0F));// enable rtc clock

}


/* init the PA and PB */
void initGPIO(){
    for (int i = 0; i < 8; i ++){
    PB_DDR |= (1 << i);
    PB_CR1 |= (1 << i);
  }

    for (int i = 2; i < 4; i ++){
    PA_DDR |= (1 << i);
    PA_CR1 |= (1 << i);
}
}

void displayClear(){
       for (int i = 0; i < 8; i ++){        
       PB_ODR = (0 << i); // set individual bit of PortB to 0 
    }
	 for (int i = 2; i < 4; i ++){        
       PA_ODR = (0 << i);// set individual bit of PortA to 0
    }
}

void displayMH(int hm){ // Show what's currently displaying on the display, 1 refer to Hour. 0 refer to Minute. LED connected to PA2, PA3 indicate minute or hour 
if (hm == 0 ) {
	PA_ODR ^= (1 << 2); // LED on PA2 on mean the display currently displaying minute both tens and units digit
	PA_ODR ^= (0 << 3);
}else if (hm == 1){
	PA_ODR ^= (1 << 3);// LED on PA3 on mean the display currently displaying hour both tens and units digit
	PA_ODR ^= (0 << 2);
}

}

void Time_Show(int mh){
while (RTC_WaitForSynchro() != SUCCESS);
displayClear();
RTC_GetTime(RTC_Format_BCD, &RTC_TimeStr); 
  if(mh == 0){  
	displayMH(0);
       PB_ODR ^= (uint8_t)RTC_TimeStr.RTC_Minutes & 0xF0  << 4 | (uint8_t)RTC_TimeStr.RTC_Minutes & 0x0F ;// Display the minute in BCD format onto the display
       //PB_ODR ^= (uint8_t)RTC_TimeStr.RTC_Minutes & 0x0F;
}else if (mh == 1){
	displayMH(1);
       PB_ODR ^= (uint8_t)RTC_TimeStr.RTC_Hours & 0xF0  << 4 | (uint8_t)RTC_TimeStr.RTC_Hours & 0x0F ;// Display the hour in BCD format onto the display
       //PB_ODR ^= (uint8_t)RTC_TimeStr.RTC_Hours & 0x0F;
}
//displayNum( ((uint8_t)RTC_TimeStr.RTC_Hours & 0xF0) , ((uint8_t)RTC_TimeStr.RTC_Hours & 0xF0));

}

void main() {
	initLSI(); // init the LSI clock
	initGPIO(); // Init the GPIOs both PA and PB	
	Calendar_Init();
    while (1) {
	Time_Show(1);
	delay_ms(500);
	Time_Show(0);
	delay_ms(1000);// can't fix the delay yet 
	displayClear();
}
}


