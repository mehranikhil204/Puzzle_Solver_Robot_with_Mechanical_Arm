/*
 * Grid_Solver.c
 *
 * Created: 09/02/2016 23:21:55
 *  Author: HP Pavilion Dv5
 */ 

#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> //included to support power function
#include "lcd.h"


void port_init();
void lcd_reset();
void lcd_init();
void lcd_wr_command(unsigned char);
void lcd_wr_char(char);
void lcd_line1();
void lcd_line2();
void lcd_string(char*);
void timer5_init();
void velocity(unsigned char, unsigned char);
void motors_delay();

unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char flag = 0;
unsigned char Left_white_line = 0;
unsigned char Center_white_line = 0;
unsigned char Right_white_line = 0;

//Function to configure LCD port
void lcd_port_config (void)
{
 DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}



//ADC pin configuration
void adc_pin_config (void)
{
 DDRF = 0x00; 
 PORTF = 0x00;
 DDRK = 0x00;
 PORTK = 0x00;
}

void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as output
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}



//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
void left_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
void right_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

//Function to configure ports to enable robot's motion
void motion_pin_config (void) 
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to Initialize PORTS
void port_init()
{
	lcd_port_config();
	adc_pin_config();
	motion_pin_config();
	buzzer_pin_config();
	left_encoder_pin_config(); //left encoder pin config
    right_encoder_pin_config(); //right encoder pin config		
}

// Timer 5 initialized in PWM mode for velocity control
// Prescale:256
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:225.000Hz
void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

//Function For ADC Conversion
unsigned char ADC_Conversion(unsigned char Ch) 
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}


void buzzer_on (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore | 0x08;
 PORTC = port_restore;
}

void buzzer_off (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore & 0xF7;
 PORTC = port_restore;
}


//Function To Print Sesor Values At Desired Row And Coloumn Location on LCD
void print_sensor(char row, char coloumn,unsigned char channel)
{
	
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}

//Function for velocity control
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 		// removing upper nibbel for the protection
 PortARestore = PORTA; 		// reading the PORTA original status
 PortARestore &= 0xF0; 		// making lower direction nibbel to 0
 PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
 PORTA = PortARestore; 		// executing the command
}

void forward (void) 
{
  motion_set (0x06);
}

void soft_right (void) //Left wheel forward, Right wheel is stationary
{
 motion_set(0x02);
}

void right (void) //Left wheel forward, Right wheel backward
{
  motion_set(0x0A);
}

void left (void) //Left wheel backward, Right wheel forward
{
  motion_set(0x05);
}


void stop (void)
{
  motion_set (0x00);
}


void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
 EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
 sei();   // Enables the global interrupt 
}

void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
 EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
 sei();   // Enables the global interrupt 
}

//ISR for right position encoder
ISR(INT5_vect)  
{
 ShaftCountRight++;  //increment right shaft position count
}


//ISR for left position encoder
ISR(INT4_vect)
{
 ShaftCountLeft++;  //increment left shaft position count
}
//Function used for turning robot by specified degrees
void angle_rotate(unsigned int Degrees)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
 ShaftCountRight = 0; 
 ShaftCountLeft = 0; 

 while (1)
 {
  if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
  break;
 }
 stop(); //Stop robot
}

void left_degrees(unsigned int Degrees) 
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 left(); //Turn left
 angle_rotate(Degrees);
}



void right_degrees(unsigned int Degrees)
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 right(); //Turn right
 angle_rotate(Degrees);
}

void linear_distance_mm(unsigned int DistanceInMM)
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
	
	ShaftCountRight = 0;
	while(1)
	{
		if(ShaftCountRight > ReqdShaftCountInt)
		{
			break;
		}
	}
	stop(); //Stop robot
}

void forward_mm(unsigned int DistanceInMM)
{
	forward();
	linear_distance_mm(DistanceInMM);
}

/*void back_mm(unsigned int DistanceInMM)
{
	back();
	linear_distance_mm(DistanceInMM);
}*/


void init_devices (void)
{
 	cli(); //Clears the global interrupts
	port_init();
	adc_init();
	timer5_init();
	left_position_encoder_interrupt_init();
 	right_position_encoder_interrupt_init();
	sei();   //Enables the global interrupts
}


void whitesensor()
{
	Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
	Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
	Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor
}

void printlcd(int x,int y)
{
	
	lcd_init();
	if (x>=0)
	lcd_print(1,1,x,1);
	
	if(x<0)
	{
		lcd_cursor(1,1);
		lcd_string("-");
		x=x*-1;
		lcd_print(1,2,x,1);
	}
	
	if (y>=0)
	lcd_print(1,4,y,1);
	
	if(y<0)
	{
		lcd_cursor(1,4);
		lcd_string("-");
		y=y*-1;
		lcd_print(1,5,y,1);
	}
}

int coor[2];
int count=1;

void linefollwer()
{
	while(1)
	{
		whitesensor();
		flag=0;
	
	
		if(Center_white_line>16)
		{
			
			flag=1;
			forward();
			velocity(140,140);
		}
		
		if((Left_white_line>16) && (flag==0))
		{
			flag=1;
			forward();
			velocity(50,130);
		}

		if((Right_white_line>16) && (flag==0))
		{
			flag=1;
			forward();
			velocity(130,50);
		}
		if(Center_white_line<16 && Left_white_line<16 && Right_white_line<16)
		{
			forward();
			velocity(140,140);
		}
		
		if((Center_white_line>16 && Left_white_line>16) || (Center_white_line>16 && Right_white_line>16))
		{
			    forward_mm(80);
				stop();
				printlcd(coor[0],coor[1]);
				break;
				buzzer_on();
				_delay_ms(100);		//delay
				buzzer_off();
				_delay_ms(100);    //delay
		}
		
	}
}


//Main Function
int main()
{   
	init_devices();
	lcd_set_4bit();
    int origin1[12][4][2]=
    {
	   {{1,3},{2,3},{1,2},{2,2}},
	   {{1,2},{2,2},{1,1},{2,1}},
	   {{1,1},{2,1},{1,0},{2,0}},
	   {{0,3},{1,3},{0,2},{1,2}},
	   {{0,2},{1,2},{0,1},{1,1}},
	   {{0,1},{1,1},{0,0},{1,0}},
	   {{-1,3},{0,3},{-1,2},{0,2}},
	   {{-1,2},{0,2},{-1,1},{0,1}},
	   {{-1,1},{0,1},{-1,0},{0,0}},
	   {{-2,3},{-1,3},{-2,2},{-1,2}},
	   {{-2,2},{-1,2},{-2,1},{-1,1}},
	   {{-2,1},{-1,1},{-2,0},{-1,0}},	   	   	   	   	   	   	         
	};
	
	int origin2[24][4][2]=
	{
		{{-3,0},{-2,0},{-3,1},{-2,1}},
		{{-3,1},{-2,1},{-3,2},{-2,2}},
		{{-3,2},{-2,2},{-3,3},{-2,3}},
		{{-3,4},{-2,4},{-3,3},{-2,3}},
		{{-2,0},{-1,0},{-2,1},{-1,1}},
		{{-2,2},{-1,2},{-2,1},{-1,1}},
		{{-2,3},{-1,3},{-2,2},{-1,2}},
		{{-2,4},{-1,4},{-2,3},{-1,3}},
		{{-1,0},{0,0},{-1,1},{0,1}},
		{{-1,1},{0,1},{-1,2},{0,2}},
		{{-1,2},{0,2},{-1,3},{0,3}},
		{{-1,4},{0,4},{-1,3},{0,3}},
		{{0,0},{1,0},{0,1},{1,1}},
		{{0,2},{1,2},{0,1},{1,1}},
		{{0,3},{1,3},{0,2},{1,2}},
		{{0,4},{1,4},{0,3},{1,3}},
		{{1,0},{2,0},{1,1},{2,1}},
		{{1,1},{2,1},{1,2},{2,2}},
		{{1,2},{2,2},{1,3},{2,3}},
		{{1,4},{2,4},{1,3},{2,3}},
		{{2,0},{3,0},{2,1},{3,1}},
		{{2,1},{3,1},{2,2},{3,2}},
		{{2,2},{3,2},{2,3},{3,3}},
		{{2,4},{3,4},{2,3},{3,3}},																							
	};
	coor[0]=-2;
	coor[1]=2;
	int flag1=1;
	while(1)
	{
		whitesensor();
		if((Center_white_line>16 && Left_white_line>16) || (Center_white_line>16 && Right_white_line>16))
		{
			forward();
			velocity(140,140);
		}
		else
		{
			break;
		}
	}		
	linefollwer();
	int compass=6;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int D1[12]={0,3,5,8,6,4,3,1,6,1,2,4};
	int D2[2][2]={{7,15},{12,19}};	
	int summ1[9]={8,3,4,15,6,3,6,4,19};
	int boxnumber;	
	int final[2];
	int recur=0;
	int recur1=0;
	int help=summ1[recur];
	
	point1 : if(flag1%2!=0)
	{
		int x1,y1;
		int result,index;
		int arr[4];
		for(int x=0;x<12;x++)
		{
			if(D1[x]==help)
			{
				boxnumber=x;
				D1[x]=-1;
				break;
			}				
		}
		for(int k=0;k<4;k++)
		{
			x1=coor[0]-origin1[boxnumber][k][0];
			y1=coor[1]-origin1[boxnumber][k][1];
			if(x1<0)
				x1=x1*-1;
			if(y1<0)
				y1=y1*-1;
			arr[k]=x1+y1;
		}
		result=arr[0];
		index=0;
		for(int j=1;j<4;j++)
		{
			if(arr[j]<result)
			{
				result=arr[j];
				index=j;
			}
		}
		final[0]=origin1[boxnumber][index][0];
		final[1]=origin1[boxnumber][index][1];
	}  			
	else
	{
		if(flag1%2==0)
		{
			int x1,y1;
			int result,index;
			int arr[4];
			for(int k=0;k<4;k++)
			{
				x1=coor[0]-origin2[boxnumber][k][0];
				y1=coor[1]-origin2[boxnumber][k][1];
				if(x1<0)
					x1=x1*-1;
				if(y1<0)
					y1=y1*-1;
				arr[k]=x1+y1;
			}
			result=arr[0];
			index=0;
			for(int j=1;j<4;j++)
			{
				if(arr[j]<result)
				{
					result=arr[j];
					index=j;
				}
			}
			final[0]=origin2[boxnumber][index][0];
			final[1]=origin2[boxnumber][index][1];
		}
		
	}
	
	
	point : while(1)         ///////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		
		if(final[1]>coor[1])
		{
			if(compass==4)
			{
				coor[1]=coor[1]+1;
				compass=4;
				_delay_ms(100);
				linefollwer();
			}
			else
			{
				
				if(compass==6)
				{
					coor[1]=coor[1]+1;
					compass=4;
					_delay_ms(500);
					left_degrees(85);
					_delay_ms(100);
					linefollwer();
				}
				else
				{
					if(compass==7)
					{
						coor[1]=coor[1]+1;
						compass=4;
						_delay_ms(500);
						right_degrees(85);
						_delay_ms(100);
						linefollwer();
					}
					else
					{
						if(compass==5)
						{
							coor[1]=coor[1]+1;
							compass=4;
							_delay_ms(500);
							right_degrees(180);
							_delay_ms(100);
							linefollwer();
							
							
						}
					}
				}	
			}					
			
		}
		if(final[1]<coor[1])
		{
			if(compass==6)
			{
				coor[1]=coor[1]-1;
				compass=5;
				_delay_ms(500);
				right_degrees(85);
				_delay_ms(100);
				linefollwer();
			}
			else
			{
				if(compass==7)
				{
					coor[1]=coor[1]-1;
					compass=5;
					_delay_ms(500);
					left_degrees(85);
					_delay_ms(100);
					linefollwer();
				}
				else
				{
					if(compass==5)
					{		   
						coor[1]=coor[1]-1;
						compass=5;
						_delay_ms(100);
						linefollwer();
					}
					else
					{
						if(compass==4)
						{
							coor[1]=coor[1]-1;
							compass=5;
							_delay_ms(500);
							right_degrees(180);
							_delay_ms(100);
							linefollwer();
							
							
						}
					}
			
				}
			}
		}							
		if(final[0]>coor[0])
		{
			if(compass==4)
			{
				coor[0]=coor[0]+1;
				compass=6;
				_delay_ms(500);
				right_degrees(85);
				_delay_ms(100);
				linefollwer();
			}
			else
			{
				if(compass==6)
				{
					coor[0]=coor[0]+1;
					compass=6;
					_delay_ms(100);
					linefollwer();
				}
				else
				{
					if(compass==5)
					{
						coor[0]=coor[0]+1;
						compass=6;
						_delay_ms(500);
						left_degrees(85);
						_delay_ms(100);
						linefollwer();
					}
					else
					{
							if(compass==7)
							{
								coor[0]=coor[0]+1;
								compass=6;
								_delay_ms(500);
								right_degrees(180);
								_delay_ms(100);
								linefollwer();
								
								
							}
					}
			
				}
			}
		}							
		if(final[0]<coor[0])
		{
			if(compass==4)
			{
				coor[0]=coor[0]-1;
				compass=7;
				_delay_ms(500);
				left_degrees(85);
				_delay_ms(100);
				linefollwer();
			}
			else
			{
				if(compass==7)
				{
					coor[0]=coor[0]-1;
					compass=7;
					_delay_ms(100);
					linefollwer();
				}
				else
				{
					if(compass==5)
					{
						coor[0]=coor[0]-1;
						compass=7;
						_delay_ms(500);
						right_degrees(85);
						_delay_ms(100);
						linefollwer();
					}
					else
					{
							if(compass==6)
							{
								coor[0]=coor[0]-1;
								compass=7;
								_delay_ms(500);
								right_degrees(180);
								_delay_ms(100);
								linefollwer();
								
								
							}
					}
				}
			}									
			
		}
		if(final[0]==coor[0] && final[1]==coor[1])
		{
			buzzer_on();
			_delay_ms(100);		//delay
			buzzer_off();
			_delay_ms(100);    //delay
			break;
		}
	}
	
	if(coor[0]==0 && coor[1]==0 && flag1%2!=0)    ////////////////////////////////////////////////////////////////////////////////////
	{
		if(compass==6)
		{
			right_degrees(85);
			_delay_ms(100);
			coor[0]=0;
			coor[1]=0;
			linefollwer();
			compass=4;
		}
		else
		{
			if(compass==7)
			{
				left_degrees(85);
				_delay_ms(100);
				coor[0]=0;
				coor[1]=0;
				linefollwer();
				compass=4;
				
			}
			else
			{
				if(compass==5)
				{
					_delay_ms(100);
					coor[0]=0;
					coor[1]=0;
					linefollwer();
					compass=4;
					
				}
			}
		}
		flag1=flag1+1;
		
		recur=recur+1;
		boxnumber=D2[recur1][0];
		recur1=recur1+1;
		goto point1;
	}
	else
	{
		if(coor[0]==0 && coor[1]==0 && flag1%2==0)
		{
			if(compass==6)
			{
				right_degrees(85);
				_delay_ms(100);
				coor[0]=0;
				coor[1]=0;
				linefollwer();
				compass=4;
			}
			else
			{
				if(compass==7)
				{
					left_degrees(85);
					_delay_ms(100);
					coor[0]=0;
					coor[1]=0;
					linefollwer();
					compass=4;
					
				}
				else
				{
					if(compass==5)
					{
						_delay_ms(100);
						coor[0]=0;
						coor[1]=0;
						linefollwer();
						compass=4;
						
					}
					else
					{
							if(recur1==2)
							{
								stop();
								goto point2;
								
							}
					}
				}
			}				
			flag1=flag1+1;
			if(summ1[recur+1]<10)
			{
			   recur=recur+1;
			   help=summ1[recur];
			   goto point1;	
			}
		}
		else
		{
			if(flag1%2==0)
			{
				if(recur1==2)
				{
					stop();
					buzzer_on();
					_delay_ms(5000);
					buzzer_off();
				}
				else
				{
					final[0]=0;
					final[1]=0;
					goto point;
				}					
			}
			else
			{
				if(flag1%2!=0)
				{
					lcd_print(2,1,help,1);
					if(summ1[recur+1]<10)
					{
						recur=recur+1;
						help=summ1[recur];
						goto point1;
					}
					else
					{
						final[0]=0;
						final[1]=0;
						goto point;
					}
					
				}
			}
		}
	}	
	point2: buzzer_on();			

}

