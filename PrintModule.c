#include <stdio.h>
#include "DB.h"

void printMenu(int pr_menu) {
    int tb = 0;
    int func = 0;
    int menu = 0;

    char tmp; // 엔터키 삭제 변수

    printf("Select Table Number\n");
    printf("1 : WHITELIST DB WHITELIST(whitelist, id) Table\n");
    printf("2 : ADMINISTRATOR DB MAC(public_key, mac) Table\n");
    printf("3 : ADMINISTRATOR DB ADMIN(id, access, pwd) Table\n");
    printf("4 : ADMINISTRATOR DB INFO(name, birth, email, phone) Table\n");
    printf("0 : EXIT\n");
    printf("Table :");
    scanf("%d", &tb);

    printf("\nSelect Number\n");
    printf("6 : insert\n");
    printf("7 : update\n");
    printf("8 : delete\n");
    printf("9 : search\n");
    printf("0 : EXIT\n");
    printf("menu : ");
    scanf("%d", &func);
    menu =  tb*10 + func;

    while((tmp = getchar()) != '\n') { //엔터키 삭제 함수
        putchar(tmp);
    }

    switch(menu) {
        case 16: // ok
            insWhiteListTable();
            break;

        case 17: // ok
            updateAdminwl();
            break;

        case 18: // ok
            delWhiteList();
            break;

        case 19: // ok
            selWhitelist();
            break;

        case 26:
            insMAC();
            break;

        case 27:
            updateMac();
            break;

        case 28:
            delAdmin();
            break;

        case 29:
            selAdminInfo();
            break;

        case 36: // ok
            //insAdminInfo();
            insAdminTable();
            break;

        case 37: // ok
            //updateAdmin();
            updateAdminTable();
            break;

        case 38: // ok
            //delAdmin();
            delAdminTable();
            break;

        case 39: // ok
            //selAdminInfo();
            selAdminTable();
            break;

        case 46: // ok
            //insAdminInfo();
            insInfoTable();
            break;

        case 47: // ok
            //updateAdmin();
            updateInfoTable();
            break;

        case 48: // ok
            //delAdmin();
            delInfoTable();
            break;

        case 49: // ok
            //selAdminInfo();
            selInfoTable();
            break;

        default :
            break;
    }

}

void printWhiteListResult(struct WhiteListTable pr_wt) {
    struct WhiteListTable wt;
}

void printAdminResult(struct InsInfo pr_i) {
    struct InsInfo i;
}
