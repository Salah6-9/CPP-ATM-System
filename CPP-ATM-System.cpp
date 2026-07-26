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
struct stClient
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

void ShowAtmMainMenu(stClient Client)
{
    system("clear");
    cout << "==================== Client Management System ====================\n";
    cout << "[1]. Quick Withdraw\n";
    cout << "[2]. Normal Withdraw.\n";
    cout << "[3]. Deposit\n";
    cout << "[4]. Check Balance.\n";
    cout << "[5]. Logout\n";
    cout << "==================================================================\n";

}
stClient ConvertLinetoRecord(string line, string delm = " ")
{
    vector<string> splitedValue = MyLib::SplitEachWordInString(line, delm);
    stClient stClien;
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
vector<stClient> LoadClientsDataFromFile(string ClientFileName)
{

    vector<stClient> Clients;
    fstream MyFile;
    MyFile.open(ClientFileName, ios::in);
    if (MyFile.is_open())
    {
        string Line;
        int i = 0;
        while (getline(MyFile, Line))
        {
            if (MyLib::TrimSpaces(Line).empty())
                continue;
            stClient c = ConvertLinetoRecord(Line, "#//#");
            if (!c.MarkForDelete)
                Clients.push_back(c);
            i++;
        }
    }
    MyFile.close();

    return Clients;
}

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
bool IsClientAuthorized (stClient Client,string ClientsFileName,stClient &FoundClient) 
{
    vector<stClient> vClient = LoadClientsDataFromFile(ClientsFileName);
    int position;
    bool IsClientAuth= AuthenticateClient(Client,vClient,FoundClient,position); 
    return (IsClientAuth);
}
void LoginScreen()
{
    stClient Client;
    cout << "\n-----------------------------------\n";
    cout << "\tLogin Screen";
    cout << "\n-----------------------------------\n";
    bool Denid=false;
    stClient Found;
    do
    {
        Client.NbrAcount = MyIO::ReadString("Enter Account Number Please: ");
        Client.PinCode = MyIO::ReadString("Enter A Pin Code Please: ");

        Denid = !IsClientAuthorized(Client,ClientFileName,Found);
        if (Denid)
            system("clear");
        else    
            Client = Found ;   
        
    } while (Denid);
    ShowAtmMainMenu(Client);
}
