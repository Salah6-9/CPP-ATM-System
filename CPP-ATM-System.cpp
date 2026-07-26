#include <iostream>
#include "../../../MyLibrary/MyLib.h"
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
using namespace std;

struct Client
{
    string NbrAcount, PinCode, FullName, NbrPhone;
    double AccountBalance;
    bool MarkForDelete = false;
};
struct stUser
{
    string UserName;
    string Pasword;
    int Permission = 0;
    bool MarkForDelete =0;
};
enum enMenuOptions
{
    AddClient = 1,
    ShowClientsList,
    FindClient,
    UpdateClientInfo,
    RemoveClient,
    TransactionOptions,
    ManageUsers,
    Logout
};

enum enTransactionOptions
{
    Deposit = 1,
    Withdraw,
    TotalBalances,
    BackToMainMenu
};
enum SystemPermissions {
    eAll = -1,              // (11111111)
    eClientListe  = 1<< 0,  // 1   (00000001)
    eAddNewClient = 1 << 1, // 2   (00000010),
    eDeleteClient =1 << 2, // 4   (00000100)
    eUpdateClient = 1<< 3, // 8   (00001000)
    eFindClient = 1 << 4, // 16  (00010000)
    eTransaction = 1 << 5, // 32  (00010000)
    eManageUsers = 1 << 6, // 64  (00010000)
};
// -----------------------Declaration------------------------
void PrintMainMenu(stUser User);
void RunTheChoice(enMenuOptions, stUser);
enMenuOptions GetMenuChoice();
void TransactionsMenuScreen(stUser User);
void DepositToAccount(string FileName,
                      vector<Client> &vClient,
                      bool isFound,
                      double Value, int position);
void WithdrawFromAccount(string FileName,
                         vector<Client> &vClient,
                         bool isFound,
                         double Value, int position);
void RunTheChoiceOfTransaction(enTransactionOptions choise,stUser User);
void LoginScreen(stUser User);
void ShowUsersDetails(string UsersFileName);
void ManageUsersMenuScreen(stUser);
void ShowAccessDeniedMessage(stUser User);
void ShowAddNewUsersScreen(string UsersFileName);
void ShowDeleteUserScreen(string UsersFileName, stUser User);
void ShowFindUserScreen(string UsersFileName);
void ShowUpdateUserScreen(string UsersFileName,stUser User);


void GoToMainMenue(stUser User)
{

    MyLib::PauseAndClearScreen();
    PrintMainMenu(User);
}
void GoBackToTransactionMenu(stUser User)
{
    MyLib::PauseAndClearScreen();
    TransactionsMenuScreen(User);
}
vector<Client> LoadClientsDataFromFile(string);
bool isAccountRegistered(const vector<Client> &Clients, const string &Value)
{

    for (const Client &c : Clients)
    {
        if (c.NbrAcount == Value)
        {
            return true;
        }
    }
    return false;
}

string ReadClientAccountNumber()
{
    string AccountNumber = "";
    cout << "\nPlease enter AccountNumber? ";
    cin >> AccountNumber;
    return AccountNumber;
}

Client FillClientStruct(const vector<Client> &vClients)
{
    Client stClient;

    do
    {
        stClient.NbrAcount = MyLib::ReadString("\nEnter Account Number?  : ");
    } while (isAccountRegistered(vClients, stClient.NbrAcount));

    stClient.PinCode = MyLib::ReadString("\nEnter Pin Code?        : ");
    stClient.FullName = MyLib::ReadString("\nEnter Full Name?       : ");
    stClient.NbrPhone = MyLib::ReadString("\nEnter Phone number     : ");
    stClient.AccountBalance = MyLib::ReadPositiveNumber("\nEnter Account Balance? : ");

    return stClient;
};

string ConvertClientToLine(Client stClien, string delm = " ")
{

    string line = "";
    line += stClien.NbrAcount + delm;
    line += stClien.PinCode + delm;
    line += stClien.FullName + delm;
    line += stClien.NbrPhone + delm;
    line += to_string(stClien.AccountBalance);

    return line;
}

//------------------------------------ConvertLineToRecord & PrintClientRecord----------------------------------
Client ConvertLinetoRecord(string line, string delm = " ")
{
    vector<string> splitedValue = MyLib::SplitEachWordInString(line, delm);
    Client stClien;
    if (splitedValue.size() < 5)
    {
        cout << "\n[Warning] Corrupted record skipped: " << line << endl;
        stClien.MarkForDelete = true; // إشارة إن هذا السجل تالف
        return stClien;
    }

    stClien.NbrAcount = splitedValue[0];
    stClien.PinCode = splitedValue[1];
    stClien.FullName = splitedValue[2];
    stClien.NbrPhone = splitedValue[3];
    stClien.AccountBalance = stod(splitedValue[4]);
    return stClien;
}

void PrintClientRecord(Client stClient)
{
    cout << "\n this is the  Client Details : \n";
    cout << "\n Account Number : " << stClient.NbrAcount;
    cout << "\n Pin Code       : " << stClient.PinCode;
    cout << "\n Full Name      :" << stClient.FullName;
    cout << "\n Phone          :" << stClient.NbrPhone;
    cout << "\n Account Balance:" << stClient.AccountBalance;
}
//------------------------------------Add New Client to File----------------------------------
void addLineInFile(fstream &MyFile, const string &line)
{
    MyFile << line << "\n";
}

void AddClientInFile(string FileName = " Clients.txt")
{
    fstream MyFile;
    char AddMore = 'y';
    MyFile.open(FileName, ios::out | ios::app);
    if (!MyFile.is_open())
    {
        cout << "\nError opening file!";
        return;
    }
    vector<Client> vClients = LoadClientsDataFromFile(FileName);

    do
    {
        Client stClient = FillClientStruct(vClients);
        cout << "\nAre you sure you want to add this Client? | Y or N: ";
        char confirm = 'n';
        cin >> confirm;
        if (toupper(confirm) != 'Y')
        {
            cout << "\nClient Add canceled.";
            return;
        }
        vClients.push_back(stClient); // Update the curent vector
        string line = ConvertClientToLine(stClient, "#//#");
        addLineInFile(MyFile, line);
        cout << "\nthe Client has been added to File";
        cout << "\nDo you want to add more Clients? | Y or N: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin >> AddMore;
    } while (toupper(AddMore) == 'Y');

    MyFile.close();
}

void ShowAddNewClientsScreen(string FileName)
{
    cout << "\n-----------------------------------\n";
    cout << "\tAdd New Clients Screen";
    cout << "\n-----------------------------------\n";
    AddClientInFile(FileName);
}
//------------------------------------Show Clients List----------------------------------
vector<Client> LoadClientsDataFromFile(string FileName)
{

    vector<Client> Clients;
    fstream MyFile;
    MyFile.open(FileName, ios::in);
    if (MyFile.is_open())
    {
        string Line;
        int i = 0;
        while (getline(MyFile, Line))
        {
            if (MyLib::TrimSpaces(Line).empty())
                continue;
            Client c = ConvertLinetoRecord(Line, "#//#");
            if (!c.MarkForDelete)
                Clients.push_back(c);
            i++;
        }
    }
    MyFile.close();

    return Clients;
}

void ShowClientDetails(string filename)
{

    vector<Client> Clients = LoadClientsDataFromFile(filename);
    int i = 0, size = Clients.size();
    cout << setw(49) << "Client List " << size << " Client(s). \n";
    cout << "_____________________________________________________________________________________________ " << endl;
    cout << "| Account Number  | Pin Code |          Client Name          |    Phone    |    Balance      |" << endl;
    cout << "_____________________________________________________________________________________________|" << endl;
    for (int i = 0; i < size; i++)
    {
        cout << "|" << setw(17) << left << Clients[i].NbrAcount;
        cout << "|" << setw(10) << left << Clients[i].PinCode;
        cout << "|" << setw(31) << left << Clients[i].FullName;
        cout << "|" << setw(13) << left << Clients[i].NbrPhone;
        cout << "|" << setw(17) << left << Clients[i].AccountBalance << "|";
        cout << endl;
    }
    cout << "_____________________________________________________________________________________________|" << endl;
}

//------------------------------------Find Client----------------------------------
bool SearchClientInVector(const string &NbrAcc, const vector<Client> &vClients, Client &FoundClient, int &position)
{
    string SearchValueUpper = MyLib::AllLettersToUpper(MyLib::TrimSpaces(NbrAcc));
    for (size_t i = 0; i < vClients.size(); i++)
    {
        const Client &c = vClients[i];
        if (MyLib::AllLettersToUpper(MyLib::TrimSpaces(c.NbrAcount)) == SearchValueUpper)
        {
            FoundClient = c;
            position = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

bool SearchClientInFile(string FileName, string ClientID, Client &FoundClient, int &position)
{
    vector<Client> vClients = LoadClientsDataFromFile(FileName);
    vector<Client> FoundClients;
    string SearchValueUpper = MyLib::AllLettersToUpper(MyLib::TrimSpaces(ClientID));
    return SearchClientInVector(ClientID, vClients, FoundClient, position);
}

void PrintFoundClients(string FileName, string ClientID)
{
    Client FoundClient;
    int position = -1;
    bool isFound = SearchClientInFile(FileName, ClientID, FoundClient, position);
    if (isFound)
    {
        PrintClientRecord(FoundClient);
    }
    else
    {
        while (!isFound)
        {
            cout << "\nNo clients found.";
            ClientID = MyLib::ReadString("\nEnter Account Number  to Search : ");
            isFound = SearchClientInFile(FileName, ClientID, FoundClient, position);
        }
        PrintClientRecord(FoundClient);
    }
}

void ShowFindClientScreen(string FileName)
{
    cout << "\n-----------------------------------\n";
    cout << "\tFind Client Screen";
    cout << "\n-----------------------------------\n";

    string ClientID = MyLib::ReadString("\nEnter Account Number  to Search : ");
    PrintFoundClients(FileName, ClientID);
}

//------------------------------------Remove Client info----------------------------------

void UpdateFile(const string &FileName, const vector<Client> &Clients)
{
    vector<string> vFileContent;
    for (const Client &c : Clients)
    {
        vFileContent.push_back(ConvertClientToLine(c, "#//#"));
    }
    MyLib::SaveVectorToFile(FileName, vFileContent);
}

void RemoveClientFromFile(string FileName, string ClientAccountID)
{
    vector<Client> Clients = LoadClientsDataFromFile(FileName);
    Client FoundClient;
    int position = -1;
    bool isFound = SearchClientInVector(ClientAccountID, Clients, FoundClient, position);
    if (!isFound)
    {
        while (!isFound)
        {
            cout << "\nNo clients found.";
            ClientAccountID = MyLib::ReadString("\nEnter Account Number  to Search : ");
            isFound = SearchClientInVector(ClientAccountID, Clients, FoundClient, position);
        }
    }
    cout << "\n the following client will be removed from the file : ";
    PrintClientRecord(FoundClient);
    cout << "\nAre you sure you want to remove this client? | Y or N: ";
    char confirm = 'n';
    cin >> confirm;
    if (toupper(confirm) != 'Y')
    {
        cout << "\nClient removal canceled.";
        return;
    }

    Clients.erase(Clients.begin() + position);

    UpdateFile(FileName, Clients);
    cout << "\nClient removed successfully.";
}

void ShowDeleteClientScreen(string FileName)
{
    cout << "\n-----------------------------------\n";
    cout << "\tDelete Client Screen";
    cout << "\n-----------------------------------\n";

    string ClientAccountID = MyLib::ReadString("\nEnter Account Number  to Remove : ");
    RemoveClientFromFile(FileName, ClientAccountID);
}

//------------------------------------Update Client info----------------------------------

Client ChangeClientData(const Client &stClient)
{
    Client updatedClient = stClient;
    cout << "\nDo you want update the name ? | Y or N: ";
    char choise = 'n';
    cin >> choise;
    if (toupper(choise) == 'Y')
        updatedClient.FullName = MyLib::ReadString("\nEnter New Full Name?       : ");

    cout << "\nDo you want update the Pin Code ? | Y or N: ";
    cin >> choise;
    if (toupper(choise) == 'Y')
        updatedClient.PinCode = MyLib::ReadString("\nEnter New Pin Code?        : ");

    cout << "\nDo you want update the Phone number ? | Y or N: ";
    cin >> choise;
    if (toupper(choise) == 'Y')
        updatedClient.NbrPhone = MyLib::ReadString("\nEnter New Phone number     : ");

    cout << "\nDo you want update the Account Balance ? | Y or N: ";
    cin >> choise;
    if (toupper(choise) == 'Y')
        updatedClient.AccountBalance = MyLib::ReadPositiveNumber("\nEnter New Account Balance? : ");

    return updatedClient;
}
void UpdateClientInFile(string FileName, string ClientAccountID)
{
    vector<Client> Clients = LoadClientsDataFromFile(FileName);
    Client FoundClient;
    int position = -1;
    bool isFound = SearchClientInVector(ClientAccountID, Clients, FoundClient, position);
    if (!isFound)
    {
        while (!isFound)
        {
            cout << "\nNo clients found.";
            ClientAccountID = MyLib::ReadString("\nEnter Account Number  to Search : ");
            isFound = SearchClientInVector(ClientAccountID, Clients, FoundClient, position);
        }
    }

    cout << "\n the following client will be updated from the file : ";
    PrintClientRecord(FoundClient);
    cout << "\nAre you sure you want to update this client? | Y or N: ";
    char confirm = 'n';
    cin >> confirm;
    if (toupper(confirm) != 'Y')
    {
        cout << "\nClient update canceled.";
        return;
    }

    Clients[position] = ChangeClientData(Clients[position]);

    UpdateFile(FileName, Clients);
    cout << "\nClient updated successfully.";
}

void ShowUpdateClientScreen(string FileName)
{
    cout << "\n-----------------------------------\n";
    cout << "\tUpdate Client Info Screen";
    cout << "\n-----------------------------------\n";
    string ClientAccountID = MyLib::ReadString("\nEnter Account Number  to Update : ");

    UpdateClientInFile(FileName, ClientAccountID);
}

void PrintMainMenu(stUser User)
{

    system("clear");
    cout << "==================== Client Management System ====================\n";
    cout << "[1]. Add New Client\n";
    cout << "[2]. Show Clients List\n";
    cout << "[3]. Find Client\n";
    cout << "[4]. Update Client Info\n";
    cout << "[5]. Remove Client\n";
    cout << "[6]. Transactions\n";
    cout << "[7]. Manage Users\n";
    cout << "[8]. Logout\n";
    cout << "==================================================================\n";
    RunTheChoice(GetMenuChoice(),User);
}

enTransactionOptions GetTransactionChoice()
{
    int choice = MyLib::Read_num_in_range("Enter your choice (1-4): ", 1, 4);
    return static_cast<enTransactionOptions>(choice);
}

enMenuOptions GetMenuChoice()
{
    int choice = MyLib::Read_num_in_range("Enter your choice (1-8): ", 1, 8);
    return static_cast<enMenuOptions>(choice);
}
string FileName = "Clients.txt";
bool HasPermission(stUser User,SystemPermissions PermissionsToCheck){
  if (User.Permission == SystemPermissions::eAll)
    return true;
  
   return ((User.Permission & PermissionsToCheck )== PermissionsToCheck);
}
void RunTheChoice(enMenuOptions choise,stUser User)
{
    system("clear");
    switch (choise)
    {
    case enMenuOptions::ShowClientsList:
    {
        if (HasPermission(User,SystemPermissions::eClientListe))
            ShowClientDetails(FileName);
        else 
            ShowAccessDeniedMessage(User);

        GoToMainMenue(User);
        break;
    }

    case enMenuOptions::AddClient:
    {
        if (HasPermission(User,SystemPermissions::eAddNewClient))
            ShowAddNewClientsScreen(FileName);
        else 
            ShowAccessDeniedMessage(User);

        GoToMainMenue(User);
        break;
    }

    case enMenuOptions::RemoveClient:
    {
        if (HasPermission(User,SystemPermissions::eDeleteClient))
            ShowDeleteClientScreen(FileName);
        else 
            ShowAccessDeniedMessage(User);

        GoToMainMenue(User);
        break;
    }

    case enMenuOptions::UpdateClientInfo:
    {
        if (HasPermission(User,SystemPermissions::eUpdateClient))
            ShowUpdateClientScreen(FileName);
        else 
            ShowAccessDeniedMessage(User);

        GoToMainMenue(User);
        break;
    }

    case enMenuOptions::FindClient:
    {
        if (HasPermission(User,SystemPermissions::eFindClient))
            ShowFindClientScreen(FileName);
        else 
            ShowAccessDeniedMessage(User);

        GoToMainMenue(User);
        break;
    }

    case enMenuOptions::TransactionOptions:
    {
        if (HasPermission(User,SystemPermissions::eTransaction))
            TransactionsMenuScreen(User);
        else 
            ShowAccessDeniedMessage(User);
        break;
    }
    case enMenuOptions::ManageUsers:
    {
        if (HasPermission(User,SystemPermissions::eManageUsers))
            ManageUsersMenuScreen(User);
        else 
            ShowAccessDeniedMessage(User);
        
        break;
    }
    case enMenuOptions::Logout:
        LoginScreen(User);
        break;
    }
}
void TransactionsMenuScreen(stUser User)
{
    system("clear");
    cout << "==================== Transactions Menu ====================\n";
    cout << "[1]. Deposit\n";
    cout << "[2]. Withdraw\n";
    cout << "[3]. Total Balances\n";
    cout << "[4]. Back to Main Menu\n";
    cout << "===========================================================\n";
    RunTheChoiceOfTransaction(GetTransactionChoice(),User);
}
//------------------------------------Deposit ----------------------------------
void ShowDepositScreen(string FileName)
{
    cout << "\n-----------------------------------\n";
    cout << "\tDeposit Screen";
    cout << "\n-----------------------------------\n";
    int position = -1;
    Client stFoundClient;
    vector<Client> vClient = LoadClientsDataFromFile(FileName);

    string NbrAcc = MyLib::ReadString("\nEnter Account Number  to Deposit : ");
    bool isFound = SearchClientInVector(NbrAcc, vClient, stFoundClient, position);

    if (!isFound)
    {
        while (!isFound)
        {
            cout << "\nClient not found.";
            NbrAcc = MyLib::ReadString("\nEnter Account Number  to Search : ");
            isFound = SearchClientInVector(NbrAcc, vClient, stFoundClient, position);
        }
    }

    PrintClientRecord(vClient[position]);

    double Value = MyLib::ReadPositiveDoubleNumber("\nEnter Amount to Deposit : ");

    DepositToAccount(FileName, vClient, isFound, Value, position);
}
char ConfirmationMessage(const Client &vClient)
{
    cout << "\nAre you sure you want Confirm the Transaction for account " << vClient.NbrAcount << "? | Y or N:    ";
    char userChoice;
    cin >> userChoice;
    if (toupper(userChoice) != 'Y')
    {
        cout << "\nTransaction canceled.";
        return userChoice;
    }
    return userChoice;
}

void DepositToAccount(string FileName, vector<Client> &vClient, bool isFound, double Value, int position)
{

    if (isFound && toupper(ConfirmationMessage(vClient[position])) == 'Y')
    {

        vClient[position].AccountBalance = vClient[position].AccountBalance + Value;
        UpdateFile(FileName, vClient);
        cout << "\nTransaction completed successfully.";
        cout << "\nNew Balance for account " << vClient[position].NbrAcount << " is: " << vClient[position].AccountBalance;
        return;
    }
}
//------------------------------------Withdraw ----------------------------------
void ShowWithdrawScreen(string FileName)
{
    cout << "\n-----------------------------------\n";
    cout << "\tWithdraw Screen";
    cout << "\n-----------------------------------\n";
    int position = -1;
    Client stFoundClient;
    vector<Client> vClient = LoadClientsDataFromFile(FileName);

    string NbrAcc = MyLib::ReadString("\nEnter Account Number  to Withdraw : ");
    bool isFound = SearchClientInVector(NbrAcc, vClient, stFoundClient, position);
    if (!isFound)
    {
        while (!isFound)
        {
            cout << "\nClient not found.";
            NbrAcc = MyLib::ReadString("\nEnter Account Number  to Search : ");
            isFound = SearchClientInVector(NbrAcc, vClient, stFoundClient, position);
        }
    }

    PrintClientRecord(vClient[position]);

    double Amount = MyLib::ReadPositiveDoubleNumber("\nEnter Amount to Withdraw : ");

    WithdrawFromAccount(FileName, vClient, isFound, Amount, position);
}
void WithdrawFromAccount(string FileName, vector<Client> &vClient, bool isFound, double Amount, int position)
{

    while (Amount > vClient[position].AccountBalance)
    {
        cout << "\nInsufficient balance. Please enter a valid amount to withdraw: ";
        Amount = MyLib::ReadPositiveDoubleNumber("\nEnter Amount to Withdraw : ");
    }
    DepositToAccount(FileName, vClient, isFound, -Amount, position);
}

void ShowTotalBalance(string FileName)
{
    vector<Client> Clients = LoadClientsDataFromFile(FileName);
    int size = Clients.size();
    double sum = 0;
    cout << setw(49) << "Client List " << size << " Client(s). \n";
    cout << "_____________________________________________________________________________________________ " << endl;
    cout << "|     Account Number      |              Client Name               |         Balance         |" << endl;
    cout << "_____________________________________________________________________________________________|" << endl;
    for (int i = 0; i < size; i++)
    {
        cout << "|" << setw(26) << left << Clients[i].NbrAcount;
        cout << "|" << setw(40) << left << Clients[i].FullName;
        cout << "|" << setw(25) << left << Clients[i].AccountBalance << "|";
        sum += Clients[i].AccountBalance;
        cout << endl;
    }
    cout << "_____________________________________________________________________________________________|" << endl;

    cout << "\nTotal Balances = "
         << sum
         << endl;
    cout << "_____________________________________________________________________________________________|" << endl;
}

void RunTheChoiceOfTransaction(enTransactionOptions choise,stUser User)
{
    system("clear");
    switch (choise)
    {
    case enTransactionOptions::Deposit:
    {
        ShowDepositScreen(FileName);
        GoBackToTransactionMenu(User);
    }
    break;

    case enTransactionOptions::Withdraw:
    {

        ShowWithdrawScreen(FileName);
        GoBackToTransactionMenu(User);
    }
    break;

    case enTransactionOptions::TotalBalances:
    {

        ShowTotalBalance(FileName);
        GoBackToTransactionMenu(User);
    }
    break;

    case enTransactionOptions::BackToMainMenu:
    {
        GoToMainMenue(User);
    }
    break;

    default:
        break;
    }
}























//---------------------------------------Extenion 2 ------------------------------------------
string UsersFileName = "Users.txt";

//---------------------------------------User Record ------------------------------------------

//its up its done
//---------------------------------------Enum Manage Users Option ------------------------------------------
enum enManageUsersOption
{
    ListUsers = 1,
    AddNewUser,
    DeleteUsers,
    UpdateUsers,
    FindUsers,
    backToMainMenu,
};


//----------------------------------------GoBackToManageUsersMenu----------------------------------------
void GoBackToManageUsersMenu(stUser User)
{
    MyLib::PauseAndClearScreen();
    ManageUsersMenuScreen(User);
}
//----------------------------------------Run  & Get The Choice----------------------------------------
enManageUsersOption GetManageUsersOption()
{
    int choice = MyLib::Read_num_in_range("Enter your choice (1-6): ", 1, 6);
    return static_cast<enManageUsersOption>(choice);
}  
// After buld the rest func wec complet the fun
void RunUserManageChoice(enManageUsersOption choise,stUser User)
{
    system("clear");
    switch (choise)
    {
    case enManageUsersOption::ListUsers:
    {
        ShowUsersDetails(UsersFileName);
        GoBackToManageUsersMenu(User);
        break;
    }

    case enManageUsersOption::AddNewUser:
    {

        ShowAddNewUsersScreen(UsersFileName);
        GoBackToManageUsersMenu(User);
        break;
    }

    case enManageUsersOption::DeleteUsers:
    {

        ShowDeleteUserScreen(UsersFileName,User);
        GoBackToManageUsersMenu(User);
        break;
    }

    case enManageUsersOption::UpdateUsers:
    {
        ShowUpdateUserScreen(UsersFileName,User);
        GoBackToManageUsersMenu(User);
        break;
    }

    case enManageUsersOption::FindUsers:
    {

        ShowFindUserScreen(UsersFileName);
        GoBackToManageUsersMenu(User);
        break;
    }


    case enManageUsersOption::backToMainMenu:
    {
        PrintMainMenu(User);
        break;
    }
    }
}

//---------------------------------------Manage Users Menu Screen ------------------------------------------
void ManageUsersMenuScreen(stUser User)
{
    system("clear");
    cout << "==================== Manage Users Menu ====================\n";
    cout << "[1]. List Users\n";
    cout << "[2]. Add New User\n";
    cout << "[3]. Delete Users\n";
    cout << "[4]. Update Users\n";
    cout << "[5]. Find Users\n";
    cout << "[6]. Back to Main Menu\n";
    cout << "===========================================================\n";
    RunUserManageChoice(GetManageUsersOption(),User);
}
//----------------------------------------login Part----------------------------------------
//----------------------------------------Auth----------------------------------------
stUser ConvertUserLinetoRecord(string line, string delm = " ")
{
    vector<string> splitedValue = MyLib::SplitEachWordInString(line, delm);
    stUser User;
    if (splitedValue.size() < 3)
    {
        cout << "\n[Warning] Corrupted record skipped: " << line << endl;
        User.MarkForDelete = true; // إشارة إن هذا السجل تالف
        return User;
    }

    User.UserName = splitedValue[0];
    User.Pasword = splitedValue[1];
    User.Permission = stoi(splitedValue[2]);
    
    return User;
}

vector<stUser> LoadUsersDataFromFile(string UsersFileName)
{

    vector<stUser> Users;
    fstream MyFile;
    MyFile.open(UsersFileName, ios::in);
    bool check =MyFile.is_open(); 
    if (check)
    {
        string Line;
        int i = 0;
        while (getline(MyFile, Line))
        {
            if (MyLib::TrimSpaces(Line).empty())
                continue;
            stUser c = ConvertUserLinetoRecord(Line, "#//#");
            if (!c.MarkForDelete)
                Users.push_back(c);
            i++;
        }
    }
    MyFile.close();

    return Users;
}

bool AuthenticateUser(const stUser &User, const vector<stUser> &vUsres, stUser &FoundUser, int &position)
{
    string UserNameUpper = MyLib::AllLettersToUpper(MyLib::TrimSpaces(User.UserName));
    string PasswordUpper = MyLib::AllLettersToUpper(MyLib::TrimSpaces(User.Pasword));

    for (size_t i = 0; i < vUsres.size(); i++)
    {
        const stUser &c = vUsres[i];
        if (MyLib::AllLettersToUpper(MyLib::TrimSpaces(c.UserName)) == UserNameUpper && MyLib::AllLettersToUpper(MyLib::TrimSpaces(c.Pasword)) == PasswordUpper)
        {
            FoundUser = c;
            position = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

bool IsUserAuthorized (stUser User,string UsersFileName,stUser &FoundUser) 
{
    vector<stUser> vUser = LoadUsersDataFromFile(UsersFileName);
    int position;
    bool IsUserAuth= AuthenticateUser(User,vUser,FoundUser,position); 
    return (IsUserAuth);
}

bool IsAdmin(stUser user) 
{
    return (user.Permission == -1);
}



void ShowAccessDeniedMessage(stUser User){
    cout << "\n-----------------------------------\n";
    cout << "\t Acces Denid,\n";
    cout << " You Dont Have Permission To Do This,\n";
    cout << " Please Contact Your Admin,\n";
    cout << "\n-----------------------------------\n";
    MyLib::PauseAndClearScreen();
    GoToMainMenue(User);  

}

//enum SystemPermissions ;
//----------------------------------------login screen----------------------------------------
void LoginScreen(stUser User)
{
    cout << "\n-----------------------------------\n";
    cout << "\tLogin Screen";
    cout << "\n-----------------------------------\n";
    bool Denid=false;
    stUser Found;
    do
    {
        User.UserName = MyIO::ReadString("Enter A User Name Please: ");
        User.Pasword = MyIO::ReadString("Enter A Password Please: ");

        Denid = !IsUserAuthorized(User,UsersFileName,Found);
        if (Denid)
            system("clear");
        else    
            User = Found ;   
        
    } while (Denid);
    PrintMainMenu(User);
}

//----------------------------------------List Users----------------------------------------

void ShowUsersDetails(string UsersFileName)
{

    vector<stUser> Users = LoadUsersDataFromFile(UsersFileName);
    int i = 0, size = Users.size();
    cout << setw(49) << "User List " << size << " User(s). \n";
    cout << "__________________________________________________ " << endl;
    cout << "| Username  | Password |        Permission        |" << endl;
    cout << "__________________________________________________|" << endl;
    for (int i = 0; i < size; i++)
    {
        cout << "|" << setw(11) << left << Users[i].UserName;
        cout << "|" << setw(10) << left << Users[i].Pasword;
        cout << "|" << setw(26) << left << Users[i].Permission;
        cout << endl;
    }
    cout << "_________________________________________________________________|" << endl;
}

//----------------------------------------Add New Users----------------------------------------
//------------------------------------Add New Client to File----------------------------------
bool isUserRegistered(const vector<stUser> &vUser, const string &Value)
{

    for (const stUser &c : vUser)
    {
        if (c.UserName == Value)
        {
            return true;
        }
    }
    return false;
}
bool ReadChoice(string msg="") 
{
    cout <<msg;
    char r ;
    cin >> r;
    return (toupper(r) == 'Y');
}
void ReadPemition(stUser &User )
{
    User.Permission = 0;
    if(ReadChoice("\nDo u Want Give Full Acces? y/n?")){
        User.Permission |= SystemPermissions::eAll;
    }else{
        cout <<"Do you want  To Give Acces to: ";
        if (ReadChoice("\nShow Client List ? y/n?"))
        {
            User.Permission  |= SystemPermissions::eClientListe;
        }
        if (ReadChoice("\n Add New Client  ? y/n?"))
        {
            User.Permission |= SystemPermissions::eAddNewClient;
        }
        if (ReadChoice("\nDelete Client ? y/n?"))
        {
            User.Permission |= SystemPermissions::eDeleteClient;
        }
        if (ReadChoice("\nUpdate Client ? y/n?"))
        {
            User.Permission |= SystemPermissions::eUpdateClient;
        }   
        if (ReadChoice("\nFind Client  ? y/n?"))
        {
            User.Permission |= SystemPermissions::eFindClient;
        }
        if (ReadChoice("\n Transaction ? y/n?"))
        {
            User.Permission |= SystemPermissions::eTransaction;
        }
        if (ReadChoice("\nManage Users ? y/n?"))
        {
            User.Permission |= SystemPermissions::eManageUsers;
        }   
    }

}
stUser FillUserStruct(const vector<stUser> &vUser)
{
    stUser User;

    do
    {
        User.UserName = MyLib::ReadString("\nEnter User Name Please?  : ");
    } while (isUserRegistered(vUser, User.UserName));

    User.Pasword = MyIO::ReadString("\nEnter Pin Code?        : ");
    ReadPemition(User);
    return User;
}

string ConvertUserRecordToLine(stUser stUser, string delm = " ")
{

    string line = "";
    line += stUser.UserName + delm;
    line += stUser.Pasword + delm;
    line += to_string(stUser.Permission);

    return line;
}



void AddUsersInFile(string UsersFileName)
{
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    fstream MyFile;
    char AddMore = 'y';
    MyFile.open(UsersFileName, ios::out | ios::app);
    if (!MyFile.is_open())
    {
        cout << "\nError opening file!";
        return;
    }

    do
    {
        stUser sUser = FillUserStruct(vUsers);
        vUsers.push_back(sUser); // Update the curent vector
        string line = ConvertUserRecordToLine(sUser, "#//#");
        cout << "\nAre you sure you want to add this User? | Y or N: ";
        char confirm = 'n';
        cin >> confirm;
        if (toupper(confirm) != 'Y')
        {
            cout << "\nClient removal canceled.";
            return;
        }
        addLineInFile(MyFile, line);
        cout << "\nthe User has been added to File";
        cout << "\nDo you want to add more Users? | Y or N: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin >> AddMore;
    } while (toupper(AddMore) == 'Y');

    MyFile.close();
}

void ShowAddNewUsersScreen(string UsersFileName)
{
    cout << "\n-----------------------------------\n";
    cout << "\tAdd New Users Screen";
    cout << "\n-----------------------------------\n";
    AddUsersInFile(UsersFileName);
}

//------------------------------------Remove Users----------------------------------

void UpdateFile(const string &UsersFileName, const vector<stUser> &Users)
{
    vector<string> vFileContent;
    for (const stUser &c : Users)
    {
        vFileContent.push_back(ConvertUserRecordToLine(c, "#//#"));
    }
    MyLib::SaveVectorToFile(UsersFileName, vFileContent);
}
void PrintUserRecord(stUser User)
{
    cout << "\n this is the  User Details : \n";
    cout << "\n User Name : " << User.UserName;
    cout << "\n Password   : " << User.Pasword;
    cout << "\n Permission :" << User.Permission;
}
bool SearchUserInVectorByUserName(string UserName, const vector<stUser> &vUsres, stUser &FoundUser, int &position)
{
    string UserNameUpper = MyLib::AllLettersToUpper(MyLib::TrimSpaces(UserName));

    for (size_t i = 0; i < vUsres.size(); i++)
    {
        const stUser &c = vUsres[i];
        if (MyLib::AllLettersToUpper(MyLib::TrimSpaces(c.UserName)) == UserNameUpper )
        {
            FoundUser = c;
            position = static_cast<int>(i);
            return true;
        }
    }
    return false;
}
void RemoveUserFromFile(string UsersFileName, string UserName)
{
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    stUser FoundUsers;
    int position = -1;
    bool isFound = SearchUserInVectorByUserName(UserName,vUsers,FoundUsers,position);
    if (!isFound)
    {
        while (!isFound)
        {
            cout << "\nNo clients found.";
            UserName = MyLib::ReadString("\nEnter UserName Of Account  to Remove : ");
            isFound = SearchUserInVectorByUserName(UserName,vUsers,FoundUsers,position);
        }
    }

    cout << "\n the following client will be removed from the file : ";
    PrintUserRecord(FoundUsers);
    if(IsAdmin(FoundUsers)){
        cout <<"\nSorry The User Is Admin, So We Can't Remove The User";
        return;
    }
    cout << "\nAre you sure you want to remove this client? | Y or N: ";
    char confirm = 'n';
    cin >> confirm;
    if (toupper(confirm) != 'Y')
    {
        cout << "\nClient removal canceled.";
        return;
    }

    vUsers.erase(vUsers.begin() + position);

    UpdateFile(UsersFileName, vUsers);
    cout << "\nClient removed successfully.";
}

void ShowDeleteUserScreen(string UsersFileName, stUser User)
{
    cout << "\n-----------------------------------\n";
    cout << "\tDelete User Screen";
    cout << "\n-----------------------------------\n";

    string UserName = MyLib::ReadString("\nEnter UserName Of Account  to Remove : ");
    RemoveUserFromFile(UsersFileName, UserName);
}
//------------------------------------Users Users----------------------------------

stUser UpdatedUserData(const stUser &User,vector <stUser> vUser)
{
    stUser updatedUser = User;
    cout << "\nDo you want update the UserName ? | Y or N: ";
    char choise = 'n';
    cin >> choise;
    if (toupper(choise) == 'Y')
        do
        {
            updatedUser.UserName = MyLib::ReadString("\nEnter New User Name?       : ");
        } while (isUserRegistered(vUser, updatedUser.UserName));

    cout << "\nDo you want update the Password ? | Y or N: ";
    cin >> choise;
    if (toupper(choise) == 'Y')
        updatedUser.Pasword = MyLib::ReadString("\nEnter New Password?: ");

    cout << "\nDo you want update Permission ? | Y or N: ";
    cin >> choise;
    if (toupper(choise) == 'Y')
        ReadPemition(updatedUser);

    return updatedUser;
}
void UpdateUserInFile(string UsersFileName, string UserName,vector<stUser> vUser)
{
    stUser FoundUser;
    int position = -1;
    bool isFound = SearchUserInVectorByUserName(UserName,vUser,FoundUser,position);
    if (!isFound)
    {
        while (!isFound)
        {
            cout << "\nNo clients found.";
            UserName = MyLib::ReadString("\nEnter Account Number  to Search : ");
            isFound = SearchUserInVectorByUserName(UserName,vUser,FoundUser,position);
        }
    }

    cout << "\n the following User will be updated from the file : ";
    PrintUserRecord(FoundUser);
    cout << "\nAre you sure you want to update this User? | Y or N: ";
    char confirm = 'n';
    cin >> confirm;
    if (toupper(confirm) != 'Y')
    {
        cout << "\nUser update canceled.";
        return;
    }

    vUser[position] = UpdatedUserData(vUser[position],vUser);

    UpdateFile(UsersFileName, vUser);
    cout << "\nUser updated successfully.";
}

void ShowUpdateUserScreen(string UsersFileName,stUser User)
{
    string UserName;
    cout << "\n-----------------------------------\n";
    cout << "\tUpdate User Info Screen";
    cout << "\n-----------------------------------\n";
    vector<stUser> vUser = LoadUsersDataFromFile(UsersFileName);
    
    UserName = MyLib::ReadString("\nEnter User Name Of Account   to Update : ");

    UpdateUserInFile(UsersFileName, UserName,vUser);
}

//------------------------------------Find Users----------------------------------
void PrintFoundUsers(string UsersFileName, string UserName)
{
    vector<stUser> vUser = LoadUsersDataFromFile(UsersFileName);
    stUser FoundUser;
    int position = -1; char AddMore;
    bool isFound = SearchUserInVectorByUserName(UserName,vUser, FoundUser, position);
  
       
        if (isFound)
        {
            PrintUserRecord(FoundUser);
        }
        else
        {
            while (!isFound)
            {
                cout << "\nNo clients found.";
                UserName = MyLib::ReadString("\nEnter User Name Of Account  to Search : ");
                isFound = SearchUserInVectorByUserName(UserName,vUser, FoundUser, position);
            }
            PrintUserRecord(FoundUser);
        }

}

void ShowFindUserScreen(string UsersFileName)
{
    char SearchAgain = 'Y';

    do
    {
        system("clear"); 

        cout << "\n-----------------------------------\n";
        cout << "\tFind User Screen";
        cout << "\n-----------------------------------\n";

        string UserName = MyLib::ReadString("\nEnter User Name Of Account to Search : ");
        
        PrintFoundUsers(UsersFileName, UserName);

        cout << "\n\nDo you want to Search for more Users? | Y or N: ";
        cin >> SearchAgain;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // تنظيف الـ Buffer للـ ReadString في الدورة التالية

    } while (toupper(SearchAgain) == 'Y');
}
//---------------------------------------First-Run Setup------------------------------------------
bool UsersFileIsEmptyOrMissing(string UsersFileName)
{
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    return vUsers.empty();
}

void FirstRunSetupScreen(string UsersFileName)
{
    cout << "\n-----------------------------------\n";
    cout << "\tFirst-Time Setup";
    cout << "\n-----------------------------------\n";
    cout << "No users found. Let's create the first Admin account.\n";

    stUser AdminUser;
    AdminUser.UserName = MyLib::ReadString("\nEnter Admin Username : ");
    AdminUser.Pasword   = MyLib::ReadString("\nEnter Admin Password : ");
    AdminUser.Permission = SystemPermissions::eAll; // -1 => full access

    fstream MyFile;
    MyFile.open(UsersFileName, ios::out | ios::app);
    if (!MyFile.is_open())
    {
        cout << "\nError creating Users file!";
        return;
    }

    string line = ConvertUserRecordToLine(AdminUser, "#//#");
    addLineInFile(MyFile, line);
    MyFile.close();

    cout << "\nAdmin account created successfully. Please log in.\n";
    MyLib::PauseAndClearScreen();
}



int main()

{
    if (UsersFileIsEmptyOrMissing(UsersFileName))
        FirstRunSetupScreen(UsersFileName);

    stUser User;
    LoginScreen(User);
    MyLib::PauseAndClearScreen();

    return 0;
}