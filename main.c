#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "CVector/vector.h"

#define char unsigned char

DECLARE_VECTOR(char)
uint32_t hist[256];
Vec_char al;

void CollectData(char* str) {
	uint32_t ln = strlen(str);
	VecInit_char(&al);
	for (uint32_t i = 0; i < ln; i++) {
		if (hist[str[i]] == 0) al.Push(&al, str[i]);
		hist[str[i]]++;
	}

	for (uint32_t i = 0; i < al.size - 1; i++) {
		for (uint32_t j = i + 1; j < al.size; j++) {
			if (hist[al.Get(&al, i)] < hist[al.Get(&al, j)]) {
				char c = al.Get(&al, j);
				al.Set(&al, j, al.Get(&al, i));
				al.Set(&al, i, c);
			}
		}
	}
}

typedef struct {
	uint32_t idx;
	uint32_t pr;
	uint32_t zero, one;
} Node;

DECLARE_VECTOR(Node)

Vec_Node tree;
Vec_Node curNodes;

void GenHuffmanTree(char* str) {
	VecInit_Node(&tree);
	VecInit_Node(&curNodes);

	CollectData(str);

	Node tmp;
	for (uint32_t i = 0; i < al.size; i++) {
		tmp.pr = hist[al.Get(&al, i)];
		tmp.idx = i;
		tree.Push(&tree, tmp);
		curNodes.Push(&curNodes, tmp);
	}

	while (curNodes.size > 1) {
		uint32_t mnPos = 0; uint32_t mn = 1e9;
		for (uint32_t i = 0; i < curNodes.size - 1; i++) {
			uint32_t sum;
			if ((sum = curNodes.Get(&curNodes, i).pr + curNodes.Get(&curNodes, i + 1).pr) < mn) {
				mn = sum;
				mnPos = i;
			}
		}

		tmp.one = curNodes.Get(&curNodes, mnPos).idx;
		tmp.zero = curNodes.Get(&curNodes, mnPos + 1).idx;
		tmp.idx = tree.size;
		tmp.pr = mn;

		for (uint32_t i = mnPos; i < curNodes.size - 2; i++) {
			curNodes.Set(&curNodes, i, curNodes.Get(&curNodes, i + 2));
		}
		curNodes.size -= 2;
		curNodes.Push(&curNodes, tmp);
		tree.Push(&tree, tmp);
	}

	VecFree_Node(&curNodes);
}

typedef struct {
	uint32_t val;
	uint8_t sz;
} Code;

Code code;
void GenHuffmanCodes(uint32_t idx) {
	if (idx < al.size) {
		printf("%c: ", al.Get(&al, idx));
		for (uint32_t i = (1u << (code.sz - 1)); i; i >>= 1) {
			printf("%c", (code.val & i ? '1' : '0'));
		} printf("\n");
		return;
	}

	code.sz++;
	code.val <<= 1;
	GenHuffmanCodes(tree.Get(&tree, idx).zero);
	code.val |= 1;
	GenHuffmanCodes(tree.Get(&tree, idx).one);
	code.val >>= 1;
	code.sz--;
}

int main() {
	char buffer[256];
	scanf("%s", buffer);

	GenHuffmanTree(buffer);
	
	code.sz = 0; code.val = 0x0;
	GenHuffmanCodes(tree.size - 1);

	VecFree_Node(&tree);
	VecFree_char(&al);

	return 0;
}