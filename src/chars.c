#include "libbracket.h"

int isalpha(int c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int isdigit(int c) {
	return c >= '0' && c <= '9';
}

int isalnum(int c) {
	return isdigit(c) || isalpha(c);
}

int toupper(int c) {
	if (c >= 'A' && c <= 'Z') {
		return c | 32;
	} else {
		return c;
	}
}

int tolower(int c) {
	if (c >= 'a' && c <= 'z') {
		return c & ~32;
	} else {
		return c;
	}
}
