#ifndef DBBackupR_H
#define DBBackupR_H

static int callback(void *NotUsed, int argc, char **argv, char **azColName);
int findIP(); // case 56 : id, pwd 분실 시 지원 기능
int RestoreB(); // case 57 : DB 복원

#endif
