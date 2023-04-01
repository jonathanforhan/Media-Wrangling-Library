#ifndef IMAGE_HANDLER_UTIL_H
#define IMAGE_HANDLER_UTIL_H

// uint32_t big_endian_to_little_endian(uint32_t n);
unsigned long be_to_le(unsigned long n);

void IH_backtrace(void);

#endif //IMAGE_HANDLER_UTIL_H
