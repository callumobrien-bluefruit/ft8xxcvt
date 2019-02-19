#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN(x) (sizeof (x) / sizeof (x)[0])

static int convert(const uint16_t pixel[4])
{
	int i;
	long x;

	for (i = x = 0; i < 3; ++i)
		x += ntohs(pixel[i]);
	x /= 3;
	return x >> 12;
}

int main(void)
{
	uint32_t hdr[4], width, height;
	uint16_t pixel[4];
	div_t x;
	int i, out;

	if (fread(hdr, sizeof hdr[0], LEN(hdr), stdin) != LEN(hdr))
		goto err;
	if (memcmp("farbfeld", hdr, sizeof "farbfeld" - 1) != 0)
		goto err;
	width = ntohl(hdr[2]);
	height = ntohl(hdr[3]);

	x = div(width*height, 2);
	for (i = 0; i < x.quot + x.rem; ++i) {
		if (fread(pixel, sizeof pixel[0], LEN(pixel), stdin) != LEN(pixel))
			goto err;
		out = convert(pixel) << 4;

		if (fread(pixel, sizeof pixel[0], LEN(pixel), stdin) != LEN(pixel))
			goto err;
		out |= convert(pixel);

		putchar(out);
	}

	return 0;

	err:
	if (feof(stdin))
		fprintf(stderr, "ff2l: unexpected eof");
	else if (ferror(stdin))
		perror("ff2l");

	return 1;
}
