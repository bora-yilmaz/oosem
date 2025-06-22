#include "header.h"
#include <stdio.h>

typedef struct env{
    int regs[64];
    unsigned char* progmem;
    int progmem_len;
    int stridmem[1024];
    int datamem[16384];
    int pc;
    int lt;
    int gt;
    int eq;
    Node* retstlines;
    Node* sectdiv;
    int halted;
}env;

Node* sepsect(Node* code){
    int l = len(code) / 8;
    Node* sep = create_node(0);
    for(int i = 1; i < l; i++){
        if (getindex(code, i*8) == 0x20) {
            push(&sep, i);
        }
    }
    push(&sep, l);
    return sep;
}

env* make_env(Node* code) {
    env* e = (env*)malloc(sizeof(env));
    if (!e) {
        perror("malloc");
        exit(1);
    }

    int total_bytes = len(code);
    e->progmem_len = total_bytes / 8;
    e->progmem = malloc(total_bytes);
    if (!e->progmem) { perror("malloc"); exit(1); }

    for (int i = 0; i < total_bytes; i++) {
        e->progmem[i] = getindex(code, i);
    }


    for (int i = 0; i < 64; i++) e->regs[i] = 0;
    for (int i = 0; i < 16384; i++) {
        e->datamem[i] = 0;
    }
    for (int i = 0; i < 1024; i++) e->stridmem[i] = 0;

    e->pc = 0;
    e->lt = 0;
    e->gt = 0;
    e->eq = 0;
    e->halted = 0;

    e->retstlines = NULL;
    e->sectdiv = sepsect(code);

    return e;
}


Node* read_file_as_int_list(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    Node* head = NULL;
    int byte;
    while ((byte = fgetc(f)) != EOF) {
        push(&head, byte);
    }


    fclose(f);
    return head;
}

int execline(env* e){
    int i, j, a;if (e->pc < 0 || e->pc >= e->progmem_len) {
        fprintf(stderr, "PC out of bounds: %d\n", e->pc);
        return 1;  // preserve error return
    }

    unsigned char* line = &e->progmem[e->pc * 8];
    int cmd = line[0];
    int arg1 = line[1];
    int arg2 = line[2];
    int arg3 = line[3];
    int arg4 = line[4];
    int arg5 = line[5];
    int arg6 = line[6];
    int arg7 = line[7];

    switch (cmd){
        case 0x00:
            break;
        case 0x10:
            e->regs[arg1] = arg2;
            break;
        case 0x11:
            e->regs[arg1] = e->regs[arg2];
            break;
        case 0x12:
            e->regs[arg1] = e->datamem[e->regs[arg2]];
            break;
        case 0x13:
            e->datamem[e->regs[arg1]] = e->regs[arg2];
            break;
        case 0x14:
            e->regs[arg1] = e->regs[arg2] + e->regs[arg3];
            break;
        case 0x15:
            e->regs[arg1] = e->regs[arg2] - e->regs[arg3];
            break;
        case 0x16:
            e->regs[arg1] = e->regs[arg2] * e->regs[arg3];
            break;
        case 0x17:
            e->regs[arg1] = e->regs[arg2] & e->regs[arg3];
            break;
        case 0x18:
            e->regs[arg1] = e->regs[arg2] | e->regs[arg3];
            break;
        case 0x19:
            e->regs[arg1] = e->regs[arg2] ^ e->regs[arg3];
            break;
        case 0x1a:
            e->regs[arg1] = !(e->regs[arg2]);
            break;
        case 0x1b:
            e->regs[arg1] = (e->regs[arg2] != 0);
            break;
        case 0x20:
            if(len(e->retstlines)){
                e->pc = getindex(e->retstlines, len(e->retstlines) - 1) + 1;
                pop(&(e->retstlines));
            }else{
                return 1;
            }
            break;
        case 0x21:
            i = 0;
            while(i < 64 && (e->stridmem[i*8] != 0 || e->stridmem[i*8+1] != 0 || e->stridmem[i*8+2] != 0 || e->stridmem[i*8+3] != 0 || e->stridmem[i*8+4] != 0 || e->stridmem[i*8+5] != 0 || e->stridmem[i*8+6] != 0 || e->stridmem[i*8+7] != 0)){
                i++;
            }
            if(i >= 64){
                printf("-----ERROR-----AUTO_INDEX_ERROR-----LINE-%d-----", e->pc);
                return 1;
                break;
            }
            e->stridmem[i] = 0;
            e->stridmem[i+1] = arg3;
            
            e->stridmem[i+2] = arg1;
            
            e->stridmem[i+3] = arg2;

            e->stridmem[i+4] = 0;

            e->stridmem[i+5] = 0;

            e->stridmem[i+6] = 0;

            e->stridmem[i+7] = 0;
            break;
        case 0x22:
            i = 0;
            while (i < 64 && e->stridmem[i*8+1] != arg1){
                i++;
            }
            if(i>=64){
                printf("-----ERROR-----AUTO_INDEX_ERROR-----LINE-%d-----", e->pc);
                return 1;
                break;
            }
            int j = 1;
            while (j < 8 && e->stridmem[i*8+j] != 0){
                j++;
            }
            if(j >= 8){
                printf("-----ERROR-----AUTO_INDEX_ERROR-----LINE-%d-----", e->pc);
                return 1;
                break;
            }
            e->stridmem[i*8+j] = arg2;
            break;
        case 0x24:
            i = 0;
            while (i < 64 && e->stridmem[i*8+1] != arg3){
                i++;
            }
            if(i>=64){
                printf("-----ERROR-----AUTO_INDEX_ERROR-----LINE-%d-----", e->pc);
                return 1;
                break;
            }
            int a = arg5;
            for(int j = 0; j < arg4; j++){
                a += e->stridmem[i*8+2+j];
            }
            e->regs[arg1] = e->datamem[arg2+a];
            break;
        case 0x25:
            i = 0;
            while (i < 64 && e->stridmem[i*8+1] != arg3){
                i++;
            }
            if(i>=64){
                printf("-----ERROR-----AUTO_INDEX_ERROR-----LINE-%d-----", e->pc);
                return 1;
                break;
            }
            a = arg5;
            for(int j = 0; j < arg4; j++){
                a += e->stridmem[i*8+2+j];
            }
            e->datamem[arg2+a] = e->regs[arg1];
            break;
        case 0x28:
            push(&(e->retstlines), e->pc);
            int npc = getindex(e->sectdiv, arg1);
            e->pc = npc;
            break;
        case 0x29:
            if(len(e->retstlines)){
                e->pc = getindex(e->retstlines, len(e->retstlines) - 1);
                pop(&(e->retstlines));
            }else{
                return 1;
            }
            break;
        case 0x30:
            e->eq = (e->regs[arg1] == e->regs[arg2]);
            e->lt = (e->regs[arg1] < e->regs[arg2]);
            e->gt = (e->regs[arg1] > e->regs[arg2]);
            break;
        case 0x31:
            if(e->eq){
                e->pc = arg1-1;
            }
            break;
        case 0x32:
            if(!(e->eq)){
                e->pc = arg1-1;
            }
            break;
        case 0x33:
            if(e->lt){
                e->pc = arg1-1;
            }
            break;
        case 0x34:
            if(!(e->lt)){
                e->pc = arg1-1;
            }
            break;
        case 0x35:
            if(e->gt){
                e->pc = arg1-1;
            }
            break;
        case 0x36:
            if(!(e->gt)){
                e->pc = arg1-1;
            }
            break;
        case 0x37:
            e->pc = arg1-1;
            break;
        case 0x40:
            printf("%c", e->regs[arg1]);
            break;
        case 0x47:
            scanf("%c", (char*)&(e->regs[arg1]));
            break;
        default:
            printf("\n-----ERROR-----UNKNOWN_COMMAND-----LINE-%d-----", e->pc);
            return 1;
    }
    e->pc += 1;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2){
        printf("not enough args. expected usage: $ oosem <filename>");
        return 0;
    }
    Node* rawcode = read_file_as_int_list(argv[1]);
    if (len(rawcode) % 8 != 0) {
        fprintf(stderr, "ERROR: program length must be a multiple of 8\n");
        exit(1);
    }

    env* runnerenv = make_env(rawcode);
    while (!runnerenv->halted) {
        runnerenv->halted = execline(runnerenv);
    }



    //free actual memory
    free(runnerenv->progmem);
    free_list(runnerenv->retstlines);
    free_list(runnerenv->sectdiv);
    free(runnerenv);
    free_list(rawcode);
    return 0;
}
