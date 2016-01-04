#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "flash.h"
#include "usart.h"	  
#include "sram.h"
#include "malloc.h"
#include "string.h"
#include "mmc_sd.h"
#include "ff.h"
#include "exfuns.h"
//#include "usmart.h"
#include "fontupd.h"
#include "text.h"
  
 int main(void)
 {	 

	u16 times,len = 0;
	u8 key;			  
	u8 Baud_index = 0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(100*Baud_Rate[0]);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始化
	LCD_Init();			  //初始化液晶 
	KEY_Init();	 		 //按键初始化
 	mem_init(SRAMIN);	//初始化内部内存池	
 	exfuns_init();					//为fatfs相关变量申请内存  
  	f_mount(0,fs[0]); 		 		//挂载SD卡 
 	f_mount(1,fs[1]); 				//挂载FLASH.
	key=KEY_Scan(0);				//按键扫描	  
	while(font_init()||key==KEY_LEFT) //检查字库
	{
   
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		while(SD_Initialize())		//检测SD卡
		{
			LCD_ShowString(60,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(60,70,200+60,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(60,70,200,16,16,"SD Card OK");
		LCD_ShowString(60,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,0);//从SD卡更新
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(60,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(60,110,200,16,16,"Font Update Success!");
		delay_ms(1500);	
		LCD_Clear(WHITE);//清屏	       
	}  	
		POINT_COLOR = BLACK;   
		Show_Str(50,10,170,16,"通信接收端-RX_Side",16,0);	
		POINT_COLOR = RED;   
		Show_Str(20,30,150,16,"波特率(BaudRate):",16,0);	
		Show_Str(20,50,150,16,"接收数据(RX_Data):",16,0);	
		POINT_COLOR = BLUE;  
		LCD_ShowNum(160,30,100*Baud_Rate[0],6,16);			
			  
 	while(1)
	{
		key=KEY_Scan(0);
		if(key == KEY_UP){ //WKUP_PRES means GPIOA.0 = '1';
			Baud_index++;
			if(Baud_index > 15){
				Baud_index = 1;
			}
			uart_init(100*Baud_Rate[Baud_index]);	
			LCD_ShowNum(160,30,100*Baud_Rate[Baud_index],6,16);			
		}
		if(key == KEY_DOWN){ //KEY2_PRES means GPIOE.3 = '0'; 
			Baud_index--;
			if(Baud_index < 1) { 
				Baud_index = 15;
			}
			uart_init(100*Baud_Rate[Baud_index]);
			LCD_ShowNum(160,30,100*Baud_Rate[Baud_index],6,16);			
		}		
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度，超过限制的长度会出bug 300	
			USART_RX_BUF[len] = '<';
			USART_RX_BUF[len+1] = '<';
			USART_RX_BUF[len+2] = ' ';
			USART_RX_BUF[len+3] =  0;
				
			LCD_ShowNum(170,50,len,4,16);
			LCD_Fill(19,69,236,311,WHITE);
			Show_Str(20,70,216,240,USART_RX_BUF,16,0);						
			USART_RX_STA=0;
		}

		if(times%30==0)LED1=!LED1;//闪烁LED,提示系统正在运行.
			delay_ms(10);   
	}	 
}


