#include <ijvm.h>
#include <binary.h>
#include <stack.h>
#include <stdlib.h>
#include <util.h>

int pc;
buffer_t *buffer;
FILE *out;
FILE *in;

int init_ijvm(char *binary_file) {
    pc = 0;
    buffer = (buffer_t *)malloc(sizeof(buffer_t));
    if (parse(buffer, binary_file) < 0) return -1;

    if (init_stack(1000) < 0) return -1;

    return 1;
}

void destroy_ijvm() {
    destroy_stack();

    free(buffer->data);
    free(buffer->constants);
    free(buffer->text);
    free(buffer);
}

void run() {
    while (step());
}

void set_input(FILE *fp) {
    in = fp;
}

void set_output(FILE *fp) {
    out = fp;
}

void doBIPUSH() {
    push(buffer->text[pc + 1]);
    pc+=2;
}

void doDUP() {
    pc++;
}

void doERR() {
    pc++;
}

void doGOTO() {
    pc+=5;
}

void doHALT() {
    pc++;
}

void doIADD() {
    word_t A = tos();
    pop();
    word_t B = tos();
    pop();
    push(A + B);
    pc++;
}

void doIAND() {
    word_t A = tos();
    pop();
    word_t B = tos();
    pop();
    push(A & B);
    pc++;
}

void doIFEQ() {
    pc+=5;
}

void doIFLT() {
    pc+=5;
}

void doICMPEQ() {
    pc+=5;
}

void doIINC() {
    pc+=3;
}

void doILOAD() {
    pc+=2;
}

void doIN() {
    pc++;
}

void doINVOKEVIRTUAL() {
    pc+=5;
}

void doIOR() {
    word_t A = tos();
    pop();
    word_t B = tos();
    pop();
    push(A | B);
    pc++;
}

void doIRETURN() {
    pc++;
}

void doISTORE() {
    pc+=2;
}

void doISUB() {
    word_t A = tos();
    pop();
    word_t B = tos();
    pop();
    push(B - A);
    pc++;
}

void doLDC_W() {
    pc+=5;
}

void doNOP() {
    pc++;
}

void doOUT() {
    pc++;
}

void doPOP() {
    pop();
    pc++;
}

void doSWAP() {
    word_t A = tos();
    pop();
    word_t B = tos();
    pop();
    push(A);
    push(B);
    pc++;
}

void doWIDE() {
    pc++;
}

bool step() {
    switch (get_instruction()) {
        case OP_BIPUSH:
            doBIPUSH();
            break;
        case OP_DUP:
            printf("%s", "DUP\n");
            doDUP();
            break;
        case OP_ERR:
            printf("%s", "ERR\n");
            doERR();
            break;
        case OP_GOTO:
            printf("%s", "GOTO\n");
            doGOTO();
            break;
        case OP_HALT:
            printf("%s", "HALT\n");
            doHALT();
            break;
        case OP_IADD:
            printf("%s", "IADD\n");
            doIADD();
            break;
        case OP_IAND:
            printf("%s", "IAND\n");
            doIAND();
            break;
        case OP_IFEQ:
            printf("%s", "IFEQ\n");
            doIFEQ();
            break;
        case OP_IFLT:
            printf("%s", "IFLT\n");
            doIFLT();
            break;
        case OP_ICMPEQ:
            printf("%s", "IF_ICMPEQ\n");
            doICMPEQ();
            break;
        case OP_IINC:
            printf("%s", "IINC\n");
            doIINC();
            break;
        case OP_ILOAD:
            printf("%s", "ILOAD\n");
            doILOAD();
            break;
        case OP_IN:
            printf("%s", "IN\n");
            doIN();
            break;
        case OP_INVOKEVIRTUAL:
            printf("%s", "INVOKEVIRTUAL\n");
            doINVOKEVIRTUAL();
            break;
        case OP_IOR:
            printf("%s", "IOR\n");
            doIOR();
            break;
        case OP_IRETURN:
            printf("%s", "IRETURN\n");
            doIRETURN();
            break;
        case OP_ISTORE:
            printf("%s", "ISTORE\n");
            doISTORE();
            break;
        case OP_ISUB:
            printf("%s", "ISUB\n");
            doISUB();
            break;
        case OP_LDC_W:
            printf("%s", "LDC_W\n");
            doLDC_W();
            break;
        case OP_NOP:
            printf("%s", "NOP\n");
            doNOP();
            break;
        case OP_OUT:
            printf("%s", "OUT\n");
            doOUT();
            break;
        case OP_POP:
            printf("%s", "POP\n");
            doPOP();
            break;
        case OP_SWAP:
            printf("%s", "SWAP\n");
            doSWAP();
            break;
        case OP_WIDE:
            printf("%s", "WIDE\n");
            doWIDE();
            break;
    }
    if (pc < text_size()) {
        return true;
    }
    return false;
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