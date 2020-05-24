#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include "BaseDefine.h"
#include "DB.h"

int createDB() {
    sqlite3 *db;
   	char *errmsg;
    int rc;
    char *sql;
    char *sql_fk; // Foreign Key 활성화 변수

    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
    	fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    sql_fk = "PRAGMA foreign_keys = 1"; // Foreign Key 활성화(1) 시킴
    rc = sqlite3_exec(db, sql_fk, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "참조키 활성화 에러 : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
    else
        return 0;

    //admin table create
   	sql = "CREATE TABLE ADMIN ("\
        "id TEXT(9) PRIMARY KEY NOT NULL,"\
        "access INTEGER(1),"\
        "pwd TEXT(513));";
    rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "Can't create ADMIN table : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
    else {
       	fprintf(stderr, "Created admin table successfully\n");
   	}

    //whitelist table create
    sql = "CREATE TABLE WHITELIST ("\
        "whitelist TEXT(30) PRIMARY KEY NOT NULL,"\
        "id TEXT(9) NOT NULL,"\
        "date TEXT(12),"\
        "CONSTRAINT id_fk FOREIGN KEY(id) REFERENCES ADMIN(id));";
   	rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "Can't create WHITELIST table : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
    else {
       	fprintf(stderr, "Created whitelist table successfully\n");
    }

    sqlite3_close(db);

    return 0;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) { // callback
    int i;
    for(i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i]:"NULL");
    }
    printf("\n");

    return 0;
}

int openDB() { // CPS.db open
    sqlite3 *db;
   	char *errmsg;
    int rc;
    char *sql_fk; // Foreign Key 활성화 변수

    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
    	fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    sql_fk = "PRAGMA foreign_keys = 1"; // Foreign Key 활성화(1) 시킴
    rc = sqlite3_exec(db, sql_fk, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "참조키 활성화 에러 : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
    else
        return 0;
}



int insWhiteListTable(struct WhiteListTable ins_wt) { // case 16
    struct WhiteListTable wt;

    sqlite3 *db;
   	char *errmsg;
    int rc;
    char input_sql[512];
    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };

    char date[DATElen] = { 0, };
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    openDB();

    puts("WHITELIST TABLE's insert\n");
    puts("input whitelist:");
    gets(whitelist);
    puts("input id:");
    gets(id);
    puts("date Enter:");
    printf("%s\n", str_now);
    strcpy(date, str_now);

    fflush(stdin);
    strcpy(input_sql, "insert into WHITELIST values('");
    strcat(input_sql, whitelist);
    strcat(input_sql, "','");
    strcat(input_sql, id);
    strcat(input_sql, "','");
    strcat(input_sql, date);
    strcat(input_sql, "');");
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't input : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print input successfully\n");
    }

    sqlite3_close(db);

    return 0;
}
