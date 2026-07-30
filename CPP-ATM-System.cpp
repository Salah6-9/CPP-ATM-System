#include <iostream>
#include "../../../MyLibrary/MyLib.h"
#include "../../../MyLibrary/modules/MyBank/MyBank.h"

#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
using namespace std;
using stClient = MyBank::stClient;

string ClientFileName = "Clients.txt";
enum enAtmMenuOptions
{
    QuickWithdraw = 1,
    NormalWithdraw = 2,
    DepositOption = 3,
    CheckBalance = 4,
    Logout = 5
};

enum enQuickWithdrawOptions
{
    Amount20 = 1,
    Amount50 = 2,
    Amount100 = 3,
    Amount200 = 4,
    Amount400 = 5,
    Amount600 = 6,
    Amount800 = 7,
    Amount1000 = 8,
    Exit = 9
};
// ================================Declaration=======================================
void ShowAtmMainMenu(stClient &Client);
void RunTheChoice(enAtmMenuOptions choise, stClient &Client);
enAtmMenuOptions GetMenuChoice();
void ShowBalanceScreen(stClient &Client);
void ShowQuickWithdrawScreen(stClient& Client);
void ShowNormalWithdrawScreen(stClient& Client);
void ShowDepositScreen(stClient& Client);

// ======================================================================================

bool AuthenticateClient(const stClient &Client, const vector<stClient> &vClient, stClient &FounClient, int &position)
{
    string NbrAcountUpper = MyLib::AllLettersToUpper(MyLib::TrimSpaces(Client.NbrAcount));
    string PinCodeUpper = MyLib::AllLettersToUpper(MyLib::TrimSpaces(Client.PinCode));

    for (size_t i = 0; i < vClient.size(); i++)
    {
        const stClient &c = vClient[i];
        if (MyLib::AllLettersToUpper(MyLib::TrimSpaces(c.NbrAcount)) == NbrAcountUpper && MyLib::AllLettersToUpper(MyLib::TrimSpaces(c.PinCode)) == PinCodeUpper)
        {
            FounClient = c;
            position = static_cast<int>(i);
            return true;
        }
    }
    return false;
}
bool IsClientAuthorized(const stClient& Client, string ClientsFileName, stClient &FoundClient)
{
    vector<stClient> vClient = MyBank::LoadClientsDataFromFile(ClientsFileName);
    int position;
    bool IsClientAuth = AuthenticateClient(Client, vClient, FoundClient, position);
    return (IsClientAuth);
}
void LoginScreen()
{
    stClient Client;
    cout << "\n-----------------------------------\n";
    cout << "\tLogin Screen";
    cout << "\n-----------------------------------\n";
    bool Denid = false;
    stClient Found;
    do
    {
        Client.NbrAcount = MyIO::ReadString("\nEnter Account Number Please: ");
        Client.PinCode = MyIO::ReadString("\nEnter A Pin Code Please: ");

        Denid = !IsClientAuthorized(Client, ClientFileName, Found);
        if (Denid)
            system("clear");
        else
            Client = Found;

    } while (Denid);
    ShowAtmMainMenu(Client);
}

//-------------------------------atm-main-menu--------------------------------------------
void ShowAtmMainMenu(stClient &Client)
{
    system("clear");
    cout << "==================== ATM System ====================\n";
    cout << "[1]. Quick Withdraw\n";
    cout << "[2]. Normal Withdraw.\n";
    cout << "[3]. Deposit\n";
    cout << "[4]. Check Balance.\n";
    cout << "[5]. Logout\n";
    cout << "====================================================\n";
    RunTheChoice(GetMenuChoice(), Client);
}
enAtmMenuOptions GetMenuChoice()
{
    int choice = MyLib::Read_num_in_range("Enter your choice (1-5): ", 1, 5);
    return static_cast<enAtmMenuOptions>(choice);
}
void GoToMainMenue( stClient &Client)
{

    MyLib::PauseAndClearScreen();
    ShowAtmMainMenu(Client);
}

void RunTheChoice(enAtmMenuOptions choise, stClient &Client)
{
    system("clear");
    switch (choise)
    {
    case enAtmMenuOptions::QuickWithdraw:
    {
        ShowQuickWithdrawScreen(Client);
        GoToMainMenue(Client);
        break;
    }
    case enAtmMenuOptions::NormalWithdraw:
    {
        ShowNormalWithdrawScreen(Client);
        GoToMainMenue(Client);
        break;
    }
    case enAtmMenuOptions::DepositOption:
    {
        ShowDepositScreen(Client);
        GoToMainMenue(Client);
        break;
    }
    case enAtmMenuOptions::CheckBalance:
    {
        ShowBalanceScreen(Client);
        GoToMainMenue(Client);
        break;
    }

    case enAtmMenuOptions::Logout:
        LoginScreen();
        break;
    }
}
//-------------------------------atm-check-balance--------------------------------------------

void ShowBalanceScreen(stClient &Client)
{
    cout << "\n-----------------------------------\n";
    cout << "\tCheck Balance";
    cout << "\n-----------------------------------\n";
    cout << "\nYour Balance is : " << Client.AccountBalance;
}
//-------------------------------atm-ShowQuickWithdrawScreen--------------------------------------------

enQuickWithdrawOptions GetQuickWithdrawChoice()
{
    int choice = MyLib::Read_num_in_range("Enter your choice (1-9): ", 1, 9);
    return static_cast<enQuickWithdrawOptions>(choice);
}

short GetAmountFromChoice(enQuickWithdrawOptions Choice)
{
    const short Amounts[] =
        {
            20,
            50,
            100,
            200,
            400,
            600,
            800,
            1000};
    if (Choice == Exit)
        return 0;
    return Amounts[Choice - 1];
}


bool CanWithdrawAmount(stClient& Client, double Amount)
{
    return (Amount <= Client.AccountBalance) ;
}
void HandleAccountTransaction(string ClientFileName,
                              vector<stClient>& vClient,
                              stClient& Client,
                              bool isFound,
                              double Amount,
                              int position,
                              bool IsWithdraw)
{
    if (IsWithdraw && !CanWithdrawAmount(vClient[position], Amount))
    {
        cout << "\nInsufficient balance.";
        return;
    }

    double TransactionAmount = IsWithdraw ? -Amount : Amount;

    MyBank::DepositToAccount(
        ClientFileName,
        vClient,
        isFound,
        TransactionAmount,
        position);

    Client = vClient[position];
}


void ProcessTransaction(string ClientFileName,
                        stClient& Client,
                        double Amount,
                        bool IsWithdraw)
{
    int Position = -1;
    stClient FoundClient;

    vector<stClient> vClient =
        MyBank::LoadClientsDataFromFile(ClientFileName);

    bool IsFound = MyBank::SearchClientInVector(
        Client.NbrAcount,
        vClient,
        FoundClient,
        Position);

    if (!IsFound || Amount == 0)
        return;

    MyBank::PrintClientRecord(vClient[Position]);

    HandleAccountTransaction(
        ClientFileName,
        vClient,
        Client,
        IsFound,
        Amount,
        Position,
        IsWithdraw);
}
void Withdraw(string ClientFileName,
              stClient& Client,
              double Amount)
{
    ProcessTransaction(ClientFileName, Client, Amount, true);
}


void ShowQuickWithdrawScreen(stClient& Client)
{
    system("clear");
    cout << "==================== QuickWithdraw ====================\n";
    cout << "[1]. 20\t \t";
    cout << "[2]. 50\n";
    cout << "[3]. 100\t\t";
    cout << "[4]. 200\n";
    cout << "[5]. 400\n";
    cout << "[6]. 600\n";
    cout << "[7]. 800\n";
    cout << "[8]. 1000\n";
    cout << "[9]. Exit\n";
    cout << "====================================================\n";
    double Amount = 0;
    Amount = GetAmountFromChoice(GetQuickWithdrawChoice());
    ProcessTransaction(ClientFileName, Client,Amount,true);
}
//-------------------------------atm-NormalWithdraw--------------------------------------------
int GetAmountMultupleOf5(){
    int amount=0;
    do
    {
        amount = MyIO::ReadPositiveNumber("\nEnter an amount Multiple Of 5's? ");  
        if(!(amount % 5 == 0))
            cout << "\nAmount must be a multiple of 5.\n";    
    } while (!(amount % 5 == 0));
    return amount;
}
void Deposit(string ClientFileName,
             stClient& Client,
             double Amount)
{
    ProcessTransaction(ClientFileName, Client, Amount, false);
}
void ShowNormalWithdrawScreen(stClient& Client)
{
    system("clear");
    cout << "==================== NormalWithdraw ====================\n";
    cout <<"\t\nYour Balance is: "<<Client.AccountBalance;
    double amount = GetAmountMultupleOf5();
    Withdraw(ClientFileName, Client,amount);
}
//-------------------------------atm-Deposit--------------------------------------------

void ShowDepositScreen(stClient& Client)
{
    system("clear");
    cout << "==================== Deposit ====================\n";
    cout <<"\t\nYour Balance is: "<<Client.AccountBalance;
    double amount = MyIO::ReadPositiveDoubleNumber("\nEnter an amount  To Deposit please? ");
    Deposit(ClientFileName,Client,amount);

}


int main()

{

    LoginScreen();
    MyLib::PauseAndClearScreen();

    return 0;
}