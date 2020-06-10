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
#include "DBProgram.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName) { // callback
    int i;
    for(i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i]:"NULL");
    }
    printf("\n");

    return 0;
}

int delCol() { // case 46
    sqlite3 *db;
    //sqlite3_stmt *res;
    char *errmsg;
    int rc;
    char sql_delcol[SQLlen] = { 0, };
    char play[IDlen] = {"삭제\0"};

    char id[IDlen] = { 0, };
    char name[NAMElen] = { 0, };
    char birth[BIRTHlen] = { 0, };
    char email[EMAILlen] = { 0, };
    char phone[PHONElen] = { 0, };
    char I_date[DATElen] = { 0, };

    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    int del_c = 0; // 삭제 분류 번호

    int i = 0; // 반복 횟수 제한

    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t); // DBManage.c

    const char *NotUsed = "Callback Function Called";

    // CPS.db OPEN
    rc = sqlite3_open("CPS.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open CPS.db : %s\n", sqlite3_errmsg(db));
        printf("DB가 연결되지 않아서 종료됩니다.\n");
       	return 0;
    }
   	else {
        //fprintf(stderr, "Opened CPS.db\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    printf("\n<Delete col!!!>\n");

    printf("삭제하려는 데이터의 id와 pwd가 일치해야 삭제됩니다.\n");

    for(i = 0; i < 5; i++) { // id 확인 5회로 제한
        while(1) { // 삭제하려는 데이터의 id
            memset(id, '\0', IDlen);
            memset(str, '\0', IDlen);
            strsize = 0;
            puts("\n삭제하려는 항목의 id(기본키) 입력:");
            gets(str);

            if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
                continue;
            strsize = strlen(str)+1;
            if(strsize >= 6 && strsize <= IDlen) // id는 5~9bytes 길이 제한
                break;
            else if(strsize < 6) {
                printf("5 btyes 보다 길게 입력하세요!\n");
                continue;
            }
            else
                printBOF_gets(str, strsize, IDlen); // DBPrintModule.c
        }
        strncpy(id, str, IDlen-1);

        if(checkID(id) == 1) // DBManage.c
            break;
        else {
            if(i <= 2)
                printf("총 5회 시도 중에 %d번 남았습니다.\n", 4-i);
            if(i == 3)
                printf("\n마지막 시도입니다. 등록된 id와 일치하지 않으면 기능 선택 화면으로 돌아갑니다.\n");
        }
    }
    if(i == 5) {
        printf("\n5번 모두 id가 일치하지 않아 기능 선택 화면으로 돌아갑니다.\n\n");
        printMenu();
    }

    if(checkFunc(id) == 0) { // DBManage.c
        sqlite3_close(db);
        printf("기능 선택 화면으로 돌아갑니다.\n");
        printMenu();
    }

    while(1) { // 삭제 항목 입력받는 무한루프
        del_c = 0;
        print_del_col(); // DBPrintModule.c
        puts("input :");
        scanf("%d", &del_c);
        removeEnter(); // DBManage.c

        if(del_c == 0) {
            sqlite3_close(db);
            exit(0);
        }
        if(del_c >= 1 && del_c < 8)
            break;
        else
            continue;
    }

    switch(del_c) {
        case 1:
            printf("삭제하려는 ");

            memset(sql_delcol, '\0', SQLlen);
            strncpy(sql_delcol, "SELECT name FROM info where id = '", 34);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            checkRep(id, play); // DBManage.c

            memset(sql_delcol, '\0', SQLlen);
            strncpy(I_date, str_now, DATElen);
            strncpy(sql_delcol, "UPDATE INFO SET (name, I_date) = ('', '", 39);
            strncat(sql_delcol, I_date, DATElen-1);
            strncat(sql_delcol, "') WHERE id = '", 15);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (name) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Delete success (name)\n");
                if(checkAg(play) == 1) // DBManage.c
                    delCol();
            }
            memset(sql_delcol, '\0', SQLlen);
            break;

        case 2:
            printf("삭제하려는 ");

            memset(sql_delcol, '\0', SQLlen);
            strncpy(sql_delcol, "SELECT birth FROM info where id = '", 35);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            checkRep(id, play); // DBManage.c

            memset(sql_delcol, '\0', SQLlen);
            strncpy(I_date, str_now, DATElen);
            strncpy(sql_delcol, "UPDATE INFO SET (birth, I_date) = ('', '", 40);
            strncat(sql_delcol, I_date, DATElen-1);
            strncat(sql_delcol, "') WHERE id = '", 15);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (birth) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Delete success (birth)\n");
                if(checkAg(play) == 1) // DBManage.c
                    delCol();
            }
            memset(sql_delcol, '\0', SQLlen);
            break;

        case 3:
            printf("삭제하려는 ");

            memset(sql_delcol, '\0', SQLlen);
            strncpy(sql_delcol, "SELECT phone FROM info where id = '", 35);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            checkRep(id, play); // DBManage.c

            memset(sql_delcol, '\0', SQLlen);
            strncpy(I_date, str_now, DATElen);
            strncpy(sql_delcol, "UPDATE INFO SET (phone, I_date) = ('', '", 40);
            strncat(sql_delcol, I_date, DATElen-1);
            strncat(sql_delcol, "') WHERE id = '", 15);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (phone) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Delete success (phone)\n");
                if(checkAg(play) == 1) // DBManage.c
                    delCol();
            }
            memset(sql_delcol, '\0', SQLlen);
            break;

        case 4:
            printf("삭제하려는 name과 birth 값\n");

            memset(sql_delcol, '\0', SQLlen);
            strncpy(sql_delcol, "SELECT name, birth FROM info where id = '", 41);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            checkRep(id, play); // DBManage.c

            memset(sql_delcol, '\0', SQLlen);
            strncpy(I_date, str_now, DATElen);
            strncpy(sql_delcol, "UPDATE INFO SET (name, birth, I_date) = ('','', '", 49);
            strncat(sql_delcol, I_date, DATElen-1);
            strncat(sql_delcol, "') WHERE id = '", 15);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (name, birth) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Delete success (name, birth)\n");
                if(checkAg(play) == 1) // DBManage.c
                    delCol();
            }
            memset(sql_delcol, '\0', SQLlen);
            break;

        case 5:
            printf("삭제하려는 name과 phone 값\n");

            memset(sql_delcol, '\0', SQLlen);
            strncpy(sql_delcol, "SELECT name, phone FROM info where id = '", 41);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            checkRep(id, play); // DBManage.c

            memset(sql_delcol, '\0', SQLlen);
            strncpy(I_date, str_now, DATElen);
            strncpy(sql_delcol, "UPDATE INFO SET (name, phone, I_date) = ('','', '", 49);
            strncat(sql_delcol, I_date, DATElen-1);
            strncat(sql_delcol, "') WHERE id = '", 15);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (name, phone) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Delete success (name, phone)\n");
                if(checkAg(play) == 1) // DBManage.c
                    delCol();
            }
            memset(sql_delcol, '\0', SQLlen);
            break;

        case 6:
            printf("삭제하려는 birth와 phone 값\n");

            memset(sql_delcol, '\0', SQLlen);
            strncpy(sql_delcol, "SELECT birth, phone FROM info where id = '", 42);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            checkRep(id, play); // DBManage.c

            memset(sql_delcol, '\0', SQLlen);
            strncpy(I_date, str_now, DATElen);
            strncpy(sql_delcol, "UPDATE INFO SET (birth, phone, I_date) = ('','', '", 50);
            strncat(sql_delcol, I_date, DATElen-1);
            strncat(sql_delcol, "') WHERE id = '", 15);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (birth, phone) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Delete success (birth, phone)\n");
                if(checkAg(play) == 1) // DBManage.c
                    delCol();
            }
            memset(sql_delcol, '\0', SQLlen);
            break;

        case 7:
            printf("삭제하려는 name과 birth와 phone 값\n");

            memset(sql_delcol, '\0', SQLlen);
            strncpy(sql_delcol, "SELECT name, birth, phone FROM info where id = '", 48);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            checkRep(id, play); // DBManage.c

            memset(sql_delcol, '\0', SQLlen);
            strncpy(I_date, str_now, DATElen);
            strncpy(sql_delcol, "UPDATE INFO SET (name, birth, phone, I_date) = ('','','', '", 59);
            strncat(sql_delcol, I_date, DATElen-1);
            strncat(sql_delcol, "') WHERE id = '", 15);
            strncat(sql_delcol, id, IDlen-1);
            strncat(sql_delcol, "';", 2);
            //printf("%s\n", sql_delcol);
            rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (name, birth, phone) : %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Delete success (name, birth, phone)\n");
                if(checkAg(play) == 1) // DBManage.c
                    delCol();
            }
            memset(sql_delcol, '\0', SQLlen);
            break;

        default:
            break;
    }
    memset(sql_delcol, '\0', SQLlen);
    strncpy(sql_delcol, "VACUUM;", 7);
    rc = sqlite3_exec(db, sql_delcol, callback, 0, &errmsg);
    if(rc != SQLITE_OK)
        fprintf(stderr, "Can't VACUUM (delCol) : %s\n", sqlite3_errmsg(db));
    else
        fprintf(stderr, "VACUUM success ((delCol)\n");
    memset(sql_delcol, '\0', SQLlen);
    sqlite3_close(db);

    return 1;
}

int delRow() { // case 47
    sqlite3 *db;
    char *errmsg;
    int rc;
    char sql_delrow[SQLlen] = { 0, };
    char play[IDlen] = {"삭제\0"};

    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    char id[IDlen] = { 0, };
    char whitelist[WLlen] = { 0, };

    int del_r = 0; // 삭제 분류 번호

    int i = 0; // 반복 횟수 제한

    const char *NotUsed = "Callback Function Called";

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

    printf("\n<Delete row!!!>\n");

    printf("삭제하려는 데이터의 id와 pwd가 일치해야 삭제됩니다.\n");

    for(i = 0; i < 5; i++) { // id 확인 5회로 제한
        while(1) { // 삭제하려는 데이터의 id
            memset(id, '\0', IDlen);
            memset(str, '\0', IDlen);
            strsize = 0;
            puts("\n삭제하려는 항목의 id(기본키) 입력:");
            gets(str);

            if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
                continue;
            strsize = strlen(str)+1;
            if(strsize >= 6 && strsize <= IDlen) // id는 5~9bytes 길이 제한
                break;
            else if(strsize < 6) {
                printf("5 btyes 보다 길게 입력하세요!\n");
                continue;
            }
            else
                printBOF_gets(str, strsize, IDlen); // DBPrintModule.c
        }
        strncpy(id, str, IDlen-1);

        if(checkID(id) == 1) // DBManage.c
            break;
        else {
            if(i <= 2)
                printf("총 5회 시도 중에 %d번 남았습니다.\n", 4-i);
            if(i == 3)
                printf("\n마지막 시도입니다. pwd가 일치하지 않으면 기능 선택 화면으로 돌아갑니다.\n");
        }
    }
    if(i == 5) {
        printf("\n5번 모두 id와 pwd가 일치하지 않아 기능 선택 화면으로 돌아갑니다.\n\n");
        printMenu();
    }

    while(1) { // 삭제 항목 입력받는 무한루프
        del_r = 0;
        print_del_row(); // DBPrintModule.c
        puts("input :");
        scanf("%d", &del_r);
        removeEnter(); // DBManage.c

        if(del_r == 0)
            exit(0);
        if(del_r == 1 || del_r == 2)
            break;
        else
            continue;
    }

    switch(del_r) {
        case 1: // id 기준으로 삭제
            printf("\n삭제하려는 데이터\n");

            memset(sql_delrow, '\0', SQLlen);
            strncpy(sql_delrow, "SELECT id, whitelist, W_date from whitelist where id = '", 56);
            strncat(sql_delrow, id, IDlen-1);
            strncat(sql_delrow, "';", 2);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : (whitelist) %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            memset(sql_delrow, '\0', SQLlen);
            strncpy(sql_delrow, "SELECT admin.access, info.name, info.birth, info.email, info.phone, info.I_date", 79);
            strncat(sql_delrow, " from admin, info where admin.id = '", 36);
            strncat(sql_delrow, id, IDlen);
            strncat(sql_delrow, "' and info.id = '", 19);
            strncat(sql_delrow, id, IDlen);
            strncat(sql_delrow, "';", 2);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : (admin info) %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            checkRep(id, play); // DBManage.c

            memset(sql_delrow, '\0', SQLlen);
            strncpy(sql_delrow, "DELETE from whitelist where id = '", 34);
            strncat(sql_delrow, id, IDlen-1);
            strncat(sql_delrow, "';", 2);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (whitelist.id:%s) : %s\n", id, sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else
                fprintf(stderr, "Delete success (whitelist.id:%s)\n", id);

            memset(sql_delrow, '\0', SQLlen);
            strncpy(sql_delrow, "DELETE from admin where id = '", 30);
            strncat(sql_delrow, id, IDlen-1);
            strncat(sql_delrow, "';", 2);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (admin.id:%s) : %s\n", id, sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else
                fprintf(stderr, "Delete success (admin.id:%s)\n", id);

            memset(sql_delrow, '\0', SQLlen);
            strncpy(sql_delrow, "DELETE from info where id = '", 29);
            strncat(sql_delrow, id, IDlen-1);
            strncat(sql_delrow, "';", 2);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (info.id:%s) : %s\n", id, sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Delete success (info.id:%s)\n", id);
                if(checkAg(play) == 1) // DBManage.c
                    delRow();
            }
            memset(sql_delrow, '\0', SQLlen);
            break;

        case 2: // whitelist 기준으로 삭제
            while(1) { // whitelist를 기준으로 먼저 부분 검색
                memset(whitelist, '\0', WLlen);
                memset(str, '\0', WLlen);
                puts("삭제를 원하는 Whitelist를 입력하세요. 먼저 부분적으로 확인합니다.");
                puts("\nSearch whitelist (30bytes 보다 길면 다시 입력함):");
                gets(str);

                if(str[0] == '\n' || str[0] == '\0') // 검색 정보는 null 불가
                    continue;
                strsize = strlen(str)+1;
                if(strsize >=4 && strsize <= WLlen)
                    break;
                else if(strsize < 4) {
                    printf("3 btyes 보다 길게 입력하세요!\n");
                    continue;
                }
                else
                printBOF_gets(str, strsize, WLlen); // DBPrintModule.c
            }
            strncpy(whitelist, str, WLlen-1);
            printf("입력한 whitelist : %s에 대한 부분 검색 결과입니다.\n", whitelist);

            memset(sql_delrow, '\0', SQLlen);
            strncpy(sql_delrow, "SELECT whitelist FROM WHITELIST WHERE WHITELIST like '%", 55);
            strncat(sql_delrow, whitelist, WLlen-1);
            strncat(sql_delrow, "%';", 3);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't Search : %s (whitelist error)\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Search success : whitelist like\n");
                printf("위의 결과에서 whitelist 명령어를 찾아 정확하게 입력하세요.\n");
            }
            memset(sql_delrow, '\0', SQLlen);

            while(1) { // whitelist를 정확하게 입력 받음
                memset(whitelist, '\0', WLlen);
                memset(str, '\0', WLlen);
                puts("\nWhitelist 검색은 정확히 일치해야 검색됩니다.");
                puts("\nSearch whitelist (30bytes 보다 길면 다시 입력함):");
                gets(str);

                if(str[0] == '\n' || str[0] == '\0') // 검색 정보는 null 불가
                    continue;
                strsize = strlen(str)+1;
                if(strsize >=4 && strsize <= WLlen)
                    break;
                else if(strsize < 4) {
                    printf("3 btyes 보다 길게 입력하세요!\n");
                    continue;
                }
                else
                    printBOF_gets(str, strsize, WLlen); // DBPrintModule.c
            }
            strncpy(whitelist, str, WLlen-1);

            if(checkWL(whitelist) == 0) { // DBManage.c : 사용자가 입력한 whitelist가 존재하지 않으면 종료
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu(); // DBPrintModule.c
            }

            printf("\n삭제하려는 데이터\n");

            memset(sql_delrow, '\0', SQLlen);
            strncpy(sql_delrow, "SELECT id, whitelist, W_date from whitelist where whitelist = '", 63);
            strncat(sql_delrow, whitelist, WLlen-1);
            strncat(sql_delrow, "';", 2);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : (whitelist) %s\n", sqlite3_errmsg(db));
                return 1;
            }

            memset(sql_delrow, '\0', SQLlen);
            strncpy(sql_delrow, "SELECT admin.id, admin.access, info.name, info.birth, info.email, info.phone, info.I_date", 89);
            strncat(sql_delrow, " from admin, info where admin.id = (select id from whitelist where whitelist = '", 80);
            strncat(sql_delrow, whitelist, WLlen-1);
            strncat(sql_delrow, "') and info.id = (select id from whitelist where whitelist = '", 62);
            strncat(sql_delrow, whitelist, WLlen-1);
            strncat(sql_delrow, "');", 3);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't print : (admin info) %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }

            checkRep(id, play); // DBManage.c

            memset(sql_delrow, '\0', SQLlen);
            strncpy(sql_delrow, "DELETE from whitelist where whitelist = '", 41);
            strncat(sql_delrow, whitelist, WLlen-1);
            strncat(sql_delrow, "';", 2);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (whitelist.whitelist:%s) : %s\n", whitelist, sqlite3_errmsg(db));
                return 1;
            }
            else
                fprintf(stderr, "Delete success (whitelist.whitelist:%s)\n", whitelist);
            memset(sql_delrow, '\0', SQLlen);

            strncpy(sql_delrow, "DELETE from admin where id = (select id from whitelist where whitelist = '", 74);
            strncat(sql_delrow, whitelist, WLlen-1);
            strncat(sql_delrow, "');", 3);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (whitelist-id-admin:%s) : %s\n", whitelist, sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else
                fprintf(stderr, "Delete success (whitelist-id-admin:%s)\n", whitelist);
            memset(sql_delrow, '\0', SQLlen);

            strncpy(sql_delrow, "DELETE from info where id = (select id from whitelist where whitelist = '", 73);
            strncat(sql_delrow, whitelist, WLlen-1);
            strncat(sql_delrow,  "');", 3);
            //printf("%s\n", sql_delrow);
            rc = sqlite3_exec(db, sql_delrow, callback, 0, &errmsg);
            if(rc != SQLITE_OK) {
                fprintf(stderr, "Can't delete (whitelist-id-info:%s) : %s\n", whitelist, sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("기능 선택 화면으로 돌아갑니다.\n");
                printMenu();
            }
            else {
                fprintf(stderr, "Delete success (whitelist-id-info:%s)\n", whitelist);
                if(checkAg(play) == 1) // DBManage.c
                    delRow();
            }
            memset(sql_delrow, '\0', SQLlen);
            break;

        default:
            break;
    }
    sqlite3_close(db);

    return 1;
}
