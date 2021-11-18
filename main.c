#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

#include <editline/readline.h>

static void repl() {
	for (;;) {
		char* input = readline("cascade~ ");
		add_history(input);
		interpret(input);
		free(input);
	}
}

static char* readFile(const char* path) {
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		fprintf(stderr, "Could not open file \"%s\".\n", path);
		exit(74);
	}

	fseek(file, 0L, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	char* buffer = (char*) malloc(fileSize + 1);
	size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
	buffer[bytesRead] = '\0';
	fclose(file);
	return buffer;
}

static void runFile(const char* path) {
	char* source = readFile(path);
	InterpretResult result = interpret(source);
	free(source);

	if (result == INTERPRET_COMPILE_ERROR) exit(65);
	if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int agrc, const char* argv[]) {
	initVM();

	if (argc == 1) {
		repl();
	}
	else if (argc == 2) {
		runFile(argv[1]);
	}
	else {
		fprintf(stderr, "Usage: cascade [path]\n");
		exit(64);
	}

	freeVM();
	return 0;
}
