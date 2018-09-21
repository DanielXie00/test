//File Name:ScanI2CAddress.ino
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("\nI2C Scanner");
}
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
  delay(5000); // wait 5 seconds for next scan
}

// 升序排序算法
//排序方法总结：https://www.cnblogs.com/eniac12/p/5329396.html
void Array_Sort(uint32_t* point, uint8_t len)
{
		uint32_t temp;
		uint8_t i,j;
		
		for(i=0;i < len-1;i++)
		{
				for(j=0;j <(len-i-1);j++)
				{
						if(point[j] > point[j+1])
						{
								temp = point[j+1];
								point[j+1] = point[j];
								point[j] = temp;
						}		
				}
		}
}

// 分类 -------------- 内部比较排序
// 数据结构 ---------- 数组
// 最差时间复杂度 ---- O(n^2)
// 最优时间复杂度 ---- 如果序列在一开始已经大部分排序过的话,会接近O(n)
// 平均时间复杂度 ---- O(n^2)
// 所需辅助空间 ------ O(1)
// 稳定性 ------------ 稳定

void Swap(int A[], int i, int j)
{
    int temp = A[i];
    A[i] = A[j];
    A[j] = temp;
}

void CocktailSort(int A[], int n)
{
    int left = 0;                            // 初始化边界
    int right = n - 1;
    while (left < right)
    {
        for (int i = left; i < right; i++)   // 前半轮,将最大元素放到后面
        {
            if (A[i] > A[i + 1])
            {
                Swap(A, i, i + 1);
            }
        }
        right--;
        for (int i = right; i > left; i--)   // 后半轮,将最小元素放到前面
        {
            if (A[i - 1] > A[i])
            {
                Swap(A, i - 1, i);
            }
        }
        left++;
    }
}

int main()
{
    int A[] = { 6, 5, 3, 1, 8, 7, 2, 4 };   // 从小到大定向冒泡排序
    int n = sizeof(A) / sizeof(int);
    CocktailSort(A, n);
    printf("鸡尾酒排序结果：");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", A[i]);
    }
    printf("\n");
    return 0;
}


//快速排序(Quick Sort)

#include <stdio.h>

// 分类 ------------ 内部比较排序
// 数据结构 --------- 数组
// 最差时间复杂度 ---- 每次选取的基准都是最大（或最小）的元素，导致每次只划分出了一个分区，需要进行n-1次划分才能结束递归，时间复杂度为O(n^2)
// 最优时间复杂度 ---- 每次选取的基准都是中位数，这样每次都均匀的划分出两个分区，只需要logn次划分就能结束递归，时间复杂度为O(nlogn)
// 平均时间复杂度 ---- O(nlogn)
// 所需辅助空间 ------ 主要是递归造成的栈空间的使用(用来保存left和right等局部变量)，取决于递归树的深度，一般为O(logn)，最差为O(n)       
// 稳定性 ---------- 不稳定

void Swap(int A[], int i, int j)
{
    int temp = A[i];
    A[i] = A[j];
    A[j] = temp;
}

int Partition(int A[], int left, int right)  // 划分函数
{
    int pivot = A[right];               // 这里每次都选择最后一个元素作为基准
    int tail = left - 1;                // tail为小于基准的子数组最后一个元素的索引
    for (int i = left; i < right; i++)  // 遍历基准以外的其他元素
    {
        if (A[i] <= pivot)              // 把小于等于基准的元素放到前一个子数组末尾
        {
            Swap(A, ++tail, i);
        }
    }
    Swap(A, tail + 1, right);           // 最后把基准放到前一个子数组的后边，剩下的子数组既是大于基准的子数组
                                        // 该操作很有可能把后面元素的稳定性打乱，所以快速排序是不稳定的排序算法
    return tail + 1;                    // 返回基准的索引
}

void QuickSort(int A[], int left, int right)
{
    if (left >= right)
        return;
    int pivot_index = Partition(A, left, right); // 基准的索引
    QuickSort(A, left, pivot_index - 1);
    QuickSort(A, pivot_index + 1, right);
}

int main()
{
    int A[] = { 5, 2, 9, 4, 7, 6, 1, 3, 8 }; // 从小到大快速排序
    int n = sizeof(A) / sizeof(int);
    QuickSort(A, 0, n - 1);
    printf("快速排序结果：");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", A[i]);
    }
    printf("\n");
    return 0;
}


//*************************************
// 函数名称：FineTab  二分查找算法 ->查温度表
// 函数功能：查找数据在表中对应的位置 表中数据从大到小
// 入口参数：表地址、表长度、要查找的数据
// 出口参数：无
// 返 回 值：数据在表中的位置
//*************************************
u8 FineTab(u16 *a,u8 TabLong,u16 data)//表中数据从大到小
{
    u8 st,ed,m ;
    u8 i ;

    st = 0 ;
    ed = TabLong-1 ;
    i = 0  ;

    if(data >= a[st]) return st ;
    else if(data <= a[ed]) return ed ;

    while(st < ed)
    {
        m = (st+ed)/2 ;

        if(data == a[m] ) break ;
        if(data < a[m] && data > a[m+1]) break ;


        if(data > a[m])  ed = m ;
        else st = m ;

        if(i++ > TabLong) break ;
    }

    if(st > ed ) return 0 ;

    return (102-m );
}
//折半查表算法
void Lookup_Table(u16 data1, u16 (*tab),u16 (*aptr))
{
		unsigned char i;
		unsigned char Top_point = 101;            //顶端指针
		unsigned char But_point = 0;              //低端指针
		unsigned char Find_point;                   //查找指针
		
		for(i=0;i<8;i++)
		{
			Find_point = But_point + ((Top_point - But_point) >> 1);//i=1;Find_point=0+((142-0))>>1=71
			if(tab[Find_point] < data1)				//如果tab[71]<data
				Top_point = Find_point;					//Top_point=71;
			else if(tab[Find_point] > data1)             //如果tab[71]>data
				But_point = Find_point;					//But_point=71;
			else															//如果tab[71]=data
			{
				aptr[0] = Find_point;                    //x1  aptr[0]=71
				aptr[1] = tab[Find_point - 1];           //y2  aptr[1] = tab[70];
				aptr[2] = tab[Find_point];               //y1  aptr[2] = tab[71]; 
				break;
			}
			if(Top_point - But_point == 1)					//如果--高端指针减去低端指针=1
			{
				aptr[0] = Find_point;                    //x1
				aptr[1] = tab[Find_point - 1];           //y2
				aptr[2] = tab[Find_point];               //y1				
				break;
			}
		}
}
//--------------------------线性插值-----------------------------
//功能：计算数据表中没有的温度值
//参数：dat是AD转换数值，tab是数据表数组名
//返回值：温度数值（放大10倍）
//说明：t是温度，k是斜率，m是截距，且t=k*dat+m
//温度t扩大100倍，等于保留了两位小数
//创建时间：2015年12月18日
//修改时间：2015年12月21日
//---------------------------------------------------------------
u16 LinearInsert(u16 dat,u16 *tab)
{
    u16 t=0;double k=0,m=0;
	u16 pointer[3]={0};
	if(dat<tab[0])  return 80;      //温度低于下限
	if(dat>tab[100]) return 100;    //温度高于上限
	Lookup_Table(dat,tab,pointer);
        if(pointer[1]==0 && pointer[2]==0)
        {
            t=pointer[0];
        }
        else
        {
            k=(double)((pointer[2]-pointer[1])/(tab[pointer[2]]-tab[pointer[1]]));
            m=(double)(pointer[1]-k*tab[pointer[1]]);
            t=(u16)(k*dat+m);
        }
	return t;
}

#define CISHU 200
void RGBTEST(void)//-----------呼吸灯，程序运行频率：小于2ms
{   
    static u8 PwmCnt=0;       //PWM计数器
    static bit  PwmDirection=0; //亮灭方向标志位
    static u8 PwmDuty=CISHU;     //占空比初始化为100
    
        if(++PwmCnt>=CISHU)            //1个周期到，调整一次占空比
        {
             PwmCnt=0;
             if(PwmDirection==0)       //从全亮到灭
             {
                  if(--PwmDuty<=0)     //占空比减1，为0则改变方向标志位
                      PwmDirection=1;                
             }
             else                      //从灭到全亮
             {
                  if(++PwmDuty>=CISHU)   //占空比加1，到100则该变方向标志位
                      PwmDirection=0;
             }  
         }
         if(PwmCnt<=PwmDuty)           //改变输出状态
             LEDR=0;
         else
            {IOCTRL(LEDR,OFF); IOCTRL(LEDB,OFF); IOCTRL(LEDG,OFF);}
}

/*-----------------------------------------------
  Project Name:Breathing LED
  Author：shuwei  
  Data:2012-11-17
  Breathing LEDs display during interruptting.
***调试心得(23:09)：
  1).应用了static变量，减少全局变量的使用
  2).标志位变量使用bit位操作定义，节省RAM的空间。但是bit定义变量应该是51所特有的.
  3).修改LED在main函数所赋的初值(0x00-->0xff)，修正了上电LED灯亮度的突变
------------------------------------------------*/

#include<reg52.h>

#define LED		P2 //定义LED灯端口
//#define SPEED	1 //设置每一个明亮级的停留时间 ；也即每个明亮级持续的时间
#define PWM		200 //设置明亮的总级数（级数过大会出现闪烁）（值域：0~65535）
#define MAX		150 //设置最大明亮（值域：小于明亮总级数）

void Init_Timer0(void);

void main (void)  //主程序 
{ 		
	Init_Timer0();
	LED=0xff;
	while(1);
		    
}


void Init_Timer0(void)
	{
		TMOD = 0x01;  //使用模式1，16位定时器
		EA=1;
		ET0=1;
		TR0=1;
	}

void Timer0_isr(void) interrupt 1  //定时器0中断程序，0.05ms中断一次
	{
		static unsigned int t=1;
		static unsigned int i;
		static bit LOOP,DELAY;     //LOOP是循环切换标志变量
		TH0=(65536-50)/256; //重新赋值 0.05ms
		TL0=(65536-50)%256;
		if(LOOP==0)  //变亮循环
		{								
			if(DELAY==0)
			{	
				i++;
				if(i==t)			
				{
					DELAY=1;
					LED=0xff;
					i=0;
				}
			}			
			if(DELAY==1)	
			{
				i++;
				if(i==(PWM-t))			
				{
					DELAY=0;
					LED=0x00;
					i=0;
					t++;
				}
			}			
			if(t>(MAX-2))
			{
				LOOP=1;
			}
		}
		if(LOOP==1)  //变暗循环
		{			
			if(DELAY==0)
			{	
				i++;
				if(i==t)			
				{
					DELAY=1;
					LED=0xff;
					i=0;
				}
			}			
			if(DELAY==1)	
			{
				i++;
				if(i==(PWM-t))			
				{
					DELAY=0;
					LED=0x00;
					i=0;
					t--;
				}
			}			
			if(t<2)
			{
				LOOP=0;
			}
		}		    	

	}	


	
	
//I2C总线地址判断程序，基于ARDUNIO平台
//File Name:ScanI2CAddress.ino
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("\nI2C Scanner");
}
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
  delay(5000); // wait 5 seconds for next scan
}



