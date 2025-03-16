#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <io.h>

#define MAXLEN 512+5
#define ASCLLNUM 256

typedef struct huffNode {
    int parent, lchild, rchild;
    unsigned long count;
    unsigned char alpha;
    char code[MAXLEN];
} HuffNode;

typedef struct ascll {
    unsigned char alpha;
    unsigned long count;
} Ascll;

void select(HuffNode* HT, int i, int* s1, int* s2) {
    unsigned int j, s = 0;
    for(j = 1; j <= i; j++) {
        if(HT[j].parent == 0) {
            if(s == 0) s = j;
            if(HT[j].count < HT[s].count) s = j;
        }
    }
    *s1 = s;

    s = 0;
    for(j = 1; j <= i; j++) {
        if(HT[j].parent == 0 && j != *s1) {
            if(s == 0) s = j;
            if(HT[j].count < HT[s].count) s = j;
        }
    }
    *s2 = s;
}

int creatHuffmanTree(HuffNode* HT, Ascll* ascll) {
    int i, j = 0;
    for(i = 0; i < 256; i++) {
        if(ascll[i].count > 0) {
            HT[++j].count = ascll[i].count;
            HT[j].alpha = ascll[i].alpha;
            HT[j].parent = HT[j].lchild = HT[j].rchild = 0;
        }
    }
    int leafNum = j;
    int nodeNum = 2 * leafNum - 1;

    for(i = leafNum + 1; i <= nodeNum; i++) {
        HT[i].count = 0;
        HT[i].code[0] = 0;
        HT[i].parent = HT[i].lchild = HT[i].rchild = 0;
    }

    int s1, s2;
    for(i = leafNum + 1; i <= nodeNum; i++) {
        select(HT, i - 1, &s1, &s2);
        HT[s1].parent = i;
        HT[s2].parent = i;
        HT[i].lchild = s2;
        HT[i].rchild = s1;
        HT[i].count = HT[s1].count + HT[s2].count;
    }
    return leafNum;
}

void HuffmanCoding(char** hTable, HuffNode* HT, int leafNum) {
    char cd[MAXLEN];
    int m = MAXLEN;
    cd[m-1] = '\0';

    for(int i = 1; i <= leafNum; i++) {
        int start = m - 1;
        int c, f;
        for(c = i, f = HT[c].parent; f != 0; c = f, f = HT[f].parent) {
            if(HT[f].lchild == c) cd[start--] = '0';
            else cd[start--] = '1';
        }
        start++;
        int j = 0;
        for(; start < m; start++) {
            HT[i].code[j++] = cd[start];
        }
        HT[i].code[j] = '\0';
        hTable[HT[i].alpha] = _strdup(HT[i].code);
    }
}

void compress() {
    FILE *infile, *outfile;
    char infileName[MAXLEN], outfileName[MAXLEN];
    printf("\n请输入文件路径:");
    scanf("%s", infileName);

    if((infile = fopen(infileName, "rb")) == NULL) {
        printf("文件打开失败!");
        return;
    }

    strcpy(outfileName, infileName);
    strcat(outfileName, ".gr");
    if((outfile = fopen(outfileName, "wb")) == NULL) {
        printf("无法创建输出文件!");
        fclose(infile);
        return;
    }

    clock_t begin = clock();
    unsigned char c;
    unsigned long total = 0;
    Ascll ascll[ASCLLNUM] = {0};

    while(fread(&c, 1, 1, infile)) {
        ascll[c].alpha = c;
        ascll[c].count++;
        total++;
    }

    HuffNode HT[MAXLEN];
    int leafNum = creatHuffmanTree(HT, ascll);
    
    char* hTable[ASCLLNUM] = {0};
    HuffmanCoding(hTable, HT, leafNum);

    fseek(infile, 0, SEEK_SET);
    fwrite(&total, sizeof(unsigned long), 1, outfile);
    for(int i = 0; i < ASCLLNUM; i++) {
        fwrite(&ascll[i].count, sizeof(unsigned long), 1, outfile);
    }

    unsigned long j = 0;
    char buffer = 0;
    int bitCount = 0;
    while(fread(&c, 1, 1, infile)) {
        char* code = hTable[c];
        for(int i = 0; code[i]; i++) {
            buffer <<= 1;
            if(code[i] == '1') buffer |= 1;
            bitCount++;
            if(bitCount == 8) {
                fwrite(&buffer, 1, 1, outfile);
                buffer = 0;
                bitCount = 0;
            }
        }
    }

    if(bitCount > 0) {
        buffer <<= (8 - bitCount);
        fwrite(&buffer, 1, 1, outfile);
    }

    clock_t end = clock();
    printf("压缩完成! 耗时: %.2f秒\n", (double)(end - begin)/CLOCKS_PER_SEC);
    
    for(int i = 0; i < ASCLLNUM; i++) {
        free(hTable[i]);
    }
    fclose(infile);
    fclose(outfile);
}

void decompress() {
    char infileName[MAXLEN], outfileName[MAXLEN];
    printf("输入要解压的文件路径:");
    scanf("%s", infileName);
    strcat(infileName, ".gr");

    FILE* infile = fopen(infileName, "rb");
    if(!infile) {
        printf("文件打开失败!");
        return;
    }

    strcpy(outfileName, infileName);
    outfileName[strlen(outfileName)-3] = '\0';
    FILE* outfile = fopen(outfileName, "wb");
    if(!outfile) {
        printf("无法创建输出文件!");
        fclose(infile);
        return;
    }

    clock_t begin = clock();
    unsigned long total;
    fread(&total, sizeof(unsigned long), 1, infile);

    Ascll ascll[ASCLLNUM] = {0};
    for(int i = 0; i < ASCLLNUM; i++) {
        fread(&ascll[i].count, sizeof(unsigned long), 1, infile);
        ascll[i].alpha = i;
    }

    HuffNode HT[MAXLEN];
    int leafNum = creatHuffmanTree(HT, ascll);
    int root = 2 * leafNum - 1;

    unsigned char byte;
    int node = root;
    unsigned long count = 0;
    while(fread(&byte, 1, 1, infile) && count < total) {
        for(int i = 7; i >= 0; i--) {
            int bit = (byte >> i) & 1;
            node = bit ? HT[node].rchild : HT[node].lchild;
            
            if(HT[node].lchild == 0 && HT[node].rchild == 0) {
                fputc(HT[node].alpha, outfile);
                node = root;
                if(++count >= total) break;
            }
        }
    }

    clock_t end = clock();
    printf("解压完成! 耗时: %.2f秒\n", (double)(end - begin)/CLOCKS_PER_SEC);
    fclose(infile);
    fclose(outfile);
}

int main() {
    while(1) {
        printf("\n1.压缩\n2.解压\n3.退出\n选择:");
        int choice;
        scanf("%d", &choice);
        
        switch(choice) {
            case 1: compress(); break;
            case 2: decompress(); break;
            case 3: exit(0);
            default: printf("无效选择!\n");
        }
    }
    return 0;
}