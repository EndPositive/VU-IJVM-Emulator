#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ijvm.h>
#include <frame.h>
#include <heap.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIB_IMPLEMENTATION

#include "../lib/nuklear.h"
#include "../lib/nuklear_xlib.h"
#include "../lib/style.c"

#define WINDOW_WIDTH    500
#define WINDOW_HEIGHT   500

typedef struct XWindow XWindow;
struct XWindow {
    Display *dpy;
    Window root;
    Visual *vis;
    Colormap cmap;
    XWindowAttributes attr;
    XSetWindowAttributes swa;
    Window win;
    int screen;
    XFont *font;
    unsigned int width;
    unsigned int height;
    Atom wm_delete_window;
};

bool running = true;

static void die(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

int init_gui(int argc, char **argv) {
    XWindow xw;
    struct nk_context *ctx;
    FILE *input_file = NULL;
    FILE *output_file = NULL;

    /* IJVM */
    if (argc < 2) {
        die("Usage: ./ijvm-gui binary");
    }

    if (init_ijvm(argv[1]) < 0) {
        die("Couldn't load binary %s", argv[1]);
    }

    output_file = fopen("tmp_output", "w+");
    if (output_file == NULL) {
        die("Couldn't open output");
    }
    set_output(output_file);

    /* X11 */
    memset(&xw, 0, sizeof xw);
    xw.dpy = XOpenDisplay(NULL);
    if (!xw.dpy) die("Could not open a display; perhaps $DISPLAY is not set?");
    xw.root = DefaultRootWindow(xw.dpy);
    // Get some screen info
    XWindowAttributes root_attr;
    XGetWindowAttributes(xw.dpy, xw.root, &root_attr);
    xw.screen = XDefaultScreen(xw.dpy);
    xw.vis = XDefaultVisual(xw.dpy, xw.screen);
    xw.cmap = XCreateColormap(xw.dpy, xw.root, xw.vis, AllocNone);

    xw.swa.colormap = xw.cmap;
    xw.swa.event_mask =
            ExposureMask | KeyPressMask | KeyReleaseMask |
            ButtonPress | ButtonReleaseMask | ButtonMotionMask |
            Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask |
            PointerMotionMask | KeymapStateMask;
    xw.win = XCreateWindow(xw.dpy, xw.root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                           XDefaultDepth(xw.dpy, xw.screen), InputOutput,
                           xw.vis, CWEventMask | CWColormap, &xw.swa);


    XStoreName(xw.dpy, xw.win, argv[1]);
    XMapWindow(xw.dpy, xw.win);
    xw.wm_delete_window = XInternAtom(xw.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(xw.dpy, xw.win, &xw.wm_delete_window, 1);
    XGetWindowAttributes(xw.dpy, xw.win, &xw.attr);
    xw.width = (unsigned int) xw.attr.width;
    xw.height = (unsigned int) xw.attr.height;

    /* GUI */
    xw.font = nk_xfont_create(xw.dpy, "fixed");
    ctx = nk_xlib_init(xw.font, xw.dpy, xw.screen, xw.win, xw.width, xw.height);

    set_style(ctx);

    while (1) {
        XGetWindowAttributes(xw.dpy, xw.win, &xw.attr);
        xw.width = (unsigned int) xw.attr.width;
        xw.height = (unsigned int) xw.attr.height;

        /* Input */
        XEvent evt;
        nk_input_begin(ctx);
        while (XPending(xw.dpy)) {
            XNextEvent(xw.dpy, &evt);
            if (evt.type == ClientMessage) {
                running = false;
                goto cleanup;
            }
            if (XFilterEvent(&evt, xw.win)) continue;
            nk_xlib_handle_event(xw.dpy, xw.screen, xw.win, &evt);
        }
        nk_input_end(ctx);

        /* GUI */
        if (nk_begin(ctx, "IJVM", nk_rect(0, 0, xw.width, xw.height), NK_WINDOW_BORDER)) {
            // Program info
            nk_layout_row_dynamic(ctx, 10, 1);
            char program_s[512] = "Program loaded: ";
            nk_label(ctx,  strcat(program_s, argv[1]), NK_TEXT_LEFT);

            // Program input
            if (nk_tree_push(ctx, NK_TREE_TAB, "Input", NK_MINIMIZED)) {
                static char input_buffer[64];
                static int input_len;
                int max_input_size = 64;
                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "Input:", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 200, 1);
                nk_edit_string(ctx, NK_EDIT_BOX, input_buffer, &input_len, max_input_size, nk_filter_default);
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "(re)load input")) {
                    if (input_file != NULL) fclose(input_file);
                    input_file = fopen("tmp_input", "w+");

                    fseek(input_file, 0, SEEK_SET);
                    for (int i = 0; i < input_len; i++) {
                        putc(input_buffer[i], input_file);
                    }
                    fclose(input_file);
                    input_file = fopen("tmp_input", "r+");
                    set_input(input_file);
                }
                nk_tree_pop(ctx);
            }

            // Program output
            if (nk_tree_push(ctx, NK_TREE_TAB, "Output", NK_MINIMIZED)) {
                nk_layout_row_dynamic(ctx, 10, 1);
                nk_label(ctx, "Output:", NK_TEXT_LEFT);
                int max_output_size = 10000;
                int i;
                char *output = malloc(max_output_size * sizeof(char));
                fseek(output_file, 0, SEEK_SET);
                for (i = 0; i < max_output_size; i++) {
                    output[i] = (char) fgetc(output_file);
                    if (output[i] == EOF) break;
                }
                nk_layout_row_dynamic(ctx, 500, 1);
                nk_edit_string(ctx, NK_EDIT_BOX | NK_EDIT_NO_CURSOR | NK_EDIT_DEACTIVATED, output, &i, max_output_size,
                               nk_filter_default);
                nk_tree_pop(ctx);
            }

            // Step
            if (nk_tree_push(ctx, NK_TREE_TAB, "Stepping", NK_MINIMIZED)) {
                static char step_buffer[512] = "1";
                static int step_len = 1;
                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "Step speed:", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 30, 3);
                nk_edit_string(ctx, NK_EDIT_SIMPLE, step_buffer, &step_len, 64, nk_filter_decimal);
                if (get_program_counter() < text_size()) {
                    if (nk_button_label(ctx, "Step")) {
                        for (int j = 0; j < atoi(step_buffer); j++)
                            if (get_program_counter() < text_size()) step();
                            else break;
                    }
                    if (nk_button_label(ctx, "Run")) {
                        while (get_program_counter() < text_size()) step();
                    }
                } else {
                    nk_button_label(ctx, "EOF reached");
                    if (nk_button_label(ctx, "Exit")) {
                        running = false;
                        goto cleanup;
                    }
                }
                char pc_s[10];
                nk_layout_row_dynamic(ctx, 10, 2);
                nk_label(ctx, "Program Counter:", NK_TEXT_LEFT);
                nk_label(ctx, nk_itoa(pc_s, get_program_counter()), NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 10, 2);

                nk_label(ctx, "Next instruction:", NK_TEXT_LEFT);
                if (get_program_counter() < text_size()) {
                    char instr[32];
                    switch (get_instruction()) {
                        case OP_BIPUSH:
                            strcpy(instr, "BIPUSH");
                            break;
                        case OP_DUP:
                            strcpy(instr, "DUP");
                            break;
                        case OP_ERR:
                            strcpy(instr, "ERR");
                            break;
                        case OP_GOTO:
                            strcpy(instr, "GOTO");
                            break;
                        case OP_HALT:
                            strcpy(instr, "HALT");
                            break;
                        case OP_IADD:
                            strcpy(instr, "IADD");
                            break;
                        case OP_IAND:
                            strcpy(instr, "IAND");
                            break;
                        case OP_IFEQ:
                            strcpy(instr, "IFEQ");
                            break;
                        case OP_IFLT:
                            strcpy(instr, "IFTL");
                            break;
                        case OP_ICMPEQ:
                            strcpy(instr, "ICMPEQ");
                            break;
                        case OP_IINC:
                            strcpy(instr, "IINC");
                            break;
                        case OP_ILOAD:
                            strcpy(instr, "ILOAD");
                            break;
                        case OP_IN:
                            strcpy(instr, "IN");
                            break;
                        case OP_INVOKEVIRTUAL:
                            strcpy(instr, "INVOKEVIRTUAL");
                            break;
                        case OP_IOR:
                            strcpy(instr, "IOR");
                            break;
                        case OP_IRETURN:
                            strcpy(instr, "IRETURN");
                            break;
                        case OP_ISTORE:
                            strcpy(instr, "ISTORE");
                            break;
                        case OP_ISUB:
                            strcpy(instr, "ISUB");
                            break;
                        case OP_LDC_W:
                            strcpy(instr, "LDC_W");
                            break;
                        case OP_NOP:
                            strcpy(instr, "NOP");
                            break;
                        case OP_OUT:
                            strcpy(instr, "OUT");
                            break;
                        case OP_POP:
                            strcpy(instr, "POP");
                            break;
                        case OP_SWAP:
                            strcpy(instr, "SWAP");
                            break;
                        case OP_WIDE:
                            strcpy(instr, "WIDE");
                            break;
                        case OP_NEWARRAY:
                            strcpy(instr, "NEWARRAY");
                            break;
                        case OP_IALOAD:
                            strcpy(instr, "IALOAD");
                            break;
                        case OP_IASTORE:
                            strcpy(instr, "IASTORE");
                            break;
                        case OP_GC:
                            strcpy(instr, "GC");
                            break;
                        case OP_NETBIND:
                            strcpy(instr, "NETBIND");
                            break;
                        case OP_NETCONNECT:
                            strcpy(instr, "NETCONNECT");
                            break;
                        case OP_NETIN:
                            strcpy(instr, "NETIN");
                            break;
                        case OP_NETOUT:
                            strcpy(instr, "NETOUT");
                            break;
                        case OP_NETCLOSE:
                            strcpy(instr, "NETCLOSE");
                            break;
                        default:
                            strcpy(instr, "????");
                    }
                    nk_label(ctx, instr, NK_TEXT_LEFT);
                }
                else {
                    nk_label(ctx, "EOF", NK_TEXT_LEFT);
                }
                nk_label(ctx, "", NK_TEXT_LEFT);
                nk_tree_pop(ctx);
            }

            // Frames
            if (nk_tree_push(ctx, NK_TREE_TAB, "Frames", NK_MINIMIZED)) {
                nk_layout_row_dynamic(ctx, 30, 1);
                nk_label(ctx, "View all frame contents", NK_TEXT_LEFT);
                int i = 0;
                frame_t *current_frame = frame;

                while (current_frame != NULL) {
                    i++;
                    current_frame = current_frame->prev_frame;
                }
                current_frame = frame;
                for (int j = i; j > 0; j--) {
                    char s[40] = "Frame ";
                    char f_s[32];
                    nk_itoa(f_s, j);
                    if (nk_tree_push_id(ctx, NK_TREE_TAB, strcat(s, f_s), NK_MINIMIZED, j)) {
                        if (current_frame->stack_size > 0) {
                            nk_label(ctx, "Stack contents:", NK_TEXT_LEFT);
                            for (int k = 0; k < current_frame->stack_size; k++) {
                                char s_s[32];
                                nk_label(ctx, nk_itoa(s_s, current_frame->stack_data[k]), NK_TEXT_LEFT);
                            }
                        } else {
                            nk_label(ctx, "Stack is empty", NK_TEXT_LEFT);
                        }

                        if (current_frame->local_size > 0) {
                            nk_label(ctx, "Local variable contents:", NK_TEXT_LEFT);
                            for (int k = 0; k < current_frame->local_size; k++) {
                                char s_s[32];
                                nk_label(ctx, nk_itoa(s_s, current_frame->local_data[k]), NK_TEXT_LEFT);
                            }
                        } else {
                            nk_label(ctx, "Local variables is empty", NK_TEXT_LEFT);
                        }
                        nk_tree_pop(ctx);
                    }
                    current_frame = current_frame->prev_frame;
                }
                nk_label(ctx, "", NK_TEXT_LEFT);
                nk_tree_pop(ctx);
            }

            // Arrays
            if (nk_tree_push(ctx, NK_TREE_TAB, "Arrays", NK_MINIMIZED)) {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (arrays->size > 0) {
                    nk_label(ctx, "View all array contents", NK_TEXT_LEFT);
                    for (int i = 0; i < arrays->size; i++) {
                        char s[40] = "Array ";
                        char f_s[32];
                        nk_itoa(f_s, i);
                        if (nk_tree_push_id(ctx, NK_TREE_TAB, strcat(s, f_s), NK_MINIMIZED, i)) {
                            nk_layout_row_dynamic(ctx, 10, 2);
                            nk_label(ctx, "Reference", NK_TEXT_LEFT);
                            char ref_s[32];
                            nk_label(ctx, nk_itoa(ref_s, arrays->arrays[i]->ref), NK_TEXT_LEFT);

                            nk_layout_row_dynamic(ctx, 10, 1);
                            nk_label(ctx, "", NK_TEXT_LEFT);
                            if (arrays->arrays[i]->size > 0) {
                                nk_label(ctx, "Array contents:", NK_TEXT_LEFT);
                                char val_s[32];
                                nk_label(ctx, nk_itoa(val_s, arrays->arrays[i]->data[i]), NK_TEXT_LEFT);
                            } else {
                                nk_label(ctx, "Array is empty", NK_TEXT_LEFT);
                            }
                            nk_tree_pop(ctx);
                        }
                    }
                } else {
                    nk_label(ctx, "No arrays found", NK_TEXT_LEFT);
                }
                nk_tree_pop(ctx);
            }

            if (nk_button_label(ctx, "Reset IJVM")) goto cleanup;
        }
        nk_window_set_size(ctx, "IJVM", nk_vec2(xw.width, xw.height));
        nk_end(ctx);
        if (nk_window_is_hidden(ctx, "IJVM")) break;

        /* Draw */
        XClearWindow(xw.dpy, xw.win);
        nk_xlib_render(xw.win, nk_rgb(0, 0, 0));
        XFlush(xw.dpy);
    }

    cleanup:
    if (input_file != NULL) fclose(input_file);
    if (output_file != NULL) fclose(output_file);
    remove("tmp_input");
    remove("tmp_output");
    destroy_ijvm();
    nk_xfont_del(xw.dpy, xw.font);
    nk_xlib_shutdown();
    XUnmapWindow(xw.dpy, xw.win);
    XFreeColormap(xw.dpy, xw.cmap);
    XDestroyWindow(xw.dpy, xw.win);
    XCloseDisplay(xw.dpy);
    return 0;
}

int main(int argc, char **argv) {
    while (running) init_gui(argc, argv);
}

