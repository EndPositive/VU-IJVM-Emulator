#include <ijvm.h>
#include <parser.h>
#include <frame.h>
#include <stdlib.h>
#include <util.h>
#include <heap.h>
#include <net.h>
#include <machine.h>

int pc;
FILE *out;
FILE *in;
bool is_finished;

int init_ijvm(char *binary_file) {
    pc = 0;
    if (init_buffer(binary_file) < 0) return -1;

    frame = init_frame(NULL, 0, 0);

    if (init_arrays() < 0) return -1;

    set_output(stdout);
    set_input(stdin);

    is_finished = false;

    return 1;
}

void destroy_ijvm() {
    while (frame != NULL) destroy_frame();
    destroy_arrays();
    destroy_buffer();
}

void run() {
    while (step());
}

void doBIPUSH() {
    int8_t value = (int8_t) get_text()[pc + 1];
    push(value);
    pc+=2;
}

void doDUP() {
    word_t A = tos();
    push(A);
    pc++;
}

void doERR(const char *msg) {
    fprintf(out, "%s, quitting...", msg);
    destroy_ijvm();
    exit(1);
}

void doGOTO() {
    short offset = read_short(pc + 1);
    if (pc + offset < 0 || pc + offset > text_size()) doERR("GOTO out of bounds");
    pc += offset;
}

void doHALT() {
    pc = text_size();
}

void doIADD() {
    word_t A = pop();
    word_t B = pop();
    push((word_t) ((unsigned)A + (unsigned) B));
    pc++;
}

void doIAND() {
    word_t A = pop();
    word_t B = pop();
    push(A & B);
    pc++;
}

void doIFEQ() {
    word_t A = pop();
    if (A == 0) {
        short offset = read_short(pc + 1);
        if (pc + offset < 0 || pc + offset > text_size()) doERR("IFEW out of bounds");
        pc += offset;
    } else {
        pc+=3;
    }
}

void doIFLT() {
    word_t A = pop();
    if (A < 0) {
        short offset = read_short(pc + 1);
        if (pc + offset < 0 || pc + offset > text_size()) doERR("IFTL out of bounds");
        pc += read_short(pc + 1);
    } else {
        pc+=3;
    }
}

void doICMPEQ() {
    word_t A = pop();
    word_t B = pop();
    if (A == B) {
        short offset = read_short(pc + 1);
        if (pc + offset < 0 || pc + offset > text_size()) doERR("ICMPEQ out of bounds");
        pc += read_short(pc + 1);
    } else {
        pc+=3;
    }
}

void doIINC() {
    byte_t index;
    int8_t constant;
    if (pc + 1 >= text_size()) doERR("Reading text out of bounds");
    index = get_text()[pc + 1];
    constant = (int8_t) get_text()[pc + 2];
    set_local_variable(index, get_local_variable(index) + constant);
    pc+=3;
}

void doILOAD() {
    byte_t index;
    if (pc + 1 >= text_size()) doERR("Reading text out of bounds");
    index = get_text()[pc + 1];
    push(get_local_variable(index));
    pc+=2;
}

void doIN() {
    word_t input = fgetc(in);
    if (input == EOF) push( 0);
    else push(input);
    pc++;
}

void doINVOKEVIRTUAL() {
    unsigned short pre_const_offset, n_args, local_size, total_local_size;
    int routine_offset;
    frame_t  *new_frame;

    pre_const_offset = read_unsigned_short(pc + 1);
    routine_offset = get_constant(pre_const_offset);
    if (routine_offset + 4 >= text_size() || routine_offset < 0) doERR("Invokevirtual out of bounds");
    n_args = (unsigned short) read_unsigned_short(routine_offset);
    local_size = (unsigned short) read_unsigned_short(routine_offset + 2);
    total_local_size = (unsigned short) ushrt_safe_addition(n_args, local_size);

    new_frame = init_frame(frame, total_local_size, pc + 3);

    for (int i = n_args - 1; i > 0; i--) {
        new_frame->local_data[i] = pop();
    }
    //BYE OBJREF :(
    pop();

    frame = new_frame;

    pc = routine_offset + 4;
}

void doIOR() {
    word_t A = pop();
    word_t B = pop();
    push(A | B);
    pc++;
}

void doIRETURN() {
    word_t return_value;
    frame_t *prev;
    pc = frame->prev_pc;
    return_value = tos();

    prev = frame->prev_frame;
    free(frame->local_data);
    free(frame->stack_data);
    free(frame);
    frame = prev;

    if (frame == NULL) doERR("Returned from last frame");

    push(return_value);
    detect_garbage();
}

void doISTORE() {
    byte_t offset;
    word_t A;
    if (pc + 1 >= text_size()) doERR("Reading text out of bounds");
    offset = get_text()[pc + 1];
    A = pop();
    set_local_variable(offset, A);
    pc+=2;
}

void doISUB() {
    word_t A = pop();
    word_t B = pop();
    push(B - A);
    pc++;
}

void doLDC_W() {
    push(get_constant(read_unsigned_short(pc + 1)));
    pc+=3;
}

void doNOP() {
    pc++;
}

void doOUT() {
    word_t A = pop();
    fprintf(out, "%c", A);
    pc++;
}

void doPOP() {
    pop();
    pc++;
}

void doSWAP() {
    word_t A = pop();
    word_t B = pop();
    push(A);
    push(B);
    pc++;
}

void doIINCWIDE() {
    byte_t constant;
    unsigned short index = read_unsigned_short(pc + 1);
    if (pc + 3 >= text_size()) doERR("Reading text out of bounds");
    constant = get_text()[pc + 3];
    set_local_variable(index, get_local_variable(index) + constant);
    pc+=4;
}

void doILOADWIDE() {
    unsigned short index = read_unsigned_short(pc + 1);
    push(get_local_variable(index));
    pc+=3;
}

void doISTOREWIDE() {
    unsigned short index = read_unsigned_short(pc + 1);
    word_t A = pop();
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
            doERR("Unknown OP code after WIDE");
    }
}

void doNEWARRAY() {
    unsigned int count = (unsigned int) pop();
    word_t array_ref = new_array(count);
    push(array_ref);
    pc+=1;
}

void doIALOAD() {
    unsigned short index;
    word_t ref = pop();
    array_t *array = get_array(ref);
    if (array == NULL) doERR("Unknown array ref");

    index = (unsigned short) pop();
    if (index > array->size) doERR("Incorrect array index");
    push(array->data[index]);
    pc+=1;
}

void doIASTORE() {
    unsigned short index;
    word_t value;
    word_t ref = pop();
    array_t *array = get_array(ref);
    if (array == NULL) doERR("Unknown array ref");

    index = (unsigned short) pop();
    value = pop();
    if (index > array->size) doERR("Array index out of bounds");

    array->data[index] = value;
    pc+=1;
}

void doGC() {
    detect_garbage();
    pc++;
}

void doNETBIND() {
    int port = pop();
    word_t netref = net_bind(port);
    push(netref);
    pc++;
}

void doNETCONNECT() {
    word_t port = pop();
    unsigned int host = swap_uint32((unsigned int) pop());
    word_t netref = net_connect(port, host);
    push(netref);
    pc++;
}

void doNETIN() {
    word_t netref = pop();
    word_t input = net_recv(netref);
    if (input == -1) doERR("Error while receiving");
    else push(input);
    pc++;
}

void doNETOUT() {
    char output = (char) pop();
    word_t netref = pop();
    if (net_send(netref, output) == -1) doERR("Error while sending");
    pc++;
}

void doNETCLOSE() {
    word_t netref = pop();
    if (net_close(netref) == -1) doERR("Error while closing");
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
            doERR("Error OP code called");
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
        case OP_NEWARRAY:
            doNEWARRAY();
            break;
        case OP_IALOAD:
            doIALOAD();
            break;
        case OP_IASTORE:
            doIASTORE();
            break;
        case OP_GC:
            doGC();
            break;
        case OP_NETBIND:
            doNETBIND();
            break;
        case OP_NETCONNECT:
            doNETCONNECT();
            break;
        case OP_NETIN:
            doNETIN();
            break;
        case OP_NETOUT:
            doNETOUT();
            break;
        case OP_NETCLOSE:
            doNETCLOSE();
            break;
        default:
            doERR("Unknown OP code");
    }

    if (pc >= text_size()) is_finished = true;
    return !is_finished;
}

int get_program_counter() {
    return pc;
}

byte_t get_instruction() {
    if (pc >= text_size()) doERR("Reading instructions out of bounds");
    return get_text()[pc];
}

bool finished() {
    return is_finished;
}

void set_input(FILE *fp) {
    in = fp;
}

void set_output(FILE *fp) {
    out = fp;
}
