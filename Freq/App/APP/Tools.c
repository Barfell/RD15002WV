#include  <includes.h>

void hextostr(unsigned char *str ,unsigned char hex)
{
	if( (hex / 16) > 9)
		{*str = (hex / 16)+'0'+ 7;}
	else
		{*str = (hex / 16)+'0';}

	if((hex%16) > 9)
		{*(str+1) = (hex%16)+'0' + 7;}
	else
		{*(str+1) = (hex%16)+'0';}
}


unsigned int SoftGenCrc32(unsigned int* puData, unsigned int uSize) 
{ 
	unsigned int crc = 0xFFFFFFFF;
	unsigned int i=0,j=0,temp; 
	for(i=0;i<uSize;i++)
	{ 
		temp = puData[i]; 
		for(j=0;j<32;j++)
		{ 
			if( (crc ^ temp) & 0x80000000 )
			{ 
				crc = 0x04C11DB7 ^ (crc<<1); 
			}
			else
			{ 
				crc <<=1; 
			} 
			temp<<=1; 
		} 
	}
	
	return crc; 
} 
unsigned int HardGenCrc32(unsigned int* puData, unsigned int uSize)
{
	CRC_ResetDR();
	return CRC_CalcBlockCRC((unsigned int *)puData, (unsigned int)uSize);
}



void usart_update_packet(unsigned char *p,unsigned int length)
{
	while(length != 0)
	{
		//while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(USART3, *(p++) );
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		length--;
	}
}


void HandleDataPackage(unsigned char *pBuf)
{
			unsigned char pData[600]={'\0'};
			unsigned char szData[700]={'\0'};
			unsigned int u32Length = 0;
			unsigned int u32TotalPackageNumber = 0xfefefefe;
			unsigned int u32CurrentPackageNumber = 0xaaaaaaaa;
			unsigned int u32CrcValue=0x55555555;
			unsigned short  u16Loop = 0;
			while(*pBuf != '\0')
				{pData[u16Loop++] = *(pBuf++);}
			u32Length = u16Loop + 19;
			u32CrcValue = SoftGenCrc32((unsigned int*)pData,u16Loop);
			//printf("%d\r\n",u32CrcValue);


			szData[0] = START_SYMBOL;					//FE
			szData[1] = PROTOCOL_EDITION;				//01
			szData[2] = HIBYTE(HIWORD(u32Length));		
			szData[3] = LOBYTE(HIWORD(u32Length));		
			szData[4] = HIBYTE(LOWORD(u32Length));		
			szData[5] = LOBYTE(LOWORD(u32Length));		

			szData[6] = DATA_DIRECTION;					//01
			szData[7] = MAINCMD_INTERNET;				//06
				
			szData[8] = HIBYTE(SUBCMD_SENSOR);			//00
			szData[9] = LOBYTE(SUBCMD_SENSOR);			//01
			szData[10] = NENCRYPTION_NCOMPRESSION;		//00
			
			szData[11] = HIBYTE(HIWORD(u32TotalPackageNumber));//00
			szData[12] = LOBYTE(HIWORD(u32TotalPackageNumber));//00
			szData[13] = HIBYTE(LOWORD(u32TotalPackageNumber));//00
			szData[14] = LOBYTE(LOWORD(u32TotalPackageNumber));//00
			
			szData[15] = HIBYTE(HIWORD(u32CurrentPackageNumber));//00
			szData[16] = LOBYTE(HIWORD(u32CurrentPackageNumber));//00
			szData[17] = HIBYTE(LOWORD(u32CurrentPackageNumber));//00
			szData[18] = LOBYTE(LOWORD(u32CurrentPackageNumber));//00
			
			strcat(szData + 19, pData);
			
			szData[19 + u16Loop] = HIBYTE(HIWORD(u32CrcValue));//CRC
			szData[20 + u16Loop] = LOBYTE(HIWORD(u32CrcValue));
			szData[21 + u16Loop] = HIBYTE(LOWORD(u32CrcValue));
			szData[22 + u16Loop] = LOBYTE(LOWORD(u32CrcValue));
			szData[23 + u16Loop] = FIRST_END_SYMBOL;			//0D
			szData[24 + u16Loop] = SECOND_END_SYMBOL;			//0A
			usart_update_packet(szData,u16Loop+25);
}

