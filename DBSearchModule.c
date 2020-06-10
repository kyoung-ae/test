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

int search_id() { // case 36 : id를 기준으로 검색
    sqlite3 *db;
    sqlite3_stmt *res;
    char *errmsg;
    int rc;
    char sql_searchid[SQLlen] = { 0, }; // 모든 항목이 나오게 검색
    char play[IDlen] = {"검색\0"};

    char id[IDlen] = { 0, };

    int sort = 0; // whitelist 정렬 기준 값
    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열 (버퍼오버플로 방지를 위해 1024로 지정)
    int i;

    char str_col[ACCESSlen] = { 0, }; // 사용자가 입력한 문자열(항목 번호)

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

    for(i = 0; i < 5; i++) {  // id 확인 5회로 제한
        while(1) { // id 검색
            memset(id, '\0', IDlen);
            memset(str, '\0', IDlen);
            strsize = 0;
            puts("\nSearch id (9bytes 보다 길면 다시 입력함):");
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

    printf("\n입력한 id : %s를 기준으로 검색결과를 출력합니다.\n", id);
    printf("\n검색 결과 항목은 pwd를 제외하고 모든 항목이 출력됩니다.\n");
    printf("모든 항목 출력이 아닌, 원하는 항목을 지정하려면 해당 번호를 누르세요.\n");

    printf("정보 확인을 위해 id와 pwd를 재확인합니다.\n");
    if(checkFunc(id) == 0) {
        sqlite3_close(db);
        printf("기능 선택 화면으로 돌아갑니다.\n");
        printMenu();
    }

    while(1) { // 검색 결과 항목 선택
        memset(str[0], '\0', ACCESSlen);
        strsize = 0;
        print_search_i(); // DBPrintModule.c
        printf("input : ");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // null 이면 모든 항목을 출력함
            break;
        if(str[0] == '0')  // 0을 선택하면 종료됨
            exit(0);

        strsize = strlen(str)+1;

        if(strsize > ACCESSlen) { // 길이 제한을 초과한 경우는 반복
            printBOF_gets(str, strsize, ACCESSlen); // DBPrintModule.c
            continue;
        }

        if(str[0] == '1' || str[0] == '2')
            break; //break 탈출이 안 됨 (세그 폴트 ㅠㅠ 단일 소스로는 이상없이 돌아감...이건..무슨 경우?)
            // 소스 수정없이 업데이트와 업그레이드 진행 후에 해결됨...(세그 폴트 발생 이유도, 해결된 이유도 여전히 모르겠음)
            // 소스 안 건드렸는데 또 세그폴트 됨...ㅠㅠ
            // break로 탈출이 안 되는 줄 알았는데, 탈출 후에 입력값을 받는 부분에서 세그폴트가 발생함을 확인함.
            //  break 부분의 세그폴트 원인 (break로 탈출 후에 다음 실행 소스가 세그폴트였음)파악되어 해결됨.
        printf("잘못 입력했습니다.\n");
    }
    strncpy(str_col, str, ACCESSlen-1);

    if(str_col[0] == '\0') { // null(EnterKey) 인 경우는 전부 출력되야 하므로 0으로 지정
        while(1) {
            sort = 0;
            print_search_sort();
            puts("select number:");
            scanf("%d", &sort);
            removeEnter();
            if(sort >= 1 && sort <=4)
                break;
            printf("1~4까지의 숫자를 입력하세요!\n");
        }

        printf("\n<id : %s 를 기준으로 검색한 결과>\n\n", id);

        switch(sort) { // whitelist 정렬 기준
            case 1: // whitelist 오름차순 검색
                memset(sql_searchid, '\0', SQLlen);
                strncpy(sql_searchid, "select whitelist.whitelist, whitelist.W_date, admin.id, admin.access,", 69);
                strncat(sql_searchid, " info.name, info.birth, info.email, info.phone, info.I_date ", 60);
                strncat(sql_searchid, "from admin, info, whitelist where admin.id = '", 46);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' and info.id = '", 17);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' and whitelist.id = '", 22);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' order by whitelist asc;", 25);
                rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Search : %s (All col)\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("기능 선택 화면으로 돌아갑니다.\n");
                    printMenu();
                }
                else {
                    fprintf(stderr, "whitelist 오름차순 검색 : all col\n");
                    printf("원하는 결과가 안 나왔다면\n");
                    printf("Whitelist 테이블에 방금 입력한 id로 등록된 게 없기 때문입니다.\n");
                    printf("whitelist 테이블에 방금의 id로 등록을 하거나,\n");
                    printf("검색 항목을 선택할 때, 2 로 선택하세요.\n");
                    if(checkAg(play) == 1) // DBManage.c
                        search_id();
                }
                memset(sql_searchid, '\0', SQLlen);
                break;

            case 2: // whitelist 내림차순 검색
                memset(sql_searchid, '\0', SQLlen);
                strncpy(sql_searchid, "select whitelist.whitelist, whitelist.W_date, admin.id, admin.access,", 69);
                strncat(sql_searchid, " info.name, info.birth, info.email, info.phone, info.I_date ", 60);
                strncat(sql_searchid, "from admin, info, whitelist where admin.id = '", 46);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' and info.id = '", 17);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' and whitelist.id = '", 22);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' order by whitelist desc;", 26);
                rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Search : %s (All col)\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("기능 선택 화면으로 돌아갑니다.\n");
                    printMenu();
                }
                else {
                    fprintf(stderr, "whitelist 내림차순 검색 : all col\n");
                    printf("원하는 결과가 안 나왔다면\n");
                    printf("Whitelist 테이블에 방금 입력한 id로 등록된 게 없기 때문입니다.\n");
                    printf("whitelist 테이블에 방금의 id로 등록을 하거나,\n");
                    printf("검색 항목을 선택할 때, 2 로 선택하세요.\n");
                    if(checkAg(play) == 1) // DBManage.c
                        search_id();
                }
                memset(sql_searchid, '\0', SQLlen);
                break;

            case 3: // W_date whitelist를 먼저 등록한 순서로 정렬
                memset(sql_searchid, '\0', SQLlen);
                strncpy(sql_searchid, "select whitelist.W_date, whitelist.whitelist, admin.id, admin.access,", 69);
                strncat(sql_searchid, " info.name, info.birth, info.email, info.phone, info.I_date ", 60);
                strncat(sql_searchid, "from admin, info, whitelist where admin.id = '", 46);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' and info.id = '", 17);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' and whitelist.id = '", 22);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' order by W_date desc;", 23);
                rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Search : %s (All col)\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("기능 선택 화면으로 돌아갑니다.\n");
                    printMenu();
                }
                else {
                    fprintf(stderr, "W_date 먼저 등록한 순서로 검색 : all col\n");
                    printf("원하는 결과가 안 나왔다면\n");
                    printf("Whitelist 테이블에 방금 입력한 id로 등록된 게 없기 때문입니다.\n");
                    printf("whitelist 테이블에 방금의 id로 등록을 하거나,\n");
                    printf("검색 항목을 선택할 때, 2 로 선택하세요.\n");
                    if(checkAg(play) == 1) // DBManage.c
                        search_id();
                }
                memset(sql_searchid, '\0', SQLlen);
                break;

            case 4: // W_date whitelist를 최근에 등록한 순서로 정렬
                memset(sql_searchid, '\0', SQLlen);
                strncpy(sql_searchid, "select whitelist.W_date, whitelist.whitelist, admin.id, admin.access,", 69);
                strncat(sql_searchid, " info.name, info.birth, info.email, info.phone, info.I_date ", 60);
                strncat(sql_searchid, "from admin, info, whitelist where admin.id = '", 46);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' and info.id = '", 17);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' and whitelist.id = '", 22);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' order by W_date asc;", 22);
                rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Search : %s (All col)\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("기능 선택 화면으로 돌아갑니다.\n");
                    printMenu();
                }
                else {
                    fprintf(stderr, "W_date 최근에 등록한 순서로 검색 : all col\n");
                    printf("원하는 결과가 안 나왔다면\n");
                    printf("Whitelist 테이블에 방금 입력한 id로 등록된 게 없기 때문입니다.\n");
                    printf("whitelist 테이블에 방금의 id로 등록을 하거나,\n");
                    printf("검색 항목을 선택할 때, 2 로 선택하세요.\n");
                    if(checkAg(play) == 1) // DBManage.c
                        search_id();
                }
                memset(sql_searchid, '\0', SQLlen);
                break;

            default:
                break;
        }
    }

    if(str_col[0] == '1') { // 화이트리스트 테이블 검색
        while(1) {
            sort = 0;
            print_search_sort();
            puts("select number:");
            scanf("%d", &sort);
            removeEnter();
            if(sort >= 1 && sort <=4)
                break;
            printf("1~4까지의 숫자를 입력하세요!\n");
        }

        printf("\n<id : %s 를 기준으로 검색한 결과>\n\n", id);

        switch(sort) { // whitelist 정렬 기준
            case 1: // whitelist 오름차순 검색
                memset(sql_searchid, '\0', SQLlen);
                strncpy(sql_searchid, "SELECT whitelist, W_date FROM whitelist WHERE id = '", 62);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' order by whitelist asc;", 25);
                rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Search : %s (1.whitelist & W_date)\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("기능 선택 화면으로 돌아갑니다.\n");
                    printMenu();
                }
                else {
                    if(checkAg(play) == 1) // DBManage.c
                        search_id();
                }
                memset(sql_searchid, '\0', SQLlen);
                break;

            case 2: // whitelist 내림차순 검색
                memset(sql_searchid, '\0', SQLlen);
                strncpy(sql_searchid, "SELECT whitelist, W_date FROM whitelist WHERE id = '", 62);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' order by whitelist desc;", 26);
                rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Search : %s (1.whitelist & W_date)\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("기능 선택 화면으로 돌아갑니다.\n");
                    printMenu();
                }
                else {
                    if(checkAg(play) == 1) // DBManage.c
                        search_id();
                }
                memset(sql_searchid, '\0', SQLlen);
                break;

            case 3: // W_date whitelist를 먼저 등록한 순서로 정렬
                memset(sql_searchid, '\0', SQLlen);
                strncpy(sql_searchid, "SELECT  W_date, whitelist FROM whitelist WHERE id = '", 62);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' order by W_date desc;", 23);
                rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Search : %s (1.whitelist & W_date)\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("기능 선택 화면으로 돌아갑니다.\n");
                    printMenu();
                }
                else {
                    if(checkAg(play) == 1) // DBManage.c
                        search_id();
                }
                memset(sql_searchid, '\0', SQLlen);
                break;

            case 4: // W_date whitelist를 최근에 등록한 순서로 정렬
                memset(sql_searchid, '\0', SQLlen);
                strncpy(sql_searchid, "SELECT W_date, whitelist FROM whitelist WHERE id = '", 62);
                strncat(sql_searchid, id, IDlen-1);
                strncat(sql_searchid, "' order by W_date asc;", 22);
                rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
                if(rc != SQLITE_OK) {
                    fprintf(stderr, "Can't Search : %s (1.whitelist & W_date)\n", sqlite3_errmsg(db));
                    sqlite3_close(db);
                    printf("기능 선택 화면으로 돌아갑니다.\n");
                    printMenu();
                }
                else {
                    if(checkAg(play) == 1) // DBManage.c
                        search_id();
                }
                memset(sql_searchid, '\0', SQLlen);
                break;

            default:
                break;
        }
    }

    if(str_col[0] == '2') { // 관리자 정보 검색
        memset(sql_searchid, '\0', SQLlen); // admin.access
        strncpy(sql_searchid, "SELECT access FROM admin WHERE id = '", 37);
        strncat(sql_searchid, id, IDlen-1);
        strncat(sql_searchid, "';", 2);
        //printf("%s\n", sql_searchid);
        rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't Search : %s (2.admin.access)\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu();
        }

        memset(sql_searchid, '\0', SQLlen); // info.name & birth & email & phone & I_date
        strncpy(sql_searchid, "SELECT name, birth, email, phone, I_date FROM info WHERE id = '", 63);
        strncat(sql_searchid, id, IDlen-1);
        strncat(sql_searchid, "';", 2);
        rc = sqlite3_exec(db, sql_searchid, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't Search : %s (2.info.name,birth,email,phone,I_date)\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu();
        }
        else {
            if(checkAg(play) == 1) // DBManage.c
                search_id();
        }
    }
    /*
    // 정렬 값이 없는 경우 : 테이블에 저장된 순서대로 출력해준다.
    select admin.id, admin.access, info.name, info.birth, info.email, info.phone, info.I_date,
    whitelist.whitelist, whitelist.W_date from admin, info, whitelist
    where admin.id = info.id and info.id=whitelist.id;

    또는 정렬하기 위해 위 문장의 맨 마지막 where 부분에 내용을 추가해서 sql문을 실행시킴
    asc는 오름차순(기본값이라서 지정하지 않으면 오름차순<으로 정렬됨), desc는 내림차순> 정렬이다.
    첫 번째 지정한 정렬을 수행하고, 첫 정렬 값이 동일한 경우에만 두 번째 지정한 정렬을 수행한다.
    ,로 몇 개든지 정렬을 추가할 수 있지만, 첫 번째 지정한 정렬을 기준으로 정렬되는 것은 동일하다.

    // 정렬 값이 있는 경우
    select admin.id, admin.access, info.name, info.birth, info.email, info.phone, info.I_date,
    whitelist.whitelist, whitelist.W_date  from admin, info, whitelist
    where admin.id = info.id and info.id=whitelist.id
    order by admin.access asc, whitelist.whitelist desc;
    */
    memset(sql_searchid, '\0', SQLlen);
    sqlite3_close(db);

    return 0;
}

int search_wl() { // case 37 : whitelist를 기준으로 검색
    sqlite3 *db;
    sqlite3_stmt *res;
    char *errmsg;
    int rc;
    char sql_searchwl[SQLlen] = { 0, }; // 입력한 whitelist의 like 검색
    char play[IDlen] = {"검색\0"};

    char id[IDlen] = { 0, };
    char whitelist[WLlen] = { 0, };

    int strsize = 0; // 실제로 사용자에게 입력 받은 글자수를 확인
    char str[MAX] = { 0, }; // 사용자에게 입력받은 임시 문자열 (버퍼오버플로 방지를 위해 1024로 지정)
    char str_col[ACCESSlen] = { 0, }; // 사용자가 입력한 문자열(항목 번호)
    int i;

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

    for(i = 0; i < 5; i++) { // whitelist 확인 5회로 제한
        while(1) { // whitelist를 기준으로 먼저 부분 검색
            memset(whitelist, '\0', WLlen);
            memset(str, '\0', WLlen);
            strsize = 0;
            puts("Whitelist 검색은 먼저, 부분 문자열 검색으로 확인합니다.");
            puts("\nSearch whitelist (30bytes 보다 길면 다시 입력함):");
            gets(str);

            if(str[0] == '\n' || str[0] == '\0') // 검색 정보는 null 불가
                continue;
            strsize = strlen(str)+1;
            if(strsize >=4 && strsize <= WLlen)
                break;
            else if(strsize < 3) {
                printf("2 btyes 보다 길게 입력하세요!\n");
                continue;
            }
            else
                printBOF_gets(str, strsize, WLlen); // DBPrintModule.c
        }
        strncpy(whitelist, str, WLlen-1);
        printf("입력한 whitelist : %s에 대한 부분 검색 결과입니다.\n", whitelist);

        memset(sql_searchwl, '\0', SQLlen);
        strncpy(sql_searchwl, "SELECT whitelist FROM WHITELIST WHERE WHITELIST like '%", 55);
        strncat(sql_searchwl, whitelist, WLlen-1);
        strncat(sql_searchwl, "%';", 3);
        rc = sqlite3_exec(db, sql_searchwl, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't Search : %s (whitelist error)\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu();
        }
        else {
            fprintf(stderr, "위의 결과에서 whitelist 명령어를 찾아 정확하게 입력하세요.\n");
        }
        memset(sql_searchwl, '\0', SQLlen);

        while(1) { // whitelist를 정확하게 입력 받음
            memset(whitelist, '\0', SQLlen);
            memset(str, '\0', SQLlen);
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

        if(checkWL(whitelist) == 1) // DBManage.c
            break;
        else {
            if(i <= 2)
                printf("총 5회 시도 중에 %d번 남았습니다.\n", 4-i);
            if(i == 3)
                printf("\n마지막 시도입니다. whitelist가 일치하지 않으면 기능 선택 화면으로 돌아갑니다.\n");
        }
    }
    if(i == 5) {
        printf("\n5번 모두 id와  whitelist가 일치하지 않아 기능 선택 화면으로 돌아갑니다.\n\n");
        printMenu();
    }

    printf("\n입력한 whitelist : %s를 기준으로 검색결과를 출력합니다.\n", whitelist);
    printf("\n검색 결과 항목은 pwd를 제외하고 모든 항목이 출력됩니다.\n");
    printf("모든 항목 출력이 아닌, 원하는 항목을 지정하려면 해당 번호를 누르세요.\n\n");

    while(1) { // 검색 결과 항목 선택
        memset(str, '\0', ACCESSlen);
        strsize = 0;
        print_search_w(); // DBPrintModule.c
        printf("input : ");
        gets(str);

        if(str[0] == '\n' || str[0] == '\0') // null 이면 모든 항목을 출력함
            break;
        if(str[0] == '0')  // 0을 선택하면 종료됨
            exit(0);

        strsize = strlen(str)+1;

        if(strsize > ACCESSlen) { // 길이 제한을 초과한 경우는 반복
            printBOF_gets(str, strsize, ACCESSlen); // DBPrintModule.c
            continue;
        }

        if(str[0] == '1' || str[0] == '2')
            break;
        printf("잘못 입력했습니다.\n");
    }
    strncpy(str_col, str, ACCESSlen-1);
    printf("\n");
    printf("<whitelist : %s 를 기준으로 검색한 결과>\n\n", whitelist);

    if(str_col[0] == '\0') { // null(EnterKey) 인 경우는 전부 출력되야 하므로 0으로 지정
        memset(sql_searchwl, '\0', SQLlen);
        strncpy(sql_searchwl, "select whitelist.id, whitelist.W_date, admin.access, ", 53);
        strncat(sql_searchwl, "info.name, info.birth, info.email, info.phone, info.I_date", 58);
        strncat(sql_searchwl, " from admin, info, whitelist where whitelist.whitelist = '", 58);
        strncat(sql_searchwl, whitelist, WLlen-1);
        strncat(sql_searchwl, "' and (select id from whitelist where whitelist = '", 51);
        strncat(sql_searchwl, whitelist, WLlen-1);
        strncat(sql_searchwl, "') = whitelist.id and (select id from whitelist where whitelist = '", 87);
        strncat(sql_searchwl, whitelist, WLlen-1);
        strncat(sql_searchwl, "') = admin.id and (select id from whitelist where whitelist = '", 63);
        strncat(sql_searchwl, whitelist, WLlen-1);
        strncat(sql_searchwl, "') = info.id;", 13);
        //printf("%s\n", sql_searchwl);
        rc = sqlite3_exec(db, sql_searchwl, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't Search : %s (All col)\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu();
        }
        else {
            fprintf(stderr, "\nSearch success : all col\n");
            printf("원하는 결과가 안 나왔다면\n");
            printf("Admin 테이블에 방금 입력한 whitelist의 id로 등록되지 않았기 때문입니다.\n");
            printf("Admin 테이블에 방금 whitelist의 id로 등록을 하거나,\n");
            printf("검색 항목을 선택할 때, 1 로 선택하세요.\n");
            if(checkAg(play) == 1) // DBManage.c
                    search_wl();
        }
        memset(sql_searchwl, '\0', SQLlen);
    }

    if(str_col[0] == '1') { // 관리자 정보 검색
        memset(sql_searchwl, '\0', SQLlen); // admin.access
        strncpy(sql_searchwl, "SELECT id, access FROM admin WHERE id = ", 40);
        strncat(sql_searchwl, "(select id from whitelist where whitelist = '", 45);
        strncat(sql_searchwl, whitelist, WLlen-1);
        strncat(sql_searchwl, "');", 3);
        rc = sqlite3_exec(db, sql_searchwl, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't Search : %s (1.admin.access)\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu();
        }

        memset(sql_searchwl, '\0', SQLlen); // info.name & birth & email & phone & I_date
        strncpy(sql_searchwl, "SELECT name, birth, email, phone, I_date FROM info WHERE id = ", 72);
        strncat(sql_searchwl, "(select id from whitelist where whitelist = '", 45);
        strncat(sql_searchwl, whitelist, WLlen-1);
        strncat(sql_searchwl, "');", 3);
        rc = sqlite3_exec(db, sql_searchwl, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't Search : %s (1.info.name,birth,email,phone,I_date)\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu();
        }
        else {
            if(checkAg(play) == 1) // DBManage.c
                search_wl();
        }
        memset(sql_searchwl, '\0', SQLlen);
    }

    if(str_col[0] == '2') { // 화이트리스트 테이블 검색
        memset(sql_searchwl, '\0', SQLlen); // whitelist.id & whitelist & W_date
        strncpy(sql_searchwl, "SELECT whitelist, id, W_date FROM whitelist WHERE whitelist = '", 63);
        strncat(sql_searchwl, whitelist, WLlen-1);
        strncat(sql_searchwl, "';", 2);
        rc = sqlite3_exec(db, sql_searchwl, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't Search : %s (2.whitelist.whitelist & id & W_date)\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            printf("기능 선택 화면으로 돌아갑니다.\n");
            printMenu();
        }
        else {
            if(checkAg(play) == 1) // DBManage.c
                search_wl();
        }
        memset(sql_searchwl, '\0', SQLlen);
    }
    sqlite3_close(db);

    return 0;
}
