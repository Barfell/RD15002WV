#include  <includes.h>

/***************************************************************/
//任务控制块
static  OS_TCB   AppTaskTASK1TCB;
static  OS_TCB   AppTaskTASK2TCB;
static  OS_TCB   AppTaskTASK3TCB;
//任务堆栈
static  CPU_STK  AppTaskTASK1Stk[APP_CFG_TASK_TASK1_STK_SIZE];
static  CPU_STK  AppTaskTASK2Stk[APP_CFG_TASK_TASK2_STK_SIZE];
static  CPU_STK  AppTaskTASK3Stk[APP_CFG_TASK_TASK3_STK_SIZE];

/***************************************************************/
extern unsigned char UploadFlag;//上传到IMS的标志
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//开始任务
void  AppTaskStart (void *p_arg)
{
	OS_ERR  err;
	CPU_Init();//初始化cpu，禁用中断，开启时间戳等
	BSP_Tick_Init();//设置滴答定时器
/*------------------------------*/		 
	OSTaskCreate((OS_TCB       *)&AppTaskTASK1TCB,              
                 (CPU_CHAR     *)"task1",
                 (OS_TASK_PTR   )task1, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_TASK1_PRIO,
                 (CPU_STK      *)&AppTaskTASK1Stk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK1_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK1_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
/*------------------------------*/
	OSTaskCreate((OS_TCB       *)&AppTaskTASK2TCB,              
                 (CPU_CHAR     *)"task2",
                 (OS_TASK_PTR   )task2, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_TASK2_PRIO,
                 (CPU_STK      *)&AppTaskTASK2Stk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK2_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK2_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);  
/*-------------------------------*/
	OSTaskCreate((OS_TCB       *)&AppTaskTASK3TCB,              
                 (CPU_CHAR     *)"task3",
                 (OS_TASK_PTR   )task3, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_TASK3_PRIO,
                 (CPU_STK      *)&AppTaskTASK3Stk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK3_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_TASK3_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
				 
	//创建好其他任务后就删除自己
    OSTaskDel((OS_TCB*)0,&err);
}




















///////////////////////////////////////////////////////////////////////////////////////////////////////////
//用户任务1
void task1(void)
{
	OS_ERR      err;
	while(1)
	{
		OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_DLY,&err);
		if(UploadFlag == 0)
		{
			SW_12V(0);//电源
			SW_5V(0);//正负电源，用于检测回波
			OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err);
			ConfigPINToListen();
			PWR_EnterSTOPMode(PWR_Regulator_ON,PWR_STOPEntry_WFI);
			SetClock();//配置各级CPU时钟
			bsp_io_init();//IO口
			SW_12V(1);//电源
			SW_5V(1);//正负电源，用于检测回波
			OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err);
			UsartConfig();//串口设置配置
		}
	}
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
//用户任务2 负责看门狗的喂狗
void task2(void)
{
	OS_ERR      err;
    while(1)
    {
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
    }
}

 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void task3(void)
{
	OS_ERR      err;
	unsigned char dev_ID[50]={'\0'};//器件ID
	unsigned char str_buff[600];//信息缓冲
	bsp_io_init();//IO口
	UsartConfig();//串口设置配置
	SW_12V(1);//电源
	SW_5V(1);//正负电源，用于检测回波Z
	FreqModuleInit();//测频率模块初始化
	LTC2402Init();
	GetFreq(1);
	read_dev_id(dev_ID);//读取ID

	while(1)
	{			
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
		if(UploadFlag == 1)//确认需要上传数据了
		{
			//printf("command confirmed!\r\n");
			FreqModuleInit();//测频率模块初始化
			LTC2402Init();
			GetFreq(1);
			read_dev_id(dev_ID);//读取ID			
sprintf(str_buff,"SCT200T15003-Time: %s\r\n\
SCT200T15003-ID: %s\r\n\
SCT200T15003-Mode: xxx minutes Internal\r\n\
SCT200T15003-Current voltage = xx.xxxV\r\n\
SCT200T15003-Channel: 1  Temperature :%f\r\n\
SCT200T15003-Channel: 2  Temperature :%f\r\n\
SCT200T15003-Channel: 3  Temperature :%f\r\n\
SCT200T15003-Channel: 4  Temperature :%f\r\n\
SCT200T15003-Channel: 1  FREQUENCY:%f\r\n\
SCT200T15003-Channel: 2  FREQUENCY:%f\r\n\
SCT200T15003-Channel: 3  FREQUENCY:%f\r\n\
SCT200T15003-Channel: 4  FREQUENCY:%f\r\n",
get_time(),dev_ID,\
GetNTCTemperature(LTC2402_GetResistance(1)),\
GetNTCTemperature(LTC2402_GetResistance(2)),\
GetNTCTemperature(LTC2402_GetResistance(3)),\
GetNTCTemperature(LTC2402_GetResistance(4)),\
GetFreq(5),GetFreq(6),GetFreq(7),GetFreq(8));
			HandleDataPackage(str_buff);//打包通过串口发送
			

			UploadFlag = 0;
			SW_12V(0);//电源
			SW_5V(0);//正负电源，用于检测回波
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_DLY,&err);
			ConfigPINToListen();
			PWR_EnterSTOPMode(PWR_Regulator_ON,PWR_STOPEntry_WFI);
			SetClock();//配置各级CPU时钟
			bsp_io_init();//IO口
			SW_12V(1);//电源
			SW_5V(1);//正负电源，用于检测回波
			//OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_DLY,&err);
			UsartConfig();//串口设置配置
			FreqModuleInit();//测频率模块初始化
			LTC2402Init();
			GetFreq(1);
			read_dev_id(dev_ID);//读取ID
		}
	}
}



