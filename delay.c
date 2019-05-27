#include <intrins.h>

void Delay100us()		//@6.000MHz
{
	unsigned char i, j;

	i = 1;
	j = 146;
	do
	{
		while (--j);
	} while (--i);
}
