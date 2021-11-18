#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

// Storing operation codes (Opcodes)
typedef enum {
	OP_RETURN,
	OP_CONSTANT,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE
} OpCode;

// Stores the instructions dynamically
typedef struct {
	int count;
	int capacity;
	uint8_t* code;
	ValueArray constants;
	int* lines;
} Chunk;

// Constructor methods to create bytecode chunks
void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t, int line);
int addConstant(Chunk* chunk, Value value);

#endif
