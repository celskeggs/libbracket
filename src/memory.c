#include "libbracket.h"

int strcmp(const char *s1, const char *s2) {
    while (1) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        } else if (*s1 == 0) {
            return 0;
        }
        s1++;
        s2++;
    }
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- > 0) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        } else if (*s1 == 0) {
            return 0;
        }
        s1++;
        s2++;
    }
    return 0;
}

size_t strlen(const char *s) {
    const char *c = s;
    while (*c) {
        c++;
    }
    return c - s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d8 = (uint8_t *) dest, *s8 = (uint8_t *) src;
    while (n-- > 0) {
        *d8++ = *s8++;
    }
    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *s8 = (uint8_t *) s;
    while (n-- > 0) {
        *s8++ = (uint8_t) c;
    }
    return s;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    uint8_t *s18 = (uint8_t *) s1, *s28 = (uint8_t *) s2;
    while (n-- > 0) {
        if (*s18 != *s28) {
            return *s18 - *s28;
        }
        s18++;
        s28++;
    }
    return 0;
}

static int starts_with(const char *haystack, const char *needle) {
    while (*haystack == *needle) {
        haystack++;
        needle++;
    }
    return *needle == 0;
}

const char *strstr(const char *haystack, const char *needle) {
    while (*haystack) {
        if (starts_with(haystack, needle)) {
            return haystack;
        }
        haystack++;
    }
    return NULL;
}

const char *strchr(const char *s, int c) {
    while (*s != c) {
        if (*s++ == 0) {
            return NULL;
        }
    }
    return s;
}

size_t strspn(const char *haystack, const char *accept) {
	size_t n = 0;
	while (strchr(accept, haystack[n]) != NULL) {
		n++;
	}
	return n;
}

size_t strcspn(const char *haystack, const char *reject) {
	size_t n = 0;
	while (strchr(reject, haystack[n]) == NULL) {
		n++;
	}
	return n;
}

const char *strpbrk(const char *s, const char *accept) {
	while (strchr(accept, *s) == NULL) {
		if (!*s++) {
			return NULL;
		}
	}
	return s;
}
