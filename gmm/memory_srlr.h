/******************************DOCUMENT*COMMENT***********************************
*D
*D �ļ�����            : memory_srlr.h
*D
*D ��Ŀ����            : 
*D
*D �汾��              : 1.1.0001
*D
*D �ļ�����            : Memory Management Head File
*D
*D
*D �ļ��޸ļ�¼
*D ------------------------------------------------------------------------------ 
*D �汾��       �޸�����       �޸���     �Ķ�����
*D ------------------------------------------------------------------------------ 
*D 1.1.0001     2007.01.01                �޸��ļ�
*D*******************************************************************************/
#ifndef _MEMORY_SRLR_H_
#define _MEMORY_SRLR_H_

extern void *Malloc(int sz,bool clear=false);
extern void *Malloc(int p_nNum,int p_nSize,bool clear=false);
extern void **Malloc(int p_nRow,int p_nCol,int p_nBlockSize,bool clear=false);
extern void ***Malloc(int p_nRow1,int p_nRow2,int p_nCol,int p_nBlockSize,bool clear=false);

extern void Free(void *ptr); 
extern void Free(void **ptr,int n1);
extern void Free(void ***ptr,int n1,int n2);

extern void *Malloc32(int sz,bool clear=false);
extern void *Malloc32(int n1,int itsz,bool clear=false);
extern void Free32(void *ptr);

#include "assert.h"

#define Malloc2D(name, xDim, yDim, type) do {               \
    name = (type **)malloc(xDim * sizeof(type *));          \
    assert(name != NULL);                                   \
    name[0] = (type *)malloc(xDim * yDim * sizeof(type));   \
    assert(name[0] != NULL);                                \
    for (size_t i = 1; i < xDim; i++)                       \
        name[i] = name[i-1] + yDim;                         \
} while (0)
        
#define Free2D(name) do { \
    if(name != NULL) { \
        free(name[0]); \
        free(name); \
        name = NULL; \
    } \
} while(0)


#endif
