#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define PAGE_SIZE 4096

static int totf;

static FILE* fp;

static FILE* sfp;
static FILE* rfp;
static int numLines;
static int curLine;

static unsigned int addr;
static char mode;
static int process;

static int memAcc;
static int pfs;
static int writes;

struct pageTable {
    unsigned int* frame;
    int* valid;
    int* dirty;
    int* age;
};

struct opTrace {
    char* modes;
    unsigned int* addrs;
};

struct nexTrace {
    unsigned int* addrs;
    int* next;
    int* init;
};

int getNext() {
    //printf("%c ",mode);
    //printf("%x ",addr);
    //printf("%d\n",process);
    return fscanf(fp, "%c %x %d", &mode, &addr, &process);
}

void fileLength() {
    numLines = 0;
    char next = fgetc(sfp);
    while (next != EOF) {
        if (strcmp(&next, "\n") == 0) numLines++;
        next = fgetc(sfp);
    }
}

struct opTrace optRead() {
    struct opTrace ot;
    //printf("%d", numLines);
    ot.modes = malloc(sizeof(char) * numLines);
    ot.addrs = malloc(sizeof(unsigned int) * numLines);
    int i = 0;
    while (fscanf(rfp, "%c %x %d", &mode, &addr, &process) != EOF) {
        ot.modes[i] = mode;
        unsigned int page = addr / PAGE_SIZE;
        ot.addrs[i] = page;
        // printf("%c ",mode);
        // printf("%x ",addr);
        // printf("%d\n",process);
        i++;
    }
    return ot;
}

struct pageTable initPT() {
    struct pageTable pt;
    pt.frame = malloc(sizeof(unsigned int) * totf);
    pt.valid = malloc(sizeof(int) * totf);
    pt.dirty = malloc(sizeof(int) * totf);
    pt.age = malloc(sizeof(int) * totf);
    for (int i = 0; i < totf; i++) {
        pt.frame[i] = -1;
        pt.valid[i] = 0;
        pt.dirty[i] = 0;
        pt.age[i] = 0;
    }
    return pt;
}

void printPT(struct pageTable pt) {
    for (int i = 0; i < totf; i++) {
        printf("f%d:%x v:%d d:%d a:%d ", i, pt.frame[i], pt.valid[i], pt.dirty[i], pt.age[i]);
    }
    printf("\n");
}

void printPTF(struct pageTable pt) {
    printf("Memory frames: [");
    for (int i = 0; i < totf; i++) {
        printf("\'%x\', ", pt.frame[i]);
    }
    printf("]");
}

void printNT(struct nexTrace nt) {
    for (int i = 0; i < numLines; i++) {
        if (nt.next[i] != -2) {
            printf("%dth element:%x next:%d init:%d ", i, nt.addrs[i], nt.next[i], nt.init[i]);
        }
    }
    printf("\n");
}

struct nexTrace initNex(struct opTrace ot) {
    struct nexTrace nt;
    nt.addrs = malloc(sizeof(unsigned int) * numLines);
    nt.next = malloc(sizeof(int) * numLines);
    nt.init = malloc(sizeof(int) * numLines);
    for (int j = 0; j < numLines; j++) {
        nt.addrs[j] = 0;
        nt.next[j] = -2;
        nt.init[j] = 0;
    }
    //iterate through opTrace
    for (int i = 0; i < numLines; i++) {
        //iterate through nexTrace
        for (int j = 0; j < numLines; j++) {
            if (nt.next[j] == -2) {
                nt.addrs[j] = ot.addrs[i];
                nt.next[j] = i;
                nt.init[j] = 1;
                break;
            }
            if (nt.addrs[j] == ot.addrs[i]) {
                break;
            }
        }
    }
    return nt;
}

struct nexTrace compNex(struct opTrace ot, struct nexTrace nt) {
    for (int i = 0; i < numLines; i++) {
        if (nt.next[i] == -2) {
            break;
        }
        if (nt.next[i] > -1 && nt.next[i] - curLine <= 0) {
            for (int j = curLine; j <= numLines; j++) {
                if (ot.addrs[j] == nt.addrs[i]) {
                    nt.next[i] = j;
                    break;
                }
            }
            if (nt.next[i] - curLine < 0) {
                nt.next[i] = -1;
            }
        }
    }
    //printNT(nt);
    return nt;
}


//All operation methods return 0 on success, 1 on file not found failure, 
int opt() {
    if (fp == NULL) {
        return 1;
    }
    struct pageTable pt = initPT();
    struct opTrace ot = optRead();
    struct nexTrace nt = initNex(ot);
    //printNT(nt);
    memAcc = 0;
    pfs = 0;
    writes = 0;
    if (getNext() == EOF) {
        return 0;
    }
    else {
        curLine = 0;
    }
    //run until EOF
    while (1) {
        memAcc++;
        unsigned int page = addr / PAGE_SIZE;
        //track location of found frame in memory
        int found = -1;
        //loop through page table to find match
        //printf("1");
        for (int i = 0; i < totf; i++) {
            //if page is valid
            if (pt.valid[i] == 1) {
                //if page matches
                if (pt.frame[i] == page) {
                    found = i;
                    break;
                }
            }
        }
        //printf("1");
        //if not found
        if (found == -1) {
            //page fault
            pfs++;
            //printf("%d ",curLine);
            //printPTF(pt);
            //now we need to load it
            //loop through page table to find a best candidate to replace
            int bc = -1;
            //if no free found a replacement is neccesary
            int latestNextUse = 0;
            int nevUsed = 0;
            for (int i = 0; i < totf; i++) {
                for (int j = 0; j < numLines; j++) {
                    if (nt.addrs[j] == pt.frame[i]) {
                        //printf("pair: %d, %d |",nt.next[j],nt.init[j]);
                        if (nt.next[j] < 0 && nt.init[j] != 0) {
                            bc = i;
                            //printf("| %d is never used |",nt.addrs[j]);
                            nevUsed = 1;
                            break;
                        }
                        else {
                            if (nt.next[j] > latestNextUse) {
                                bc = i;
                                //printf("%d is used in %d steps\n",nt.addrs[j],nt.next[j]);
                                latestNextUse = nt.next[j];
                                break;
                            }
                            else {
                                if (nt.next[j] == latestNextUse) {
                                    if (bc != -1) {
                                        if (pt.age[i] > pt.age[bc]) {
                                            bc = i;
                                            //printf("%d is used in %d steps\n",nt.addrs[j],nt.next[j]);
                                            latestNextUse = nt.next[j];
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (nevUsed) {
                    break;
                }
            }
            // for(int i = 0; i < numLines; i++) {
            //     //printf("%d address next used in %d, ", nt.addrs[i],nt.next[i]);
            //     for(int j = 0; j < totf; j++) {
            //         if (nt.next[i] > latestNextUse && nt.addrs[i] == pt.frame[j]) {
            //         latestNextUse = nt.next[i];
            //         bc = j;
            //         //printf("--%d with distance %d--",i, nt.next[i]);
            //         }
            //     }
            // }
            //printf("\n");
            for (int i = 0; i < totf; i++) {
                //if page is invalid it is best candidate
                if (pt.valid[i] == 0) {
                    bc = i;
                    //printf("-%d-",bc);
                    break;
                }
            }
            //we have our best candidate for replacemen
            //if page dirty we need to write
            if (pt.dirty[bc] == 1) {
                writes++;
            }
            char str[2] = { mode, '\0' };
            if (strcmp(str, "s") == 0) {
                //printf("|%d|",bc);
                //printf("| overwriting f%d:%x v:%d d:%d as its next us is step %d with f%d:%x v:%d d:%d |", bc, pt.frame[bc], pt.valid[bc], pt.dirty[bc], latestNextUse, bc, addr, 1, 1);
                pt.dirty[bc] = 1;
            }
            else {
                //printf("|%d|",bc);
                //printf("| overwriting f%d:%x v:%d d:%d as its next us is step %d with f%d:%x v:%d d:%d |", bc, pt.frame[bc], pt.valid[bc], pt.dirty[bc], latestNextUse, bc, addr, 1, 0);
                pt.dirty[bc] = 0;
            }
            pt.frame[bc] = page;
            pt.valid[bc] = 1;
            pt.age[bc] = 0;
            //printf(" ");
            //printPTF(pt);
            //printf("\n");
        }
        else {
            //Here we found a matching page
            pt.age[found] = 0;
            char str[2] = { mode, '\0' };
            if (strcmp(str, "s") == 0) {
                pt.dirty[found] = 1;
            }
        }
        if (getNext() == EOF) {
            //printPT(pt);
            return 0;
        }
        else {
            curLine++;
            //printf("%d ",curLine);
            nt = compNex(ot, nt);
            for (int i = 0; i < totf; i++) {
                if (pt.frame[i] != -1) {
                    pt.age[i] = pt.age[i] + 1;
                }
            }
            //printf("\n");

        }
    }

    return 0;
}

int lru() {
    if (fp == NULL) {
        return 1;
    }
    struct pageTable pt = initPT();
    memAcc = 0;
    pfs = 0;
    writes = 0;
    if (getNext() == EOF) {
        return 0;
    }
    else {
        curLine = 0;
    }
    while (1) {
        memAcc++;
        unsigned int page = addr / PAGE_SIZE;
        //track location of found frame in memory
        int found = -1;
        //loop through page table to find match
        //printf("1");
        for (int i = 0; i < totf; i++) {
            //if page is valid
            if (pt.valid[i] == 1) {
                //if page matches
                if (pt.frame[i] == page) {
                    found = i;
                    break;
                }
            }
        }
        //printf("1");
        //if not found
        if (found == -1) {
            //page fault
            pfs++;
            //now we need to load it
            //loop through page table to find a best candidate to replace
            int bc = -1;
            //if no free found a replacement is neccesary
            for (int i = 0; i < totf; i++) {
                if (bc != -1) {
                    if (pt.age[i] > pt.age[bc]) {
                        bc = i;
                    }
                }
                else {
                    bc = i;
                }
            }
            for (int i = 0; i < totf; i++) {
                //if page is invalid it is best candidate
                if (pt.valid[i] == 0) {
                    bc = i;
                    //printf("-%d-",bc);
                    break;
                }
            }
            //we have our best candidate for replacemen
            //if page dirty we need to write
            if (pt.dirty[bc] == 1) {
                writes++;
            }
            char str[2] = { mode, '\0' };
            if (strcmp(str, "s") == 0) {
                //printf("|%d|",bc);
                //printf("| overwriting f%d:%x v:%d d:%d as age is %d with f%d:%x v:%d d:%d |", bc, pt.frame[bc], pt.valid[bc], pt.dirty[bc], pt.age[bc], bc, addr, 1, 1);
                pt.dirty[bc] = 1;
            }
            else {
                //printf("|%d|",bc);
                //printf("| overwriting f%d:%x v:%d d:%d as its age is %d with f%d:%x v:%d d:%d |", bc, pt.frame[bc], pt.valid[bc], pt.dirty[bc], pt.age[bc], bc, addr, 1, 0);
                pt.dirty[bc] = 0;
            }
            pt.frame[bc] = page;
            pt.valid[bc] = 1;
            pt.age[bc] = 0;
            //printf("\n");
        }
        else {
            //Here we found a matching page
            pt.age[found] = 0;

            char str[2] = { mode, '\0' };
            if (strcmp(str, "s") == 0) {
                pt.dirty[found] = 1;
            }
        }
        if (getNext() == EOF) {
            //printPT(pt);
            return 0;
        }
        else {
            curLine++;
            for (int i = 0; i < totf; i++) {
                if (pt.frame[i] != -1) {
                    pt.age[i] = pt.age[i] + 1;
                }
            }
            //printf("Element %x : ", addrs);
            //printPT(pt);
            //printf("\n");

        }
    }
    return 0;
}

int second() {
    if (fp == NULL) {
        return 1;
    }
    struct pageTable pt = initPT();
    memAcc = 0;
    pfs = 0;
    writes = 0;
    int hand = 0;
    if (getNext() == EOF) {
        return 0;
    }
    else {
        curLine = 0;
    }

    while (1) {
        memAcc++;
        unsigned int page = addr / PAGE_SIZE;
        //track location of found frame in memory
        int found = -1;
        //printf("Hand is pointing at index: %d. PT:", hand);
        //printPT(pt);
        //loop through page table to find match
        //printf("1");
        for (int i = 0; i < totf; i++) {
            //if page is valid
            if (pt.valid[i] == 1) {
                //if page matches
                if (pt.frame[i] == page) {
                    found = i;
                    break;
                }
            }
        }
        //printf("1");
        //if not found
        if (found == -1) {
            pfs++;
            //now we need to load it
            //loop through page table to find a best candidate to replace
            int bc = -1;
            while (1) {
                if (pt.valid[hand] != 1) {
                    bc = hand;
                    hand++;
                    hand = hand % totf;
                    break;
                }
                if (pt.age[hand] == 0) {
                    bc = hand;
                    hand++;
                    hand = hand % totf;
                    break;
                }
                else {
                    pt.age[hand] = 0;
                    hand++;
                    hand = hand % totf;
                }
            }
            //we have our best candidate for replacemen
            //if page dirty we need to write
            if (pt.dirty[bc] == 1) {
                writes++;
            }
            char str[2] = { mode, '\0' };
            if (strcmp(str, "s") == 0) {
                //printf("|%d|",bc);
                //printf("| overwriting f%d:%x v:%d d:%d as age is %d with f%d:%x v:%d d:%d |", bc, pt.frame[bc], pt.valid[bc], pt.dirty[bc], pt.age[bc], bc, addr, 1, 1);
                pt.dirty[bc] = 1;
            }
            else {
                //printf("|%d|",bc);
                //printf("| overwriting f%d:%x v:%d d:%d as its age is %d with f%d:%x v:%d d:%d |", bc, pt.frame[bc], pt.valid[bc], pt.dirty[bc], pt.age[bc], bc, addr, 1, 0);
                pt.dirty[bc] = 0;
            }
            pt.frame[bc] = page;
            pt.valid[bc] = 1;
            pt.age[bc] = 0;
        }
        else {
            //Here we found a matching page
            pt.age[found] = 1;
            char str[2] = { mode, '\0' };
            if (strcmp(str, "s") == 0) {
                pt.dirty[found] = 1;
            }
        }
        if (getNext() == EOF) {
            //printPT(pt);
            return 0;
        }
        else {
            curLine++;
            //printf("Hand is pointing at index: %d. PT:", hand);
            //printPT(pt);
            //printf("------------------\n");
        }
    }
    return 0;
}

int secondR(char* fileName) {
    int frames = 2;
    while (frames <= totf) {
        fp = fopen(fileName, "r");
        if (fp == NULL) {
            return 1;
        }
        struct pageTable pt = initPT();
        memAcc = 0;
        pfs = 0;
        writes = 0;
        int hand = 0;
        if (getNext() == EOF) {
            return 0;
        }
        else {
            curLine = 0;
        }

        while (1) {
            memAcc++;
            unsigned int page = addr / PAGE_SIZE;
            //track location of found frame in memory
            int found = -1;
            //printf("Hand is pointing at index: %d. PT:", hand);
            //printPT(pt);
            //loop through page table to find match
            //printf("1");
            for (int i = 0; i < frames; i++) {
                //if page is valid
                if (pt.valid[i] == 1) {
                    //if page matches
                    if (pt.frame[i] == page) {
                        found = i;
                        break;
                    }
                }
            }
            //printf("1");
            //if not found
            if (found == -1) {
                pfs++;
                //now we need to load it
                //loop through page table to find a best candidate to replace
                int bc = -1;
                while (1) {
                    if (pt.valid[hand] != 1) {
                        bc = hand;
                        hand++;
                        hand = hand % frames;
                        break;
                    }
                    if (pt.age[hand] == 0) {
                        bc = hand;
                        hand++;
                        hand = hand % frames;
                        break;
                    }
                    else {
                        pt.age[hand] = 0;
                        hand++;
                        hand = hand % frames;
                    }
                }
                //we have our best candidate for replacemen
                //if page dirty we need to write
                if (pt.dirty[bc] == 1) {
                    writes++;
                }
                char str[2] = { mode, '\0' };
                if (strcmp(str, "s") == 0) {
                    //printf("|%d|",bc);
                    //printf("| overwriting f%d:%x v:%d d:%d as age is %d with f%d:%x v:%d d:%d |", bc, pt.frame[bc], pt.valid[bc], pt.dirty[bc], pt.age[bc], bc, addr, 1, 1);
                    pt.dirty[bc] = 1;
                }
                else {
                    //printf("|%d|",bc);
                    //printf("| overwriting f%d:%x v:%d d:%d as its age is %d with f%d:%x v:%d d:%d |", bc, pt.frame[bc], pt.valid[bc], pt.dirty[bc], pt.age[bc], bc, addr, 1, 0);
                    pt.dirty[bc] = 0;
                }
                pt.frame[bc] = page;
                pt.valid[bc] = 1;
                pt.age[bc] = 0;
            }
            else {
                //Here we found a matching page
                pt.age[found] = 1;
                char str[2] = { mode, '\0' };
                if (strcmp(str, "s") == 0) {
                    pt.dirty[found] = 1;
                }
            }
            if (getNext() == EOF) {
                //printPT(pt);
                break;
            }
            else {
                curLine++;
                //printf("Hand is pointing at index: %d. PT:", hand);
                //printPT(pt);
                //printf("------------------\n");
            }
        }
        printf("Iteration %d had %d page faults\n", frames, pfs);
        frames++;
    }
    return 0;
}





int main(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            i++;
            totf = atoi(argv[i]);
            //printf("%d\n",totf);
        }
        if (strcmp(argv[i], "-a") == 0) {
            i++;
            if (strcmp(argv[i], "opt") == 0) {
                i++;
                fp = fopen(argv[i], "r");
                sfp = fopen(argv[i], "r");
                rfp = fopen(argv[i], "r");
                fileLength();
                opt();
                printf("Algorithm: OPT\n");
                printf("Number of frames: %d\n", totf);
                printf("Total memory accesses: %d\n", memAcc);
                printf("Total page faults: %d\n", pfs);
                printf("Total writes to disk: %d\n", writes);
            }
            else if (strcmp(argv[i], "lru") == 0) {
                i++;
                fp = fopen(argv[i], "r");
                lru();
                printf("Algorithm: LRU\n");
                printf("Number of frames: %d\n", totf);
                printf("Total memory accesses: %d\n", memAcc);
                printf("Total page faults: %d\n", pfs);
                printf("Total writes to disk: %d\n", writes);
            }
            else if (strcmp(argv[i], "second") == 0) {
                i++;
                fp = fopen(argv[i], "r");
                second();
                printf("Algorithm: SECOND\n");
                printf("Number of frames: %d\n", totf);
                printf("Total memory accesses: %d\n", memAcc);
                printf("Total page faults: %d\n", pfs);
                printf("Total writes to disk: %d\n", writes);
            }
            else if (strcmp(argv[i], "secondR") == 0) {
                i++;
                secondR(argv[i]);
                printf("Algorithm: second\n");
                printf("Number of frames: %d\n", totf);
                printf("Total memory accesses: %d\n", memAcc);
                printf("Total page faults: %d\n", pfs);
                printf("Total writes to disk: %d\n", writes);
            }
        }
    }

    return 0;
}
