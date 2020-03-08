#include <ijvm.h>
#include <binary.h>
#include <stdlib.h>

int pc = 0;
buffer_t *buffer;

int init_ijvm(char *binary_file)
{
    buffer = (buffer_t *)malloc(sizeof(buffer_t));
    if (!parse(buffer, binary_file)) {
        printf("DEBUG2");
        return -1;
    }
    return 1;
}

void destroy_ijvm()
{
    free(buffer->data);
    free(buffer->constants);
    free(buffer->text);
    free(buffer);
}

void run()
{
    while (step());
}

void set_input(FILE *fp)
{
  // TODO: implement me
}

void set_output(FILE *fp)
{
  // TODO: implement me
}

bool step() {
    switch (get_instruction()) {
        case OP_BIPUSH:
            printf("%s", "BIPUSH\n");
            break;
        case OP_DUP:
            printf("%s", "DUP\n");
            break;
        case OP_ERR:
            printf("%s", "ERR\n");
            break;
        case OP_GOTO:
            printf("%s", "GOTO\n");
            break;
        case OP_HALT:
            printf("%s", "HALT\n");
            break;
        case OP_IADD:
            printf("%s", "IADD\n");
            break;
        case OP_IAND:
            printf("%s", "IAND\n");
            break;
        case OP_IFEQ:
            printf("%s", "IFEQ\n");
            break;
        case OP_IFLT:
            printf("%s", "IFLT\n");
            break;
        case OP_ICMPEQ:
            printf("%s", "IF_ICMPEQ\n");
            break;
        case OP_IINC:
            printf("%s", "IINC\n");
            break;
        case OP_ILOAD:
            printf("%s", "ILOAD\n");
            break;
        case OP_IN:
            printf("%s", "IN\n");
            break;
        case OP_INVOKEVIRTUAL:
            printf("%s", "INVOKEVIRTUAL\n");
            break;
        case OP_IOR:
            printf("%s", "IOR\n");
            break;
        case OP_IRETURN:
            printf("%s", "IRETURN\n");
            break;
        case OP_ISTORE:
            printf("%s", "ISTORE\n");
            break;
        case OP_ISUB:
            printf("%s", "ISUB\n");
            break;
        case OP_LDC_W:
            printf("%s", "LDC_W\n");
            break;
        case OP_NOP:
            printf("%s", "NOP\n");
            break;
        case OP_OUT:
            printf("%s", "OUT\n");
            break;
        case OP_POP:
            printf("%s", "POP\n");
            break;
        case OP_SWAP:
            printf("%s", "SWAP\n");
            break;
        case OP_WIDE:
            printf("%s", "WIDE\n");
            break;
    }
    if (pc < text_size() - 1) {
        pc += 1;
        return true;
    } else {
        return false;
    }
}

byte_t *get_text() {
    return buffer->text;
}

int get_program_counter() {
    return pc;
}

byte_t get_instruction() {
    return buffer->text[pc];
}

int text_size() {
    return buffer->text_size;
}