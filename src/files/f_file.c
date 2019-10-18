/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_file.v.h"
#include <faur.v.h>

AFile* f_file_new(const char* Path, AFileMode Mode)
{
    AFile* f = NULL;
    APath* path = f_path_new(Path);

    if(F_FLAGS_TEST_ANY(Mode, F_FILE_WRITE)
        || f_path_test(path, F_PATH_FILE | F_PATH_REAL)) {

        f = f_file_real__new(path, Mode);
    } else if(f_path_test(path, F_PATH_FILE | F_PATH_EMBEDDED)) {
        f = f_file_embedded__new(path);
    } else {
        f_out__warning("f_file_new(%s): File does not exist", Path);
    }

    if(f == NULL) {
        f_path_free(path);
    }

    return f;
}

void f_file_free(AFile* File)
{
    if(File == NULL) {
        return;
    }

    if(f_path_test(File->path, F_PATH_REAL)) {
        fclose(File->u.handle);
    }

    f_path_free(File->path);

    f_mem_free(File->lineBuffer);
    f_mem_free(File);
}

const APath* f_file_pathGet(const AFile* File)
{
    return File->path;
}

FILE* f_file_handleGet(const AFile* File)
{
    if(f_path_test(File->path, F_PATH_REAL)) {
        return File->u.handle;
    } else {
        return NULL;
    }
}

const AEmbeddedFile* f_file__dataGet(AFile* File)
{
    if(f_path_test(File->path, F_PATH_EMBEDDED)) {
        return File->u.e.data;
    } else {
        return NULL;
    }
}

uint8_t* f_file_toBuffer(const char* Path)
{
    if(f_path_exists(Path, F_PATH_FILE | F_PATH_REAL)) {
        return f_file_real__toBuffer(Path);
    } else if(f_path_exists(Path, F_PATH_FILE | F_PATH_EMBEDDED)) {
        return f_file_embedded__toBuffer(Path);
    }

    return NULL;
}

bool f_file_prefixCheck(AFile* File, const char* Prefix)
{
    size_t size = strlen(Prefix) + 1;
    char buffer[size];

    f_file_seekStart(File, 0);

    if(!f_file_read(File, buffer, size)) {
        return false;
    }

    buffer[size - 1] = '\0';

    return f_str_equal(buffer, Prefix);
}

void f_file_prefixWrite(AFile* File, const char* Prefix)
{
    f_file_write(File, Prefix, strlen(Prefix) + 1);
}

bool f_file_read(AFile* File, void* Buffer, size_t Size)
{
    bool ret = File->interface->read(File, Buffer, Size);

    if(!ret) {
        f_out__warning("f_file_read(%s): Could not read %u bytes",
                       f_path_getFull(File->path),
                       Size);
    }

    return ret;
}

bool f_file_write(AFile* File, const void* Buffer, size_t Size)
{
    bool ret = File->interface->write(File, Buffer, Size);

    if(!ret) {
        f_out__error("f_file_write(%s): Could not write %u bytes",
                     f_path_getFull(File->path),
                     Size);
    }

    return ret;
}

bool f_file_writef(AFile* File, const char* Format, ...)
{
    va_list args;
    va_start(args, Format);

    bool ret = File->interface->writef(File, Format, args);

    va_end(args);

    if(!ret) {
        f_out__error(
            "f_file_writef(%s): Could not write", f_path_getFull(File->path));
    }

    return ret;
}

bool f_file_flush(AFile* File)
{
    return File->interface->flush(File);
}

static int readChar(AFile* File)
{
    int ch = File->interface->getchar(File);

    if(ch == '\r') {
        // Check if \r is followed by \n for CRLF line endings
        ch = File->interface->getchar(File);

        if(ch != '\n') {
            // \r not followed by \n, assume CR line endings and put char back
            ch = File->interface->ungetchar(File, ch);

            if(ch != EOF) {
                File->lineNumber++;
                ch = '\r';
            }
        }
    }

    if(ch == '\n') {
        // Sequence was \n or \r\n
        File->lineNumber++;
    } else if(ch == EOF) {
        File->eof = true;
    }

    return ch;
}

bool f_file_lineRead(AFile* File)
{
    int ch;

    do {
        ch = readChar(File);
    } while(ch == '\n' || ch == '\r');

    if(File->eof) {
        return false;
    }

    unsigned index = 0;

    do {
        if(index + 1 >= File->lineBufferSize) {
            unsigned newSize = f_math_maxu(File->lineBufferSize * 2, 64);
            char* newBuffer = f_mem_malloc(newSize);

            if(File->lineBufferSize > 0) {
                memcpy(newBuffer, File->lineBuffer, File->lineBufferSize);
            }

            f_mem_free(File->lineBuffer);

            File->lineBuffer = newBuffer;
            File->lineBufferSize = newSize;
        }

        File->lineBuffer[index++] = (char)ch;
        ch = readChar(File);
    } while(!File->eof && ch != '\n' && ch != '\r');

    File->lineBuffer[index] = '\0';

    return true;
}

const char* f_file_lineBufferGet(const AFile* File)
{
    return File->lineBuffer;
}

unsigned f_file_lineNumberGet(const AFile* File)
{
    return File->lineNumber;
}

bool f_file_rewind(AFile* File)
{
    bool ret = File->interface->seek(File, 0, F_FILE__OFFSET_START);

    if(ret) {
        File->lineNumber = 0;
        File->eof = false;
    } else {
        f_out__error("f_file_rewind(%s) failed", f_path_getFull(File->path));
    }

    return ret;
}

bool f_file_seekStart(AFile* File, int Offset)
{
    bool ret = File->interface->seek(File, Offset, F_FILE__OFFSET_START);

    if(!ret) {
        f_out__error("f_file_seekStart(%s, %d) failed",
                     f_path_getFull(File->path),
                     Offset);
    }

    return ret;
}

bool f_file_seekEnd(AFile* File, int Offset)
{
    bool ret = File->interface->seek(File, Offset, F_FILE__OFFSET_END);

    if(!ret) {
        f_out__error("f_file_seekEnd(%s, %d) failed",
                     f_path_getFull(File->path),
                     Offset);
    }

    return ret;
}

bool f_file_seekCurrent(AFile* File, int Offset)
{
    bool ret = File->interface->seek(File, Offset, F_FILE__OFFSET_CURRENT);

    if(!ret) {
        f_out__error("f_file_seekCurrent(%s, %d) failed",
                     f_path_getFull(File->path),
                     Offset);
    }

    return ret;
}
