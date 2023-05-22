#include "common.h"
#include "fs.h"
#include "memory.h"
#define DEFAULT_ENTRY ((void *)0x8048000)


// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
void ramdisk_read(void *buf, off_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
void ramdisk_write(const void *buf, off_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
size_t get_ramdisk_size();
uintptr_t loader(_Protect *as, const char *filename) {
  // TODO();

  //ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size()); 
  int fd = fs_open(filename, 0, 0);
  Log("fd=%d\n",fd);
  size_t f_size = fs_filesz(fd);
  //Log("filesize=%d",f_size);
  fs_read(fd, DEFAULT_ENTRY, f_size);
  fs_close(fd);
  
  return (uintptr_t)DEFAULT_ENTRY;

}
