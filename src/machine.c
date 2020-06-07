#include <ijvm.h>
#include <parser.h>
#include <frame.h>
#include <stdlib.h>
#include <util.h>

int pc;
FILE *out;
FILE *in;
bool isfinished = false;

int init_ijvm(char *binary_file) {
    pc = 0;
    if (init_buffer(binary_file) < 0) return -1;

    if (init_frame(NULL, 0, 0, 0) < 0) return -1;

    set_output(stderr);

    return 1;
}

void destroy_ijvm() {
    while (frame != NULL) {
        destroy_frame();
    }
    destroy_buffer();
}

void run() {
    while (step());
}

void doBIPUSH() {
    int8_t value = get_text()[pc + 1];
    push(value);
    pc+=2;
}

void doDUP() {
    word_t A = tos();
    push(A);
    pc++;
}

void doERR() {
    fprintf(out, "Error message... HALT");
    pc = text_size();
}

void doGOTO() {
    pc += read_short(pc + 1);
}

void doHALT() {
    pc = text_size();
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
        pc += read_short(pc + 1);
    } else {
        pc+=3;
    }
}

void doIFLT() {
    word_t A = tos();
    pop();
    if (A < 0) {
        pc += read_short(pc + 1);
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
        pc += read_short(pc + 1);
    } else {
        pc+=3;
    }
}

void doIINC() {
    byte_t index = get_text()[pc + 1];
    int8_t constant = get_text()[pc + 2];
    set_local_variable(index, get_local_variable(index) + constant);
    pc+=3;
}

void doILOAD() {
    byte_t index = get_text()[pc + 1];
    push(get_local_variable(index));
    pc+=2;
}

void doIN() {
    word_t input = fgetc(in);
    if (input == EOF) {
        push( 0);
    } else {
        push(input);
    }
    pc++;
}

void doINVOKEVIRTUAL() {
    unsigned short const_offset = read_short(pc + 1);
    int routine_offset = get_constant(const_offset);
    unsigned short n_args = read_short(routine_offset) - 1;
    unsigned short local_size = read_short(routine_offset + 2);
    word_t *arguments = frame->stack_data + frame->stack_size - n_args;
    init_frame(frame, local_size + n_args + 1, pc + 3, n_args);

    // push the arguments onto the new frame's local data
    for (int i = 0; i < n_args; ++i) {
        frame->local_data[i + 1] = arguments[i];
    }

    pc = routine_offset + 4;
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
    word_t return_value = tos();
    int n_args = frame->n_args;

    frame_t *prev = frame->prev_frame;
    free(frame->local_data);
    free(frame->stack_data);
    free(frame);
    frame = prev;

    if (frame == NULL) {
        return;
    }

    for (int i = 0; i < n_args + 1; ++i) {
        pop();
    }
    push(return_value);
}

void doISTORE() {
    byte_t offset = get_text()[pc + 1];
    word_t A = tos();
    pop();
    set_local_variable(offset, A);
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
    push(get_constant(read_short(pc + 1)));
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

void doIINCWIDE() {
    unsigned short index = read_short(pc + 1);
    byte_t constant = get_text()[pc + 3];
    set_local_variable(index, get_local_variable(index) + constant);
    pc+=4;
}

void doILOADWIDE() {
    unsigned short index = read_short(pc + 1);
    push(get_local_variable(index));
    pc+=3;
}

void doISTOREWIDE() {
    unsigned short index = read_short(pc + 1);
    word_t A = tos();
    pop();
    set_local_variable(index, A);
    pc+=3;
}

void doWIDE() {
    pc++;
    switch (get_instruction()) {
        case OP_IINC:
            doIINCWIDE();
            break;
        case OP_ILOAD:
            doILOADWIDE();
            break;
        case OP_ISTORE:
            doISTOREWIDE();
            break;
        default:
            doERR();
    }
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
            doIN();
            break;
        case OP_INVOKEVIRTUAL:
            doINVOKEVIRTUAL();
            break;
        case OP_IOR:
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
            doNOP();
            break;
        case OP_OUT:
            doOUT();
            break;
        case OP_POP:
            doPOP();
            break;
        case OP_SWAP:
            doSWAP();
            break;
        case OP_WIDE:
            doWIDE();
            break;
        default:
            doERR();
    }
    if (pc < text_size()) {
        isfinished = true;
        return true;
    }
    return false;
}

int get_program_counter() {
    return pc;
}

byte_t get_instruction() {
    return get_text()[pc];
}

bool finished() {
    return isfinished;
}

void set_input(FILE *fp) {
    in = fp;
}

void set_output(FILE *fp) {
    out = fp;
}
