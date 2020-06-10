#ifndef DBDeleteModule_H
#define DBDeleteModule_H

static int callback(void *NotUsed, int argc, char **argv, char **azColName); // callback
int delCol(); // case 46
int delRow(); // case 47

#endif
