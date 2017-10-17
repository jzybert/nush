#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "vector.h"
#include "parse.h"

char*
read_and_or(const char* text, int ii)
{
	if ((text[ii] == '|' && text[ii + 1] == '|') || (text[ii] == '&' && text[ii + 1] == '&')) {
		char* andOr = malloc(3);
		memcpy(andOr, text + ii, 2);
		andOr[2] = 0;
		return andOr;
	} else {
		char* andOr = malloc(2);
		memcpy(andOr, text + ii, 1);
		andOr[1] = 0;
		return andOr;
	}
}

char*
read_str(const char* text, int ii)
{
	int nn = 0;
	while(text[ii + nn] != '|' && text[ii + nn] != '&' && text[ii + nn] != '<' && text[ii + nn] != '>' && text[ii + nn] != ';' && !isspace(text[ii + nn])) {
		++nn;
	}
	
	char* str = malloc(nn + 1);
	memcpy(str, text + ii, nn);
	str[nn] = 0;
	return str;
}

vector*
tokenize(const char* text)
{
        vector* sv = make_vector();
	int nn = strlen(text);
	int ii = 0;
        while(ii < nn) { // loop through all characters in text
		if (isspace(text[ii])) {
			++ii;
			continue;
		}

		if (text[ii] == ';' || text[ii] == '<' || text[ii] == '>') {
			char op[4] = "x";
			op[0] = text[ii];
			vector_push_back(sv, op);
			++ii;
			continue;
		}
		
		if (text[ii] == '|' || text[ii] == '&') {
			char* andOr = read_and_or(text, ii);
			vector_push_back(sv, andOr);
			ii += strlen(andOr);
			free(andOr);
			continue;
		}
		
		// Else, character
		char* str = read_str(text, ii);
		vector_push_back(sv, str);
		ii += strlen(str);
		free(str);
	}
	
	return sv;
}

vector*
shell_parse(char* text)
{
	vector* tokens = tokenize(text);
	return tokens;
}
