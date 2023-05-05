#include "cpu.h"

int main(void) {
	pcb contexto;

	char* string = string_new();
	string_append(&string, "!!!Hello CPU!!!");
	puts(string);

	while(1)
	{
		char *instruccion;
		instruccion = list_get(contexto->program_counter);
		switch (instruccion)
		{
		case "set"{

		}
		}
	}



	return EXIT_SUCCESS;
}
