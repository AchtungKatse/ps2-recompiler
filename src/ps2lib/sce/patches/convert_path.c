#include "ps2ctx/sce/patches/convert_path.h"
#include <stdlib.h>
#include <string.h>

char* ConvertDiskPath(char* path)
{
    const char* cdRomPath = "cdrom0:\\";
    const int cdRomLen = strlen(cdRomPath);

    int match = strncmp(path, cdRomPath, strlen(cdRomPath));
    if (match == 0)
    {
        const char* gameFiles = "GameFiles/";
        char* newPath = (char*)malloc(strlen(path) - strlen(cdRomPath) + strlen(gameFiles) + 1);
        strcpy(newPath, gameFiles);

        for (int i = 0; i < strlen(path) - cdRomLen; i++)
        {
            char c = path[cdRomLen + i];

            if (c == ';')
            {
                newPath[cdRomLen + i + 2] = '\x0';
                break;
            }

            newPath[cdRomLen + i + 2] = c;
        }
        return newPath;
    }
    else
    {
        return path;
    }
}

