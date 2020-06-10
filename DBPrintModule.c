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

void print_login() { // DB 로그인 메뉴
    printf("\nLogin Number\n");
    printf("1 : DB 접속 (로그인)\n");
    printf("2 : 새로운 계정 등록\n");
    printf("3 : id, pwd 찾기\n");
    printf("0 : 종료 EXIT\n");
}

void print_pwd() { // pwd 확인 항목
    printf("\n1개 항목이 등록 정보와 일치하면 pwd를 출력합니다.\n");
    printf("\nColumn Number\n");
    printf("1 : name\n");
    printf("2 : birth\n");
    printf("3 : phone\n");
    printf("0 : 종료 EXIT\n");
}

void printMenu() { // DB 시작 메뉴 : 자체 입력 받아서 해당 함수를 호출
    int tb = 0;
    int func = 0;
    int menu = 0;

    printf("\nDB Number\n");
    printf("1 : 등록 insert\n");
    printf("2 : 수정 update\n");
    printf("3 : 검색 search\n");
    printf("4 : 삭제 delete\n");
    printf("5 : 복원(찾기) id,pwd,DB\n");
    printf("0 : 종료 EXIT\n");
    printf("input :");
    scanf("%d", &func);
    removeEnter(); // DBManage.c

    switch(func) { // 1, 2, 3, 4, 5를 제외한 수를 선택하면 종료
        case 0:
            exit(0);
            break;

        case 1: // insert 기능
            printf("\nInsert Number\n");
            printf("6 : id, access, pwd, name, birth, email, phone (=>Admin Info)\n");
            printf("7 : 허용 명령어 목록 (=>Whitelist tbl)\n");
            printf("0 : 종료 EXIT\n");
            printf("input : ");
            scanf("%d", &tb);
            menu =  func*10 + tb;
            removeEnter(); // DBManage.c
            break;

        case 2: // update 기능
            printf("\nUpdate Number\n");
            printf("6 : id, access, pwd, name, birth, email, phone (=>Admin Info)\n");
            printf("0 : 종료 EXIT\n");
            printf("input : ");
            scanf("%d", &tb);
            menu =  func*10 + tb;
            removeEnter(); // DBManage.c
            break;

        case 3: // search 기능
            printf("\nSearch Number\n");
            printf("6 : id를 기준으로 검색\n");
            printf("7 : whitelist를 기준으로 검색\n");
            printf("0 : 종료 EXIT\n");
            printf("input : ");
            scanf("%d", &tb);
            menu =  func*10 + tb;
            removeEnter(); // DBManage.c
            break;

        case 4: // delete 기능
            printf("\n6번은 지정 항목의 값만 삭제\n");
            printf("6번 예) name 값을 지정해서 name 값만 삭제\n");
            printf("\n7번은 지정 항목의 값을 가지는 행의 모든 데이터를 삭제\n");
            printf("7번 예) 지정한 name ~ birth ~ email ~ phone까지 삭제\n");
            printf("\nDelete Number\n");
            printf("\n6 : name, birth, phone 지정해서 삭제\n");
            printf("7 : id, email, whitelist 기준으로 행 삭제\n");
            printf("0 : 종료 EXIT\n");
            printf("input : ");
            scanf("%d", &tb);
            menu =  func*10 + tb;
            removeEnter(); // DBManage.c
            break;

        case 5: // id, pwd 분실지원 찾기 기능
            printf("\nRestore Number\n");
            printf("6 : id, pwd 찾기\n");
            printf("7 : DB Restore\n");
            printf("0 : 종료 EXIT\n");
            printf("input : ");
            scanf("%d", &tb);
            menu =  func*10 + tb;
            removeEnter(); // DBManage.c
            break;

        default:
            printf("입력 값이 잘못되어 종료됩니다.\n");
            exit(0);
    }

    switch(menu) {
        case 10:
            exit(0);
            break;

        case 16:
            inAD_INFO(); // DBInsertModule.c
            break;

        case 17:
            inWL(); // DBInsertModule.c
            break;

        case 20:
            exit(0);
            break;

        case 26:
            upAD_INFO(); // DBUpdateModule.c
            break;

        case 30:
            exit(0);
            break;

        case 36:
            search_id(); // DBSearchModule.c
            break;

        case 37:
            search_wl(); // DBSearchModule.c
            break;

        case 40:
            exit(0);
            break;

        case 46:
            delCol(); // DBDeleteModule.c
            break;

        case 47:
            delRow(); // DBDeleteModule.c
            break;

        case 50:
            exit(0);
            break;

        case 56:
            findIP(); // DBBackupR.c
            break;

        case 57:
            RestoreB(); // DBBackupR.c
            break;

       default :
            printf("입력 값이 잘못되어 종료됩니다.\n");
    }
}

void print_search_i() { // id를 기준으로 검색
    puts("\nEnterKey : 모든 항목 (whitelist에 방금의 id가 있어야 출력)");
    puts("1 : whitelist, 등록날짜 (whitelist에 방금의 id가 있어야 출력)");
    puts("2 : access, name, birth, email, phone, 등록날짜");
    puts("0 : DB종료 EXIT");
    puts("Search input:");
}

void print_search_sort() { // id 기준으로  whitelist 정렬 검색
    printf("\n한 id로 여러 whitelist 등록이 가능합니다.\n");
    printf("whitelist 의 검색 정렬 기준을 선택하세요.\n");
    puts("1 : whitelist 를 오름차순(a->z)으로 출력");
    puts("2 : whitelist 를 내림차순(z->a)으로 출력");
    puts("3 : whitelist 를 먼저 등록한 순서로 출력");
    puts("4 : whitelist 를 최근에 등록한 순서로 출력");
}

void print_search_w() { // whitelist를 기준으로 검색
    puts("\nEnterKey : 모든 항목 (admin에도 방금 whitelist의 id가 있어야 출력)");
    puts("1 : access, name, birth, email, phone, 등록(수정) 날짜");
    puts("2 : id, whitelist를 등록한 날짜");
    puts("0 : DB종료 EXIT");
    puts("Search input:");
}

void print_del_col() { // 컬럼만 삭제
    printf("\n삭제 후에는 복원되지 않습니다.\n");
    printf("1 : name 값만 삭제\n");
    printf("2 : birth 값만 삭제\n");
    printf("3 : phone 값만 삭제\n");
    printf("4 : name과 birth 값을 삭제\n");
    printf("5 : name과 phone 값을 삭제\n");
    printf("6 : birth와 phone 값을 삭제\n");
    printf("7 : name과 birth와 phone 값을 삭제\n");
    printf("0 : 종료 EXIT\n");
}

void print_del_row() { // 레코드 삭제
    printf("\n삭제 후에는 복원되지 않습니다.\n");
    printf("1 : id를 기준으로 일괄 삭제\n");
    printf("2 : whitelist 기준으로 일괄 삭제\n");
    printf("0 : 종료 EXIT\n");
}

void printBOF_gets(char str[], int strsize, int define_size) { // gets() 버퍼오버플로우 방지 : 재입력 요구 메시지 출력
    printf("입력한 byte(s)는 %dbyte(s)입니다.\n", strsize-1);
    printf("입력 최대 길이 %dbyte(s)보다 %dbyes(s)가 초과됐습니다.\n", define_size-1, strsize-define_size);
    printf("입력할 데이터를 다시 확인해보세요.\n");
}
