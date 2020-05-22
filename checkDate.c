//#define _CTR_SECURE_NO_WARNINGS
/*
input birth (yyyymmdd 8개 유효 날짜(숫자)가 아니면 다시 입력함):
shjfhjk
66번째 줄...
67번째 줄...
str문자열은 shjfhjk이고, str의 크기는 8입니다.
85번째 줄...
yyyymmdd 8개의 유효 날짜(숫자)로 입력해야 합니다!
86번째 줄...
input birth (yyyymmdd 8개 유효 날짜(숫자)가 아니면 다시 입력함):
1234567890
66번째 줄...
67번째 줄...
str문자열은 1234567890이고, str의 크기는 11입니다.
85번째 줄...
yyyymmdd 8개의 유효 날짜(숫자)로 입력해야 합니다!
86번째 줄...
input birth (yyyymmdd 8개 유효 날짜(숫자)가 아니면 다시 입력함):
20010568
66번째 줄...
67번째 줄...
str문자열은 20010568이고, str의 크기는 9입니다.
68번째 줄...
69번째 줄...
70번째 줄...
71번째 줄...

2001년 5월 68일
75번째 줄...
유효 날짜가 아닙니다!!!
76번째 줄...
70번째 줄...
71번째 줄...

2001년 5월 68일
75번째 줄...
유효 날짜가 아닙니다!!!
76번째 줄...
70번째 줄...
71번째 줄...

2001년 5월 68일
75번째 줄...
유효 날짜가 아닙니다!!!
76번째 줄...
70번째 줄...
71번째 줄...

2001년 5월 68일
75번째 줄...
유효 날짜가 아닙니다!!!
76번째 줄...
70번째 줄...
71번째 줄...

2001년 5월 68일
75번째 줄...
유효 날짜가 아닙니다!!!
76번째 줄...
70번째 줄...
71번째 줄...

2001년 5월 68일
75번째 줄...
유효 날짜가 아닙니다!!!
76번째 줄...
70번째 줄...
71번째 줄...

2001년 5월 68일
75번째 줄...
유효 날짜가 아닙니다!!!
76번째 줄...
70번째 줄...
71번째 줄...

2001년 5월 68일
75번째 줄...
유효 날짜가 아닙니다!!!
76번째 줄...
input birth (yyyymmdd 8개 유효 날짜(숫자)가 아니면 다시 입력함):
20010501
66번째 줄...
67번째 줄...
str문자열은 20010501이고, str의 크기는 9입니다.
68번째 줄...
69번째 줄...
70번째 줄...
71번째 줄...

2001년 5월 1일
72번째 줄...
input birth (yyyymmdd 8개 유효 날짜(숫자)가 아니면 다시 입력함):

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define IDlen 9 
#define MAX 1024 // 입력받는 임시 문자열

void printBOF_gets(char str[], int strsize, int define_size) { 
    printf("입력한 byte(s)는 %dbyte(s)입니다.\n", strsize-1);
    printf("입력 가능한 최대 길이 %dbyte(s)보다 %dbyte(s)가 초과됐습니다.\n", define_size-1, strsize-define_size);
    printf("입력할 데이터를 다시 확인해보세요.\n");
}

int checkDate(int date) { // 입력받은 날짜의 유효성 검사-> 1이면 유효한 날짜임. (0이면 없는 날짜임) 
    int yyyy, mm, dd = 0;
	
	yyyy = date / 10000;
    mm = (date-(yyyy*10000)) / 100;
	dd = date % 100 ;
	printf("\n%d년 %d월 %d일\n", yyyy, mm, dd);
	
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

void main() {
	int date = 0;	
	char str[MAX] = { 0, };
	char id[IDlen] = { 0, };
	int strsize = 0;
	
	while(1) {
    	printf("Input the date (yyyymmdd): ");   
    	scanf("%d", &date);
    		
    	//scanf("%hhd", str);

    	sprintf(str, "%d", date);
    	strsize = strlen(str); //출력됨 
    	printf("%d\n", date); //출력됨 
    	printf("%s\n", str); // 출력됨 
    	printf("%d\n", strsize);
    
    	if(strsize == (IDlen-1)) {
			if(checkDate(date) == 1) // 0이면 거짓
				break;
			else
				printf("날짜를 숫자로만 정확히 입력하세요!!!\n");
		}
		if(strsize >= IDlen)
			printBOF_gets(str, (strsize+1), IDlen);
		if(strsize > IDlen)
			printf("8자리 숫자로 정확히 입력하세요!\n");
	}
	strncpy(id, str, IDlen-1);
}
