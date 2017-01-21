/*
    Copyright 2010, 2016 Alex Margarit

    This file is part of a2x-framework.

    a2x-framework is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    a2x-framework is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with a2x-framework.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "a2x_pack_file.v.h"

struct AFile {
    FILE* handle;
    char* path;
    char* name;
    char* lineBuffer;
    size_t lineBufferSize;
    int eof;
    AListNode* node;
};

static AList* g_openedFiles;
static void a_file__close(AFile* File);

void a_file__init(void)
{
    g_openedFiles = a_list_new();
}

void a_file__uninit(void)
{
    A_LIST_ITERATE(g_openedFiles, AFile*, f) {
        a_out__warning("You should close %s/%s with a_file_close",
                       f->path, f->name);
        a_file__close(f);
    }

    a_list_free(g_openedFiles);
}

AFile* a_file_open(const char* Path, const char* Modes)
{
    FILE* handle = fopen(Path, Modes);

    if(!handle) {
        a_out__error("a_file_open: Can't open %s for '%s'", Path, Modes);
        return NULL;
    }

    AFile* f = a_mem_malloc(sizeof(AFile));

    f->handle = handle;
    f->path = a_str_getPrefixLastFind(Path, '/');
    f->name = a_str_getSuffixLastFind(Path, '/');
    f->lineBuffer = NULL;
    f->lineBufferSize = 0;
    f->eof = 0;
    f->node = a_list_addLast(g_openedFiles, f);

    if(f->name == NULL) {
        f->name = a_str_dup(Path);
    }

    if(f->path == NULL) {
        f->path = a_str_dup(".");
    }

    return f;
}

void a_file_close(AFile* File)
{
    a_list_removeNode(File->node);
    a_file__close(File);
}

void a_file__close(AFile* File)
{
    free(File->path);
    free(File->name);
    free(File->lineBuffer);

    if(File->handle) {
        fclose(File->handle);
    }

    free(File);
}

bool a_file_checkPrefix(AFile* File, const char* Prefix)
{
    size_t size = strlen(Prefix) + 1;
    char buffer[size];

    fseek(File->handle, 0, SEEK_SET);

    if(!a_file_read(File, buffer, size)) {
        return false;
    }

    buffer[size - 1] = '\0';

    return a_str_equal(buffer, Prefix);
}

void a_file_writePrefix(AFile* File, const char* Prefix)
{
    a_file_write(File, Prefix, strlen(Prefix) + 1);
}

bool a_file_read(AFile* File, void* Buffer, size_t Size)
{
    size_t readCount;

    readCount = fread(Buffer, Size, 1, File->handle);

    if(readCount != 1) {
        a_out__warning("a_file_read: could not read %u bytes from %s",
                       Size, File->name);
        return false;
    }

    return true;
}

bool a_file_write(AFile* File, const void* Buffer, size_t Size)
{
    size_t writeCount;

    writeCount = fwrite(Buffer, Size, 1, File->handle);

    if(writeCount != 1) {
        a_out__error("a_file_write: could not write %u bytes to %s",
                     Size, File->name);
        return false;
    }

    return true;
}

bool a_file_writef(AFile* File, char* Format, ...)
{
    int ret;
    va_list args;
    va_start(args, Format);

    ret = vfprintf(File->handle, Format, args);

    va_end(args);

    if(ret < 0) {
        a_out__error("a_file_writef: could not write to %s", File->name);
        return false;
    }

    return true;
}

bool a_file_readLine(AFile* File)
{
    if(File->eof) {
        return false;
    }

    long offset = 0;
    FILE* handle = File->handle;

    while(!File->eof && offset == 0) {
        for(int c = fgetc(handle); !iscntrl(c); c = fgetc(handle), offset++) {
            if(c == EOF) {
                File->eof = true;
                break;
            }
        }
    }

    if(offset > 0) {
        if(File->eof) {
            fseek(handle, 0 - offset, SEEK_CUR);
        } else {
            fseek(handle, 0 - offset - 1, SEEK_CUR);
        }

        if((unsigned long)offset >= File->lineBufferSize) {
            free(File->lineBuffer);
            File->lineBufferSize = (unsigned long)offset + 1;
            File->lineBuffer = a_mem_malloc(File->lineBufferSize);
        }

        for(int i = 0; i < offset; i++) {
            File->lineBuffer[i] = (char)fgetc(handle);
        }

        File->lineBuffer[offset] = '\0';

        fseek(handle, 1, SEEK_CUR);

        return true;
    }

    return false;
}

char* a_file_getLine(const AFile* File)
{
    return File->lineBuffer;
}

void a_file_rewind(const AFile* File)
{
    rewind(File->handle);
}

void a_file_seekStart(const AFile* File, long int Offset)
{
    if(fseek(File->handle, Offset, SEEK_SET) != 0) {
        a_out__error("%s: could not seek %ld bytes from start",
                     File->name, Offset);
    }
}

void a_file_seekEnd(const AFile* File, long int Offset)
{
    if(fseek(File->handle, Offset, SEEK_END) != 0) {
        a_out__error("%s: could not seek %ld bytes from end",
                     File->name, Offset);
    }
}

void a_file_seekCurrent(const AFile* File, long int Offset)
{
    if(fseek(File->handle, Offset, SEEK_CUR) != 0) {
        a_out__error("%s: could not seek %ld bytes from current",
                     File->name, Offset);
    }
}

const char* a_file_path(const AFile* File)
{
    return File->path;
}

const char* a_file_name(const AFile* File)
{
    return File->name;
}

FILE* a_file_handle(const AFile* File)
{
    return File->handle;
}

bool a_file_exists(const char* Path)
{
    FILE* f = fopen(Path, "r");

    if(f) {
        fclose(f);
        return true;
    }

    return false;
}

bool a_file_isDir(const char* Path)
{
    struct stat info;

    if(stat(Path, &info) != 0) {
        a_out__error("a_file_isDir: stat(%s) failed", Path);
        return false;
    }

    return S_ISDIR(info.st_mode);
}

size_t a_file_size(const char* Path)
{
    struct stat info;

    if(stat(Path, &info) != 0) {
        a_out__error("a_file_size: stat(%s) failed", Path);
        return 0;
    }

    return (size_t)info.st_size;
}

uint8_t* a_file_toBuffer(const char* Path)
{
    AFile* f = a_file_open(Path, "rb");

    if(!f) {
        return NULL;
    }

    size_t size = a_file_size(Path);
    uint8_t* buffer = a_mem_malloc(size);

    if(!a_file_read(f, buffer, size)) {
        free(buffer);
        buffer = NULL;
    }

    a_file_close(f);

    return buffer;
}
