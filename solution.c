#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "DB.h"
#include "BaseDefine.h"
#include <string.h>

#pragma foreign_keys = 1 // 참조키 활성화

int createDB() {
    sqlite3 *db;
   	char *errmsg;
    int rc;
    char *sql;

    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
    	fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "=CPS.db open=\n");
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
       	return 1;
    }
    else {
       	fprintf(stderr, "Created ADMIN table successfully\n");
   	}

    //whitelist table create
    sql = "CREATE TABLE WHITELIST ("\
        "whitelist TEXT(30) PRIMARY KEY NOT NULL,"\
        "id TEXT(9) NOT NULL,"\
        //"date TEXT(20));";
        "date TEXT(20),"\
        "CONSTRAINT id FOREIGN KEY(id) REFERENCES ADMIN(id));";

   	rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if(rc != SQLITE_OK) {
       	fprintf(stderr, "Can't create WHITELIST table : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
    else {
       	fprintf(stderr, "Created WHITELIST table successfully\n");
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

void printBOF_gets(char str[], int strsize, int define_size) { // gets() 버퍼오버플로우 방지 : 재입력 요구 메시지 출력
    printf("입력한 byte(s)는 %dbyte(s)입니다.\n", strsize-1);
    printf("입력 가능한 최대 길이 %dbyte(s)보다 %dbyes(s)가 초과됐습니다.\n", define_size-1, strsize-define_size);
    printf("입력할 데이터를 다시 확인해보세요.\n");
}

int inADMIN() { // case 16 ok
    sqlite3 *db;
    char *errmsg;
    int rc;
    char input_sql[SQLlen] = { 0, };
    char id[IDlen] = { 0, };
    char pwd[PWDlen] = { 0, };
    char access[ACCESSlen] = { 0, };
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened CPS database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    puts("\nADMIN TABLE's insert\n");

    while(1) { // id 필수로 입력받음
        printf("ID는 등록 후에 수정이 불가능합니다!!!\n");
        printf("ID는 필수입력 정보입니다.\n");
        puts("input id (9bytes 보다 길면 다시 입력함):");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize <= IDlen)
            break;

        printBOF_gets(str, strsize, IDlen);
    }

    strncpy(id, str, IDlen-1);

    while(1) { // access 입력받음
        printf("ACCESS는 수정이 가능합니다.\n");
        puts("input access (1byte 보다 길면 다시 입력함):");
        gets(str);

        strsize = strlen(str)+1;
        if(strsize <= ACCESSlen)
            break;

        printBOF_gets(str, strsize, ACCESSlen);
    }

    strncpy(access, str, ACCESSlen-1);

    while(1) { // pwd 필수로 입력받음
        printf("PWD는 수정이 가능합니다.\n");
        printf("PWD는 필수입력 정보입니다.\n");
        puts("input pwd (513bytes 보다 길면 다시 입력함):");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize <= PWDlen)
            break;

        printBOF_gets(str, strsize, PWDlen);
    }

    strncpy(pwd, str, PWDlen-1);

    __fpurge(stdin);
    strncpy(input_sql, "insert into ADMIN(id, access, pwd) values('", 43);
    strncat(input_sql, id, IDlen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, access, ACCESSlen-1);
    strncat(input_sql, "','", 3);
    strncat(input_sql, pwd, PWDlen-1);
    strncat(input_sql, "');", 3);
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't input : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "admin table input successfully\n");
    }

    sqlite3_close(db);

    return 0;
}
