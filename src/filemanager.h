#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "model.h"

void open_file(char *filename, model_t *model);
void save_file(char *filename, model_t *model);

#endif  // FILEMANAGER_H