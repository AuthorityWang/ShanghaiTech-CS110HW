#include "dir.h"
#include "explorer.h"
#include "file.h"
#include "node.h"
#include <stdlib.h>
#include <string.h>

static bool dir_add_sub(struct directory *dirnode, struct node *sub) {
  if (!dirnode || !sub) {
    return false;
  }
  for (int i = 0; i < dirnode->size; i++) {
    if (strcmp(dirnode->subordinates[i]->name, sub->name) == 0) {
      return false;
    }
  }
  if (dirnode->size == dirnode->capacity) {
    dirnode->capacity *= 2;
    dirnode->subordinates = realloc(dirnode->subordinates,
                                    dirnode->capacity * sizeof(struct node *));
  }
  dirnode->subordinates[dirnode->size++] = sub;
  return true;
}

struct directory *dir_new(char *name) {
  /* Initialization */
  struct directory *dir = NULL;
  /* Check for null pointer */
  if (!name) {
    return NULL;
  }
  /* Allocate memory */
  dir = calloc(1, sizeof(struct directory));
  dir->capacity = DEFAULT_DIR_SIZE;
  dir->subordinates = calloc(dir->capacity, sizeof(struct node));
  dir->parent = NULL;
  /* Create base node */
  dir->base = node_new(true, name, dir);
  return dir;
}
#include <stdio.h>
void dir_release(struct directory *dir) {
  /* Initialization */
  int i = 0;
  if (!dir) {
    return;
  }
  /* Release all the subordiniates */
  for (i = 0; i < dir->size; i++) {
    node_release(dir->subordinates[i]);
  }
  /* Release the resources */
  /* Check if base has already been released. Prevent circular call. */
  if (dir->base) {
    dir->base->inner.dir = NULL;
    node_release(dir->base);
  }
  /* Release data and self. */
  free(dir->subordinates);
  free(dir);
}

struct node *dir_find_node(const struct directory *dir, const char *name) {
  /* YOUR CODE HERE */
  if (!dir || !name) {
    return NULL;
  }
  if (dir->size == 0) {
    return NULL;
  }
  for (int i = 0; i < dir->size; i++) {
    if (strcmp(dir->subordinates[i]->name, name) == 0) {
      return dir->subordinates[i];
    }
  }
  return NULL;
}

bool dir_add_file(struct directory *dir, int type, char *name) {
  /* YOUR CODE HERE */
  if (!dir || !name) {
    return false;
  }
  if (dir_find_node(dir, name)) {
    return false;
  }
  struct file *file = file_new(type, name);
  if (!file) {
    file_release(file);
    return false;
  }
  struct node *node = node_new(false, name, file);
  if (!node) {
    node_release(node);
    return false;
  }
  if (!dir_add_sub(dir, node)) {
    node_release(node);
    return false;
  }
  return true;
}

bool dir_add_subdir(struct directory *dir, char *name) {
  /* YOUR CODE HERE */
  if (!dir || !name) {
    return false;
  }
  if (dir_find_node(dir, name)) {
    return false;
  }
  struct directory *subdir = dir_new(name);
  if (!subdir) {
    dir_release(subdir);
    return false;
  }
  struct node *node = node_new(true, name, subdir);
  if (!node) {
    node_release(node);
    return false;
  }
  if (!dir_add_sub(dir, node)) {
    node_release(node);
    return false;
  }
  return true;
}

bool dir_delete(struct directory *dir, const char *name) {
  /* YOUR CODE HERE */
  if (!dir || !name) {
    return false;
  }
  if (dir->size == 0) {
    return false;
  }
  for (int i = 0; i < dir->size; i++) {
    if (strcmp(dir->subordinates[i]->name, name) == 0) {
      node_release(dir->subordinates[i]);
      for (int j = i; j < dir->size - 1; j++) {
        dir->subordinates[j] = dir->subordinates[j + 1];
      }
      dir->size--;
      return true;
    }
  }
  return false;
}
