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
    if (init_buffer(buffer, binary_file) < 0) return -1;

    if (init_frame(NULL, 1000, 1000, 0, 0) < 0) return -1;

    set_output(stderr);

    return 1;
}

void destroy_ijvm() {
    while (frame != NULL) {
        fprintf(out, "1");
        destroy_frame();
    }

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
    fprintf(out, "S_BIPUSH\n");
    push(buffer->text[pc + 1]);
    pc+=2;
    fprintf(out, "E_BIPUSH\n");
}

void doDUP() {
    word_t A = tos();
    push(A);
    pc++;
}

void doERR() {
    pc++;
}

void doGOTO() {
    pc += to_short(buffer->text[pc + 1], buffer->text[pc + 2]);;
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
    word_t A = tos();
    pop();
    if (A == 0) {
        pc += to_short(buffer->text[pc + 1], buffer->text[pc + 2]);
    } else {
        pc+=3;
    }
}

void doIFLT() {
    word_t A = tos();
    pop();
    if (A < 0) {
        pc += to_short(buffer->text[pc + 1], buffer->text[pc + 2]);
    } else {
        pc+=3;
    }
}

void doICMPEQ() {
    word_t A = tos();
    pop();
    word_t B = tos();
    pop();
    if (A == B) {
        pc += to_short(buffer->text[pc + 1], buffer->text[pc + 2]);
    } else {
        pc+=3;
    }
}

void doIINC() {
    int offset = buffer->text[pc + 1];
    byte_t constant = (char) buffer->text[pc + 2];
    frame->local_data[offset] += constant;
    pc+=3;
}

void doILOAD() {
    int offset = buffer->text[pc + 1];
    push(frame->local_data[offset]);
    pc+=2;
}

void doIN() {
    pc++;
}

void doINVOKEVIRTUAL() {
    fprintf(out, "S_INVOKE\n");
    int const_offset = to_short(buffer->text[pc + 1], buffer->text[pc + 2]);
    int routine_offset = buffer->constants[const_offset];
    int max_local_size = to_short(buffer->text[routine_offset + 2], buffer->text[routine_offset + 3]);
    int n_args = to_short(buffer->text[routine_offset], buffer->text[routine_offset + 1]);
    init_frame(frame, 10000, max_local_size, pc + 3, n_args);
    pc = routine_offset + 4;
    fprintf(out, "E_INVOKE\n");
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
    pc = frame->prev_pc;
    destroy_frame();
}

void doISTORE() {
    int offset = buffer->text[pc + 1];
    word_t A = tos();
    pop();
    frame->local_data[offset] = A;
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
    push(get_constant(to_short(buffer->text[pc + 1], buffer->text[pc + 2])));
    pc+=3;
}

void doNOP() {
    pc++;
}

void doOUT() {
    word_t A = tos();
    pop();
    fprintf(out, "%c", A);
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
            doDUP();
            break;
        case OP_ERR:
            printf("%s", "ERR\n");
            doERR();
            break;
        case OP_GOTO:
            doGOTO();
            break;
        case OP_HALT:
            doHALT();
            break;
        case OP_IADD:
            doIADD();
            break;
        case OP_IAND:
            printf("%s", "IAND\n");
            doIAND();
            break;
        case OP_IFEQ:
            doIFEQ();
            break;
        case OP_IFLT:
            doIFLT();
            break;
        case OP_ICMPEQ:
            doICMPEQ();
            break;
        case OP_IINC:
            doIINC();
            break;
        case OP_ILOAD:
            doILOAD();
            break;
        case OP_IN:
            printf("%s", "IN\n");
            doIN();
            break;
        case OP_INVOKEVIRTUAL:
            doINVOKEVIRTUAL();
            break;
        case OP_IOR:
            printf("%s", "IOR\n");
            doIOR();
            break;
        case OP_IRETURN:
            doIRETURN();
            break;
        case OP_ISTORE:
            doISTORE();
            break;
        case OP_ISUB:
            doISUB();
            break;
        case OP_LDC_W:
            doLDC_W();
            break;
        case OP_NOP:
            printf("%s", "NOP\n");
            doNOP();
            break;
        case OP_OUT:
            doOUT();
            break;
        case OP_POP:
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

word_t get_local_variable(int i) {
    return (int8_t) frame->local_data[i];
}

word_t get_constant(int i) {
    return buffer->constants[i];
}