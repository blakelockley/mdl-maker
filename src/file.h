#ifndef FILE_H
#define FILE_H

#include "model.h"

void open_file(char *filename, model_t *model);
void save_file(char *filename, model_t *model);

#endif  // FILE_H