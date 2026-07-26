#include <iostream>
#include "../../../MyLibrary/MyLib.h"
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
using namespace std;

string ClientFileName="Clients.txt";
struct sClient
{
    string NbrAcount, PinCode, FullName, NbrPhone;
    double AccountBalance;
    bool MarkForDelete = false;
};

enum enAtmMenuOptions { 
    QuickWithdraw = 1
    , NormalWithdraw = 2
    , Deposit = 3
    , CheckBalance = 4
    , Logout = 5 
};

enum enQuickWithdrawOptions { 
    Amount20     = 1
    , Amount50   = 2
    , Amount100  = 3
    , Amount200  = 4
    , Amount400  = 5
    , Amount600  = 6
    , Amount800  = 7
    , Amount1000 = 8
    , Exit = 9
};

