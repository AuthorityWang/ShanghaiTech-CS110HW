#include "file.h"
#include "explorer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct file *file_new(int type, char *name) {
  /* Initialization */
  struct file *file = NULL;
  /* Check for integrity */
  if (!name || type < 0 || type >= MAX_FT_NO) {
    return NULL;
  }
  /* Allocate memory and initialze the file. */
  file = calloc(1, sizeof(struct file));
  file->type = type;
  file->size = DEFAULT_FILE_SIZE;
  file->data = calloc(file->size, 1);
  /* Crate associtate node and set it to base. */
  file->base = node_new(false, name, file);
  return file;
}

void file_release(struct file *file) {
  /* Check for null pointer. */
  if (!file) {
    return;
  }
  /* relase the data. */
  /* Check if base has already been released. Prevent circular call. */
  if (file->base) {
    file->base->inner.file = NULL;
    node_release(file->base);
  }
  /* Release the resources. */
  free(file->data);
  free(file);
}

bool file_write(struct file *file, int offset, int bytes, const char *buf) {
  /* YOUR CODE HERE */
  /* firstly judge if there is special cases */
  if (offset < 0) {
    return false;
  }
  if (!file || !buf) {
    return false;
  }
  /* if bigger, then realloc memory to store the data */
  if (offset + bytes > file->size) {
    file->data = realloc(file->data, offset + bytes);
    file->size = offset + bytes;
  }
  /* after realloc, copy data to that */
  memcpy(file->data + offset, buf, bytes);
  return true;
}

bool file_read(const struct file *file, int offset, int bytes, char *buf) {
  /* YOUR CODE HERE */
  /* firstly judge the special case */
  if (offset < 0) {
    return false;
  }
  if (offset + bytes > file->size) {
    return false;
  }
  /* deal with null ptr*/
  if (!file || !buf) {
    return false;
  }
  /* copy the previous data to memory */
  memcpy(buf, file->data + offset, bytes);
  return true;
}
