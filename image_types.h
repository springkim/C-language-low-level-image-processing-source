#if !defined(IMAGE_TYPES_H_7E0_C_1B_INCLUDED)
#define IMAGE_TYPES_H_7E0_C_1B_INCLUDED
#include<stdio.h>
#pragma pack(push,1)
typedef struct IMAGE_PIXEL{
	unsigned char r, g, b;
}IMAGE_PIXEL;
typedef struct IMAGE_TYPE{
	IMAGE_PIXEL** data;
	size_t width;
	size_t height;
}IMAGE_TYPE;
#pragma pack(pop)
#define IMAGE_FAILURE		0
#define IMAGE_SUCCESS		1
/*
*	@Malloc2D : Memory Allocate 2-Demension.
*	@param1  : row(same as x,width).
*	@param2 : col(same as y,height) - column based.
*	@param3 : Size of element.
*	@return : 2D matrix pointer, if fails, the return value is NULL.
*/
void** Malloc2D(size_t row, size_t col, size_t sizeOfType) {
	assert(row > 0 && col > 0);
	void** p = (void**)malloc(col*sizeof(void*) + col*row*sizeOfType);
	if (p == NULL)return NULL;
	char* data = ((char*)p) + col*sizeof(void*);
	for (int i = 0; i < col; i++)
		p[i] = data + i*row*sizeOfType;
	return p;
}
#endif