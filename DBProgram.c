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

int selMenu() {
    createDB(); // DB 와 Table 생성 함수 // DBCreate.c
    if(login() == 1) { // DB 로그인 : id & pwd로 확인
        printf("\n");
        printMenu(); // DBPrintModule.c

    //system("echo '.dump' | sqlite CPS.db");
    }

        return 0;
}
//system("sqlite3 ex.db.dump > ex.sql");
//system("sqlite3 ex.db < ex.sql");
