#include "project.h"
#include <stdlib.h>
// 内存分配函数替代
void* my_malloc(unsigned int size) {
    // 此处应实现平台无关的内存分配
    // 示例实现，实际使用时需替换为具体平台的实现
//    return (void*)0x1000; // 模拟返回内存地址
    return malloc( size );
}

// 内存释放函数替代
void my_free(void* ptr) {
    // 此处应实现平台无关的内存释放
	free( ptr );
}

// 内存复制函数替代
void my_memcpy(unsigned char* dest, const unsigned char* src, unsigned int n) {
    unsigned int i;
    for (i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

loopbuf_t* loopbuf_init(unsigned int size) {
    loopbuf_t* lb = (loopbuf_t*)my_malloc(sizeof(loopbuf_t));
    if (!lb) return 0;

    lb->buffer = (unsigned char*)my_malloc(size);
    if (!lb->buffer) {
        my_free(lb);
        return 0;
    }

    lb->size = size;
    lb->head = 0;
    lb->tail = 0;
    return lb;
}

void loopbuf_free(loopbuf_t* lb) {
    if (lb) {
        if (lb->buffer) my_free(lb->buffer);
        my_free(lb);
    }
}

unsigned int loopbuf_write(loopbuf_t* lb, const unsigned char* data, unsigned int n) {
    if (!lb || !data || n == 0) return 0;

    unsigned int space = lb->size - ((lb->head >= lb->tail)? (lb->head - lb->tail) : (lb->size - (lb->tail - lb->head)));
    if (n > space) n = space;

    if (n == 0) return 0;

    unsigned int part1 = lb->size - lb->head;
    if (n <= part1) {
        my_memcpy(lb->buffer + lb->head, data, n);
        lb->head = (lb->head + n) % lb->size;
    } else {
        my_memcpy(lb->buffer + lb->head, data, part1);
        my_memcpy(lb->buffer, data + part1, n - part1);
        lb->head = n - part1;
    }

    return n;
}

unsigned int loopbuf_read(loopbuf_t* lb, unsigned char* data, unsigned int n) {
    if (!lb || !data || n == 0) return 0;

    unsigned int available = (lb->head >= lb->tail)? (lb->head - lb->tail) : (lb->size - (lb->tail - lb->head));
    if (n > available) n = available;

    if (n == 0) return 0;

    unsigned int part1 = lb->size - lb->tail;
    if (n <= part1) {
        my_memcpy(data, lb->buffer + lb->tail, n);
        lb->tail = (lb->tail + n) % lb->size;
    } else {
        my_memcpy(data, lb->buffer + lb->tail, part1);
        my_memcpy(data + part1, lb->buffer, n - part1);
        lb->tail = n - part1;
    }

    return n;
}
