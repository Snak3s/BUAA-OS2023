#include <blib.h>

size_t strlen(const char *s) {
	const char *p = s;
	while (*p) {
		p++;
	}
	return p - s;
}

char *strcpy(char *dst, const char *src) {
	char *res = dst;
	while (*src) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
	char *res = dst;
	while (*src && n--) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strcat(char *dst, const char *src) {
	char *res = dst;
	while (*dst) {
		dst++;
	}
	while (*src) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

int strcmp(const char *s1, const char *s2) {
	while (*s1 || *s2) {
		if (*s1 != *s2)
			return *s1 - *s2;
		s1++;
		s2++;
	}
	return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			break;
		}
		s1++;
		s2++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	char *p = s;
	while (n--) {
		*p++ = c;
	}
	return s;
}

void *memcpy(void *out, const void *in, size_t n) {
	char *csrc = (char *)in;
	char *cdest = (char *)out;
	for (int i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	const char *p1 = s1;
	const char *p2 = s2;
	while (n--) {
		if (*p1 != *p2) {
			return *p1 - *p2;
		}
		p1++;
		p2++;
	}
	return 0;
}
