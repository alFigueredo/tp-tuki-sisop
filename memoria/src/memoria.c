#include "memoria.h"

int main(void) {
	char* string = string_new();
	string_append(&string, "!!!Hello Memory!!!");
	puts(string);
	return EXIT_SUCCESS;
}
