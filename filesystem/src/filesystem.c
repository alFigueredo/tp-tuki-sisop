#include "filesystem.h"

int main(void) {
	char* string = string_new();
	string_append(&string, "!!!Hello File System!!!");
	puts(string);
	return EXIT_SUCCESS;
}
