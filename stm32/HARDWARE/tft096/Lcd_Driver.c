
#include "Lcd_Driver.h"
#include "delay.h"
#include "font.h"
#include "string.h"
#define CS_L GPIOA->BRR=CS_PIN;
#define CS_H GPIOA->BSRR=CS_PIN;

#define RS_L GPIOA->BRR=RS_PIN;
#define RS_H GPIOA->BSRR=RS_PIN;

#define WR_L GPIOA->BRR=WR_PIN;
#define WR_H GPIOA->BSRR=WR_PIN;
uint16_t POINT_COLOR = 0xFEFD  ;
uint16_t BACK_COLOR  = 0x0000 ;
#define DATAPORT GPIOB->ODR
#define Chinese_Lib_Len  40
void Lcd_Gpio_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;
	      
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO ,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

}


//��Һ����дһ��8λָ��
void Lcd_WriteCommand(uint8_t Command)
{
	CS_L;
	RS_L;
	DATAPORT=Command;
	WR_L;
	WR_H;
	RS_H;
	CS_H;
}

//��Һ����дһ��8λ����
void Lcd_WriteData(uint8_t Data)
{
	CS_L;
	RS_H;
	DATAPORT=Data;
	WR_L;
	WR_H;
	CS_H;
}
//��Һ����дһ��16λ����
void Lcd_WriteData16(uint16_t Data)
{

	 Lcd_WriteData(Data>>8); 	//д���8λ����
	 Lcd_WriteData(Data); 			//д���8λ����

}
//д�Ĵ��� дָ���д����
void Lcd_WriteReg(uint8_t Command,uint8_t Data)
{
    Lcd_WriteCommand(Command);
    Lcd_WriteData(Data);
}



//Lcd Init 
void Lcd_Init(void)
{	
	Lcd_Gpio_Init();


	Lcd_WriteCommand(0x11);
	delay_ms(10); //Delay 120ms
	//--------------------------------Display and color format setting-------------------

	Lcd_WriteCommand(0x36);
	Lcd_WriteData(0x00);
	Lcd_WriteCommand(0x3a);
	Lcd_WriteData(0x05);
	//--------------------------------ST7789S Frame rate setting-------------------------

	Lcd_WriteCommand(0xb2);
	Lcd_WriteData(0x0c);
	Lcd_WriteData(0x0c);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x33);
	Lcd_WriteData(0x33);
	Lcd_WriteCommand(0xb7);
	Lcd_WriteData(0x35);
	//---------------------------------ST7789S Power setting-----------------------------

	Lcd_WriteCommand(0xbb);
	Lcd_WriteData(0x35);
	Lcd_WriteCommand(0xc0);
	Lcd_WriteData(0x2c);
	Lcd_WriteCommand(0xc2);
	Lcd_WriteData(0x01);
	Lcd_WriteCommand(0xc3);
	Lcd_WriteData(0x13);
	Lcd_WriteCommand(0xc4);
	Lcd_WriteData(0x20);
	Lcd_WriteCommand(0xc6);
	Lcd_WriteData(0x0f);
	Lcd_WriteCommand(0xca);
	Lcd_WriteData(0x0f);
	Lcd_WriteCommand(0xc8);
	Lcd_WriteData(0x08);
	Lcd_WriteCommand(0x55);
	Lcd_WriteData(0x90);
	Lcd_WriteCommand(0xd0);
	Lcd_WriteData(0xa4);
	Lcd_WriteData(0xa1);
	//--------------------------------ST7789S gamma setting------------------------------
	Lcd_WriteCommand(0xe0);
	Lcd_WriteData(0x70);
	Lcd_WriteData(0x04);
	Lcd_WriteData(0x08);
	Lcd_WriteData(0x09);
	Lcd_WriteData(0x09);
	Lcd_WriteData(0x05);
	Lcd_WriteData(0x2a);
	Lcd_WriteData(0x33);
	Lcd_WriteData(0x41);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x13);
	Lcd_WriteData(0x13);
	Lcd_WriteData(0x29);
	Lcd_WriteData(0x2f);
	Lcd_WriteCommand(0xe1);
	Lcd_WriteData(0x70);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x09);
	Lcd_WriteData(0x0a);
	Lcd_WriteData(0x09);
	Lcd_WriteData(0x06);
	Lcd_WriteData(0x2b);
	Lcd_WriteData(0x34);
	Lcd_WriteData(0x41);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x12);
	Lcd_WriteData(0x14);
	Lcd_WriteData(0x28);
	Lcd_WriteData(0x2e);
	Lcd_WriteCommand(0x29);
}


/*************************************************
��������Lcd_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
void Lcd_Set_Region(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{		
	Lcd_WriteCommand(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end);

	Lcd_WriteCommand(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end);
	
	Lcd_WriteCommand(0x2c);

}

/*************************************************
��������Lcd_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(uint16_t x,uint16_t y)
{
  	Lcd_Set_Region(x,y,x,y);
}

	
/*************************************************
��������Lcd_Draw_Point
���ܣ���һ����
��ڲ�������
����ֵ����
*************************************************/
void Lcd_Draw_Point(uint16_t x,uint16_t y,uint16_t color)
{
	Lcd_SetXY(x,y);
	Lcd_WriteCommand(0x2C);
	color = ~color;
	Lcd_WriteData16(color);

}    

/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Clear(uint16_t color)               
{	
   unsigned int i,m;
   color = ~color;
   Lcd_Set_Region(0,0,239,239);
   Lcd_WriteCommand(0x2C);
   for(i=0;i<240;i++)
    for(m=0;m<240;m++)
    {	
	  	Lcd_WriteData16(color);
    }   
}


//����
//x_start,y_start:�������
//x_end,y_end:�յ�����
void Lcd_Draw_Line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
	uint16_t point = POINT_COLOR;
	uint16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x_end-x_start; //������������
	delta_y=y_end-y_start;
	uRow=x_start;
	uCol=y_start;
	if(delta_x>0)incx=1; //���õ�������
	else if(delta_x==0)incx=0;//��ֱ��
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//ˮƽ��
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //ѡȡ��������������
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//�������
	{
		Lcd_Draw_Point(uRow,uCol,point);//����
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

/*************************************************
���ܣ����ָ������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Filled(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end,uint16_t color)               
{	
	uint16_t i;
	uint16_t j;
	color = ~color;
	Lcd_Set_Region(x_start,y_start, x_end, y_end);

	for(i=y_start; i<=y_end; i++)
	{
		for(j=x_start; j<=x_end; j++)
			Lcd_WriteData16(color);	//���ù��λ��
	}
}
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void Lcd_Show_Char(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{

	uint8_t temp,t1,t;
	uint16_t y0 = y;
	uint8_t csize;
	//uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	if(size == 18) csize =27;
	else if(size == 32) csize =64;
	else if(size == 24) csize = 36;
		
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{
		if(size==24)temp=asc2_2412[num][t]; 	 	//����1206����
		else if(size==32)temp=asc2_3321[num][t];	//����2412����
		else if(size==18)temp=asc2_1809[num][t];
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)Lcd_Draw_Point(x,y,POINT_COLOR);
			else if(mode==0)Lcd_Draw_Point(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=HEIGHT)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=WEIGHT)return;	//��������
				break;
			}
		}
	}
}

//m^n����
//����ֵ:m^n�η�.
uint32_t Lcd_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)result*=m;
	return result;
}


//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
void Lcd_Show_Num(uint16_t x,uint16_t y,u32 num,uint8_t len,uint8_t size,uint8_t mode)
{
	uint8_t t,temp;
	uint8_t enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/Lcd_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)Lcd_Show_Char(x+(size/2)*t,y,'0',size,mode&0X01);
				else Lcd_Show_Char(x+(size/2)*t,y,' ',size,mode&0X01);
 				continue;
			}else enshow=1;

		}
	 	Lcd_Show_Char(x+(size/2)*t,y,temp+'0',size,mode&0X01);
	}
}
//��ʾ�ַ���
//x,y:�������
//width,height:�����С
//size:�����С
//*p:�ַ�����ʼ��ַ
void Lcd_Show_String(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{
	uint8_t x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        Lcd_Show_Char(x,y,*p,size,0);
        x+=size/2;
        p++;
    }
}

//===============================================
//��������Lcd_ShowImage //���ܣ���ʾͼƬ ��ڲ�����ͼƬ����
//============================================
void Lcd_Show_Image(uint16_t x, uint16_t y, unsigned char *p) 
{
int i, j;
	uint16_t y0;
	y0 = y;
	for(i=0;i<120;i++)
	{	
    uint8_t temp;
	temp = p[i];
	for(j=0;j<8;j++)
	  {
		if(temp&0x80)Lcd_Draw_Point(x,y,POINT_COLOR);
		else Lcd_Draw_Point(x,y,BACK_COLOR);
		temp<<=1;
		y++;
		if((y-y0)==30){y=y0;x++;break;}
	  }
	}	
}
void chinese_Char(uint16_t x, uint16_t  y, uint8_t size, unsigned char c[3])
{
	uint16_t i,j,k;
		uint16_t y0;
	uint8_t csize;
	uint16_t dcolor, bgcolor;
	dcolor = POINT_COLOR;
	dcolor = ~dcolor;
	bgcolor = BACK_COLOR;
	bgcolor = ~bgcolor;
    if(size==18) csize = 54;
	//else if(size == 24) csize = 72;
	y0 =y;
	//TFT_SetWindow(x,y,x+32-1, y+32-1);     //ѡ������λ��
	//Lcd_Set_Region(x,y,31,31);
	for (k=0;k<Chinese_Lib_Len;k++) { //15��ʾ�Խ����ֿ��еĸ�����ѭ����ѯ����
	  if ((weather_Char[k].Chinese[0]==c[0])&&(weather_Char[k].Chinese[1]==c[1])&&(weather_Char[k].Chinese[2]==c[2])){ 
    	for(i=0;i<csize;i++) {
		  unsigned short m=weather_Char[k].char_Model[i];
		  for(j=0;j<8;j++) {
			if(m&0x80) {
				Lcd_Draw_Point(x,y,POINT_COLOR);
				}
			else {
			    Lcd_Draw_Point(x,y,BACK_COLOR);
				}
			m<<=1;
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
	  }	
	}
 
	
void chinese_Str(uint16_t x,uint16_t y,uint8_t size, char *str)	 
{  
   while(*str)
   {

	 while((*str<='~')&&(*str>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {
        Lcd_Show_Char(x,y,*str,size,0);
        x+=size/2;
        str++;
    }
	chinese_Char(x,y,size,(unsigned char*)str);
	str++;
	x+=(size/4)+2;
   }
}

void showlogo(const unsigned char * p)
{

   unsigned int i,m;
    uint16_t val;
   Lcd_Set_Region(100,100,199,199);
   Lcd_WriteCommand(0x2C);
   for(i=0;i<100;i++)
    for(m=0;m<100;m++)
    {	
	  	val = *p;
		p++;
		val= (val<<8)|(*p) ;
		Lcd_WriteData16(val);
		p++;
    }   


}
	