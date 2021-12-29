#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "object.h"

void open_file(char *filename, object_t *model);
void save_file(char *filename, object_t *model);

#endif  // FILEMANAGER_H