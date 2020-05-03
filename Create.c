#include <stdio.h>
#include <sqlite3.h>
#include "BaseDefine.h"
#include "DB.h"

int createDB() {
    sqlite3 *db;
   	char *errmsg;
    int rc;
    char *sql;

    //WHITELIST DB CREATE
    rc = sqlite3_open("WHITELIST.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open WHITELIST DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened WHITELIST database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    //whitelist table create
    sql = "CREATE TABLE WHITELIST ("\
        "whitelist TEXT(30) PRIMARY KEY NOT NULL,"\
        "id TEXT(9) NOT NULL,"\
        "date TEXT(12));";
   	rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "Can't create WHITELIST table : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
    else {
       	fprintf(stderr, "Created whitelist table successfully\n");
    }

    //ADMINISTRATOR DB CREATE
    rc = sqlite3_open("ADMINISTRATOR.db", &db);
    if(rc != SQLITE_OK) {
    	fprintf(stderr, "Can't open ADMINISTRATOR DB : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Opened ADMINISTRATOR database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    //mac table create
    sql = "CREATE TABLE MAC ("\
        "id TEXT(9) PRIMARY KEY NOT NULL,"\
        "public_key TEXT(270),"\
        "mac0 TEXT(17) NOT NULL,"\
        "mac1 TEXT(17),"\
        "mac2 TEXT(17));";
    rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "Can't create MAC table : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
    else {
       	fprintf(stderr, "Created mac table successfully\n");
    }

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

   	//info table create
   	sql = "CREATE TABLE INFO ("\
        "id TEXT(9) PRIMARY KEY NOT NULL,"\
        "name TEXT(30),"\
        "birth TEXT(6),"\
        "email TEXT(50),"\
        "phone TEXT(20),"\
        "date TEXT(12));";
    rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "Can't create INFO table : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
    else {
       	fprintf(stderr, "Created info table successfully\n");
    }

    sqlite3_close(db);

    return 0;
}
