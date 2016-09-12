#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "E--_lexer.tab.h"

int is_hex(char c) {
	if (c >= 'a' && c <= 'f')
		return 1;
	else if (c >= 'A' && c <= 'F')
		return 1;
	else if (c >= '0' && c <= '9')
		return 1;
	else
		return 0;
}

char hex2byte(char c) {
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else if (c >= '0' && c <= '9')
		return c - '0';
	else {
		printf("[%s]ERROR: non-hex digit :%c\n", __func__, c);
		return 0;
	}
}

int extract_string_val(char *in, char *out) {
	int i, len, idx;
	
	len = strlen(in);
	assert(len < MAX_STR_LEN);

	idx = 0;
	assert(in[0] == '\"');
	for (i = 1; i < len; i++) {
		if (in[i] != '\\' && in[i] != '\"' && in[i] != '\n')
			out[idx++] = in[i];
		else if (in[i] == '\\') {
			switch (in[i + 1]) {
				case 'n':
					out[idx++] = '\n';
					break;
				case 't':
					out[idx++] = '\t';
					break;
				case '\"':
					out[idx++] = '\"';
					break;
				case 'r':
					out[idx++] = '\r';
					break;
				case '?':
					out[idx++] = '\?';
					break;
				case 'x':
					if (is_hex(in[i+2]) == 0) {
						fprintf(stderr, "unrecognized hex value:\\x%c\n", in[i+2]);
						out[idx++] = in[i];
						out[idx++] = in[i+1];
					} else if (is_hex(in[i+2]) == 1 && is_hex(in[i+3]) == 0) {
						out[idx++] = hex2byte(in[i+2]);
						i += 1;
					} else {
						out[idx++] = hex2byte(in[i+2]) * 16 + hex2byte(in[i+3]);
						i += 2;
					}

					break;
				case '\\':
					out[idx++] = '\\';
					break;
				default:
					fprintf(stderr, "unrecognized symbol: \\%c\n", in[i + 1]);
			}
			i += 1;
			continue;
		} else if (in[i] == '\"' || in[i] == '\n') {
			assert (i + 1 == len);
			out[idx++] = '\0';
			break;
		}
	}
	/* fprintf(stdout, "%s in: %s\n", __func__, in); */
	return 0;
}

int is_digit(char c) {
	if (c >= '0' && c <= '9')
		return 1;
	else
		return 0;
}
/**
 * line_info has fixed format: # line_num "filename" ...
 */
int extract_line_info(char *line_info, char *fname, int *lnum) {
	int i, idx, len;
	char num_str[MAX_STR_LEN];


	i = 0;
	len = strlen(line_info);
	while(is_digit(line_info[i]) == 0 && i < MAX_STR_LEN)
		i++;

	idx = 0;
	while(is_digit(line_info[i]) == 1 && idx < MAX_STR_LEN)
		num_str[idx++] = line_info[i++];

	num_str[idx] == '\0';
	*lnum = atoi(num_str);

	idx = 0;
	while(line_info[i] != '\"' && i < MAX_STR_LEN && line_info[i] != '\0')
		i++;
	
	if (line_info[i] == '\"') {
		i++;
		while(line_info[i] != '\"' && line_info[i] != '\0' && i < MAX_STR_LEN) {
			// skip directory name, eg. "tests/in10" will only keep "in10"
			if (line_info[i] == '/') {
				idx = 0;
				i++;
			}
			fname[idx++] = line_info[i++];
		}
		fname[idx] = '\0';
	}

	//printf("%s line_info:%s, fname:%s, lnum:%d\n", __func__, line_info, fname, *lnum);
	return 1;
}
