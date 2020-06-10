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

void removeEnter() { // scanf()의 Enter key 제거
    char tmp;

    while((tmp = getchar()) != '\n') {
        putchar(tmp);
    }
}

void removeSpace(char str_col[]) { // str_col 문자열의 공백 제거
    int i = 0;
    int j = 0;

    while(str_col[i] != '\0') {
        if(str_col[i] != ' ') {
            str_col[j++] = str_col[i];
        }
        i++;
    }
    str_col[j] = '\0'; // 맨 끝에 null 붙이기
}

char *dateNow(struct tm *t) { // date 가져오는 함수
    static char now[DATElen] = { 0, };

    sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900,
                t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    return now;
}

int checkDate(int date) { // 입력받은 날짜의 유효성 검사-> 1이면 유효한 날짜 (0이면 없는 날짜임)
    int yyyy, mm, dd = 0;

	yyyy = date / 10000;
    mm = (date-(yyyy*10000)) / 100;
	dd = date % 100 ;
	printf("%d년 %d월 %d일\n", yyyy, mm, dd);

	if(yyyy <= 1000)
		return 0;
	if(mm == 0 || mm >= 13)
		return 0;
	if(dd == 0 || dd >= 32)
		return 0;

	if (mm == 2) { // 2월달의 윤년(29일), 평년(28일) 구분
        if(yyyy%400 == 0) { // 400으로 나눠지면 윤년
            if(dd <= 0 || dd >30)
            	return 0;
		}
		else if((yyyy%4 == 0) && (yyyy%100 != 0)) { // 4와 100으로 나눠지면 윤년
            if(dd <= 0 || dd >30)
            	return 0;
		}
		else { // 그 외에는 전부 평년
			if(dd <=0 || dd > 28)
					return 0;
		}
	}
	else if (mm == 4 || mm == 6 || mm == 9 || mm == 11) { // 4, 6, 9, 11월 -> 30일까지 있음
        if (dd <= 0 || dd > 30)
			return 0;
	}
	else { // 1, 3, 5, 7, 8, 10, 12월 -> 31일까지 있음
		if (dd <= 0 || dd > 31)
			return 0;
	}
	return 1;
}

int checkIDPWD(char id[]) { // 입력한 id를 매개변수로 받아서 id&pwd 확인 (성공하면 1을 반환)
    sqlite3 *db;
    sqlite3_stmt *res;
    char *errmsg;
    int rc;
    char sql_checkidpwd[SQLlen] = { 0, };

    char pwd[PWDlen] = { 0, };

    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

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

    while(1) { // 확인하려는 데이터 id의 pwd
        memset(str, '\0', PWDlen);
        strsize = 0;
        puts("\n확인하려는 id의 원래 PWD 입력:");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // 필수 확인 정보는 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize <= PWDlen)
            break;

        printBOF_gets(str, strsize, PWDlen); // DBPrintModule.c
    }
    strncpy(pwd, str, PWDlen-1);

    memset(sql_checkidpwd, '\0', SQLlen);
    strncpy(sql_checkidpwd, "SELECT id, pwd FROM ADMIN WHERE id = '", 38);
    strncat(sql_checkidpwd, id, IDlen-1);
    strncat(sql_checkidpwd, "' AND pwd = '", 13);
    strncat(sql_checkidpwd, pwd, PWDlen-1);
    strncat(sql_checkidpwd, "';", 2);
    /*
    strncpy(sql_checkidpwd, "SELECT CASE WHEN id=='", 22);
    strncat(sql_checkidpwd, id, IDlen-1);
    strncat(sql_checkidpwd, "' AND pwd=='", 12);
    strncat(sql_checkidpwd, pwd, PWDlen-1);
    strncat(sql_checkidpwd, "' THEN 'Success' ELSE 'Failure' END FROM ADMIN;", 47);
    /*
    rc = sqlite3_exec(db, sql_checkidpwd, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print Admin Table : %s\n", sqlite3_errmsg(db));
        return 0;
    }
    */
    rc = sqlite3_prepare_v2(db, sql_checkidpwd, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW) { // 등록된 id와 pwd가 일치함
        sqlite3_finalize(res);
        memset(sql_checkidpwd, '\0', SQLlen);
        sqlite3_close(db);
        return 1;
    }
    else { // 등록된 id와 pwd가 일치하지 않음
        printf("id와 pwd가 일치하지 않습니다.\n");
        sqlite3_finalize(res);
        memset(sql_checkidpwd, '\0', SQLlen);
        sqlite3_close(db);
        return 0;
    }
}

int checkID(char id[]) { // 입력한 id를 매개변수로 받아서 등록된 id인지 확인 (성공하면 1을 반환)
    sqlite3 *db;
    sqlite3_stmt *res;
    char *errmsg;
    int rc;
    char sql_checkid[SQLlen] = { 0, };
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

    memset(sql_checkid, '\0', SQLlen);
    strncpy(sql_checkid, "SELECT id From Admin where id='", 31);
    strncat(sql_checkid, id, IDlen-1);
    strncat(sql_checkid, "';", 2);
    //printf("%s\n", sql_checkid);
    /*
    strncpy(sql_checkid, "SELECT CASE WHEN id=='", 22);
    strncat(sql_checkid, id, IDlen-1);
    strncat(sql_checkid, "' AND id=='", 11);
    strncat(sql_checkid, id, IDlen-1);
    strncat(sql_checkid, "' THEN 'Success' ELSE 'Failure' END FROM admin;", 47);
    */
    rc = sqlite3_prepare_v2(db, sql_checkid, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW) { // 등록된 id임이 확인됨.
        sqlite3_finalize(res);
        memset(sql_checkid, '\0', SQLlen);
        sqlite3_close(db);
        return 1;
    }
    else { // 등록되지 않은 id임
        printf("존재하지 않는 id입니다.\n");
        sqlite3_finalize(res);
        memset(sql_checkid, '\0', SQLlen);
        sqlite3_close(db);
        return 0;
    }
}

int checkWL(char whitelist[]) { // 입력한 whitelist를 매개변수로 받아 등록된 whitelist인지 확인 (성공하면 1 반환)
    sqlite3 *db;
    sqlite3_stmt *res;
    char *errmsg;
    int rc;
    char sql_checkwl[SQLlen] = { 0, };
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

    memset(sql_checkwl, '\0', SQLlen);
    strncpy(sql_checkwl, "SELECT whitelist FROM WHITELIST WHERE whitelist = '", 51);
    strncat(sql_checkwl, whitelist, WLlen-1);
    strncat(sql_checkwl, "';", 2);
    /*
    strncpy(sql_checkwl, "SELECT CASE WHEN whitelist=='", 29);
    strncat(sql_checkwl, whitelist, WLlen-1);
    strncat(sql_checkwl, "' AND whitelist=='", 18);
    strncat(sql_checkwl, whitelist, WLlen-1);
    strncat(sql_checkwl, "' THEN 'Success' ELSE 'Failure' END FROM WHITELIST;", 51);
    */
    //printf("%s\n", sql_checkwl);
    rc = sqlite3_prepare_v2(db, sql_checkwl, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW) { // 등록된 whitelist 임이 확인됨.
        sqlite3_finalize(res);
        memset(sql_checkwl, '\0', SQLlen);
        sqlite3_close(db);
        return 1;
    }
    else { // 등록되지 않은 whitelist 임
        printf("존재하지 않는 whitelist입니다.\n");
        sqlite3_finalize(res);
        memset(sql_checkwl, '\0', SQLlen);
        sqlite3_close(db);
        return 0;
    }
}

int checkEmail(char email[]) { // 입력한 email을 매개변수로 받아 등록된 email인지 확인 (성공하면 1반환)
    sqlite3 *db;
    sqlite3_stmt *res;
    char *errmsg;
    int rc;
    char sql_checkemail[SQLlen] = { 0, };
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

    memset(sql_checkemail, '\0', SQLlen);
    strncpy(sql_checkemail, "SELECT email FROM info WHERE email = '", 38);
    strncat(sql_checkemail, email, EMAILlen-1);
    strncat(sql_checkemail, "';", 2);

    //printf("%s\n", sql_checkemail);
    rc = sqlite3_prepare_v2(db, sql_checkemail, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW) { // 등록된 email 임이 확인됨.
        sqlite3_finalize(res);
        memset(sql_checkemail, '\0', SQLlen);
        sqlite3_close(db);
        return 1;
    }
    else { // 등록되지 않은 email 임
        printf("존재하지 않는 email입니다. 종료됩니다.\n");
        sqlite3_finalize(res);
        memset(sql_checkemail, '\0', SQLlen);
        sqlite3_close(db);
        return 0;
    }
}

int checkFunc(char id[]) { // pwd 확인을 5회로 제한
    int i = 0; // 반복 횟수 제한

    for(i = 0; i < 5; i++) {
        if(checkIDPWD(id) == 0) { // 입력한 id&pwd 일치하지 않으면 DB 메뉴로 돌아감
            if(i <= 2)
                printf("총 5회 시도 중에 %d번 남았습니다.\n", 4-i);
            if(i == 3)
                printf("\n마지막 시도입니다. pwd가 일치하지 않으면 DB메뉴로 돌아갑니다.\n");
        }
        else {
            break;
            return 1;
        }
    }
    if(i == 5) {
        printf("\n5번 모두 id와 pwd가 일치하지 않았습니다.\n");
        return 0;
    }
}

void checkRep(char id[], char play[]) { // 작업 진행 여부 확인 루프
    sqlite3 *db;
    int i = 0; // 반복 횟수 제한
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열

    while(1) {
        memset(str, '\0', ACCESSlen);
        strsize = 0;

        puts("\n0 : 종료");
        printf("1 : %s 진행\n", play);
        puts("2 : DB menu");
        printf("input : ");
        gets(str);

        if(str[0] == '1' ||str[0] == '2')
            break;
        if(str[0] == '0') {
            sqlite3_close(db);
            exit(0);
        }
        if(str[0] == '\n' || str[0] == '\0') // 필수 입력 정보는 null 불가
            continue;
        strsize = strlen(str)+1;
        if(strsize > ACCESSlen) {
            printBOF_gets(str, strsize, IDlen); // DBPrintModule.c
            continue;
        }
    }

    if(str[0] == '2')
        printMenu();

    if(str[0] == '1') {
        printf("%s 과정을 위해 pwd를 재확인합니다.\n", play);
        for(i = 0; i < 5; i++) { // pwd 확인을 5회로 제한
            if(checkIDPWD(id) == 0) { // DBManage.c : 입력한 id&pwd 일치하지 않으면 종료
                if(i <= 2)
                    printf("총 5회 시도 중에 %d번 남았습니다.\n", 4-i);
                if(i == 3)
                    printf("\n마지막 시도입니다. pwd가 일치하지 않으면 DB메뉴로 돌아갑니다.\n");
            }
            else
                break;
        }
        if(i == 5) {
            printf("\n5번 모두 id와 pwd가 일치하지 않아 DB메뉴로 돌아갑니다.\n\n");
            printMenu();
        }
    }
}

int checkAg(char play[]) { // 동일 작업 반복 여부
    int i = 0; // 진행 선택 값
    sqlite3 *db;

    while(1) {
        printf("\nselect number\n");
        printf("1 : 계속 %s\n", play);
        printf("2 : DB 시작 화면\n");
        printf("0 : 종료 EXIT\n");
        printf("input : ");
        scanf("%d", &i);
        removeEnter(); // DBMagage.c

        if(i >= 0 && i <= 2)
            break;
        else
            printf("잘못 입력하셨습니다.\n");
    }

    switch(i) {
        case 0 :
            sqlite3_close(db);
            exit(0);
            break;

        case 2 :
            printMenu();
            break;

        default:
            return 1;
            break;
    }
}
