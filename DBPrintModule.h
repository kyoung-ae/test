#ifndef DBPrintModule_H
#define DBPrintModule_H

void print_login(); // DB 로그인 메뉴
void print_pwd(); // pwd 확인 항목
void printMenu(); // DB 시작 메뉴
void print_search_i(); // id를 기준으로 검색
void print_search_sort(); // id 기준으로  whitelist 정렬 검색
void print_search_w(); // whitelist를 기준으로 검색
void print_del_col(); // 컬럼만 삭제
void print_del_row(); // 레코드 삭제
void printBOF_gets(char str[], int strsize, int define_size); // gets() 버퍼오버플로우 방지 : 재입력 요구 메시지 출력

#endif
