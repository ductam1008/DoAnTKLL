#include "main.h"
#include "string.h"
// Noi khai bao hang so
#define     LED     PORTD
#define     ON      1
#define     OFF     0

#define     INIT_SYSTEM             0
#define     ENTER_PASSWORD_USER     1
#define     CHECK_PASSWORD_USER     2
#define     UNLOCK_DOOR             3
#define     WRONG_PASSWORD          4
#define     OPTION_MODE             5
#define     USER_MODE               6
#define     ADMIN_MODE              7
#define     USER_CHOOSE             8
#define     ADMIN_CHOOSE            9
#define     CHANGE_PASSWORD_USER    10
#define     CHANGE_PASSWORD_ADMIN   11
#define     ENTER_PASSWORD_ADMIN    12
#define     CHECK_PASSWORD_ADMIN    13
#define     APPLY_NEW_PASS_ADMIN    14
#define     APPLY_NEW_PASS_USER     15
// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput [8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
unsigned char statusOutput[8] = {0, 0, 0, 0, 0, 0, 0, 0};
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);
void Test_KeyMatrix();
//Chuong trinh Password Door
#define PASSWORD_LENGTH 5
#define MAX_ACCOUNT     5
unsigned char arrayMapOfNumber [16] = {1, 2, 3, 'A', 4, 5, 6, 'B',
    7, 8, 9, 'C', '*', 0, 'E', 'D'};
unsigned char arrayMapOfPasswordU [5][4] = {
    {1, 2, 3, 4},
    {2, 7, 8, 9},
    {3, 3, 3, 3},
    {4, 8, 6, 8},
    {5, 6, 7, 8},
};
unsigned char arrayMapOfPasswordA [5][5]= {
    {1, 2, 3, 4, 'A'},
    {2, 7, 8, 9, 'B'},
    {3, 3, 3, 3, 'C'},
    {4, 8, 6, 8, 'D'},
    {5, 6, 7, 8, 'A'},
};

typedef struct user_account
{
    unsigned int ID_U;
    unsigned char passwordU[4];
} user_account;
user_account accountU[MAX_ACCOUNT]={
    {0, {1, 2, 3, 4}},
    {1, {2, 7, 8, 9}},
    {2, {3, 3, 3, 3}},
    {3, {4, 8, 6, 8}},
    {4, {5, 6, 7, 8}}
};

typedef struct admin_account
{
    unsigned int ID_A;
    unsigned char passwordA[5];
} admin_account;
admin_account accountA[MAX_ACCOUNT]={
    {0, {1, 2, 3, 4, 'A'}},
    {1, {2, 7, 8, 9, 'B'}},
    {2, {3, 3, 3, 3, 'C'}},
    {3, {4, 8, 6, 8, 'D'}},
    {4, {5, 6, 7, 8, 'A'}}
};

unsigned char arrayPasswordU[4];
unsigned char arrayPasswordA[5];
unsigned char statusPassword = INIT_SYSTEM;
unsigned char indexOfNumber = 0;
unsigned char timeDelay = 0;

unsigned int current_user =0;
unsigned int current_admin =0;

void App_PasswordDoor();
unsigned char CheckPasswordUser();
unsigned char CheckPasswordAdmin();
unsigned char isButtonNumber();
unsigned char numberValue;
unsigned char isButtonEnter();
void UnlockDoor();
void LockDoor();
unsigned char isButtonBack();
unsigned char isButtonPressOne();
unsigned char isButtonPressTwo();
void reset_data();

#define     LIGHT_ON      1
#define     LIGHT_OFF     0
void BaiTap_Den();
unsigned char isButtonLight();
unsigned char statusLight = LIGHT_OFF;
void LightOn();
void LightOff();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////

void main(void) {
    unsigned int k = 0;
    init_system();
    //TestOutput();
    while (1) {
        while (!flag_timer3);
        flag_timer3 = 0;
        // thuc hien cac cong viec duoi day
        scan_key_matrix();
        App_PasswordDoor();
        DisplayLcdScreen();
    }
}
// Hien thuc cac module co ban cua chuong trinh

void delay_ms(int value) {
    int i, j;
    for (i = 0; i < value; i++)
        for (j = 0; j < 238; j++);
}

void init_system(void) {
    TRISB = 0x00; //setup PORTB is output
    TRISD = 0x00;
    init_lcd();
    LcdClearS();
    //        LED = 0x00;
    init_interrupt();
    delay_ms(1000);
    init_timer0(4695); //dinh thoi 1ms sai so 1%
    init_timer1(9390); //dinh thoi 2ms
    init_timer3(46950); //dinh thoi 10ms
    SetTimer0_ms(2);
    SetTimer1_ms(10);
    SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
    init_key_matrix();
}

void OpenOutput(int index) {
    if (index >= 0 && index <= 7) {
        LED = LED | arrayMapOfOutput[index];
    }

}

void CloseOutput(int index) {
    if (index >= 0 && index <= 7) {
        LED = LED & ~arrayMapOfOutput[index];
    }
}

void ReverseOutput(int index) {
    if (statusOutput[index] == ON) {
        CloseOutput(index);
        statusOutput[index] = OFF;
    } else {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}

void TestOutput(void) {
    int k;
    for (k = 0; k <= 7; k++) {
        OpenOutput(k);
        delay_ms(500);
        CloseOutput(k);
        delay_ms(500);
    }
}

unsigned char isButtonMotorOn() {
    if (key_code[2] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonMotorOff() {
    if (key_code[3] == 1)
        return 1;
    else
        return 0;
}

void App_PasswordDoor() {
    switch (statusPassword) {
        case INIT_SYSTEM:
            LcdPrintStringS(0, 0, "HELLO !!!");
            LcdPrintStringS(1, 0, "PRESS # TO NEXT");
            LockDoor();
            if (isButtonEnter()) {
                indexOfNumber = 0;
                timeDelay = 0;
                statusPassword = OPTION_MODE;
                LcdClearS();
            }
            break;
        case OPTION_MODE:
            LcdPrintStringS(0, 0, "1: USER MODE");
            LcdPrintStringS(1, 0, "2: ADMIN MODE");
            timeDelay++;            
            if (isButtonPressOne()) {
                indexOfNumber++;
                timeDelay = 0;
                statusPassword = ENTER_PASSWORD_USER;
                LcdClearS();
            }
            if (isButtonPressTwo()) {
                indexOfNumber = 0;
                timeDelay = 0;
                statusPassword = ENTER_PASSWORD_ADMIN;
                LcdClearS();
            }
            if (timeDelay>=100)
                statusPassword=INIT_SYSTEM;
            break;            
        case ENTER_PASSWORD_USER:
            LcdPrintStringS(0, 0, "1:ENTER PW USER:");
            timeDelay++;
            if (isButtonNumber())
            {
                LcdPrintStringS(1, indexOfNumber, "*");
                arrayPasswordU[indexOfNumber] = numberValue;
                indexOfNumber++;
                timeDelay = 0;
            }
            if (indexOfNumber >= 4)            
                statusPassword = CHECK_PASSWORD_USER;
            if (timeDelay >= 100)
                statusPassword = INIT_SYSTEM;
            if (isButtonEnter())
                statusPassword = INIT_SYSTEM;
            break;
        case ENTER_PASSWORD_ADMIN:
             LcdPrintStringS(0, 0, "1: ENTER PW ADMIN:    ");
             timeDelay++;
             if (isButtonNumber())
            {
                LcdPrintStringS(1, indexOfNumber, "*");
                arrayPasswordA[indexOfNumber] = numberValue;
                indexOfNumber++;
                timeDelay = 0;
            }
             if (indexOfNumber >= 5)
                statusPassword = CHECK_PASSWORD_ADMIN;
            if (timeDelay >= 100)
                statusPassword = INIT_SYSTEM;
            if (isButtonEnter())
                statusPassword = INIT_SYSTEM;
             break;
            
        case USER_MODE:
            LcdPrintStringS(0, 0, "1: CHANGE PW     ");
            LcdPrintStringS(1, 0, "2: OPEN DOOR     ");
            if(isButtonPressOne()){
                indexOfNumber++;
                timeDelay = 0;
                statusPassword = CHANGE_PASSWORD_USER;
                LcdClearS();
            }
            if(isButtonPressTwo()){
                indexOfNumber++;
                timeDelay = 0;
                statusPassword = UNLOCK_DOOR;
                LcdClearS();
            }
            break;
        case CHECK_PASSWORD_USER:
            timeDelay = 0;
            if (CheckPasswordUser())
                statusPassword = USER_MODE;
            else
                statusPassword = WRONG_PASSWORD;
            break;
        case CHANGE_PASSWORD_USER:
            break;
        case ADMIN_MODE:
            LcdPrintStringS(0, 0, "1: CHANGE PW     ");
            LcdPrintStringS(1, 0, "2: OPEN DOOR    ");
            if(isButtonPressOne()){
                indexOfNumber++;
                timeDelay = 0;
                statusPassword = CHANGE_PASSWORD_ADMIN;
                LcdClearS();
            }
            if(isButtonPressTwo()){
                indexOfNumber++;
                timeDelay = 0;
                statusPassword = UNLOCK_DOOR;
                LcdClearS();
            }
            break;
        case CHECK_PASSWORD_ADMIN:
            timeDelay = 0;
            if (CheckPasswordAdmin())
                statusPassword = ADMIN_MODE;
            else
                statusPassword = WRONG_PASSWORD;
            break;
        case CHANGE_PASSWORD_ADMIN:
            timeDelay++;
            LcdPrintStringS(0, 0, "ENTER NEW PW");
            if(isButtonNumber()) {
                if (numberValue == '*') {
                    if (indexOfNumber > 0) {
                        indexOfNumber--;
                        LcdPrintStringS(1, indexOfNumber, " ");
                    }
                }
                else if (numberValue < 10) {
                    if (indexOfNumber < 5) {
                        LcdPrintStringS(1, indexOfNumber, "*");
                        arrayPasswordA[indexOfNumber] = numberValue;
                        indexOfNumber++;
                    }
                }
                timeDelay =0;
            }
            if (isButtonEnter() && indexOfNumber >= 5){
                statusPassword = APPLY_NEW_PASS_ADMIN;
                reset_data();
            }
            if (timeDelay >= 100)
                statusPassword = INIT_SYSTEM;
            break;
        case APPLY_NEW_PASS_ADMIN:
            timeDelay++;
            LcdPrintStringS(0, 0, "CHANGE SUCCESS");
            if(timeDelay==1) {
                memmove(accountA[current_admin].passwordA, arrayPasswordA, sizeof(arrayPasswordA));
            }
            if (timeDelay >40) {
                statusPassword = INIT_SYSTEM;
            }
            break;
        case UNLOCK_DOOR:
            timeDelay++;
            LcdPrintStringS(0, 0, "OPENING DOOR    ");
            UnlockDoor();
            if (timeDelay >= 100)
                statusPassword = INIT_SYSTEM;
            break;
        case WRONG_PASSWORD:
            timeDelay++;
            LcdPrintStringS(0, 0, "PASSWORD WRONG  ");
            if (timeDelay >= 40)
                statusPassword = INIT_SYSTEM;
            break;
        default:
            break;
    }
}

unsigned char CheckPasswordUser()
{
     unsigned char i,j;
    for (i=0; i<4;i++) {
        if (accountU[current_user].passwordU[i] != arrayPasswordU[i]) {
            return 0;
        }
    }
    return 1;

}
unsigned char CheckPasswordAdmin()
{
    unsigned char i,j;
    for (i=0; i<5;i++) {
        if (accountA[current_admin].passwordA[i] != arrayPasswordA[i]) {
            return 0;
        }
    }
    return 1;

}

unsigned char isButtonNumber() {
    unsigned char i;
    for (i = 0; i <= 15; i++)
        if (key_code[i] == 1) {
            numberValue = arrayMapOfNumber[i];
            return 1;
        }
    return 0;
}
//press "1" to next state
unsigned char isButtonPressOne() {
    if (key_code[0] == 1)
        return 1;
    else
        return 0;
}

//press "2" to next state
unsigned char isButtonPressTwo() {
    if (key_code[1] == 1)
        return 1;
    else
        return 0;
}

//press "*" to BACK
unsigned char isButtonBack() {
    if (key_code[12] == 1)
        return 1;
    else
        return 0;
}

//press "#" to OK
unsigned char isButtonEnter() {
    if (key_code[14] == 1)
        return 1;
    else
        return 0;
}

void reset_data() {
    LcdClearS();
    timeDelay = 0;
    indexOfNumber =0;
}

void UnlockDoor() {
    OpenOutput(0);
    //OpenOutput(4);
}

void LockDoor() {
    CloseOutput(0);
    //CloseOutput(4);
}
