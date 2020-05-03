/*
 * 최초 작성자	:	이준영
 * 최초 작성일	:	2019.12.28
 * 최종 변경일	:	2020.03.22
 * 목적			:	자료형 변수 및 기본적 함수 반환값 일치화
 * 개정 이력	:	이준영, 2019.12.28.(ver. 01)
					이준영, 2019.12.28.(ver. 02)	:	경고 및 에러, 실패, 성공 시의 반환값 일치화
					김경애, 2019.12.28.(ver. 03)	:	경고 주석 관리
					이준영, 2019.12.28.(ver. 04)	:	전체적 주석 관리
					이준영, 2020.02.02.(ver. 05)	:	time.h 추가
					이준영, 2020.02.06.(ver. 06)	:	windows에서의 재 정의 오류 해결
					이준영, 2020.02.07.(ver. 07)	:	false 및 true 정의
					이준영, 2020.02.10.(ver. 08)	:	stdio.h 및 string.h 추가
					이준영, 2020.02.15.(ver. 09)	:	버퍼 사이즈 정의
					이준영, 2020.02.17.(ver. 10)	:	매개변수의 IN, OUT 정의
					이준영, 2020.02.20.(ver. 11)	:	재 정의된 자료형으로 변환 및 길이 관련 정의 변경
					이준영, 2020.02.20.(ver. 12)	:	BITSTOBYTE 정의 위치 변경 및 HEXNUMBITS, HEXCOUNT, CHARBITS 새롭게 정의
					이준영, 2020.03.15.(ver. 13)	:	비대칭키 암호화에서 필요한 PADDING 길이 정보 추가
					이준영, 2020.03.22.(ver. 14)	:	BaseDefine.h파일 정리(FAIL과 ERROR값 통일)
					이준영, 2020.03.24.(ver. 15)	:	Linux에서 헤더파일 관련 에러 처리
 * 저작권		:	White
 */

#ifndef _BASE_DEFINE_H_
#define _BASE_DEFINE_H_

#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#ifndef DATA_TYPE
#define DATA_TYPE
typedef int8_t						S_CHAR;		//signed char, 1Byte
typedef int16_t						S_SHORT;	//signed short,2Byte
typedef int32_t						S_INT;		//signed int,  4Byte
typedef int64_t						S_LONG;		//signed long long, 8Byte
typedef uint8_t						U_CHAR;		//unsigned char, 1Byte
typedef uint16_t					U_SHORT;	//unsigned short,2Byte
typedef uint32_t					U_INT;		//unsigned int, 4Byte
typedef uint64_t					U_LONG;		//unsigned long,8Byte

#define U_CHAR_MAX					(0xff)					//	unsigned char type의 최대 크기
#define U_SHORT_MAX					(0xffff)				//	unsigned short type의 최대 크기
#define U_INT_MAX					(0xffffffff)			//	unsigned int type의 최대 크기
#define U_LONG_MAX					(0xffffffffffffffff)	//	unsigned long type의 최대 크기
#define S_CHAR_MAX					((U_CHAR_MAX)	>> 1)	//	signed char type의 최대 크기
#define S_SHORT_MAX					((U_SHORT_MAX) >> 1)	//	signed short type의 최대 크기
#define S_INT_MAX					((U_INT_MAX) >> 1)		//	signed int type의 최대 크기
#define S_LONG_MAX					((U_LONG_MAX) >> 1)		//	signed long type의 최대 크기
#endif // !DATA_TYPE

#ifndef SUCCESS
#define SUCCESS						(1)						//	함수내의 모든 것을 정상적으로 실행했을 경우
#endif // !SUCCESS

#ifndef ERROR
#define ERROR						(0)
#endif // !ERROR

#ifndef WARNINGS
#define WARNINGS					(((ERROR)) - (1))		//	문제가 발생하였으나 강제종료가 필요 없을 경우
#endif // !WARNINGS

#ifndef FAIL
#define FAIL						((ERROR))				//	함수내에서 문제가 발생하였고 강제종료가 필요한 경우
#endif // !FAIL

#ifndef _MINWINDEF_
#ifndef TURE_AND_FALSE
#define TRUE_AND_FALSE
#define TRUE						(1)						//	같은(진실)
#define FALSE						((TRUE) - (1))			//	다른(거짓)
#endif
#endif // !_MINWINDEF_

#ifndef BITS_TO_CHAR
#define BITS_TO_CHAR
#define BITS_TO_BYTE				(8)						//	1byte 당 bit 수
#define HEX_NUM_BITS				(4)						//	hex값을 표현하기 위해서 사용되는 bit 수
#define HEX_COUNT					((BITS_TO_BYTE) / (HEX_NUM_BITS))	//	1byte당 표현되는 hex의 자릿수
#define CHAR_BITS					((sizeof(S_CHAR)) * (HEX_COUNT))	//	1 S_CHAR당 표현되는 hex의 자릿수
#endif // !BITS_TO_CHAR

#ifndef MESSAGE_LEN
#define MESSAGE_LEN
#define ASYM_PADDING_LEN			(11)					// PADDING 길이
#define CRYPTO_MESSAGE_LEN			(256)					//	암호화된 메세지의 길이
#define PLAIN_TEXT_MESSAGE_LEN		(((CRYPTO_MESSAGE_LEN) - (ASYM_PADDING_LEN)) + 1)	//	암호화가 가능한 평문의 메세지 최대 길이
#define PLAIN_TEXT_MESSAGE_LEN_HALF	((((PLAIN_TEXT_MESSAGE_LEN) % 2) == 0) ? (((PLAIN_TEXT_MESSAGE_LEN) >> 1) - 1) : (((PLAIN_TEXT_MESSAGE_LEN) - 1) >> 1))	//	암호화가 가능한 평문의 메세지 최대 길이
#define SEND_MESSAGE_LEN			(((CRYPTO_MESSAGE_LEN) << 2) + 1)	//	전송되는 메세지의 최대 크기
#endif // !MESSAGE_LEN

#define IN							const					//	입력받는 매개변수에 사용
#define OUT													//	출력받는 매개변수에 사용

#endif // !_BASE_DEFINE_H_

