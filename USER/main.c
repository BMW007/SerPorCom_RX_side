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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(100*Baud_Rate[0]);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			  //��ʼ��Һ�� 
	KEY_Init();	 		 //������ʼ��
 	mem_init(SRAMIN);	//��ʼ���ڲ��ڴ��	
 	exfuns_init();					//Ϊfatfs��ر��������ڴ�  
  	f_mount(0,fs[0]); 		 		//����SD�� 
 	f_mount(1,fs[1]); 				//����FLASH.
	key=KEY_Scan(0);				//����ɨ��	  
	while(font_init()||key==KEY_LEFT) //����ֿ�
	{
   
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		while(SD_Initialize())		//���SD��
		{
			LCD_ShowString(60,70,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(60,70,200+60,70+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(60,70,200,16,16,"SD Card OK");
		LCD_ShowString(60,90,200,16,16,"Font Updating...");
		key=update_font(20,110,16,0);//��SD������
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(60,110,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(60,110,200,16,16,"Font Update Success!");
		delay_ms(1500);	
		LCD_Clear(WHITE);//����	       
	}  	
		POINT_COLOR = BLACK;   
		Show_Str(50,10,170,16,"ͨ�Ž��ն�-RX_Side",16,0);	
		POINT_COLOR = RED;   
		Show_Str(20,30,150,16,"������(BaudRate):",16,0);	
		Show_Str(20,50,150,16,"��������(RX_Data):",16,0);	
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
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ��ȣ��������Ƶĳ��Ȼ��bug 300	
			USART_RX_BUF[len] = '<';
			USART_RX_BUF[len+1] = '<';
			USART_RX_BUF[len+2] = ' ';
			USART_RX_BUF[len+3] =  0;
				
			LCD_ShowNum(170,50,len,4,16);
			LCD_Fill(19,69,236,311,WHITE);
			Show_Str(20,70,216,240,USART_RX_BUF,16,0);						
			USART_RX_STA=0;
		}

		if(times%30==0)LED1=!LED1;//��˸LED,��ʾϵͳ��������.
			delay_ms(10);   
	}	 
}


