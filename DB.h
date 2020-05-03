#define WLlen 31
#define IDlen 10
#define DATElen 20
#define PKlen 271
#define MAClen 18
#define PWDlen 513
#define NAMElen 30
#define EMAILlen 50
#define PHONElen 20
#define BIRTHlen 7

struct WhiteListTable {
    char whitelist[WLlen];
    char id[IDlen];
    char date[DATElen];
};

struct MACTable {
    char id[IDlen];
    char public_key[PKlen];
    char mac0[MAClen];
    char mac1[MAClen];
    char mac2[MAClen];
};

struct AdminTable {
    char idp[IDlen];
    int access;
    char pwd[PWDlen];
};

struct InfoTable {
    char id[IDlen];
    char name[NAMElen];
    char birth[BIRTHlen];
    char email[EMAILlen];
    char phone[PHONElen];
    char date[DATElen];
};

struct InsInfo {
    char id[IDlen];
    char pwd[PWDlen];
    char public_key[PKlen];
    char mac[MAClen];
    char name[NAMElen];
    char birth[BIRTHlen];
    char email[EMAILlen];
    char phone[PHONElen];
    char date[DATElen];
    int access;
};

enum Account {
    Robot, CPS, Root
};
