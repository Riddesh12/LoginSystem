
/*
For Running the code:-
Install the MySQL client development libraries:

Copy code for Terminal:-

sudo apt update
sudo apt install libmysqlclient-dev

Compile the code:-

g++ -std=c++11 login_system.cpp -o login_system -lmysqlclient

Run the program:

./login_system
*/
#include <iostream>
#include <mysql.h>
#include <mysqld_error.h>
#include <chrono>
#include <thread>
using namespace std;

const char *HOST = "localhost";
const char *USER = "root";
const char *PW = "your_password";
const char *DB = "mydb";

class Login
{
private:
    string userId, userPW;

public:
    Login() : userId(""), userPW("") {}

    void setId(string id)
    {
        userId = id;
    }

    void setPW(string pw)
    {
        userPW = pw;
    }

    string getId() const
    {
        return userId;
    }

    string getPW() const
    {
        return userPW;
    }
};

char encryptCh(char ch, int shift)
{
    if (isalpha(ch))
    {
        char base = isupper(ch) ? 'A' : 'a';
        char encrypted_ch = (ch - base + shift + 26) % 26 + base;
        return encrypted_ch;
    }
    if (isdigit(ch))
    {
        char encrypted_ch = (ch - '0' + shift + 10) % 10 + '0';
        return encrypted_ch;
    }
    return ch;
}

string encrypt(const string &password, int shift)
{
    string encrypted = "";
    for (int i = 0; i < password.length(); i++)
    {
        char ch = password[i];
        char encryptedChar = encryptCh(ch, shift);
        encrypted += encryptedChar;
    }
    return encrypted;
}

char decryptCh(char ch, int shift)
{
    if (isalpha(ch))
    {
        char base = isupper(ch) ? 'A' : 'a';
        char decrypted_ch = (ch - base - shift + 26) % 26 + base;
        return decrypted_ch;
    }
    else if (isdigit(ch))
    {
        char decrypted_ch = (ch - '0' - shift + 10) % 10 + '0';
        return decrypted_ch;
    }
    return ch;
}

string decrypt(const string &encrypted, int shift)
{
    string decrypted = "";
    for (size_t i = 0; i < encrypted.length(); i++)
    {
        char ch = encrypted[i];
        char decryptedChar = decryptCh(ch, shift);
        decrypted += decryptedChar;
    }
    return decrypted;
}

string DBpw(MYSQL *conn, const string &id)
{
    string encryptedPW;

    string get = "SELECT PW FROM password WHERE Id='" + id + "'";
    if (mysql_query(conn, get.c_str()))
    {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else
    {
        MYSQL_RES *res;
        res = mysql_store_result(conn);
        if (res)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row)
            {
                encryptedPW = row[0];
            }
        }
    }
    return encryptedPW;
}

// Cross-platform replacements
void clearConsole()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void delay(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int main()
{
    Login l;

    MYSQL *conn;
    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, HOST, USER, PW, DB, 3306, NULL, 0))
    {
        cout << "Error: " << mysql_error(conn) << endl;
        return 1;
    }
    else
    {
        cout << "Logged In Database!" << endl;
    }
    delay(3000);
    int shift = 3;

    bool exit = false;
    while (!exit)
    {
        clearConsole();
        cout << "1. Signup." << endl;
        cout << "2. Login." << endl;
        cout << "0. Exit." << endl;
        cout << "Enter Your Choice: ";
        int val;
        cin >> val;

        if (val == 1)
        {
            clearConsole();
            string id, pw;
            cout << "Enter ID For Signup: ";
            cin >> id;
            l.setId(id);
            cout << "Enter A Strong Password: ";
            cin >> pw;
            l.setPW(pw);

            string encryptedPW = encrypt(l.getPW(), shift);

            string Sup = "INSERT INTO password (Id, PW) VALUES ('" + l.getId() + "', '" + encryptedPW + "')";
            if (mysql_query(conn, Sup.c_str()))
            {
                cout << "Error: " << mysql_error(conn) << endl;
            }
            else
            {
                cout << "Signup Successfully" << endl;
            }
            delay(3000);
        }
        else if (val == 2)
        {
            clearConsole();
            string id, pw;
            cout << "Enter ID: ";
            cin >> id;
            cout << "Enter Your Password: ";
            cin >> pw;

            string getDB = DBpw(conn, id);

            if (!getDB.empty())
            {
                string decryptedPW = decrypt(getDB, shift);

                if (decryptedPW == pw)
                {
                    cout << "Welcome!" << endl;
                }
                else
                {
                    cout << "Incorrect Password. Try Again!" << endl;
                }
            }
            else
            {
                cout << "User ID not found. Try Again!" << endl;
            }

            delay(5000);
        }
        else if (val == 0)
        {
            exit = true;
            cout << "Bye!" << endl;
        }
        else
        {
            cout << "Invalid Input" << endl;
        }
    }

    mysql_close(conn);
    return 0;
}
