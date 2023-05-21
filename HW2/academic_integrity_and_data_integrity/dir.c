#include "dir.h"
#include "explorer.h"
#include "file.h"
#include "node.h"
#include <stdlib.h>
#include <string.h>

/* define a dir_add_sub function */
static bool dir_add_sub(struct directory *dirnode, struct node *sub) {
  /* Initialization */
  struct node **new_subordinates = NULL;
  /* Check for null pointer */
  if (!dirnode || !sub) {
    return false;
  }
  /* Check if the subordinates is full */
  if (dirnode->size == dirnode->capacity) {
    /* Allocate memory */
    new_subordinates = calloc(dirnode->capacity * 2, sizeof(struct node));
    /* Copy the subordinates */
    memcpy(new_subordinates, dirnode->subordinates,
           dirnode->capacity * sizeof(struct node));
    /* Release the old subordinates */
    free(dirnode->subordinates);
    /* Update the subordinates */
    dirnode->subordinates = new_subordinates;
    dirnode->capacity *= 2;
  }
  /* Add the subordinates */
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
  /* deal with null ptr */
  if (!dir || !name) {
    return NULL;
  }
  /* travseral the subordinates and find the name */
  for (int i = 0; i < dir->size; i++) {
    if (strcmp(dir->subordinates[i]->name, name) == 0) {
      return dir->subordinates[i];
    }
  }
  /* default return NULL */
  return NULL;
}

bool dir_add_file(struct directory *dir, int type, char *name) {
  /* YOUR CODE HERE */
  if (dir_find_node(dir, name)) {
    return false;
  }
  /* deal with null ptr */
  if (!dir || !name) {
    return false;
  }
  /* init subdir and node*/
  struct file *subfile = file_new(type, name);
  struct node *node = node_new(false, name, subfile);
  /* deal with possible fail case */
  dir_add_sub(dir, node);
  /* default return true */
  return true;
}

bool dir_add_subdir(struct directory *dir, char *name) {
  /* YOUR CODE HERE */
  /* deal with same name */
  if (dir_find_node(dir, name)) {
    return false;
  }
  /* deal with null ptr */
  if (!dir || !name) {
    return false;
  }
  /* init subdir and node*/
  struct directory *subdir = dir_new(name);
  subdir->parent = dir;
  struct node *node = node_new(true, name, subdir);
  /* deal with possible fail case */
  dir_add_sub(dir, node);
  /* default return true */
  return true;
}

bool dir_delete(struct directory *dir, const char *name) {
  /* YOUR CODE HERE */
  /* deal with null ptr */
  if (!dir || !name) {
    return false;
  }
  /* travseral the subordinates and find the name */
  for (int i = 0; i < dir->size; i++) {
    if (strcmp(dir->subordinates[i]->name, name) == 0) {
      /* delete the node */
      node_release(dir->subordinates[i]);
      /* move the last node to the deleted node */
      for (int j = i; j < dir->size - 1; j++) {
        dir->subordinates[j] = dir->subordinates[j + 1];
      }
      /* decrease the size */
      dir->size--;
      return true;
    }
  }
  /* default return false */
  return false;
}
