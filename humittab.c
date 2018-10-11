//
const unsigned int TAD[]={	
//0		5		10		15		20		25		30		35		40		45		50		55		60      // ℃  

2500000	1500000	1200000	1000000	670000	500000	390000	300000	240000	175000	145000	115000	97000 	 //20 %RH
1500000 1000000	700000	500000	340000	260000	190000	150000	110000	88000	70000	56000	45000  	 //25
640000	460000	320000	230000	175000	130000	97000	74000	57000	42000	34000	27000	21500 	 //30
290000	210000	150000	110000	85000	63000	46000	38000	28000	21000	17000	15000	13000 	 //35
140000	100000	75000	54000	42000	31000	23500	19000	14000	11000	8800	7000	5700  	 //40
70000	50000	38000	28000	21000	16000	12500	10000	7800	6400	5000	4100	3400 	 //45
37000	26000	20000	15000	11500	8700	6900	5600	4500	3800	3100	2500	2100  	 //50
19000	14000	11000	8400	6400	4900	3900	3300	2700	2400	1950	1700	1400  	 //55
10500	8000	6200	5000	3900	3100	2500	2000	1750	1500	1300	1100	940  	 //60
6200	4800	3700	3000	2400	1950	1600	1300	1150	1000	860		760		680 	 //65
3800	3000	2400	1900	1550	1300	1050	900		800		700		600		540		480 	 //70
2300	1800	1500	1200	1000	840		720		620		560		490		420		380		340  	 //75 
1550	1200	1000	800		700		570		500		430		390		340		300		270		250    	 //80
1050	820	    680		550		480		400		350		310		280		240		210		190		180  	 //85 
710	    530		470		400		330		280		250		220		200		180		155		140		130  	 //90  

}
//假设温度4℃，阻值=5602;


#define LIE  13//指表的列数，亦指温度范围（13*5-5=60度）
#define HANG 15//指表的行数，亦指湿度范围(15*5+20=90%RH)

//湿度处理程序
const unsigned char code HumTab[HANG*LIE]={0};//湿度表格


unsigned int HumCalculate(int Tem,unsigned int HumAD)//双线性插值
{
	//假设温度4℃，阻值=5602;t=0℃,R=5602
	unsigned int HumTab_temp[4]={0};
	unsigned char t,k,h;
	unsigned char temp=0,Hum=0;
	unsigned int y1=0,y2=0,y=0;
	if(Tem<=0)//温度为0时，额外处理
	{   for(k=0;k<HANG;k++)//确定HumAD范围,列查找
		{	
			temp=0+LIE*k;
			if(HumAD>=HumTab[0])//找到对应位置k=9
			{
				Hum=20;//
				return Hum;
			}
			if(HumAD>HumTab[temp])//找到对应位置k=9
			{
				y1=HumTab_temp[temp-LIE];//找出大于值的前一列值（大）HumTab_temp[130-13]=6200
				y2=HumTab_temp[temp];    //找出所大于的那个值（小）
			}
			for(h=0;h<=5;h++)
			{
				y=y1-(y1-y2)/5*h;//确定HumAD的值在表中的位置
				if(Tem>=y) { Hum=20+5*(k-1)+h;return Hum;}//Hum=20+5*8+4=64
			}
		}
	}
	for(t=1;t<LIE;t++)//湿度判断范围0-60度
	{
		if(Tem<t*5)//确定温度范围  t=1;
		{
			
			for(k=0;k<HANG;k++)//确定HumAD范围,列查找
			{
				
				temp=t+LIE*k;//表格的下标值 temp=1+13*1=14,27,30, temp=0+13*1=13,26,39,k=10,temp=130
				if(HumAD>=HumTab[t])//找到对应位置k=9
				{
					Hum=20;//
					return Hum;
				}
				if(HumAD>HumTab[temp])//找到对应位置k=9
				{
					HumTab_temp[2]=HumTab_temp[temp-LIE];//找出大于值的前一列值（小）HumTab_temp[130-13]=6200
					HumTab_temp[3]=HumTab_temp[temp-LIE-1];//找出大于值的前一列值的前一个值(大)HumTab_temp[130-13]=6200
					HumTab_temp[0]=HumTab_temp[temp];//找出所大于的那个值（小）
					HumTab_temp[1]=HumTab_temp[temp-1];//找出大于值的前一个值（大）
					switch(Tem-(t-1)*5)//选定对应的温度区间，找到y1,y2,且y1>y2;
					{
						case 0:y1=HumTab_temp[3]-(HumTab_temp[3]-HumTab_temp[2])/5*0;y2=HumTab_temp[1]-(HumTab_temp[1]-HumTab_temp[0])/5*0;			break;
						case 1:y1=HumTab_temp[3]-(HumTab_temp[3]-HumTab_temp[2])/5*1;y2=HumTab_temp[1]-(HumTab_temp[1]-HumTab_temp[0])/5*1;			break;
						case 2:y1=HumTab_temp[3]-(HumTab_temp[3]-HumTab_temp[2])/5*2;y2=HumTab_temp[1]-(HumTab_temp[1]-HumTab_temp[0])/5*2;			break;
						case 3:y1=HumTab_temp[3]-(HumTab_temp[3]-HumTab_temp[2])/5*3;y2=HumTab_temp[1]-(HumTab_temp[1]-HumTab_temp[0])/5*3;			break;
						case 4:y1=HumTab_temp[3]-(HumTab_temp[3]-HumTab_temp[2])/5*4;y2=HumTab_temp[1]-(HumTab_temp[1]-HumTab_temp[0])/5*4;			break;
					}
					for(h=0;h<=5;h++)
					{
						y=y1-(y1-y2)/5*h;//确定HumAD的值在表中的位置
						if(Tem>=y)  {Hum=20+5*(k-1)+h;return Hum;}//Hum=20+5*8+4=64
					}
				
				}
			}
			
		}
	}
}