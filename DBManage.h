#ifndef DBManage_H
#define DBManage_H

static int callback(void *NotUsed, int argc, char **argv, char **azColName); // callback
void removeEnter(); // scanf()의 Enterkey 제거
void removeSpace(char str_col[]); // str_col 문자열의 공백 제거
char *dateNow(struct tm *t); // date 가져오는 함수
int checkDate(int date); // 입력받은 날짜의 유효성 검사-> 1이면 유효한 날짜 (0이면 없는 날짜임)
int checkIDPWD(char id[]); // 호출 직전에 입력한 id를 매개변수로 받아서 수정이나 삭제 시 id&pwd로 확인 (성공하면 1을 반환)
int checkID(char id[]); // 입력한 id를 매개변수로 받아서 등록된 id인지 확인 (성공하면 1을 반환)
int checkWL(char whitelist[]); // 입력한 whitelist를 매개변수로 받아서 등록된 whitelist인지 확인 (성공하면 1을 반환)
int checkEmail(char email[]); // 입력한 email을 매개변수로 받아 등록된 email인지 확인 (성공하면 1반환)
int checkFunc(char id[]); // pwd 확인을 5회로 제한
void checkRep(char id[], char play[]); // 작업 진행 여부 확인 루프
int checkAg(char play[]); // 동일 작업 반복 여부

#endif
