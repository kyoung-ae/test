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

int upAD_INFO() { // case 26
    sqlite3 *db;
    char *errmsg;
    int rc;
    char sql_upadinfo[SQLlen] = { 0, };
    char play[IDlen] = {"수정\0"};

    char id[IDlen] = { 0, };
    char pwd[PWDlen] = { 0, };
    char access[ACCESSlen] = { 0, };

    char name[NAMElen] = { 0, };
    char birth[BIRTHlen] = { 0, };
    char email[EMAILlen] = { 0, };
    char phone[PHONElen] = { 0, };
    char I_date[DATElen] = { 0, };

    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    int b_date = 0; // birth 문자열을 정수형으로 받을 변수

    int i = 0;
    int pos_at = 0; // email 문자열의 @ 위치
    int pos_dot = 0; // email 문자열의 . 위치

    int p_number = 0; // phone 문자열이 숫자가 아닐 때마다 1씩 증가되는 변수

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

    printf("\nADMIN INFO Update\n");

    for(i = 0; i < 5; i++) { // id 확인 5회로 제한
        while(1) { // 수정하려는 데이터의 id
            memset(id, '\0', IDlen);
            memset(str, '\0', IDlen);
            strsize = 0;
            puts("\n수정하려는 항목의 id(기본키) 입력:");
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

    printf("\n=수정이 가능한 데이터=\n");
    printf("pwd =  수정이 가능합니다.\n");
    memset(sql_upadinfo, '\0', SQLlen);
    strncpy(sql_upadinfo, "SELECT admin.access, info.name, info.birth, info.email, info.phone ", 67);
    strncat(sql_upadinfo, "FROM ADMIN, INFO WHERE admin.id = '", 35);
    strncat(sql_upadinfo, id, IDlen-1);
    strncat(sql_upadinfo, "' and info.id = '", 17);
    strncat(sql_upadinfo, id, IDlen-1);
    strncat(sql_upadinfo, "';", 2);
    //printf("%s\n", sql_upadinfo);
    rc = sqlite3_exec(db, sql_upadinfo, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        printf("기능 선택 화면으로 돌아갑니다.\n");
        printMenu(); // DBPrintModule.c
    }

    memset(sql_upadinfo, '\0', SQLlen);
    strncpy(sql_upadinfo, "select access from admin where id = '", 37);
    strncat(sql_upadinfo, id, IDlen-1);
    strncat(sql_upadinfo, "';", 2);
    //printf("%s\n", sql_upadinfo);
    rc = sqlite3_exec(db, sql_upadinfo, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        printf("기능 선택 화면으로 돌아갑니다.\n");
        printMenu(); // DBPrintModule.c
    }
    memset(sql_upadinfo, '\0', SQLlen);

    while(1) { // 수정하려는 id의 access 수정
        memset(access, '\0', ACCESSlen);
        memset(str, '\0', ACCESSlen);
        strsize = 0;
        printf("수정을 건너띄려면 EnterKey만 누르세요.\n");
        puts("ACCESS(권한) 변경 값 입력:");
        gets(str);

        str[0] == '\n'; // EnterKey를 누르면 무한푸프 탈출
            break;
        strsize = strlen(str)+1;
        if(strsize <= ACCESSlen)
            break;

        printBOF_gets(str, strsize, ACCESSlen); // DBPrintModule.c
    }
    strncpy(access, str, ACCESSlen-1);
    //update admin set access = 'access' where access is not null and id = 'id'; // null이 아닌 access 값 수정

    while(1) { // 수정하려는 id의 pwd 수정
        memset(pwd, '\0', PWDlen);
        memset(str, '\0', PWDlen);
        strsize = 0;
        printf("수정을 건너띄려면 EnterKey만 누르세요.\n");
        puts("PWD(비밀번호) 변경 값 입력:");
        gets(str);

        str[0] == '\n'; // EnterKey를 누르면 무한푸프 탈출
            break;
        strsize = strlen(str)+1;
        if(strsize <= PWDlen)
            break;

        printBOF_gets(str, strsize, PWDlen); // DBPrintModule.c
    }
    strncpy(pwd, str, PWDlen-1);

    printf("admin 정보 수정을 위해 id와 pwd를 재확인합니다.\n");
    checkRep(id, play); // DBManage.c

    if((strlen(access) != 0) && (strlen(pwd) == 0)) { // access만 수정
        memset(sql_upadinfo, '\0', SQLlen);
        strncpy(sql_upadinfo, "UPDATE ADMIN SET access = '", 27);
        strncat(sql_upadinfo, access, ACCESSlen-1);
        strncat(sql_upadinfo, "' WHERE id = '", 14);
        strncat(sql_upadinfo, id, IDlen-1);;
        strncat(sql_upadinfo, "';", 2);
        //printf("%s\n", sql_upadinfo);
        rc = sqlite3_exec(db, sql_upadinfo, callback, NotUsed, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Update ERROR (access) : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu(); // DBPrintModule.c
        }
        else {
            fprintf(stderr, "access 값을 %s로 수정 성공\n\n", access);
            if(checkAg(play) == 1) // DBManage.c
                upAD_INFO();
        }
        memset(sql_upadinfo, '\0', SQLlen);
    }

    if((strlen(access) == 0) && (strlen(pwd) != 0)) { // pwd만 수정
        memset(sql_upadinfo, '\0', SQLlen);
        strncpy(sql_upadinfo, "UPDATE ADMIN SET pwd = '", 24);
        strncat(sql_upadinfo, pwd, PWDlen-1);
        strncat(sql_upadinfo, "' WHERE id = '", 14);
        strncat(sql_upadinfo, id, IDlen-1);
        strncat(sql_upadinfo, "';", 2);
        //printf("%s\n", sql_upadinfo);
        rc = sqlite3_exec(db, sql_upadinfo, callback, NotUsed, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Update ERROR (pwd) : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu(); // DBPrintModule.c
        }
        else {
            fprintf(stderr, "pwd 값을 %s로 수정 성공\n", pwd);
            printf("지금부터 pwd 입력란에는 %s로 입력하세요.\n\n", pwd);
            if(checkAg(play) == 1) // DBManage.c
                upAD_INFO();
        }
        memset(sql_upadinfo, '\0', SQLlen);
    }

    if((strlen(access) != 0) && (strlen(pwd) != 0)) { // access & pwd 함께 수정
        memset(sql_upadinfo, '\0', SQLlen);
        strncpy(sql_upadinfo, "UPDATE ADMIN SET (access, pwd) = ('", 35);
        strncat(sql_upadinfo, access, ACCESSlen-1);
        strncat(sql_upadinfo, "', '", 4);
        strncat(sql_upadinfo, pwd, PWDlen-1);
        strncat(sql_upadinfo, "') WHERE id = '", 15);
        strncat(sql_upadinfo, id, IDlen-1);
        strncat(sql_upadinfo, "';", 2);
        //printf("%s\n", sql_upadinfo);
        rc = sqlite3_exec(db, sql_upadinfo, callback, NotUsed, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Update ERROR (access&pwd) : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu(); // DBPrintModule.c
        }
        else {
            fprintf(stderr, "access 값은 %s로, pwd 값은 %s로 수정 성공\n\n", access, pwd);
            if(checkAg(play) == 1) // DBManage.c
                upAD_INFO();
        }
        memset(sql_upadinfo, '\0', SQLlen);
    }

    printf("\n지금부터 입력하는 정보는 id, pwd 분실 시 확인 정보로 사용됩니다!!!\n");
    printf("기존 정보에서 수정을 원하지 않는 항목은 EnterKey만 누르세요.\n");

    memset(sql_upadinfo, '\0', SQLlen);
    strncpy(sql_upadinfo, "select name from info where id = '", 34);
    strncat(sql_upadinfo, id, IDlen-1);
    strncat(sql_upadinfo, "';", 2);
    //printf("%s\n", sql_upadinfo);
    rc = sqlite3_exec(db, sql_upadinfo, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        printf("기능 선택 화면으로 돌아갑니다.\n");
        printMenu(); // DBPrintModule.c
    }
    memset(sql_upadinfo, '\0', SQLlen);

    while(1) { // 수정하려는 id의 name 수정
        memset(name, '\0', NAMElen);
        memset(str, '\0', NAMElen);
        strsize = 0;
        printf("수정을 건너띄려면 EnterKey만 누르세요.\n");
        puts("NAME(이름) 변경 값 입력:");
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

    strncpy(I_date, str_now, DATElen);

    if(strlen(name) != 0) { // name 변경 값이 있으면 수정
        memset(sql_upadinfo, '\0', SQLlen);
        strncpy(sql_upadinfo, "UPDATE INFO SET (name, I_date) = ('", 35);
        strncat(sql_upadinfo, name, NAMElen-1);
        strncat(sql_upadinfo, "','", 3);
        strncat(sql_upadinfo, I_date, DATElen-1);
        strncat(sql_upadinfo, "') WHERE id = '", 15);
        strncat(sql_upadinfo, id, IDlen-1);
        strncat(sql_upadinfo, "';", 2);
        //printf("%s\n", sql_upadinfo);
        rc = sqlite3_exec(db, sql_upadinfo, callback, NotUsed, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Update ERROR (name) : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu(); // DBPrintModule.c
        }
        else {
            fprintf(stderr, "name 값을 %s로 수정 성공\n\n", name);
            if(checkAg(play) == 1) // DBManage.c
                upAD_INFO();
        }
        memset(sql_upadinfo, '\0', SQLlen);
    }

    memset(sql_upadinfo, '\0', SQLlen);
    strncpy(sql_upadinfo, "select birth from info where id = '", 35);
    strncat(sql_upadinfo, id, IDlen-1);
    strncat(sql_upadinfo, "';", 2);
    //printf("%s\n", sql_upadinfo);
    rc = sqlite3_exec(db, sql_upadinfo, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        printf("기능 선택 화면으로 돌아갑니다.\n");
        printMenu(); // DBPrintModule.c
    }
    memset(sql_upadinfo, '\0', SQLlen);

    while(1) { // 수정하려는 id의 생년월일(yyyymmdd 8개의 유효 날짜) 수정
        memset(birth, '\0', BIRTHlen);
        memset(str, '\0', BIRTHlen);
        strsize = 0;
        printf("수정을 건너띄려면 EnterKey만 누르세요.\n");
        puts("birth(생년월일 yyyymmdd 8개 날짜) 변경 값 입력:");
        gets(str);

        str[0] == '\0'; // EnterKey를 누르면 무한푸프 탈출
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

    strncpy(I_date, str_now, DATElen);

    if(strlen(birth) != 0) { // birth 변경 값이 있으면 수정
        memset(sql_upadinfo, '\0', SQLlen);
        strncpy(sql_upadinfo, "UPDATE INFO SET (birth, I_date) = ('", 36);
        strncat(sql_upadinfo, birth, BIRTHlen-1);
        strncat(sql_upadinfo, "','", 3);
        strncat(sql_upadinfo, I_date, DATElen-1);
        strncat(sql_upadinfo, "') WHERE id = '", 15);
        strncat(sql_upadinfo, id, IDlen-1);
        strncat(sql_upadinfo, "';", 2);
        //printf("%s\n", sql_upadinfo);
        rc = sqlite3_exec(db, sql_upadinfo, callback, NotUsed, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Update ERROR (birth) : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu(); // DBPrintModule.c
        }
        else {
            fprintf(stderr, "birth 값을 %s로 수정 성공\n\n", birth);
            if(checkAg(play) == 1) // DBManage.c
                upAD_INFO();
        }
        memset(sql_upadinfo, '\0', SQLlen);
    }

    checkRep(id, play); // DBManage.c

    memset(sql_upadinfo, '\0', SQLlen);
    strncpy(sql_upadinfo, "select email from info where id = '", 35);
    strncat(sql_upadinfo, id, IDlen-1);
    strncat(sql_upadinfo, "';", 2);
    //printf("%s\n", sql_upadinfo);
    rc = sqlite3_exec(db, sql_upadinfo, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        printf("기능 선택 화면으로 돌아갑니다.\n");
        printMenu(); // DBPrintModule.c
    }
    memset(sql_upadinfo, '\0', SQLlen);

    while(1) { // 이메일 수정 : @는 [1] ~ [끝-2] && .은 [@위치+2] ~ [끝]
        memset(email, '\0', EMAILlen);
        memset(str, '\0', EMAILlen);
        strsize = 0;
        printf("수정을 건너띄려면 EnterKey만 누르세요.\n");
        puts("email 변경 값 입력:");
        gets(str);

        if(str[0] == '\0') // EnterKey를 누르면 무한루프 탈출
            break;
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

    strncpy(I_date, str_now, DATElen);

    if(strlen(email) != 0) { // email 변경 값이 있으면 수정
        memset(sql_upadinfo, '\0', SQLlen);
        strncpy(sql_upadinfo, "UPDATE INFO SET (email, I_date) = ('", 36);
        strncat(sql_upadinfo, email, EMAILlen-1);
        strncat(sql_upadinfo, "','", 3);
        strncat(sql_upadinfo, I_date, DATElen-1);
        strncat(sql_upadinfo, "') WHERE id = '", 15);
        strncat(sql_upadinfo, id, IDlen-1);
        strncat(sql_upadinfo, "';", 2);
        //printf("%s\n", sql_upadinfo);
        rc = sqlite3_exec(db, sql_upadinfo, callback, NotUsed, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Update ERROR (email) : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu(); // DBPrintModule.c
        }
        else {
            fprintf(stderr, "email 값을 %s로 수정 성공\n\n", email);
            if(checkAg(play) == 1) // DBManage.c
                upAD_INFO();
        }
        memset(sql_upadinfo, '\0', SQLlen);
    }

    memset(sql_upadinfo, '\0', SQLlen);
    strncpy(sql_upadinfo, "select phone from info where id = '", 35);
    strncat(sql_upadinfo, id, IDlen-1);
    strncat(sql_upadinfo, "';", 2);
    //printf("%s\n", sql_upadinfo);
    rc = sqlite3_exec(db, sql_upadinfo, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        printf("기능 선택 화면으로 돌아갑니다.\n");
        printMenu(); // DBPrintModule.c
    }
    memset(sql_upadinfo, '\0', SQLlen);

    while(1) { // 전화번호 수정
        memset(phone, '\0', PHONElen);
        memset(str, '\0', PHONElen);
        strsize = 0;
        printf("수정을 건너띄려면 EnterKey만 누르세요.\n");
        printf("수정을 원하면 2 ~ 20개의 수를 -표시 없이 입력하세요.\n");
        puts("phone(전화번호) 변경 값 입력:");
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

        for(p_number = 0, i = 0; i < strsize; i++) { // 입력받은 이메일의 한 글자씩 확인하는 루프
            if((str[i] < '0') || (str[i] > '9')) // 숫자가 아닌 문자가 입력된 경우 반복
                p_number++; // 숫자가 아니면 p_number 변수 값에 1을 더함
            else
                p_number = 0;
        }

        if(p_number == 1) // 전화번호가 길이제한을 넘기지 않고, 모두 숫자로만 입력된 경우는 무한루프 탈출
            break;
    }
    strncpy(phone, str, PHONElen-1);

    strncpy(I_date, str_now, DATElen);

    if(strlen(phone) != 0) { // phone 변경 값이 있으면 수정
        memset(sql_upadinfo, '\0', SQLlen);
        strncpy(sql_upadinfo, "UPDATE INFO SET (phone, I_date) = ('", 36);
        strncat(sql_upadinfo, phone, PHONElen-1);
        strncat(sql_upadinfo, "','", 3);
        strncat(sql_upadinfo, I_date, DATElen-1);
        strncat(sql_upadinfo, "') WHERE id = '", 15);
        strncat(sql_upadinfo, id, IDlen-1);
        strncat(sql_upadinfo, "';", 2);
        //printf("%s\n", sql_upadinfo);
        rc = sqlite3_exec(db, sql_upadinfo, callback, NotUsed, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Update ERROR (phone) : %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu(); // DBPrintModule.c
        }
        else {
            fprintf(stderr, "phone 값을 %s로 수정 성공\n", phone);
            if(checkAg(play) == 1) // DBManage.c
                upAD_INFO();
        }
        memset(sql_upadinfo, '\0', SQLlen);
    }

    sqlite3_close(db);

    return 0;
}
