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

static int callback(void *NotUsed, int argc, char **argv, char **azColName) { // callback
    int i;
    for(i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i]:"NULL");
    }
    printf("\n");

    return 0;
}

int findIP() { // case 56 : id, pwd 분실 시 지원 기능
    sqlite3 *db;
    sqlite3_stmt *res;
    char *errmsg;
    int rc;
    char sql_findip[SQLlen] = { 0, };

    char id[IDlen] = { 0, };
    char pwd[PWDlen] = { 0, };
    char access[ACCESSlen] = { 0, };
    char name[NAMElen] = { 0, };
    char birth[BIRTHlen] = { 0, };
    char email[EMAILlen] = { 0, };
    char phone[PHONElen] = { 0, };
    char whitelist[WLlen] = { 0, };

    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열
    char str_find[ACCESSlen] = { 0, }; // 사용자가 찾을 항목
    char str_col[ACCESSlen] = { 0, }; // name, birth, phoen 중 사용자가 입력할 항목

    int b_date = 0; // birth 문자열을 정수형으로 받을 변수

    int i = 0;
    int pos_at = 0; // email 문자열의 @ 위치
    int pos_dot = 0; // email 문자열의 . 위치

    int p_number = 0; // phone 문자열이 숫자가 아닐 때마다 1씩 증가되는 변수

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

    puts("\nID & PWD Find\n");

    while(1) {
        memset(str, '\0', ACCESSlen);
        memset(str_find, '\0', ACCESSlen);
        strsize = 0;
        printf("\n1 : id 찾기\n");
        printf("2 : pwd 찾기\n");
        printf("3 : id & pwd 찾기\n");
        printf("0 : 종료\n");
        puts("Find number:");
        gets(str);

        if(str[0] == '\0') // 필수 입력정보는 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize > ACCESSlen) {
            printBOF_gets(str, strsize, ACCESSlen); // DBPrintModule.c
            continue;
        }
        if(str[0] == '0')
            exit(0);
        if(str[0] == '1' || str[0] == '2' || str[0] == '3')
            break;
    }
    strncpy(str_find, str, strsize);

    printf("Email 은 필수 입력 정보입니다.\n");

    while(1) { // 이메일 : @는 [1] ~ [끝-2] && .은 [@위치+2] ~ [끝]
        memset(email, '\0', EMAILlen);
        memset(str, '\0', EMAILlen);
        strsize = 0;
        puts("등록한 계정의 email :");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수입력 정보여서 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize > EMAILlen) {
            printBOF_gets(str, strsize, EMAILlen); // DBPrintModule.c
            continue;
        }

        for(i = 0; i < strsize; i++) { // 입력받은 이메일의 한 글자씩 확인하는 루프
            if(i > 0 && str[i] == '@') // @는 맨 앞에 올 수 없음
                pos_at = i+1;
            if(i > pos_at && str[i] =='.') // .은 @+1 보다 뒤에 있음
                pos_dot = i+1;
        }

        if(pos_at > 0 && pos_dot > 3 && pos_dot < i) // @는 맨 앞이면 안됨 && .은 4번째~끝(i) 사이
            break;
        else {
            printf("이메일 형식 -@-.- 에 맞지 않습니다!\n");
            continue;
        }
    }
    strncpy(email, str, EMAILlen-1);

    if(checkEmail(email) == 0) { // DBManage.c : 등록된 email이 아니면 종료
        printf("\n새로 계정을 만들거나, 이메일을 다시 확인하세요.\n\n");
        sqlite3_close(db);
        printf("DB 시작 화면으로 돌아갑니다.\n");
        login(); // DBLogin.c
    }

    printf("\n");
    if(str_find[0] == '1' || str_find[0] == '3') {
        if(str_find[0] == '3')
            printf("pwd를 찾기 전에, id부터 찾습니다.\n");

        memset(sql_findip, '\0', SQLlen);
        strncpy(sql_findip, "Select id from info where email = '", 35);
        strncat(sql_findip, email, EMAILlen-1);
        strncat(sql_findip, "';", 2);
        //printf("%s\n", sql_findip);
        rc = sqlite3_exec(db, sql_findip, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't find id : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("DB 시작 화면으로 돌아갑니다.\n");
            login(); // DBLogin.c
        }
        else {
            //fprintf(stderr, "Find success (id)\n");
        }
        memset(sql_findip, '\0', SQLlen);
        if(str_find[0] == '1')
            login(); // DBLogin.c
    }

    // 2 : pwd 찾기 // 3 : id & pwd 찾기
    if(str_find[0] == '2' || str_find[0] == '3') {
        while(1) { // 찾으려는 pwd의 id
            memset(id, '\0', IDlen);
            memset(str, '\0', IDlen);
            strsize = 0;
            puts("\npwd를 찾으려는 id 입력:");
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

        memset(sql_findip, '\0', SQLlen);
        strncpy(sql_findip, "SELECT id from info where id = '", 32);
        strncat(sql_findip, id, IDlen-1);
        strncat(sql_findip, "' and email = '", 15);
        strncat(sql_findip, email, EMAILlen-1);
        strncat(sql_findip, "';", 2);
        rc = sqlite3_prepare_v2(db, sql_findip, -1, &res, 0);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("DB 시작 화면으로 돌아갑니다.\n");
            login(); // DBLogin.c
        }

        rc = sqlite3_step(res);
        if (rc == SQLITE_ROW) { // id와 email이 일치
            sqlite3_finalize(res);
            printf("입력한 id와 email이 일치합니다.\n\n");
        }
        else {  // id와 email이 일치하지 않음
            sqlite3_finalize(res);
            memset(sql_findip, '\0', SQLlen);
            sqlite3_close(db);
            printf("입력한 id와 email이 일치하지 않아, 처음 화면으로 갑니다.\n");
            login(); // DBLogin.c
        }
        memset(sql_findip, '\0', SQLlen);

        printf("등록 당시에 입력한 값을 최대한 입력해주세요.\n\n");

        memset(sql_findip, '\0', SQLlen);
        strncpy(sql_findip, "SELECT id from whitelist where id = '", 44);
        strncat(sql_findip, id, IDlen-1);
        strncat(sql_findip, "';", 2);

        rc = sqlite3_prepare_v2(db, sql_findip, -1, &res, 0);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("DB 시작 화면으로 돌아갑니다.\n");
            login(); // DBLogin.c
        }

        rc = sqlite3_step(res);
        if (rc == SQLITE_ROW) { // 화이트리스트에 등록된 id임
            sqlite3_finalize(res);
            memset(sql_findip, '\0', SQLlen);
            while(1) { // 화이트리스트 검증
                while(1) { // whitelist를 기준으로 먼저 부분 검색
                    memset(whitelist, '\0', WLlen);
                    memset(str, '\0', WLlen);
                    strsize = 0;
                    puts("\nWhitelist 검색은 먼저, 부분 문자열 검색으로 확인합니다.");
                    puts("whitelist 를 등록한 적이 없거나, 기억이 안 나면 EnterKey로 넘기세요.");
                    puts("\nSearch whitelist (30bytes 보다 길면 다시 입력함):");
                    gets(str);

                    if(str[0] == '\n' || str[0] == '\0') // 임시 검색에서 null 일 경우는 검색 sql 안 함
                        break;

                    strsize = strlen(str)+1;
                    if(strsize >=3 && strsize <= WLlen)
                        break;
                    else if(strsize < 3) {
                        printf("2 btyes 보다 길게 입력하세요!\n"); // 3bytes 이상이지만 like 검색이러서 2bytes로 낮춤
                        continue;
                    }
                    else
                        printBOF_gets(str, strsize, WLlen); // DBPrintModule.c
                }
                strncpy(whitelist, str, WLlen-1);

                if(whitelist[0] != '\0') {
                    printf("입력한 whitelist : %s에 대한 부분 검색 결과입니다.\n\n", whitelist);

                    memset(sql_findip, '\0', SQLlen);
                    strncpy(sql_findip, "SELECT whitelist FROM WHITELIST WHERE WHITELIST like '%", 55);
                    strncat(sql_findip, whitelist, WLlen-1);
                    strncat(sql_findip, "%' and id = '", 13);
                    strncat(sql_findip, id, IDlen-1);
                    strncat(sql_findip, "';", 2);
                    rc = sqlite3_exec(db, sql_findip, callback, 0, &errmsg);
                    if(rc != SQLITE_OK) {
                        fprintf(stderr, "Can't Search : %s (whitelist error)\n", sqlite3_errmsg(db));
                    }
                    else {
                        fprintf(stderr, "위의 결과에서 whitelist 명령어를 찾아 정확하게 입력하세요.\n");
                    }
                    memset(sql_findip, '\0', SQLlen);

                    while(1) { // whitelist를 정확하게 입력 받음
                        memset(whitelist, '\0', WLlen);
                        memset(str, '\0', WLlen);
                        strsize = 0;
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

                    memset(sql_findip, '\0', SQLlen);
                    strncpy(sql_findip, "SELECT whitelist FROM WHITELIST WHERE WHITELIST = '", 51);
                    strncat(sql_findip, whitelist, WLlen-1);
                    strncat(sql_findip, "' and id = '", 12);
                    strncat(sql_findip, id, IDlen-1);
                    strncat(sql_findip, "';", 2);
                    rc = sqlite3_prepare_v2(db, sql_findip, -1, &res, 0);
                    if (rc != SQLITE_OK) {
                        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
                        sqlite3_close(db);
                        printf("DB 시작 화면으로 돌아갑니다.\n");
                        login(); // DBLogin.c
                    }

                    rc = sqlite3_step(res);
                    if (rc == SQLITE_ROW) { // id와 whitelist가 일치
                        sqlite3_finalize(res);
                        memset(sql_findip, '\0', SQLlen);
                        printf("입력한 id와 whitelist가 일치합니다.\n\n");
                        break;
                    }
                    else { // id와 whitelist가 일치하지 않음
                        sqlite3_finalize(res);
                        memset(sql_findip, '\0', SQLlen);
                        printf("입력한 id와 whitelist가 일치하지 않아, whitelist를 다시 입력합니다.\n");
                        continue;
                    }
                }
                break;
            }
        }
        else {  // 화이트리스트에 등록되지 않은 id임
            sqlite3_finalize(res);
        }
        memset(sql_findip, '\0', SQLlen);

        while(1) { // access
            memset(sql_findip, '\0', SQLlen);
            while(1) { // access 입력받음
                memset(access, '0', ACCESSlen);
                memset(str, '\0', ACCESSlen);
                strsize = 0;
                puts("\n권한 취득 전이었으면 EnterKey 또는 N 을 선택하세요.");
                puts("input access (E, C, R):");
                puts("E : EV3 ROBOT 관리자");
                puts("C : CPS 관리자");
                puts("R : ROOT");
                puts("N : (default) id 등록 당시 권한 취득 전");
                gets(str);

                strsize = strlen(str)+1;
                if(strsize > ACCESSlen) {
                    printBOF_gets(str, strsize, ACCESSlen); // DBPrintModule.c
                    continue;
                }

                if(str[0] == 'E' || str[0] == 'C' || str[0] == 'R' || str[0] == 'N')
                    break;
                if(str[0] == '\0') {
                    str[0] = 'N';
                    break;
                }
                else
                    printf("잘못 입력하셨습니다.\n"); // 엔터키(N), E, C, R 을 제외하고는 무한루프
            }
            strncpy(access, str, ACCESSlen-1);

            memset(sql_findip, '\0', SQLlen);
            strncpy(sql_findip, "SELECT access from admin where id = '", 37);
            strncat(sql_findip, id, IDlen-1);
            strncat(sql_findip, "' and access = '", 16);
            strncat(sql_findip, access, ACCESSlen-1);
            strncat(sql_findip, "';", 2);
            //printf("%s\n", sql_findip);
            rc = sqlite3_prepare_v2(db, sql_findip, -1, &res, 0);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                printf("DB 시작 화면으로 돌아갑니다.\n");
                login(); // DBLogin.c
            }

            rc = sqlite3_step(res);
            if (rc == SQLITE_ROW) { // id와 access가 일치
                sqlite3_finalize(res);
                memset(sql_findip, '\0', SQLlen);
                printf("입력한 id와 access가 일치합니다.\n");
            }
            else {  // id와 access가 일치하지 않음
                sqlite3_finalize(res);
                memset(sql_findip, '\0', SQLlen);
                printf("입력한 id와 access가 일치하지 않아, 재입력합니다.\n");
                continue;
            }
            break;
        }

        while(1) { // pwd 확인 항목 선택
            memset(str, '\0', ACCESSlen);
            memset(str_col, '\0', ACCESSlen);
            strsize = 0;
            print_pwd(); // DBPrintModule.c
            gets(str);

            strsize = strlen(str)+1;
            if(strsize > ACCESSlen) {
                printBOF_gets(str, strsize, ACCESSlen); // DBPrintModule.c
                continue;
            }

            if(str[0] == '0') {
                sqlite3_close(db);
                exit(0);
            }
            if(str[0] == '1' || str[0] == '2' || str[0] == '3')
                break;
            if(str[0] == '\0') // 필수 선택 항목은 null 불가
                continue;
            else
                printf("잘못 입력하셨습니다.\n"); // 0 ~ 3이 아니면 무한루프
        }
        strncpy(str_col, str, ACCESSlen-1);

        if(str_col[0] == '1') { // 1 : name
            while(1) { // name
                while(1) { // name 입력받음
                    memset(name, '\0', NAMElen);
                    memset(str, '\0', NAMElen);
                    strsize = 0;
                    printf("\n등록 당시에 건너띄었으면 바로 EnterKey를 누르세요.\n");
                    puts("input name (2~30bytes 길이로 입력하세요):");
                    gets(str);

                    str[0] == '\n'; // EnterKey를 누르면 무한푸프 탈출
                        break;
                    strsize = strlen(str)+1;

                    if(strsize >= 3 && strsize <= NAMElen)
                        break;
                    else if(strsize < 3) {
                        printf("2bytes 보다 길게 입력하세요!\n");
                        continue;
                    }
                    else
                        printBOF_gets(str, strsize, NAMElen); // DBPrintModule.c
                }
                strncpy(name, str, NAMElen-1);

                memset(sql_findip, '\0', SQLlen);
                strncpy(sql_findip, "SELECT name from info where id = '", 34);
                strncat(sql_findip, id, IDlen-1);
                strncat(sql_findip, "' and name = '", 14);
                strncat(sql_findip, name, NAMElen-1);
                strncat(sql_findip, "';", 2);
                //printf("%s\n", sql_findip);
                rc = sqlite3_prepare_v2(db, sql_findip, -1, &res, 0);
                if (rc != SQLITE_OK) {
                    fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("DB 시작 화면으로 돌아갑니다.\n");
                    login(); // DBLogin.c
                }

                rc = sqlite3_step(res);
                if (rc == SQLITE_ROW) { // id와 name이 일치
                    sqlite3_finalize(res);
                    memset(sql_findip, '\0', SQLlen);
                    printf("입력한 id와 name이 일치합니다.\n");
                }
                else {  // id와 name이 일치하지 않음
                    sqlite3_finalize(res);
                    memset(sql_findip, '\0', SQLlen);
                    printf("입력한 id와 name이 일치하지 않아, 재입력합니다.\n");
                    continue;
                }
                break;
            }
        }

        if(str_col[0] == '2') { // 2 : birth
            while(1) { // birth
                while(1) { // 생년월일은 yyyymmdd 8개의 유효 날짜로 입력받음
                    memset(birth, '\0', BIRTHlen);
                    memset(str, '\0', BIRTHlen);
                    strsize = 0;
                    printf("\n등록 당시에 건너띄었으면 바로 EnterKey를 누르세요.\n");
                    puts("input birth (yyyymmdd 8개 유효 날짜(숫자)가 아니면 다시 입력함):");
                    gets(str);

                    str[0] == '\n'; // EnterKey를 누르면 무한푸프 탈출
                        break;
                    strsize = strlen(str)+1;

                    if(strsize == BIRTHlen) {
                        b_date = atoi(str);
                        if(checkDate(b_date) == 1) // DBManage.c : 1이면 참(유효날짜)
                             break;
                        else {
                            printf("유효 날짜가 아닙니다!!!\n");
                            continue;
                        }
                    }

                    else
                        printf("yyyymmdd 8개의 유효 날짜(숫자)로 입력해야 합니다!\n");
                }
                strncpy(birth, str, BIRTHlen-1);

                memset(sql_findip, '\0', SQLlen);
                strncpy(sql_findip, "SELECT birth from info where id = '", 35);
                strncat(sql_findip, id, IDlen-1);
                strncat(sql_findip, "' and birth = '", 15);
                strncat(sql_findip, birth, BIRTHlen-1);
                strncat(sql_findip, "';", 2);
                //printf("%s\n", sql_findip);
                rc = sqlite3_prepare_v2(db, sql_findip, -1, &res, 0);
                if (rc != SQLITE_OK) {
                    fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("DB 시작 화면으로 돌아갑니다.\n");
                    login(); // DBLogin.c
                }

                rc = sqlite3_step(res);
                if (rc == SQLITE_ROW) { // id와 birth가 일치
                    sqlite3_finalize(res);
                    memset(sql_findip, '\0', SQLlen);
                    printf("입력한 id와 birth가 일치합니다.\n");
                }
                else {  // id와 birth가 일치하지 않음
                    sqlite3_finalize(res);
                    memset(sql_findip, '\0', SQLlen);
                    printf("입력한 id와 birth가 일치하지 않아, 재입력합니다.\n");
                    continue;
                }
                break;
            }
        }

        if(str_col[0] == '3') { // 3 : phone
            while(1) { // phone
                while(1) { // 전화번호는 자연수만 입력 가능
                    memset(phone, '\0', PHONElen);
                    memset(str, '\0', PHONElen);
                    strsize = 0;
                    printf("\n등록 당시에 건너띄었으면 바로 EnterKey를 누르세요.\n");
                    printf("-표시 없이 숫자만 입력하세요.\n");
                    puts("input phone (2~20개의 숫자로만 입력하세요):");
                    gets(str);
                    if(str[0] == '\0') // EnterKey를 누르면 무한루프 탈출
                        break;
                    strsize = strlen(str)+1;
                    if(strsize > PHONElen) {
                        printBOF_gets(str, strsize, PHONElen); // DBPrintModule.c
                        continue;
                    }

                    if(strsize < 3) { // 숫자 1개만 입력하면 반복
                        printf("최소 2개의 숫자를 입력하세요!!!\n");
                        continue;
                    }

                    for(p_number = 0, i = 0; i < strsize; i++) { // 입력받은 숫자를 하나씩 확인하는 루프
                        if((str[i] < '0') || (str[i] > '9')) // 숫자가 아닌 문자가 입력된 경우 반복
                            p_number++; // 숫자가 아니면 p_number 변수 값에 1을 더함
                    }
                    if(p_number == 1) // 전화번호가 길이제한을 넘기지 않고, 모두 숫자로만 입력된 경우는 무한루프 탈출
                        break;
                }
                strncpy(phone, str, PHONElen-1);

                memset(sql_findip, '\0', SQLlen);
                strncpy(sql_findip, "SELECT phone from info where id = '", 35);
                strncat(sql_findip, id, IDlen-1);
                strncat(sql_findip, "' and phone = '", 15);
                strncat(sql_findip, phone, PHONElen-1);
                strncat(sql_findip, "';", 2);
                //printf("%s\n", sql_findip);
                rc = sqlite3_prepare_v2(db, sql_findip, -1, &res, 0);
                if (rc != SQLITE_OK) {
                    fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("DB 시작 화면으로 돌아갑니다.\n");
                    login(); // DBLogin.c
                }

                rc = sqlite3_step(res);
                if (rc == SQLITE_ROW) { // id와 phone이 일치
                    sqlite3_finalize(res);
                    memset(sql_findip, '\0', SQLlen);
                    printf("입력한 id와 phone이 일치합니다.\n");
                }
                else {  // id와 phone이 일치하지 않음
                    sqlite3_finalize(res);
                    memset(sql_findip, '\0', SQLlen);
                    printf("입력한 id와 phone이 일치하지 않아, 재입력합니다.\n");
                    continue;
                }
                break;
            }
        }

        memset(sql_findip, '\0', SQLlen);
        strncpy(sql_findip, "SELECT pwd from admin where id = '", 34);
        strncat(sql_findip, id, IDlen-1);
        strncat(sql_findip, "';", 2);
        rc = sqlite3_exec(db, sql_findip, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't find pwd : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("DB 시작 화면으로 돌아갑니다.\n");
            login(); // DBLogin.c
        }
        else {
            //fprintf(stderr, "Success (pwd)\n");
        }
        memset(sql_findip, '\0', SQLlen);
    }
    sqlite3_close(db);
    printf("DB 시작 화면으로 돌아갑니다.\n");
    login(); // DBLogin.c

    return 1;
}

int RestoreB() { // case 57 : DB 복원
    //system("sqlite3 CPS.db.dump > CPS.sql"); // backup : sqlite DB이름.db.dump > sql이름.sql
    //system("sqlite3 CPS.db < CPS.sql"); // restore : sqlite3 DB이름.db < sql이름.sql
}
