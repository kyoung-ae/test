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

int login() { // DB 로그인 : id & pwd로 확인
    sqlite3 *db;
    sqlite3_stmt *res;
    char *errmsg;
    int rc;
    char sql_login[SQLlen] = { 0, };
    const char *NotUsed = "Callback Function Called";

    char str[MAX] = { 0, };
    char str_login[ACCESSlen] = { 0, };
    int strsize = 0;

    char id[IDlen] = { 0, };

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

    while(1) { // 접속 메뉴 선택
        memset(str, '\0', ACCESSlen);
        strsize = 0;
        print_login(); // DBPrintModule.c
        printf("input : ");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수 선택 정보는 null 불가
            continue;
        if(str[0] == '0')  // 0을 선택하면 종료됨
            exit(0);

        strsize = strlen(str)+1;
        if(strsize > ACCESSlen) { // 길이 제한을 초과한 경우는 반복
            printBOF_gets(str, strsize, ACCESSlen); // DBPrintModule.c
            continue;
        }

        if(str[0] == '1' || str[0] == '2' || str[0] == '3')
            break;
        printf("잘못 입력했습니다.\n");
    }
    strncpy(str_login, str, ACCESSlen-1);

    if(str_login[0] == '1') { // 접속 시도
        while(1) { // id 검색
            memset(id, '\0', IDlen);
            memset(str, '\0', IDlen);
            strsize = 0;
            puts("\nLogin id (9bytes 보다 길면 다시 입력함):");
            gets(str);

            if(str[0] == '\n' || str[0] == '\0') // 검색 정보는 null 불가
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

        if(checkID(id) == 0) { // 입력한 id를 매개변수로 받아서 등록된 id인지 확인 (성공하면 1을 반환)
            sqlite3_close(db);
            printf("DB 시작 화면으로 돌아갑니다.\n");
            login();
        }

        if(checkFunc(id) == 0) { // DBManage.c : pwd 확인을 5회로 제한
            sqlite3_close(db);
            printf("DB 시작 화면으로 돌아갑니다.\n");
            login();
        }
    }

    if(str_login[0] == '2') { // 새 게정 등록
        inAD_INFO(); // DBInsertModule.c
    }

    if(str_login[0] == '3') { // id, pwd 찾기, DB복원
        findIP(); // DBBackupR.c // case 56 : id, pwd 분실 시 지원 기능
    }
    sqlite3_close(db);

    return 1;
}
