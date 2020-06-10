#ifndef DBSearchModule_H
#define DBSearchModule_H

static int callback(void *NotUsed, int argc, char **argv, char **azColName); // callback
int search_id(); // case 36 : id를 기준으로 검색
int search_wl(); // case 37 : whitelist를 기준으로 검색

#endif
