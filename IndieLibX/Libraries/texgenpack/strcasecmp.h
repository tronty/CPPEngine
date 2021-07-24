/*

Copyright (c) 2015 Harm Hanemaaijer <fgenfb@yahoo.com>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/


static void strtoupper(char *str) {
	int n = strlen(str);
	for (int i = 0; i < n; i++)
		str[i] = toupper(str[i]);
}

static int strcasecmp(const char *str1, const char *str2) {
	char *str1_upper = (char *)alloca(strlen(str1) + 1);
	strcpy(str1_upper, str1);
	strtoupper(str1_upper);
	char *str2_upper = (char *)alloca(strlen(str2) + 1);
	strcpy(str2_upper, str2);
	strtoupper(str2_upper);
	int r = strcmp(str1_upper, str2_upper);
	return r;
}

