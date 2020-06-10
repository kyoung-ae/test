#ifndef DB_H
#define DB_H

// #define len 크기 지정은 실제크기+1 로 지정
#define SQLlen 800 // 입력 길이 제한이 가장 긴 PWDlen(514) 를 포함하는 admin 테이블을 기준으로 설정함
#define ACCESSlen 2 // int형의 access값을 문자로 받기 위한 변수 크기 -> 크기 1로 했더니 id[0]을 null로 덮어쓰는 참사 발생ㅠㅠ
#define WLlen 31
#define IDlen 10
#define DATElen 21
#define PWDlen 514
#define NAMElen 31
#define EMAILlen 51
#define PHONElen 21
#define BIRTHlen 9
#define MAX 1024 // 입력받는 임시 문자열

#endif
