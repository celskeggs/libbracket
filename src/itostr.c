#include "libbracket.h"

int itostr(int64_t value, char *output, size_t len) {
	if (len < 2) {
		if (len == 1) {
			output[0] = '\0';
		}
		return 0;
	}
	if (value < 0) {
		output[0] = '-';
		return 1 + utostr((uint64_t) -value, output + 1, len - 1);
	} else {
		return utostr((uint64_t) value, output, len);
	}
}

int utostr(uint64_t value, char *output, size_t len) {
	if (len < 2) {
		if (len == 1) {
			output[0] = '\0';
		}
		return 0;
	}
	char buf[24];
	int bufi = 0;
	while (value) {
		if (bufi > 21) {
			__builtin_trap(); // should not happen!
		}
		bufi++;
		buf[24 - bufi] = '0' + (value % 10);
		value /= 10;
	}
	if (bufi > len - 1) {
		bufi = len - 1;
	}
	memcpy(output, buf + sizeof(buf) - bufi, bufi);
	output[bufi] = '\0';
	return bufi;
}
