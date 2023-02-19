#include "dir.h"
#include "explorer.h"
#include "file.h"
#include "node.h"
#include <stdlib.h>
#include <string.h>

/* define a dir_add_sub function */
static bool dir_add_sub(struct directory *dirnode, struct node *sub) {
  /* define the dir_add_sub for convience*/
  int i = 0;
  if (!dirnode || !sub) {
    return false;
  }
  /* travseral the directory and find if same name  */
  for (i = 0; i < dirnode->size; i++) {
    if (strcmp(dirnode->subordinates[i]->name, sub->name) == 0) {
      return false;
    }
  }
  /* if size overflow, then realloc the previous memory */
  if (dirnode->size == dirnode->capacity) {
    dirnode->capacity *= 2;
    dirnode->subordinates = realloc(dirnode->subordinates,
                                    dirnode->capacity * sizeof(struct node *));
  }
  /* add the new sub to the subordinates */
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
  /* check the null ptr*/
  int i = 0;
  if (!dir || !name) {
    return NULL;
  }
  if (dir->size == 0) {
    return NULL;
  }
  /* travseral the subordinates and find the name */
  for (i = 0; i < dir->size; i++) {
    if (strcmp(dir->subordinates[i]->name, name) == 0) {
      return dir->subordinates[i];
    }
  }
  /* return NULL if no same name find */
  return NULL;
}

bool dir_add_file(struct directory *dir, int type, char *name) {
  /* YOUR CODE HERE */
  struct file *file = file_new(type, name);
  struct node *node = node_new(false, name, file);
  /* deal with null ptr */
  if (!file) {
    file_release(file);
    return false;
  }
  /* deal with null ptr */
  if (!node) {
    node_release(node);
    return false;
  }
  /* deal with null ptr */
  if (!dir || !name) {
    file_release(file);
    node_release(node);
    return false;
  }
  /* deal with same name */
  if (dir_find_node(dir, name)) {
    file_release(file);
    node_release(node);
    return false;
  }
  /* deal with possible fail case */
  if (!dir_add_sub(dir, node)) {
    node_release(node);
    return false;
  }
  /* default return true */
  return true;
}

bool dir_add_subdir(struct directory *dir, char *name) {
  /* YOUR CODE HERE */
  struct directory *subdir = dir_new(name);
  struct node *node = node_new(true, name, subdir);
  /* deal with null ptr */
  if (!subdir) {
    dir_release(subdir);
    return false;
  }
  /* deal with null ptr */
  if (!node) {
    node_release(node);
    return false;
  }
  /* deal with null ptr */
  if (!dir || !name) {
    dir_release(subdir);
    node_release(node);
    return false;
  }
  /* deal with same name */
  if (dir_find_node(dir, name)) {
    dir_release(subdir);
    node_release(node);
    return false;
  }
  /* deal with possible fail case */
  if (!dir_add_sub(dir, node)) {
    node_release(node);
    return false;
  }
  /* default return true */
  return true;
}

bool dir_delete(struct directory *dir, const char *name) {
  /* YOUR CODE HERE */
  int i = 0;
  int j = 0;
  /* deal with null ptr */
  if (!dir || !name) {
    return false;
  }
  if (dir->size == 0) {
    return false;
  }
  /* travseral the subordinates and find the name */
  for (i = 0; i < dir->size; i++) {
    if (strcmp(dir->subordinates[i]->name, name) == 0) {
      node_release(dir->subordinates[i]);
      /* move the subordinates */
      for (j = i; j < dir->size - 1; j++) {
        dir->subordinates[j] = dir->subordinates[j + 1];
      }
      /* decrease the size */
      dir->size--;
      return true;
    }
    /* default return false */
  }
  return false;
}
