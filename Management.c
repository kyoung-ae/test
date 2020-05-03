#include <stdio.h>
#include <sqlite3.h>
#include <time.h>
#include <string.h>
#include "BaseDefine.h"
#include "DB.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i]:"NULL");
    }
    printf("\n");

    return 0;
}

char *dateNow(struct tm *t) { //date 가져오는 함수
    static char now[DATElen] = { 0, };

    sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900,
                t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    return now;
}

int selWhitelist(const char sel_wl[WLlen], OUT struct WhiteListTable sel_wt) { // case 19
    //char whitelist[WLlen];
    OUT struct WhiteListTable wt;

    sqlite3 *db;
   	char *errmsg;
   	char *sql; // table schema sql
    sqlite3_stmt *res;
    int rc;
    char input_sql[512];
    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };

    int menu; // search menu
    char tmp; // Enter Key remove

    rc = sqlite3_open("WHITELIST.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open WHITELIST DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); // db open시 timeout 500ms로 설정

    // WHITELIST Table 출력
    sql = "select * from WHITELIST";
    rc = sqlite3_exec(db, sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print schema : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Print schema successfully\n");
    }

    int sel_wl_wl() { // case 1
        puts("search whitelist:");
        gets(whitelist);

        fflush(stdin);
        strcpy(input_sql, "SELECT WHITELIST FROM WHITELIST WHERE WHITELIST like '%");
        strcat(input_sql, whitelist);
        strcat(input_sql, "%';");
        printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    int sel_wl_id() { // case 2
        puts("search id:");
        gets(id);

        fflush(stdin);
        strcpy(input_sql, "SELECT ID FROM WHITELIST WHERE ID like '%");
        strcat(input_sql, id);
        strcat(input_sql, "%';");
        printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    int sel_wl_wi() { // case 3
        puts("search whitelist:");
        gets(whitelist);
        puts("search id:");
        gets(id);

        fflush(stdin);
        strcpy(input_sql, "SELECT WHITELIST, ID FROM WHITELIST WHERE WHITELIST like '%");
        strcat(input_sql, whitelist);
        strcat(input_sql, "%' OR ID like '%");
        strcat(input_sql, id);
        strcat(input_sql, "%';");
        printf("%s\n", input_sql);

        rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
        if(rc != SQLITE_OK) {
            fprintf(stderr, "Can't search : %s\n", sqlite3_errmsg(db));
            return 1;
        }
        else {
            fprintf(stderr, "Print search successfully\n");
        }
    }

    printf("WHITELIST TABLE's search\n");
    printf("1. whitelist 로 검색\n");
    printf("2. id 로 검색\n");
    printf("3. whitelist 와 id 로 검색\n");
    printf("0. EXIT\n");
    printf("검색하려는 항목의 번호를 고르세요:");
    scanf("%d", &menu);

    while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
        putchar(tmp);
    }

    switch(menu) {
        case 1:
            sel_wl_wl();
            break;

        case 2:
            sel_wl_id();
            break;

        case 3:
            sel_wl_wi();
            break;

        default:
            break;
    }

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

int selAdminInfo(const char sel_id[IDlen], const char sel_pwd[PWDlen], OUT struct InfoTable sel_it) {
    char id[IDlen];
    char pwd[PWDlen];
    OUT struct InfoTable it;
}

int selPublicKey(const char sel_id[IDlen], const char sel_pwd[PWDlen], OUT char sel_pk[PKlen]) {
    char id[IDlen];
    char pwd[PWDlen];
    OUT char public_key[PKlen]; // OUT publickey를 반환함
}

int searchPWD(const char search_id[IDlen], const char seearch_pwd[PWDlen]) {
    char id[IDlen];
    char pwd[PWDlen];
}

int delAdmin(const char del_id[IDlen], const char del_pwd[PWDlen]) {
    char id[IDlen];
    char pwd[PWDlen];
}

int delWhiteList(char del_wl[WLlen]) { // case 18
    //char whitelist[WLlen];

    sqlite3 *db;
   	char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char input_sql[512];
    char whitelist[WLlen] = { 0, };
    char id[IDlen] = { 0, };

    rc = sqlite3_open("WHITELIST.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open WHITELIST DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    res = "Callback Function Called";

    puts("WHITELIST TABLE's records delete.\n");
    puts("input whitelist:");
    gets(whitelist);
    puts("input id:");
    gets(id);

    fflush(stdin);
    strcpy(input_sql, "DELETE from WHITELIST where whitelist = '");
    strcat(input_sql, whitelist);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);
    rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't delete : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Delete successfully\n");
    }

    sqlite3_close(db);

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

    rc = sqlite3_open("WHITELIST.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open WHITELIST DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

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

int insAdminInfo(struct InsInfo ins_ai) { // case 36 & 46 참고
    struct InsInfo ai;
}

int insAdminTable(struct AdminTable ins_at) { // case 36
    struct AdminTable at;

    sqlite3 *db;
    char *errmsg;
    int rc;
    char input_sql[512];
    char id[IDlen] = { 0, };
    char pwd[PWDlen] = { 0, };
    int access;
    char tmp; //엔터키 삭제 변수
    char buf_access[2]; // int형의 access값을 문자로 받을 변수

    rc = sqlite3_open("ADMINISTRATOR.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open ADMINISTRATOR DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened ADMINISTRATOR database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    puts("ADMIN TABLE's insert\n");
    puts("input id:");
    gets(id);
    puts("input access:");
    scanf("%d", &access);
    while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
        putchar(tmp);
    }
    puts("pwd:");
    gets(pwd);

    sprintf(buf_access, "%d", access); // access를 문자로 변환

    fflush(stdin);
    strcpy(input_sql, "insert into ADMIN values('");
    strcat(input_sql, id);
    strcat(input_sql, "',");
    strcat(input_sql, buf_access);
    strcat(input_sql, ",'");
    strcat(input_sql, pwd);
    strcat(input_sql, "');");
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

int insInfoTable(struct InfoTable ins_it) { // case 46
    struct InfoTable it;

    sqlite3 *db;
    char *errmsg;
    int rc;
    char input_sql[512];
    char id[IDlen] = { 0, };
    char name[NAMElen] = { 0, };
    char birth[BIRTHlen] = { 0, };
    char email[EMAILlen] = { 0, };
    char phone[PHONElen] = { 0, };
    char date[DATElen] = { 0, };

    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    rc = sqlite3_open("ADMINISTRATOR.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open ADMINISTRATOR DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened ADMINISTRATOR database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    printf("\n등록 데이터는 입력 후 EnterKey,\n");
    printf("등록을 건너띄려면 바로 EnterKey를 누르세요.\n");

    puts("\nINFO TABLE's insert\n");
    puts("input id:");
    gets(id);
    puts("input name:");
    gets(name);
    puts("input birth:");
    gets(birth);
    puts("input email:");
    gets(email);
    puts("input phone:");
    gets(phone);
    puts("date EnterKey를 누르세요:");
    printf("%s\n", str_now);
    strcpy(date, str_now);

    fflush(stdin);
    strcpy(input_sql, "insert into INFO values('");
    strcat(input_sql, id);
    strcat(input_sql, "','");
    strcat(input_sql, name);
    strcat(input_sql, "','");
    strcat(input_sql, birth);
    strcat(input_sql, "','");
    strcat(input_sql, email);
    strcat(input_sql, "','");
    strcat(input_sql, phone);
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
        fprintf(stderr, "info table input successfully\n");
    }
    sqlite3_close(db);

    return 0;
}

int updateAdmin(struct InsInfo up_a) { // case 37 & 47 참고
    struct InsInfo a;
}

int updateInfoTable(struct InfoTable up_it) { // case 47 코딩 해야함...
    struct AdminTable it;

    sqlite3 *db;
   	char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char *sql;
    char input_sql[512];
    char id[IDlen] = { 0, };

    char src_white[WLlen] = { 0, }; // 수정할 화이트리스트 ->다른 화이트리스트로 변경됨.
    char whitelist[WLlen] = { 0, };

    char date[DATElen] = { 0, };
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    rc = sqlite3_open("WHITELIST.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open WHITELIST DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    res = "Callback Function Called";

    puts("WHITELIST TABLE's Update\n");
    puts("update source whitelist:");
    gets(src_white);
    puts("update whitelist:");
    gets(whitelist);

    puts("date Enter:");
    printf("%s\n", str_now);
    strcpy(date, str_now);

    fflush(stdin);
    strcpy(input_sql, "UPDATE WHITELIST SET (date, whitelist) = ('");
    strcat(input_sql, date);
    strcat(input_sql, "', '");
    strcat(input_sql, whitelist);
    strcat(input_sql, "') ");
    strcat(input_sql, "where whitelist = '");
    strcat(input_sql, src_white);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);

    rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't whitelist Update : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Whitelist update successfully\n");
    }
    sqlite3_close(db);

    return 0;
}

int updateAdminTable(struct AdminTable up_at) { // case 37 코딩 중임...
    struct AdminTable at;

    sqlite3 *db;
    char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char *sql;
    char input_sql[512];
    char id[IDlen] = { 0, };
    int menu; // 수정 항목을 선택하는 변수
    char tmp; //엔터키 삭제 변수

    int src_access; // 수정할 access ->다른 access로 변경됨.
    int access;

    char src_pwd[PWDlen] = { 0, }; // 수정할 PWD -> 다른 PWD로 변경됨.
    char pwd[PWDlen] = { 0, };

    char buf_access[2]; // int형의 access값을 문자로 받을 변수

    rc = sqlite3_open("ADMINISTRATOR.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open ADMINISTRATOR DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened ADMINISTRATOR database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    res = "Callback Function Called";

    printf("ADMIN TABLE's Update\n");

    puts("수정할 데이터의 id(기본키) 입력:");
    gets(id);

    fflush(stdin);
    strcpy(input_sql, "SELECT * FROM ADMIN WHERE ID = '");
    strcat(input_sql, id);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);

    rc = sqlite3_exec(db, input_sql, callback, 0, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't print Admin Table : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, " Print Admin Table successfully\n");
    }

    printf("1. ACCESS(권한) 수정\n");
    printf("2. PWD(비밀번호) 수정\n");
    printf("3. ACCESS(권한) 과 PWD(비밀번호) 모두 수정\n");
    puts("수정 할 번호 입력:");
    scanf("%d", &menu);
    while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
        putchar(tmp);
    }

    switch(menu) {
        case 1:
            printf("수정할 ACCESS(권한)값 입력:\n");
            scanf("%d", &access);
            while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
                putchar(tmp);
            }
            break;

        case 2:
            puts("수정할 PWD(비밀번호)값 입력:");
            gets(pwd);

            break;

        case 3:
            printf("수정할 ACCESS(권한)값 입력:\n");
            scanf("%d", &access);
            while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
                putchar(tmp);
            }
            puts("수정할 PWD(비밀번호)값 입력:");
            gets(pwd);

            break;

        default:
           break;
    }

    fflush(stdin); /*
    strcpy(input_sql, "UPDATE WHITELIST SET (date, whitelist) = ('");
    strcat(input_sql, date);
    strcat(input_sql, "', '");
    strcat(input_sql, whitelist);
    strcat(input_sql, "') ");
    strcat(input_sql, "where whitelist = '");
    strcat(input_sql, src_white);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);

    rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't whitelist Update : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Whitelist update successfully\n");
    } */
    sqlite3_close(db);

    return 0;
}

int updateAdminwl(struct WhiteListTable up_w) { // case 17
    struct WhiteListTable wt;

    sqlite3 *db;
   	char *errmsg;
    sqlite3_stmt *res;
    int rc;
    char *sql;
    char input_sql[512];
    char id[IDlen] = { 0, };

    char src_white[WLlen] = { 0, }; // 수정할 화이트리스트 ->다른 화이트리스트로 변경됨.
    char whitelist[WLlen] = { 0, };

    char date[DATElen] = { 0, };
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    char *str_now = dateNow(t);

    rc = sqlite3_open("WHITELIST.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open WHITELIST DB : %s\n", sqlite3_errmsg(db));
       	return 1;
    }
   	else {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_busy_timeout(db, 500); //db open시 timeout 500ms로 설정

    res = "Callback Function Called";

    puts("WHITELIST TABLE's Update\n");
    puts("update source whitelist:");
    gets(src_white);
    puts("update whitelist:");
    gets(whitelist);

    puts("date Enter:");
    printf("%s\n", str_now);
    strcpy(date, str_now);

    fflush(stdin);
    strcpy(input_sql, "UPDATE WHITELIST SET (date, whitelist) = ('");
    strcat(input_sql, date);
    strcat(input_sql, "', '");
    strcat(input_sql, whitelist);
    strcat(input_sql, "') ");
    strcat(input_sql, "where whitelist = '");
    strcat(input_sql, src_white);
    strcat(input_sql, "';");
    printf("%s\n", input_sql);

    rc = sqlite3_exec(db, input_sql, callback, res, &errmsg);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't whitelist Update : %s\n", sqlite3_errmsg(db));
        return 1;
    }
    else {
        fprintf(stderr, "Whitelist update successfully\n");
    }

    sqlite3_close(db);

    return 0;
}

int insMAC(const char insmac_id[IDlen], const char isnmac_mac[MAClen]) {
    char id[IDlen];
    char mac[MAClen];
}

int updateMac(const char upmac_id[IDlen], const char upmac_mac[MAClen]) {
    char id[IDlen];
    char mac[MAClen];
}

int updatePWD(const char uppwd_id[IDlen], const char uppwd_pwd[PWDlen]) {
    char id[IDlen];
    char pwd[PWDlen];
}

int updateAccess(const char upacc_id[IDlen], int upacc_acc) {
    char id[IDlen];
    int access;

    return 0;
}
