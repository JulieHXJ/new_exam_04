#include <unistd.h>

void print_bits(unsigned char octet)
{
	int i = 7;
	unsigned char bit;

	while (i >= 0)
	{
		bit = '0' + ((octet >> i) & 1);
		write(1, &bit, 1);
		i--;
	}
}

unsigned char reverse_bits(unsigned char octet)
{
	int i = 0;
	unsigned char bit = 0;
	while (i < 8)
	{
		bit = bit * 2 + ((octet >> i) & 1);
		// same meaning but better: bit = (bit << 1) | ((octet >> i) & 1);

		i++;
	}
	return (bit);
	
}

unsigned char swap_bits(unsigned char octet)
{
	// the order doesn't matter
	return ((octet >> 4) | (octet << 4));
}

int main(void)
{
	unsigned char octet = 5; // Example value
	print_bits(octet);
	write(1, "\n", 1);
	char reversed =	reverse_bits(octet);
	print_bits(reversed);
	write(1, "\n", 1);
	char swapped = swap_bits(octet);
	print_bits(swapped);
	write(1, "\n", 1);
	return 0;
}