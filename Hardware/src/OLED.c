#include "OLED.h"
#include "oledfont.h"
#include <math.h>
//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
unsigned char OLED_GRAM[128][8];	

//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{	
	unsigned char i;
		if(cmd)//д����
			OLED_DC_SET();
		else
			OLED_DC_CLR();
		
	OLED_CS_CLR(); //Ƭѡѡ�� 		  
	for(i=0;i<8;i++)
	{			  
		OLED_CLK_CLR();// ������д��
		if(dat&0x80) OLED_DIN_SET();// 
		else OLED_DIN_CLR();//  
		OLED_CLK_SET();//  
		dat<<=1;   
	}				 
	OLED_CS_SET();//		    	  
} 

//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	static unsigned char i,j;	
	taskENTER_CRITICAL();
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(j=0;j<128;j++)
		OLED_WR_Byte(OLED_GRAM[j][i],OLED_DATA); 
	}   
	taskEXIT_CRITICAL();
}

//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	unsigned char i,j;  
	for(i=0;i<8;i++)
		for(j=0;j<128;j++)
		OLED_GRAM[j][i]=0x00;  
	OLED_Refresh_Gram();//������ʾ
}

//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t)
{
	unsigned char pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,�߽�;1,���	  
void OLED_Line(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot) 
{
	u8 dx,dy;
	u8 x,y;
	u8 temp;
	dx = x2 - x1;
	dy = y2 - y1;
	if(dx > dy)
	{
		if(dy == 0)
		{
			for(x=x1;x<=x2;x++)
			{
				OLED_DrawPoint(x,y1,1);
			}
		}
		else
		{
			temp = dx / dy;
			for(x=x1;x<=x2;x++)
			{
				OLED_DrawPoint(x,temp*x,1);
			}
		}
	}	
	else
	{
		if(dx == 0)
		{
			for(y=y1;y<=y2;y++)
			{
				OLED_DrawPoint(x1,y,1);
			}
		}
		else
		{
			temp = dy / dx;
			for(y=y1;y<=y2;y++)
			{
				OLED_DrawPoint(temp*y,y,1);
			}
		}
	}
		
	
}

//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,�߽�;1,���	  
void OLED_Rectangle(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{
	u8 i,j;
	if(x1<x2 && y1<y2)
	{
		if(dot == 0)
		{
			for(i=x1;i<=x2;i++)
			{
				OLED_DrawPoint(i,y1,1);
				OLED_DrawPoint(i,y2,1);
			}
			for(i=y1+1;i<y2;i++)
			{
				OLED_DrawPoint(x1,i,1);
				OLED_DrawPoint(x2,i,1);
			}
		}
		else
		{
			for(i=x1;i<=x2;i++)
			{
			for(j=y1;j<=y2;j++)
			OLED_DrawPoint(i,j,1);
			}
		}
	}
	else
		return;
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(unsigned char x1,unsigned char y1,
							 unsigned char x2,unsigned char y2,unsigned char dot)  
{  
	unsigned char x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
			OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
void OLED_ShowChar(unsigned char x,unsigned char y,
									 unsigned char chr,unsigned char size,unsigned char mode)
{      			    
	unsigned char temp,t,t1;
	unsigned char y0=y;
	unsigned char csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	  //����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								              //û�е��ֿ�
			
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n����
unsigned int mypow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u16 num,u8 len,u8 size,u8 mode)
{         	
	unsigned char t,temp;
	unsigned char enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,mode);
				continue;
			}
			else
				enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,mode); 
	}
} 

//��ʾ2������
//x,y :�������	 
//num_f:��ֵ(0~4294967295);	
//integer_len :�������ֵ�λ��
//decimals_len :С�����ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
void OLED_ShowFloatNum(u8 x,u8 y,float num_f,u8 integer_len,u8 decimals_len,u8 size,u8 mode)
{        
	u16 integer_nmb,decimals_nmb;
 	integer_nmb  = (u16)num_f;
	decimals_nmb = (u16)(num_f * pow(10,decimals_len))%(u16)(pow(10,decimals_len));
	OLED_ShowNum(x,y,integer_nmb,integer_len,size,mode);
	OLED_ShowChar(x+integer_len*size/2,y,'.',size,mode);
	OLED_ShowNum(x+(integer_len+1)*size/2,y,decimals_nmb,decimals_len,size,mode);
} 

//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
}	
//��ʾ����
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowChinese(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u8 y0=y;
	u8 x0=x;
	u8 csize=(size/8+((size%8)?1:0))*size;		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
//	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
	for(t=0;t<csize;t++)
	{   
		if(size==12)		 temp=Chinese_12[chr][t]; 	//����1206����
		else if(size==16)temp=Chinese_16[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								              //û�е��ֿ�
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}          
}

void ShowBMP(u8 width,u8 hight,s8 x1,s8 y1,u8 size,u8 image)
{
	u8 i,j;
	u8 temp;
	for(i=0;i<size/8;i++)
	{
		for(j=0;j<width;j++)
		{
			if(i+y1<8 && i+y1>=0 && j+x1>=0 && j+x1<128)
			{
				     if(size == 32) temp = Image_32[image][size*i+j];
				else if(size == 64) temp = Image_64[image][size*i+j];
				else return;
 				OLED_GRAM[j+x1][7-i-y1]=temp;
			}
		}
	}
}
//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	 		 
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA~E,PORTGʱ��
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1|GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��		
	
	OLED_DIN_SET();
	OLED_CLK_SET();
	OLED_CS_SET();
					  
	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(0x80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0x66,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0x22,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 
	
	OLED_Clear();
}  
