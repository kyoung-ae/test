#define _CTR_SECURE_NO_WARNINGS

#pragma foreign_keys = 1 // 참조키 활성화

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <memory.h>

#include "sqlite3.h"
#include "DBCreate.h"
#include "DBPrintModule.h"
#include "DBSearchModule.h"
#include "DBManage.h"
#include "DBInsertModule.h"
#include "DBUpdateModule.h"
#include "DBDeleteModule.h"
#include "DBlen.h"
#include "DBLogin.h"
#include "DBBackupR.h"

int createDB() {
    sqlite3 *db;
   	char *errmsg;
    int rc;
    char *sql;

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS.db : %s\n", sqlite3_errmsg(db));
        printf("DB가 연결되지 않아서 종료됩니다.\n");
       	return 0;
    }
   	else {
        fprintf(stderr, "Opened CPS.db\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    //admin table create
   	sql = "CREATE TABLE ADMIN ("\
        "id TEXT(9) PRIMARY KEY NOT NULL,"\
        "access TEXT(1),"\
        "pwd TEXT(513) NOT NULL);";
    rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "Can't create ADMIN table : %s\n", sqlite3_errmsg(db));
       	//return 0;
    }
    else {
       	fprintf(stderr, "Created ADMIN table successfully\n");
   	}

   	//info table create
   	sql = "CREATE TABLE INFO ("\
        "id TEXT(9) PRIMARY KEY NOT NULL,"\
        "name TEXT(30),"\
        "birth TEXT(6),"\
        "email TEXT(50) NOT NULL,"\
        "phone TEXT(20),"\
        //"I_date TEXT(20));";
        "I_date TEXT(20),"\
        "UNIQUE(email)," // email은 유일한 값이어야 함.
        "CONSTRAINT id FOREIGN KEY(id) REFERENCES ADMIN(id));";

    rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "Can't create INFO table : %s\n", sqlite3_errmsg(db));
       	//return 0;
    }
    else {
       	fprintf(stderr, "Created INFO table successfully\n");
    }

    //whitelist table create
    sql = "CREATE TABLE WHITELIST ("\
        "whitelist TEXT(30) PRIMARY KEY NOT NULL,"\
        "id TEXT(9) NOT NULL,"\
        //"W_date TEXT(20));";
        "W_date TEXT(20),"\
        "CONSTRAINT id FOREIGN KEY(id) REFERENCES ADMIN(id));";

   	rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "Can't create WHITELIST table : %s\n", sqlite3_errmsg(db));
       	//return 0;
    }
    else {
       	fprintf(stderr, "Created WHITELIST table successfully\n");
    }

    sqlite3_close(db);
    return 1;
}
