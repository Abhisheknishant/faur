/*
    Copyright 2010 Alex Margarit

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

#include "a2x_pack_file.h"

int a_file_exists(const char* const path)
{
    File* const f = a_file_openRead(path);

    if(f) {
        a_file_close(f);
        return 1;
    }

    return 0;
}

int a_file_dirExists(const char* const path)
{
    Dir* const d = a_file_openDir(path);

    if(d) {
        a_file_closeDir(d);
        return 1;
    }

    return 0;
}

int a_file_hasKey(const char* const path, const char* const key)
{
    File* const f = a_file_openRead(path);

    if(f) {
        char* const buffer = a_str_malloc(key);
        a_file_rp(f, buffer, a_str_size(key));

        int verdict = a_str_same(buffer, key);

        free(buffer);
        a_file_close(f);

        return verdict;
    }

    return 0;
}

int a_file_isDir(const char* const f)
{
    struct stat info;
    stat(f, &info);

    return S_ISDIR(info.st_mode);
}

int _a_file_selector(const struct dirent* f)
{
    return a_file_validName(f->d_name);
}

List* a_file_listFilenames(const char* const path, int (*selector)(const struct dirent* f))
{
    List* list = a_list_set();

    if(!a_file_dirExists(path)) return list;
    if(!selector) selector = &_a_file_selector;

    struct dirent** dlist;
    const int numFiles = scandir(path, &dlist, selector, alphasort);

    for(int i = numFiles; i--; ) {
        a_list_addFirst(list, a_str_dup(dlist[i]->d_name));

        free(dlist[i]);
    }

    free(dlist);

    return list;
}

List* a_file_listFullpath(const char* const path, int (*selector)(const struct dirent* f))
{
    List* list = a_list_set();

    if(!a_file_dirExists(path)) return list;
    if(!selector) selector = &_a_file_selector;

    struct dirent** dlist;
    const int numFiles = scandir(path, &dlist, selector, alphasort);

    for(int i = numFiles; i--; ) {
        char* const name = a_str_malloc2(3, path, "/", dlist[i]->d_name);
        sprintf(name, "%s/%s", path, dlist[i]->d_name);

        a_list_addFirst(list, name);

        free(dlist[i]);
    }

    free(dlist);

    return list;
}

List* a_file_listFullpath2(const char* const path, List* const files)
{
    List* const list = a_list_set();

    while(a_list_iterate(files)) {
        char* const file = a_list_current(files);
        char* const full = a_str_malloc2(3, path, "/", file);

        sprintf(full, "%s/%s", path, file);

        a_list_addLast(list, full);
    }

    return list;
}

char* a_file_pathFileString(const char* const path, const char* const file)
{
    char* const full = a_str_malloc2(3, path, "/", file);
    sprintf(full, "%s/%s", path, file);

    return full;
}

int a_file_size(const char* const f)
{
    struct stat info;
    stat(f, &info);

    return info.st_size;
}

char* a_file_readLine(File* const f)
{
    int offset = 1;

    while(offset == 1) {
        int c;

        for(c = fgetc(f); !iscntrl(c) && c != EOF; c = fgetc(f), offset++) {
            continue;
        }

        if(c == EOF) {
            break;
        }
    }

    if(offset > 1) {
        a_file_jump(f, -offset);

        char* str = malloc(offset * sizeof(char));

        for(int i = 0; i < offset - 1; i++) {
            str[i] = fgetc(f);
        }

        str[offset - 1] = '\0';

        return str;
    }

    return NULL;
}
