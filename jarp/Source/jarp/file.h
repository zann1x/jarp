#ifndef FILE_H
#define FILE_H

struct FileContent {
	char* buffer;
	long length;
};

struct FileContent file_read_asc(const char* path);
struct FileContent file_read_bin(const char* path);

#endif
