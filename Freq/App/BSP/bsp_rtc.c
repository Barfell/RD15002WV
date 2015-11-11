#include <includes.h>



char* get_time(void)
{  
	char time[100]={'\0'};
	RTC_TimeTypeDef   	RTC_GetTimeStructure;
	RTC_DateTypeDef 	RTC_GetDateStructure;
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)//假如修改了，则再次进行配置
		{RTC_Config();}
	else
		{//没有修改，则直接继续
//			if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
//				{printf("Power On Reset occurred\r\n");}
//			else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
//				{printf("External Reset occurred\r\n");}
			RTC_GetTime(RTC_Format_BIN, &RTC_GetTimeStructure);
			RTC_GetDate(RTC_Format_BIN, &RTC_GetDateStructure);
			
			sprintf(time,"%d/%d/%d %d:%d:%d",\
			RTC_GetDateStructure.RTC_Year,RTC_GetDateStructure.RTC_Month,\
			RTC_GetDateStructure.RTC_Date,RTC_GetTimeStructure.RTC_Hours,\
			RTC_GetTimeStructure.RTC_Minutes,RTC_GetTimeStructure.RTC_Seconds);
		}
		return time;
}




void RTC_Config(void)
{
	
	RTC_InitTypeDef RTC_InitStructure;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
  
  /* Configure the RTC data register and RTC prescaler */
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv = 0xFF;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);
  
 
  RTC_DateStructure.RTC_Year = 0x15;
  RTC_DateStructure.RTC_Month = RTC_Month_November;
  RTC_DateStructure.RTC_Date = 0x04;
  RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Wednesday;
  RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
  RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
  RTC_TimeStructure.RTC_Hours   = 0x16;
  RTC_TimeStructure.RTC_Minutes = 0x28;
  RTC_TimeStructure.RTC_Seconds = 0x00; 
  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
  /* Indicator for the RTC configuration */
  RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
}
