#pragma once

struct FileContent
{
	char* buffer;
};

struct FileContent file_read_asc(const char* path);
