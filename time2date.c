/**********************************************************************
**
** Program:   Time2Date
**
** Purpose:  Convert the GPS time from decimal integer or hex data to
**           the corresponding date/time.  Also converts from date/time
**           back to integer or hex data that represents the number
**           of seconds elapsed sinced midnight the morning of 1/6/80.
**           An offset can be entered for number to date tests.   This
**           is useful for entering the burn time when computing data 
**           for the GNC maneuvver plan when performing reboost tests
**           with the Russians.
**
**
** Author: L. Johnson,
** Created: 05/27/22
** Current Version: 1.0
***********************************************************************
**  Revision   Date       Engineer       Description of Change
**  --------   --------   ------------   ------------------------------
**  1.0        05/27/22   L. Johnson     Initial Release
**  1.1        05/28/22   L. Johnson     Minor code cleanup
**********************************************************************/
// #include "stdafx.h"
// #include "Time2Date.h"

#include <windows.h>

#include <wchar.h>
#include <stdbool.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// void LoadMyImage(void);
void date2num(HWND);
void num2date(HWND);

#define ID_CONVERT 1
#define ID_QUIT 2
#define ID_USEHEX 3
#define ID_USEINT 4
#define ID_EDIT_DATE 5
#define ID_EDIT_TIME 6
#define ID_EDIT_MSW  7
#define ID_EDIT_LSW  8
#define ID_EDIT_INT 9
#define ID_EDIT_OFFSET 10
#define ID_USE_OFFSET  11
#define ID_NO_OFFSET   12
#define ID_DATE2NUM    13
#define ID_NUM2DATE    14

// Static Globals
static HWND hwndEdit_Date;
static HWND hwndEdit_Time;
static HWND hwndEdit_MSW;
static HWND hwndEdit_LSW;
static HWND hwndEdit_Int;
static HWND hwndEdit_Offset;
static HWND rb1;  //Date to Number
static HWND rb2;  //Number to Date
static HWND rb3;  //Use Hex
static HWND rb4;  //Use Integer
static HWND rb5;  //No Offset
static HWND rb6;  //Use Offset

static  int useDateF;  // Use Date Flag
static  int  useHexF;  // Use Hex Flag
static  int  useOffsetF;  // Use Offset Flag

HINSTANCE g_hinst;
HINSTANCE g_hinst2;
HINSTANCE g_hinst3;
HINSTANCE g_hinst4;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
  PWSTR lpCmdLine, int nCmdShow) {

  HWND hwnd;
  MSG  msg;
  WNDCLASSW wc = { 0 };
  wc.lpszClassName = L"Time to Date";
  wc.hInstance = hInstance;
  wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
  wc.lpfnWndProc = WndProc;
  wc.hCursor = LoadCursor(0, IDC_ARROW);

  RegisterClassW(&wc);
  hwnd = CreateWindowW(wc.lpszClassName, L"Time to Date",
    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    150, 150, 320, 300, 0, 0, hInstance, 0);

  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
  WPARAM wParam, LPARAM lParam) {
  static  int RBFlags;
  HWND hsti;
  switch (msg) {
  case WM_CREATE:
    // LoadMyImage();
    hsti = CreateWindowW(L"static", L"",
      WS_CHILD | WS_VISIBLE | SS_BITMAP,
      266, 225, 80, 80, hwnd, (HMENU)6, NULL, NULL);
    // *** Create Convert/Quit Buttons
    CreateWindowW(L"Button", L"Convert",
      WS_VISIBLE | WS_CHILD,
      170, 220, 80, 25, hwnd, (HMENU)ID_CONVERT, NULL, NULL);
    rb1 = CreateWindowW(L"Button", L"Date to Number",
      WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
      30, 210, 120, 30, hwnd,
      (HMENU)ID_DATE2NUM, g_hinst3, NULL);
    rb2 = CreateWindowW(L"Button", L"Number to Date",
      WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
      30, 230, 120, 30, hwnd,
      (HMENU)ID_NUM2DATE, g_hinst3, NULL);
    SendMessage(rb1, BM_SETCHECK, 1, 0);
    SendMessage(rb2, BM_SETCHECK, 0, 0);
    useDateF = TRUE; // Set Use Date Flag

    /*  Uncomment the next statement to add a quit button
    CreateWindowW(L"Button", L"Quit",
      WS_VISIBLE | WS_CHILD ,
      220, 5, 80, 25, hwnd, (HMENU) ID_QUIT, NULL, NULL);
    */

    // Create Static and Edit Boxes for Date/Time
    CreateWindowW(L"static", L"Date/Time ",
      WS_CHILD | WS_VISIBLE,
      150, 20, 75, 20,
      hwnd, (HMENU)1, NULL, NULL);
    hwndEdit_Date = CreateWindowW(L"Edit", NULL,
      WS_CHILD | WS_VISIBLE | WS_BORDER,
      10, 20, 60, 20, hwnd, (HMENU)ID_EDIT_DATE,
      NULL, NULL);
    SetWindowTextW(hwndEdit_Date, L"09/19/22");
    hwndEdit_Time = CreateWindowW(L"Edit", NULL,
      WS_CHILD | WS_VISIBLE | WS_BORDER,
      80, 20, 60, 20, hwnd, (HMENU)ID_EDIT_TIME,
      NULL, NULL);
    SetWindowTextW(hwndEdit_Time, L"12:00:00");
    // Create Static and Edit Boxes for Hex and Integer Data
    CreateWindowW(L"static", L"MSW",
      WS_CHILD | WS_VISIBLE,
      10, 60, 40, 20,
      hwnd, (HMENU)2, NULL, NULL);
    hwndEdit_MSW = CreateWindowW(L"Edit", NULL,
      WS_CHILD | WS_VISIBLE | WS_BORDER,
      10, 80, 40, 20, hwnd, (HMENU)ID_EDIT_MSW,
      NULL, NULL);
    SetWindowTextW(hwndEdit_MSW, L"5053");
    CreateWindowW(L"static", L"LSW",
      WS_CHILD | WS_VISIBLE,
      60, 60, 40, 20,
      hwnd, (HMENU)3, NULL, NULL);
    hwndEdit_LSW = CreateWindowW(L"Edit", NULL,
      WS_CHILD | WS_VISIBLE | WS_BORDER,
      60, 80, 40, 20, hwnd, (HMENU)ID_EDIT_LSW,
      NULL, NULL);
    SetWindowTextW(hwndEdit_LSW, L"1c40");
    CreateWindowW(L"static", L"Integer",
      WS_CHILD | WS_VISIBLE,
      160, 60, 80, 20,
      hwnd, (HMENU)4, NULL, NULL);
    hwndEdit_Int = CreateWindowW(L"Edit", NULL,
      WS_CHILD | WS_VISIBLE | WS_BORDER,
      160, 80, 90, 20, hwnd, (HMENU)ID_EDIT_INT,
      NULL, NULL);
    SetWindowTextW(hwndEdit_Int, L"1347624000");
    // *** Create Use Hex/Use Integer Buttons
    rb3 = CreateWindowW(L"Button", L"Use Hex",
      WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
      50, 100, 100, 30, hwnd,
      (HMENU)ID_USEHEX, g_hinst, NULL);
    rb4 = CreateWindowW(L"Button", L"Use Integer",
      WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
      160, 100, 100, 30, hwnd,
      (HMENU)ID_USEINT, g_hinst, NULL);
    SendMessage(rb3, BM_SETCHECK, 1, 0);
    SendMessage(rb4, BM_SETCHECK, 0, 0);
    useHexF = TRUE;  // Set Use Hex Flag
    // Create Edit buttons, window and static field for offset 
    CreateWindowW(L"static", L"Offset",
      WS_CHILD | WS_VISIBLE,
      210, 150, 80, 20,
      hwnd, (HMENU)5, NULL, NULL);
    hwndEdit_Offset = CreateWindowW(L"Edit", NULL,
      WS_CHILD | WS_VISIBLE | WS_BORDER,
      160, 150, 40, 20, hwnd, (HMENU)ID_EDIT_OFFSET,
      NULL, NULL);
    SetWindowTextW(hwndEdit_Offset, L"0");
    rb5 = CreateWindowW(L"Button", L"No Offset",
      WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
      50, 170, 100, 30, hwnd,
      (HMENU)ID_NO_OFFSET, g_hinst2, NULL);
    rb6 = CreateWindowW(L"Button", L"Use Offset",
      WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
      160, 170, 100, 30, hwnd,
      (HMENU)ID_USE_OFFSET, g_hinst2, NULL);
    SendMessage(rb5, BM_SETCHECK, 1, 0);
    SendMessage(rb6, BM_SETCHECK, 0, 0);
    useOffsetF = FALSE;  // Clear Usee Offset Flag
    break;
  case WM_COMMAND:
    if (LOWORD(wParam) == ID_EDIT_MSW) {
      SendMessage(rb1, BM_SETCHECK, 0, 0);
      SendMessage(rb2, BM_SETCHECK, 1, 0);
      useDateF = FALSE; // Clear Use Date Flag
      SendMessage(rb3, BM_SETCHECK, 1, 0);
      SendMessage(rb4, BM_SETCHECK, 0, 0);
      useHexF = TRUE; // Set Use Hex Flag
    }
    if (LOWORD(wParam) == ID_EDIT_LSW) {
      SendMessage(rb1, BM_SETCHECK, 0, 0);
      SendMessage(rb2, BM_SETCHECK, 1, 0);
      useDateF = FALSE; // Clear Use Date Flag
      SendMessage(rb3, BM_SETCHECK, 1, 0);
      SendMessage(rb4, BM_SETCHECK, 0, 0);
      useHexF = TRUE; // Set Use Hex Flag
    }
    if (LOWORD(wParam) == ID_EDIT_INT) {
      SendMessage(rb1, BM_SETCHECK, 0, 0);
      SendMessage(rb2, BM_SETCHECK, 1, 0);
      useDateF = FALSE; // Clear Use Date Flag
      SendMessage(rb3, BM_SETCHECK, 0, 0);
      SendMessage(rb4, BM_SETCHECK, 1, 0);
      useHexF = FALSE; // Clear Use Hex Flag
    }
    if (LOWORD(wParam) == ID_EDIT_DATE) {
      SendMessage(rb1, BM_SETCHECK, 1, 0);
      SendMessage(rb2, BM_SETCHECK, 0, 0);
      useDateF = TRUE; // Set Use Date Flag
    }
    if (LOWORD(wParam) == ID_EDIT_TIME) {
      SendMessage(rb1, BM_SETCHECK, 1, 0);
      SendMessage(rb2, BM_SETCHECK, 0, 0);
      useDateF = TRUE; // Set Use Date Flag
    }
    if (LOWORD(wParam) == ID_EDIT_OFFSET) {
      SendMessage(rb5, BM_SETCHECK, 0, 0);
      SendMessage(rb6, BM_SETCHECK, 1, 0);
      useOffsetF = TRUE; // Set Use Offset Flag
    }
    // Here is where all the work will occur...
    if (LOWORD(wParam) == ID_CONVERT) {
      if (useDateF) date2num(hwnd); else num2date(hwnd);
    }
    // If I had a quit button, this is what it would do
    if (LOWORD(wParam) == ID_QUIT) {
      PostQuitMessage(0);
    }
    if (HIWORD(wParam) == BN_CLICKED) {
      switch (LOWORD(wParam)) {
      case ID_USEHEX:
        SendMessage(rb3, BM_SETCHECK, 1, 0);
        SendMessage(rb4, BM_SETCHECK, 0, 0);
        useHexF = TRUE; // Set Use Hex Flag
        SendMessage(rb1, BM_SETCHECK, 0, 0);
        SendMessage(rb2, BM_SETCHECK, 1, 0);
        useDateF = FALSE; // Clear Use Date Flag
        break;
      case ID_USEINT:
        SendMessage(rb3, BM_SETCHECK, 0, 0);
        SendMessage(rb4, BM_SETCHECK, 1, 0);
        useHexF = FALSE; // Clear Use Hex Flag
        SendMessage(rb1, BM_SETCHECK, 0, 0);
        SendMessage(rb2, BM_SETCHECK, 1, 0);
        useDateF = FALSE; // Clear Use Date Flag
        break;
      case ID_USE_OFFSET:
        SendMessage(rb5, BM_SETCHECK, 0, 0);
        SendMessage(rb6, BM_SETCHECK, 1, 0);
        useOffsetF = TRUE; // Set Use Offset Flag
        break;
      case ID_NO_OFFSET:
        SendMessage(rb5, BM_SETCHECK, 1, 0);
        SendMessage(rb6, BM_SETCHECK, 0, 0);
        useOffsetF = FALSE; // Clear Use Offset Flag
        break;
      case ID_DATE2NUM:
        SendMessage(rb1, BM_SETCHECK, 1, 0);
        SendMessage(rb2, BM_SETCHECK, 0, 0);
        useDateF = TRUE; // Set Use Date Flag
        break;
      case ID_NUM2DATE:
        SendMessage(rb1, BM_SETCHECK, 0, 0);
        SendMessage(rb2, BM_SETCHECK, 1, 0);
        useDateF = FALSE; // Clear Use Date Flag
        break;
      }
    }
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }
  return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void date2num(HWND hwnd) {
  int done;
  int month, day, year, rday;
  int mm, dd, yy;
  int hh, m, ss;
  int max_day[12] = { 31,29,31,30,31,30,31,31,30,31,30,31 };
  int errflg;
  wchar_t strDate[9];
  wchar_t strTime[9];
  int len;  // Length of string
  int s_cnt;  // Scan Count
  wchar_t str[255];
  int etime;
  errflg = 0;
  if (errflg == 0) {//date format check
    len = GetWindowTextLengthW(hwndEdit_Date);
    GetWindowTextW(hwndEdit_Date, strDate, 9);
    s_cnt = swscanf_s(strDate, L"%02d/%02d/%02d", &mm, &dd, &yy);
    if ((s_cnt != 3) || (len != 8)) {
      errflg = 1;
      MessageBoxW(NULL, L"Date Format Error!", L"Error Messages", MB_OK);
    }
  }
  if (errflg == 0) {//illegal date check
    if ((yy % 4) == 0) max_day[1] = 29; else max_day[1] = 28;
    if (mm<1 || mm>12) errflg = 1;
    if (errflg == 0) {
      if (dd<1 || dd>max_day[mm - 1]) errflg = 1;
    }
    if (errflg == 1) MessageBoxW(NULL, L"Illegal Date!", L"Error Messages", MB_OK);
  } //end illegal date check
    // Validate Time
  if (errflg == 0) {//time format check
    len = GetWindowTextLengthW(hwndEdit_Time);
    GetWindowTextW(hwndEdit_Time, strTime, 9);
    s_cnt = swscanf_s(strTime, L"%02d:%02d:%02d", &hh, &m, &ss);
    if ((s_cnt != 3) || (len != 8)) {
      errflg = 1;
      MessageBoxW(NULL, L"Time Format Error!", L"Error Messages", MB_OK);
    }
  }
  if (errflg == 0) {//illegal time check
    if ((hh<0) || (hh>23) || (m<0) || (m>59) || (ss<0) || (ss>59)) {
      errflg = 1;
      // wsprintfW(temp , L"%02d %02d %02d", hh,m,ss);
      MessageBoxW(NULL, L"Illegal Time!", L"Error Messages", MB_OK);
    }
  }//end illegal time check  
  if (errflg == 0) {// do conversion
    yy = yy + 1900;
    if (yy<1980) yy = yy + 100;
    if ((yy == 1980) && (mm == 1) && (dd<6)) yy = yy + 100;
    // Per the space to ground ICD, the beginning of time is midnight
    //  January 5/6, 1980. 
    month = 1;
    day = 6;
    year = 1980;
    rday = 0;
    max_day[1] = 29; // 1980 is a leapyear
    // Lets do the date first 
    done = 0;
    if (yy == year) done = 1;
    if (yy == (year + 1) && dd<day && mm == 1) done = 1;
    while (done == 0) {
      if ((year % 4) == 0) rday = rday + 366; else rday = rday + 365;
      year++;
      if (yy == (year + 1) && dd<day && mm == 1) done = 1;
      if (yy == year) done = 1;
    }
    if ((year % 4) == 0) max_day[1] = 29; else max_day[1] = 28; // Correct max days
    if ((month == mm) && (day == dd) && (year == yy)) done = 1; else done = 0;
    while (done == 0) {
      day++;
      if (day>max_day[month - 1]) {
        month++;
        day = 1;
        if (month>12) {
          month = 1;
          year++;
          // Adjust for leap year/nonleap year 
          if ((year % 4) == 0) max_day[1] = 29; else max_day[1] = 28;
        }  /* if (month==12) */
      } /* if (day>max_day[month-1]) */
      rday++;
      if ((month == mm) && (day == dd) && (year == yy)) done = 1;
    } /* while (month!=mm && day!=dd &&  year!= yy) { */
    etime = (rday * 24 + hh) * 3600 + m * 60 + ss;
    wsprintfW(str, L"%d", etime);
    // MessageBoxW(NULL, str, L"Error Messages", MB_OK);
    SetWindowTextW(hwndEdit_Int, str);
    // Doing the LSW
    wsprintfW(str, L"%08x", etime);
    // MessageBoxW(NULL, str, L"Error Messages", MB_OK);
    SetWindowTextW(hwndEdit_LSW, &str[4]);
    // Doing the MSW
    str[4] = 0;//  Terminate the String after MSW
    SetWindowTextW(hwndEdit_MSW, str);
  }
  //Reselect Use Date  (Set Use Date Flag to True)
  SendMessage(rb1, BM_SETCHECK, 1, 0);
  SendMessage(rb2, BM_SETCHECK, 0, 0);
  useDateF = TRUE; // Set Use Date Flag
           // wsprintfW(temp , L"%ld %2d", len, s_cnt);
           // MessageBoxW(NULL, temp, L"Error Messages", MB_OK);
           // MessageBoxW(NULL, strTime, L"Error Messages", MB_OK);
           // if(errflg==0)  MessageBoxW(NULL, L"No Errors!", L"Error Messages", MB_OK);
}

void num2date(HWND hwnd) {
  int month, day, year, rday;
  int max_day[12] = { 31,29,31,30,31,30,31,31,30,31,30,31 };
  int errflg;
    float eday; // Number of elapsed days 
  int yday;  // Number of days in a year
  int hours, minutes, lseconds;
  int eTime;    // Previouisly named etime for elapsed time.
  int ctlsw, ctmsw;   // Coarse time words 
  int tot_time;
  int offset;
  wchar_t strMSW[5];
  wchar_t strLSW[5];
  wchar_t strInt[12];
  wchar_t strOff[7];
  wchar_t temp[255];
  wchar_t str[255];
  int lenLSW, lenMSW;
  int s_cnt1, s_cnt2; // Scan Counts 1 & 2
  // Get Offset (if any)
  offset = 0;
  if (useOffsetF) {
    GetWindowTextW(hwndEdit_Offset, strOff, 6);
    swscanf_s(strOff, L"%d", &offset);
  }
  if (useHexF) {
    GetWindowTextW(hwndEdit_MSW, strMSW, 5);
    GetWindowTextW(hwndEdit_LSW, strLSW, 5);
    errflg = 0;
    lenLSW = GetWindowTextLengthW(hwndEdit_LSW);
    lenMSW = GetWindowTextLengthW(hwndEdit_MSW);
    s_cnt1 = swscanf_s(strMSW, L"%4x", &ctmsw);
    s_cnt2 = swscanf_s(strLSW, L"%4x", &ctlsw);

    if (lenLSW != 4 || lenMSW != 4 || s_cnt1 != 1 || s_cnt2 != 1) {
      errflg = 1;
      MessageBoxW(NULL, L"Hex Data Format Error!", L"Error Messages", MB_OK);
    }
    tot_time = (ctmsw << 16) + ctlsw;
    if (useOffsetF) tot_time = tot_time + offset;
    if (tot_time<0) {
      errflg = 1;
      MessageBoxW(NULL, L"Hex Data Format Error!", L"Error Messages", MB_OK);
    }
    if (errflg == 0) {
      wsprintfW(str, L"%d", tot_time);
      SetWindowTextW(hwndEdit_Int, str);
      wsprintfW(temp, L"%08x", tot_time);
      SetWindowTextW(hwndEdit_LSW, &temp[4]);
      temp[4] = 0;  // Terminate after MSW
      SetWindowTextW(hwndEdit_MSW, temp);
    }
  }
  else {  // If it isn't hex than it is integer
      // Doing Integer
    tot_time = -1;
    errflg = 0;
    GetWindowTextW(hwndEdit_Int, strInt, 11);
    swscanf_s(strInt, L"%d", &tot_time);
    if (tot_time<0 || tot_time> 2147483647) {
      errflg = 1;
      MessageBoxW(NULL, L"Illegal Integer!", L"Error Messages", MB_OK);
    }
    if (useOffsetF) tot_time = tot_time + offset;
    if (errflg == 0) {
      wsprintfW(temp, L"%08x", tot_time);
      SetWindowTextW(hwndEdit_LSW, &temp[4]);
      temp[4] = 0;  // Terminate after MSW
      SetWindowTextW(hwndEdit_MSW, temp);
      wsprintfW(str, L"%d", tot_time);
      SetWindowTextW(hwndEdit_Int, str);
    }
  }// end if (useHexF) {  
  if (errflg == 0) {
    eday = (float)tot_time / (3600 * 24); // Elapsed time (days) 
    eTime = tot_time % (3600 * 24); // Leftover seconds 
    month = 1;
    day = 5;
    year = 1980;
    rday = 0;
    if (eTime == 0) day++;//Midnights the next day
     // First peel off the years...  
    yday = 366;  // Starting with a leap year
    while (eday>yday) {
      eday = eday - (float)yday;
      year++;  // increment to next year 
      if ((year % 4) == 0) yday = 366;  // days in a leap year
      else yday = 365; // days in a normal year
    } // end while (eday>yday)
      // Adjust for leap year/nonleap year 
    if ((year % 4) == 0) max_day[1] = 29; else max_day[1] = 28;
    // peel off the days
    while (rday<eday) {
      day++;
      if (day>max_day[month - 1]) {
        month++;
        day = 1;
        if (month>12) {
          month = 1;
          year++;
          // Adjust for leap year/nonleap year 
          if ((year % 4) == 0) max_day[1] = 29; else max_day[1] = 28;
        }  // if (month==12)  
      } // if (day>max_day[month-1]) 
      rday++;
    } // while (rday<eday) 
    // Did the date, now do the time using the left over second 
    hours = (int)(eTime / 3600);
    eTime = eTime % 3600;  // Collect the spare time 
    minutes = (int)(eTime / 60);
    lseconds = eTime % 60;
    //  MessageBoxW(NULL, L"No Errors!", L"Error Messages", MB_OK);
    // Update Edit Fields for Time & Date
    wsprintfW(str, L"%02d/%02d/%02d", month, day, year - ((int)(year / 100)) * 100);
    SetWindowTextW(hwndEdit_Date, str);
    wsprintfW(str, L"%02d:%02d:%02d", hours, minutes, lseconds);
    SetWindowTextW(hwndEdit_Time, str);
    //Reselect Use Number  (Set Use Date Flag to false)
    SendMessage(rb1, BM_SETCHECK, 0, 0);
    SendMessage(rb2, BM_SETCHECK, 1, 0);
    useDateF = FALSE; // Clear Use Date Flag
  }  // if (errflg==0) {
}
