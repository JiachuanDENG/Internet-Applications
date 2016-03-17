#include <stdio.h>
#include <stdlib.h>

int *transfer(unsigned int num)
{
	unsigned int month, day;
	static int var[2];
	
	if (num > 0 && num <= 31)
	{
		month = 1;
		day = num;
	}
	if (num > 31 && num <= 59)
	{
		month = 2;
		day = num - 31;
	}
	if (num > 59 && num <= 90)
	{
		month = 3;
		day = num - 59;
	}
	if (num > 90 && num <= 120)
	{
		month = 4;
		day = num - 90;
	}
	if (num > 120 && num <= 151)
	{
		month = 5;
		day = num - 120;
	}
	if (num > 151 && num <= 181)
	{
		month = 6;
		day = num - 151;
	}
	if (num > 181 && num <= 212)
	{
		month = 7;
		day = num - 181;
	}
	if (num > 212 && num < 243)
	{
		month = 8;
		day = num - 212;
	}
	if (num > 243 && num <= 273)
	{
		month = 9;
		day = num - 243;
	}
	if (num > 273 && num <= 304)
	{
		month = 10;
		day = num - 273;
	}
	if (num > 304 && num <= 334)
	{
		month = 11;
		day = num - 304;
	}
	if (num > 334 && num <= 365)
	{
		month = 12;
		day = num - 334;
	}
	
	var[0] = month;
	var[1] = day;
	
	return var;
}

int main()
{
	unsigned int c;
	int *p;
	unsigned int mm, dd;

	printf("This is a program for Lab 3, and I'm going to ask you to type in a number.\n");
	printf("Then I can generate the number you type-in to the day of the number-th day in 2013.\n");
	printf("\nPlease do me a favor & make sure the number you type in is between 1 and 365!\n");

	printf("\nEnter a value : ");
	c = getchar();
	printf("\ncalculating......\n");
	p = transfer(c);
	mm = p[0];
	dd = p[1];
	
	printf("\nThe generated day is: 2013 ");

	switch(mm)
	{
	case 1:
		printf("JAN %d\n", dd);
		break;
	case 2:
		printf("FEB %d\n", dd);
		break;
	case 3:
		printf("MAR %d\n", dd);
		break;
	case 4:
		printf("APR %d\n", dd);
		break;
	case 5:
		printf("MAY %d\n", dd);
		break;
	case 6:
		printf("JUN %d\n", dd);
		break;
	case 7:
		printf("JUL %d\n", dd);
		break;
	case 8:
		printf("AUG %d\n", dd);
		break;
	case 9:
		printf("SEP %d\n", dd);
		break;
	case 10:
		printf("OCT %d\n", dd);
		break;
	case 11:
		printf("NOV %d\n", dd);
		break;
	case 12:
		printf("DEC %d\n", dd);
		break;
	default:
		printf("......\nERROR!\nInvalid Input!\n");
	}
}
