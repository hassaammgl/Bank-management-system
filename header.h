#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>

using namespace std;

class Bank
{
private:
    int total;
    string id;
    struct person
    {
        string name, ID, address;
        int contact, cash;
    } person[100];

public:
    Bank()
    {
        total = 0;
    }
    void choice();
    void perData();
    void show();
    void upDate();
    void search();
    void transactions();
    void del();
};


void Bank::choice()
{
    char ch;
    while (1)
    {
        for (int i = 0; i < 120; i++)
        {
            cout << "*";
        }
        cout << "\n\nPRESS...!!" << endl;
        cout << "1. Create new Account" << endl;
        cout << "2. View costumers list" << endl;
        cout << "3. Update information of Existing account" << endl;
        cout << "4. Check details of existing account" << endl;
        cout << "5. For transactions" << endl;
        cout << "6. Remove existing account" << endl;
        cout << "7. Exit" << endl;
        ch = getch();
        
        system("CLS");
        for (int i = 0; i < 120; i++)
        {
            cout << "*";
        }
        switch (ch)
        {
        case '1':
            Bank::perData();
            for (int i = 0; i < 120; i++)
            {
                cout << "*";
            }
            cout << endl;
            break;
        case '2':
            if (total == 0)
            {
                cout << "No record found......" << endl;
            }
            else
            {
                Bank::show();
                for (int i = 0; i < 120; i++)
                {
                    cout << "*";
                }
            }
            cout << endl;
            break;
        case '3':
            if (total == 0)
            {
                cout << "No record found......" << endl;
            }
            else
            {
                Bank::upDate();
                for (int i = 0; i < 120; i++)
                {
                    cout << "*";
                }
            }
            cout << endl;
            break;
        case '4':
            if (total == 0)
            {
                cout << "No record found......" << endl;
            }
            else
            {
                Bank::search();
                for (int i = 0; i < 120; i++)
                {
                    cout << "*";
                }
            }
            cout << endl;
            break;
        case '5':
            if (total == 0)
            {
                cout << "No record found......" << endl;
            }
            else
            {
                Bank::transactions();
                for (int i = 0; i < 120; i++)
                {
                    cout << "*";
                }
            }
            cout << endl;
            break;
        case '6':
            if (total == 0)
            {
                cout << "No record found......" << endl;
            }
            else
            {
                Bank::del();
                for (int i = 0; i < 120; i++)
                {
                    cout << "*";
                }
            }
            cout << endl;
            break;
        case '7':
            cout<<"Good Bye.....!";
            exit(7);
            break;

        default:
            cout << "No such choice" << endl;
        }
    }
}




void Bank::perData()
{
    cout << "Enter data of person " << total + 1 << endl;
    cout << "Enter name: ";
    cin >> person[total].name;
    cout << "ID: ";
    cin >> person[total].ID;
    cout << "Address: ";
    cin >> person[total].address;
    cout << "Contact: ";
    cin >> person[total].contact;
    cout << "Total Cash: ";
    cin >> person[total].cash;
    total++;
}

void Bank::show()
{
    for (int i = 0; i < total; i++)
    {
        cout << "Data of Person " << i + 1 << endl;
        cout << "Name: " << person[i].name << endl;
        cout << "ID: " << person[i].ID << endl;
        cout << "Address: " << person[i].address << endl;
        cout << "Contact: " << person[i].contact << endl;
        cout << "Cash: " << person[i].cash << endl;
    }
}

void Bank::upDate()
{
    cout << "Enter id of person whose wants to be updated..." << endl;
    cin >> id;
    for (int i = 0; i < total; i++)
    {
        if (id == person[i].ID)
        {
            cout << "Your previous Data." << endl;
            cout << "Data of Person " << i + 1 << endl;
            cout << "Name: " << person[i].name << endl;
            cout << "ID: " << person[i].ID << endl;
            cout << "Address: " << person[i].address << endl;
            cout << "Contact: " << person[i].contact << endl;
            cout << "Cash: " << person[i].cash << endl;
            cout << endl
                 << "Enter new Data" << endl;
            cout << "Enter name: ";
            cin >> person[i].name;
            cout << "ID: ";
            cin >> person[i].ID;
            cout << "Address: ";
            cin >> person[i].address;
            cout << "Contact: ";
            cin >> person[i].contact;
            cout << "Total Cash: ";
            cin >> person[i].cash;
            break;
        }
        if (i == total - 1)
        {
            cout << "No such record found" << endl;
        }
    }
}

void Bank::search()
{
    cout << "Enter id of person whose wants to be searching..." << endl;
    cin >> id;
    for (int i = 0; i < total; i++)
    {
        if (id == person[i].ID)
        {
            cout << "Data of Person " << i + 1 << endl;
            cout << "Name: " << person[i].name << endl;
            cout << "ID: " << person[i].ID << endl;
            cout << "Address: " << person[i].address << endl;
            cout << "Contact: " << person[i].contact << endl;
            cout << "Cash: " << person[i].cash << endl;
            break;
        }
        if (i == total - 1)
        {
            cout << "No such record found..." << endl;
        }
    }
}

void Bank::transactions()
{
    char ch;
    int cash;
    cout << "Enter id of person who wants for transaction..." << endl;
    cin >> id;
    for (int i = 0; i < total; i++)
    {
        if (id == person[i].ID)
        {
            cout << "Name: " << person[i].name << endl;
            cout << "ID: " << person[i].ID << endl;
            cout << "Address: " << person[i].address << endl;
            cout << "Contact: " << person[i].contact << endl;
            cout << "Available Cash: " << person[i].cash << endl;
            cout << "1. For Deposit" << endl;
            cout << "2. For Withdraw" << endl;
            ch = getch();
            switch (ch)
            {
            case '1':
                cout << "Enter the amount you want to deposit?" << endl;
                cin >> cash;
                person[i].cash = person[i].cash + cash;
                break;
            case '2':
            back:
                cout << "Enter the amount you want to withdraw?" << endl;
                cin >> cash;
                if (cash > person[i].cash)
                {
                    cout << "Your existing cash is just " << person[i].cash << endl;
                    Sleep(5000);
                    goto back;
                }
                person[i].cash -= cash;
                cout << "Your new cash is: " << person[i].cash << endl;
                break;
            default:
                cout << "Invalid input....." << endl;
                break;
            }
            break;
        }
        if (i == total - 1)
        {
            cout << "No such record found..." << endl;
        }
    }
}

void Bank::del()
{
    char ch;
    cout << "Press 1 to remove specific record: " << endl;
    cout << "Press 1 to remove full record: " << endl;
    ch = getch();
    switch (ch)
    {
    case '1':
        cout << "Enter id of client whose specific data you want to remove: " << endl;
        cin >> id;
        for (int i = 0; i < total; i++)
        {
            if (id == person[i].ID)
            {
                for (int j = i; j < total; j++)
                {
                    person[j].name = person[j + 1].name;
                    person[j].ID = person[j + 1].ID;
                    person[j].address = person[j + 1].address;
                    person[j].contact = person[j + 1].contact;
                    person[j].cash = person[j + 1].cash;
                    total--;
                    break;
                }
            }
            if (i == total - 1)
            {
                cout << "No such record found..." << endl;
            }
        }
        break;
    case '2':
        total = 0;
        cout << "All Record is deleted....." << endl;
        break;
    }
}