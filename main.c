#include "main.h"
#include <ctype.h>
#include <string.h>
// Noi khai bao hang so
#define     LED     PORTD
#define     ON      1
#define     OFF     0

#define     INIT_SYSTEM             0
#define     ENTER_PASSWORD          1
#define     CHECK_PASSWORD          2
#define     UNLOCK_DOOR             3
#define     WRONG_PASSWORD          4
#define     USER_MODE               6
#define     ADMIN_MODE              7
#define     CHANGE_PASSWORD_USER    10
#define     CHANGE_PASSWORD_ADMIN   11
#define     ENTER_STT               16
#define     CHECK_STT               17
#define     WRONG_STT               18
#define     ACCEPT_NEW_PW_USER      19
#define     ACCEPT_NEW_PW_ADMIN     12
#define     ADMIN_OPTIONS           20
#define     MANAGE_MEMBER           21
#define     ADMIN_ADD_MEMBER        22
#define     ADMIN_REMOVE_MEMBER     23
#define     CREATE_NEW_MEMBER       24
#define     NEW_PW_ADMIN_INVALID    25
#define     NEW_PW_USER_INVALID     26
#define     CHANGE_PW_USER_AGAIN    27
#define     CHANGE_PW_ADMIN_AGAIN   28
#define     CONFIRM_REMOVE          29
#define     REMOVE_COMPLETE         30
// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
unsigned char statusOutput[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);
void Test_KeyMatrix();
//Chuong trinh Password Door
#define PASSWORD_LENGTH 6
#define MAX_ACCOUNT     10
#define ERROR_VALID     0xffff
#define MAX_STT_LENGTH  3
#define CHAR_ERROR_RETURN 0xf
unsigned char arrayMapOfNumber[16] = { 1, 2, 3, 'A', 4, 5, 6, 'B',
    7, 8, 9, 'C', '*', 0, 'E', 'D' };
unsigned char arrayMapOfPassword[5][PASSWORD_LENGTH] = {
    {1, 2, 3, 4, 5, 6},
    {1, 7, 8, 9, 9, 7},
    {0, 3, 3, 3, 3, 8},
    {0, 8, 6, 8, 6, 9},
    {0, 6, 7, 8, 9, 0},
};

//char*str1="OK1";
//char str2[] = "OK2";

typedef struct user_account{
    unsigned int STT;
    unsigned char password[PASSWORD_LENGTH];
    char* name;
} user_account;
user_account account[MAX_ACCOUNT] = {
    {0, {1, 2, 3, 4, 5, 6}, "TAM"},
    {1, {1, 7, 8, 9, 9, 7}, "TA1"},
    {2, {0, 3, 3, 3, 3, 8}, "TA2"},
    {3, {0, 8, 6, 8, 6, 9}, "TA3"},
    {4, {0, 6, 7, 8, 9, 0}, "TA4"}
};

unsigned char arrayPassword[PASSWORD_LENGTH];
unsigned int STT_value = 0;

unsigned char statusPassword = INIT_SYSTEM;

unsigned char indexOfNumber = 0;
unsigned char indexOfSTT =0;
unsigned char timeDelay = 0;

unsigned int current_user = 0;
unsigned int num_of_user = 5;
unsigned int index_user = 5;

unsigned char ad_num_member_list = 0;
unsigned char ad_cur_mem_list = 0;
unsigned char ad_current_member = 0;

void App_PasswordDoor();
unsigned char CheckPassword();
unsigned char isModeOn();
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
        //Test_KeyMatrix();
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
    }
    else {
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


/*
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
*/

//Check STT account
unsigned int CheckSTT (unsigned int stt) {
    int i = 0;
    for ( i = 0; i < num_of_user; i++) {
        if (account[i].STT == stt) return i;
    }
    return ERROR_VALID;
}



void App_PasswordDoor() {
    switch (statusPassword) {
    case INIT_SYSTEM:
        LcdPrintStringS(0, 0, "PRESS C TO");
        LcdPrintStringS(1, 0, "ENTER PW");
        LockDoor();
        if (isButtonEnter()) {
            indexOfNumber = 0;
            timeDelay = 0;
            STT_value = 0;
            statusPassword = ENTER_STT;
            LcdClearS();
        }
        break;
    case ENTER_STT:
        timeDelay++;
        LcdPrintStringS(0, 0, "ENTER STT:");
        if (isButtonNumber()) {
            if (numberValue == 'D') {
                if (indexOfSTT>0) {
                    STT_value = STT_value / 10;
                    indexOfSTT --;
                }
            }
            else if (numberValue <10) {
                LcdPrintStringS(1, indexOfSTT, "*");
                STT_value = STT_value * 10 + numberValue;
                indexOfSTT++;
            }
            timeDelay = 0;
        }
        if (indexOfSTT >= MAX_STT_LENGTH || isButtonEnter()) {
            reset_data();
            statusPassword = CHECK_STT;
            
        }
        if (isButtonBack()) {
            statusPassword = INIT_SYSTEM;
            LcdClearS();
        }
        if (timeDelay >= 600) {
            statusPassword = INIT_SYSTEM;
            LcdClearS();
        }
        break;
    case CHECK_STT:
        timeDelay=0;
        current_user = CheckSTT(STT_value);
        if (current_user != ERROR_VALID) {
            statusPassword = ENTER_PASSWORD;
            LcdClearS();
        }
        else {
            statusPassword = WRONG_STT;
            LcdClearS();
        }
        break;
    case WRONG_STT:
        timeDelay++;
        LcdPrintStringS(0,0,"WRONG STT");
        if (timeDelay>= 60) {
            statusPassword = INIT_SYSTEM;
            LcdClearS();
        }
        break;
    case ENTER_PASSWORD:
        LcdPrintStringS(0, 0, "ENTER PW");
        LcdPrintNumS(0,10,account[current_user].STT);
        LcdPrintStringS(0, 11, ":");
        LcdPrintStringS(0, 12, account[current_user].name);
        timeDelay++;
        if(isButtonNumber()) {
            if (numberValue == 'D') {
                if (indexOfNumber > 0) {
                    indexOfNumber--;
                    LcdPrintStringS(1, indexOfNumber, " ");
                }
            }
            else if(numberValue < 10){
                LcdPrintStringS(1, indexOfNumber, "*");
                arrayPassword[indexOfNumber] = numberValue;
                indexOfNumber++;
            }
            timeDelay = 0;
        }
        if (indexOfNumber >= PASSWORD_LENGTH){
            statusPassword = CHECK_PASSWORD;
            LcdClearS();
        }
        if (timeDelay >=600) {
            statusPassword = INIT_SYSTEM;
            LcdClearS();
        }
        if (isButtonBack()) {
            statusPassword = INIT_SYSTEM;
            LcdClearS();
        }
        break;
    case CHECK_PASSWORD:
        timeDelay = 0;
        if (CheckPassword()) {
            
            if (account[current_user].password[0] == 0) {
                statusPassword = USER_MODE;
                LcdClearS();
            }
            else {
                if(account[current_user].password[0] == 1) {
                    statusPassword = ADMIN_MODE;
                    LcdClearS();
                }
            }     
        }
        else {
            statusPassword = WRONG_PASSWORD;
        }
        break;   
    case USER_MODE:
        timeDelay++;
        LcdPrintStringS(0, 0, "1.OPEN DOOR");
        LcdPrintStringS(1, 0, "2.CHANGE PW");
        if (isButtonBack()) {
            statusPassword=INIT_SYSTEM;
        } 
        if (isButtonPressOne()){
            statusPassword = UNLOCK_DOOR;
            reset_data();
        }
        if (isButtonPressTwo()) {
            statusPassword = CHANGE_PASSWORD_USER;
            reset_data();
        }
        break;
    
    case CHANGE_PASSWORD_USER:
        timeDelay++;
        LcdPrintStringS(0, 0, "CHANGE USER PW");
        if (isButtonNumber()) {
            if (numberValue == 'D') {
                if (indexOfNumber > 0) {
                    indexOfNumber--;
                    LcdPrintStringS(1, indexOfNumber, " ");
                }
            }
            else if (numberValue < 10) {
                if (indexOfNumber < PASSWORD_LENGTH) {
                    LcdPrintStringS(1, indexOfNumber, "*");
                    arrayPassword[indexOfNumber] = numberValue;
                    indexOfNumber++;
                }
            }
            timeDelay = 0;
        }
        if (isButtonEnter() && indexOfNumber >= PASSWORD_LENGTH) {
            statusPassword = ACCEPT_NEW_PW_USER;
            reset_data();
        }   
        if (isButtonBack()) {
            statusPassword = USER_MODE;
            reset_data();
        }
        if(timeDelay>600) {
            statusPassword = INIT_SYSTEM;
        }
        break;
    case ACCEPT_NEW_PW_USER:
        timeDelay++;
        LcdPrintStringS(0,0, "CHECKING ...");
        if (timeDelay==1) {
            memmove(account[current_user].password, arrayPassword, sizeof(arrayPassword));            
        }
        if (timeDelay>50) {
            if(account[current_user].password[0]!=0) {
                statusPassword=NEW_PW_USER_INVALID;
                reset_data();
            }
            else {
                LcdPrintStringS(0,0, "CHANGE SUCCESS");
                if (isButtonEnter()) {
                    statusPassword = INIT_SYSTEM;
                    reset_data();
                }
            }
        }
        break;
    case NEW_PW_USER_INVALID:
        timeDelay++;
        LcdPrintStringS(0,0, "NEW PW INVALID");
        LcdPrintStringS(1,0, "1ST NUM IS ERROR");
        if (isButtonEnter()) {            
            statusPassword = CHANGE_PW_USER_AGAIN;
            reset_data();            
        }
        if (timeDelay > 600) {
            statusPassword = CHANGE_PW_USER_AGAIN;
            reset_data();
        }
        break;
    case CHANGE_PW_USER_AGAIN:
        timeDelay++;
        LcdPrintStringS(0,0, "1.CHANGE AGAIN");
        LcdPrintStringS(1,0, "1ST NUM IS 0");
        if (isButtonPressOne()) {
            statusPassword = CHANGE_PASSWORD_USER;
            reset_data();
        }
        break;
    case ADMIN_MODE:
        timeDelay++;
        LcdPrintStringS(0, 0, "1.OPEN DOOR");
        LcdPrintStringS(1, 0, "2.OPTIONS");
        if (isButtonBack()) {
            statusPassword=INIT_SYSTEM;
        } 
        if (isButtonPressOne()){
            statusPassword = UNLOCK_DOOR;
            reset_data();
        }
        if (isButtonPressTwo()) {
            statusPassword = ADMIN_OPTIONS;
            reset_data();
        }
        break;
    case ADMIN_OPTIONS:
        timeDelay++;
        LcdPrintStringS(0, 0, "1.CHANGE PW");
        LcdPrintStringS(1, 0, "2.MANAGE MEM");
        if (isButtonBack()) {
            statusPassword = ADMIN_MODE;
        } 
        if (isButtonPressOne()){
            statusPassword = CHANGE_PASSWORD_ADMIN;
            reset_data();
        }
        if (isButtonPressTwo()) {
            statusPassword = MANAGE_MEMBER;
            reset_data();
        }
        break;
    case MANAGE_MEMBER:
        timeDelay++;
        LcdPrintStringS(0, 0, "1.ADD MEM");
        LcdPrintStringS(1, 0, "2.DELETE MEM");
        if (isButtonBack()) {
            statusPassword = ADMIN_MODE;
            reset_data();
        } 
        if (isButtonPressOne()){
            statusPassword = ADMIN_ADD_MEMBER;
            reset_data();
        }
        if (isButtonPressTwo()) {
            statusPassword = ADMIN_REMOVE_MEMBER;
            reset_data();
        }
         break;          
    case CHANGE_PASSWORD_ADMIN:
        timeDelay++;
        LcdPrintStringS(0, 0, "CHANGE AD PW");
        if (isButtonNumber()) {
            if (numberValue == 'D') {
                if (indexOfNumber > 0) {
                    indexOfNumber--;
                    LcdPrintStringS(1, indexOfNumber, " ");
                }
            }
            else if (numberValue < 10) {
                if (indexOfNumber < PASSWORD_LENGTH) {
                    LcdPrintStringS(1, indexOfNumber, "*");
                    arrayPassword[indexOfNumber] = numberValue;
                    indexOfNumber++;
                }
            }
            timeDelay = 0;
        }
        if (isButtonEnter() && indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = ACCEPT_NEW_PW_ADMIN;
                reset_data();
        }   
        if (isButtonBack()) {
            statusPassword = ADMIN_MODE;
            reset_data();
        }
        if(timeDelay>600) {
            statusPassword = INIT_SYSTEM;
        }
        break;
    case ACCEPT_NEW_PW_ADMIN:
        timeDelay++;
        LcdPrintStringS(0,0, "CHECKING ...");
        if (timeDelay==1) {
            memmove(account[current_user].password, arrayPassword, sizeof(arrayPassword));            
        }
        if (timeDelay>60) {
            if(account[current_user].password[0]!=1) {
                statusPassword=NEW_PW_ADMIN_INVALID;
                reset_data();
            }
            else {
                LcdPrintStringS(0,0, "CHANGE SUCCESS");
                if (isButtonEnter()) {
                    statusPassword = INIT_SYSTEM;
                    reset_data();
                }
            }
        }
        break;
    case NEW_PW_ADMIN_INVALID:
        timeDelay++;
        LcdPrintStringS(0,0, "NEW PW INVALID");
        LcdPrintStringS(1,0, "1ST NUM IS ERROR");
        if (isButtonEnter()) {            
            statusPassword = CHANGE_PW_ADMIN_AGAIN;
            reset_data();            
        }
        if (timeDelay > 600) {
            statusPassword = CHANGE_PW_ADMIN_AGAIN;
            reset_data();
        }
        break;  
    case CHANGE_PW_ADMIN_AGAIN:
        timeDelay++;
        LcdPrintStringS(0,0, "1.CHANGE AGAIN");
        LcdPrintStringS(1,0, "1ST NUM IS 1");
        if (isButtonPressOne()) {
            statusPassword = CHANGE_PASSWORD_ADMIN;
            reset_data();
        }
        break;
    case ADMIN_ADD_MEMBER:
        timeDelay++;
        LcdPrintStringS(0, 0, "PW FOR MEMBER");
        LcdPrintNumS(0,14,index_user);
        if (isButtonNumber())
        {
            if (numberValue == 'D') {
                if (indexOfNumber > 0) {
                    indexOfNumber--;
                    LcdPrintStringS(1, indexOfNumber, " ");
                }
            }
            else if (numberValue < 10) {
                if (indexOfNumber < PASSWORD_LENGTH) {
                    LcdPrintStringS(1, indexOfNumber, "*");
                    arrayPassword[indexOfNumber] = numberValue;
                    indexOfNumber++;
                }
            }
            timeDelay = 0;
        }
        if (isButtonEnter() && indexOfNumber >= PASSWORD_LENGTH) {
            statusPassword = CREATE_NEW_MEMBER;
            reset_data();
        }
        if (isButtonBack()) {
            statusPassword = MANAGE_MEMBER;
            reset_data();
        }
        if (timeDelay > 600) {
            statusPassword = ADMIN_MODE;
            reset_data();
        }
        break;
    case CREATE_NEW_MEMBER:
        timeDelay++;
        LcdPrintStringS(0, 0, "NEW STT: ");
        LcdPrintStringS(1, 0, "PASSWORD: ");
        if (timeDelay ==1) {
        if (arrayPassword[0]==0 || arrayPassword[0]==1) {
            int i=0;
            account[num_of_user].STT = index_user;
            for (i =0; i<PASSWORD_LENGTH; i++) {
                account[num_of_user].password[i]=arrayPassword[i];
                LcdPrintCharS(1,10 +i, arrayPassword[i] +'0');
            }
            index_user++;
            num_of_user++;
            
        }
        else {
            statusPassword = MANAGE_MEMBER;
            reset_data();
            break;
        }
        }
        LcdPrintNumS(0, 9, index_user - 1);
        if(isButtonEnter()) {
            statusPassword = MANAGE_MEMBER;
            reset_data();
        }
        if(timeDelay > 400) {
            statusPassword = MANAGE_MEMBER;
            reset_data();
        }
        break;
    case ADMIN_REMOVE_MEMBER:
        timeDelay++;
       
        if ((ad_cur_mem_list * 2 + 0) < num_of_user) {
            int i=0;
            LcdPrintStringS(0, 0, "1. ");
            LcdPrintNumS(0, 3, account[ad_cur_mem_list * 2 + 0].STT);
            for (i =0; i<PASSWORD_LENGTH; i++) {
                arrayPassword[i]=account[ad_cur_mem_list * 2 + 0].password[i];
                LcdPrintCharS(1,0 +i, arrayPassword[i] +'0');
            }
            
        }
        if ((ad_cur_mem_list * 2 + 1) < num_of_user) {
            int i=0;
            LcdPrintStringS(0, 10, "2. ");
            LcdPrintNumS(0, 13, account[ad_cur_mem_list * 2 + 1].STT);
            for (i =0; i<PASSWORD_LENGTH; i++) {
                arrayPassword[i]=account[ad_cur_mem_list * 2 + 1].password[i];               
                LcdPrintCharS(1,10 +i, arrayPassword[i] +'0');
            }
            
        }
        if (isButtonEnter()) {
            ad_cur_mem_list = (ad_cur_mem_list + 1) % ad_num_member_list;
            LcdClearS();
            timeDelay = 0;
        }
        if (isButtonBack()) {
            statusPassword = MANAGE_MEMBER;
            reset_data();
        }
        if (isButtonNumber()) {
            if (numberValue == 1 && ((ad_cur_mem_list * 2 + 0) < num_of_user)) {
                ad_current_member = ad_cur_mem_list * 2 + 0;
            }
            else if (numberValue == 2 && ((ad_cur_mem_list * 2 + 1) < num_of_user)) {
                ad_current_member = ad_cur_mem_list * 2 + 1;
            }
            
            else {
                ad_current_member = CHAR_ERROR_RETURN;
            }
            if (ad_current_member != CHAR_ERROR_RETURN) {
                statusPassword = CONFIRM_REMOVE;
                reset_data();
            }
            timeDelay = 0;
        }
        if (timeDelay > 600) {
            statusPassword = MANAGE_MEMBER;
            reset_data();
        }
        break;
    case CONFIRM_REMOVE:
        timeDelay++;
        LcdPrintStringS(0, 0, "REMOVE MS:");
        LcdPrintNumS(0, 11, account[ad_current_member].STT);
        LcdPrintCharS(0, 15, '?');
        LcdPrintStringS(1, 0, "C:YES");
        LcdPrintStringS(1, 8, "B:NO");
        if (isButtonEnter()) {
            reset_data();
            statusPassword = REMOVE_COMPLETE;
        }
        if (isButtonBack()) {
            reset_data();
            statusPassword = ADMIN_REMOVE_MEMBER;
        }
        if (timeDelay > 600) {
            reset_data();
            statusPassword = MANAGE_MEMBER;
        }
        break;
    case REMOVE_COMPLETE:
        timeDelay++;
        LcdPrintStringS(0, 0, "REMOVE COMPLETE");
        if (timeDelay == 1) {
            memmove(account + ad_current_member, account + ad_current_member + 1,
                (num_of_user - ad_current_member - 1) * sizeof(user_account));
            if (num_of_user > 0) {
                num_of_user--;
            }

        }
        if (timeDelay > 40) {
            reset_data();
            statusPassword = MANAGE_MEMBER;
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

unsigned char CheckPassword()
{
    unsigned char i, j;
    for (i = 0; i < 6; i++) {
        if (account[current_user].password[i] != arrayPassword[i]) {
            return 0;
        }
    }
    return 1;

}
/*
//check PW to state ADMIN_OPTION
unsigned char isModeOn() {
    if (account[current_user].password[0]==0) statusPassword = ADMIN_MODE;
    if (account[current_user].password[0]==0) statusPassword = USER_MODE;
}
*/


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
    if (key_code[7] == 1)
        return 1;
    else
        return 0;
}

//press "#" to OK
unsigned char isButtonEnter() {
    if (key_code[11] == 1)
        return 1;
    else
        return 0;
}


void reset_data() {
    LcdClearS();
    timeDelay = 0;
    indexOfNumber = 0;
    indexOfSTT = 0;
    ad_cur_mem_list = 0;        //list of member
}

void UnlockDoor() {
    OpenOutput(0);
    //OpenOutput(4);
}

void LockDoor() {
    CloseOutput(0);
    //CloseOutput(4);
}
void Test_KeyMatrix() {
    int i =0;
    for(i=0; i<16; i++) {
        if (key_code[i]>0) {
            LcdPrintNumS(0, i, key_code[i]);
        }
        else {
            LcdPrintNumS(0,i,0);
        }
    }
}
