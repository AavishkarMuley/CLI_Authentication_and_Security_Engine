#include <iostream>
#include <unordered_map>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include <random>
#include <ctime>
#include<thread>
#include<chrono>


using namespace std;


class Login {
public:
    struct Userdata {
        public:
        string username;
        string salt;
        string hashkey;
    };

    string tolowerstr(string str){
        for(char& c : str){
            c = tolower(c);
        }
        return str;
    }

    unordered_map<string,Userdata> m;
    //Hashing password alone is still insecure as hackers may use rainbow table(standard hash-password pairs).
    //So we add a random string for each user named salt and hasedpassword and salt are both stored in the file
    string generatesalt(){
        string characterset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$*&";
        string salt;
        int randomidx;
        for(int i = 0;i<5;i++){
            randomidx = rand()%characterset.length(); //basically pick any random index from the length of the above character set
            salt += characterset[randomidx];
        }
        return salt;
    }

    //we cannot store the password as it is. So we hash it.
    string hashpassword(string password,string salt){
        string newpass = password + salt;
        unsigned long long hashkey = 5381;
        for(char c : newpass){
            hashkey = ((hashkey << 5) + hashkey) + c;
        }
        return to_string(hashkey);
    }

    bool isstrongpass(string password){
        bool haslen = (password.length() >= 8);
        bool hasdigit = false;
        bool hasupper = false;
        bool haslower = false;
        bool hasspecial = false;
        for(char& ch : password){
            if(isdigit(ch)){
               hasdigit = true;
            }
            else if(isupper(ch)){
               hasupper = true;
            }
            else if(islower(ch)){
                haslower = true;
            }
            else if(!isalnum(ch)){
                hasspecial = true;
            }

        }
        if(haslen){
            cout << "\033[32m \n✅ Length of password should be atleast 8. \033[0m\n" << endl;
        }else{
            cout << "\033[31m \n❌ Length of password should be atleast 8. \033[0m\n" << endl;
        }

        if(hasdigit){
            cout << "\033[32m ✅ It should contain atleast one digit. \033[0m\n" << endl;
        }else{
            cout << "\033[31m ❌ It should contain atleast one digit. \033[0m\n" << endl;
        }

        if(hasupper){
            cout << "\033[32m ✅ It should contain atleast one uppercase character. \033[0m\n" << endl;
        }else{
            cout << "\033[31m ❌ It should contain atleast one uppercase character. \033[0m\n" << endl;
        }

        if(haslower){
            cout << "\033[32m ✅ It should contain atleast one lowercase character. \033[0m\n" << endl;
        }else{
            cout << "\033[31m ❌ It should contain atleast one lowercase character. \033[0m\n" << endl;
        }

        if(hasspecial){
            cout << "\033[32m ✅ It should contain atleast one special character. \033[0m\n" << endl;
        }else{
            cout << "\033[31m ❌ It should contain atleast one special character. \033[0m\n" << endl;
        }

        return (haslen && hasdigit && haslower && hasupper && hasspecial);
    }

    bool verification(int otp){
        //Generate a random 4 digit otp and giving it to check with user input
        int uotp;
        cout << "Enter the otp: " << endl;
        cin >> uotp;
        if(otp == uotp){
            cout << "Correct OTP entered!" << endl;
            cout << "Loading your Account...." << endl;
            Sleep(2000);
            return true;
        }
        else {
            cout << "Incorrect OTP" << endl;
            Sleep(2000);
            return false;
        }

    }

    void createaccount(string username,string salt,string hashkey){
        ofstream file("login.txt",ios::app);  //ios::app becoz we dont want to delete old users
        file << username << "|" << salt << "|" << hashkey << endl;
        cout << "Account created successfully" << endl;
        cout << "\n\n====================================================" << endl;
        cout << "\t\tWelcome " << username << "!" << endl;
        cout << "====================================================" << endl;
        m[username] = {username,salt,hashkey};
        file.close();
    }

    void finduser(string username, string password){
        ifstream authfile("login.txt");
        string un,salt,hashkey;
        bool founduser = false;
        if(authfile){
            while(getline(authfile,un,'|') && getline(authfile,salt,'|') && getline(authfile,hashkey)){
                if(tolowerstr(un) == tolowerstr(username)){
                    founduser = true;
                    string newhashkey = hashpassword(password,salt);
                    int attempts = 3;
                    while(attempts > 0){
                        if(newhashkey == hashkey){
                            cout << "\nUser found" << endl;
                            int otp = (rand()%9000) + 1000;
                            Sleep(1000);
                            cout << "Your OTP for login is: " << otp << endl;
                            if(verification(otp)){
                                cout << "\n\n====================================================" << endl;
                                cout << "\t\tWelcome " << username << "!" << endl;
                                cout << "====================================================" << endl;
                                cout << "\nPassword : " << password << endl;
                                m[username] = {un,salt,hashkey};
                            }
                            authfile.close();
                            usermenu(username);
                            return;
                        }
                        else{
                            cout << "\nInvalid login credentials.\n" << endl;
                            cout << attempts << " attempt(s) left\n" << endl;
                            cout << "Try entering password again!" << endl;
                            password = "";
                            while(true){
                                char ch = _getch();
                                //\r is the carriage return or escape character and \n is new line character
                                if(ch == '\r' || ch == '\n'){
                                    break;
                                }
                                //8 is the ascii code for backspace
                                else if(ch == 8){
                                    if(!password.empty()){
                                        password.pop_back();
                                        cout << "\b \b";
                                    }
                                }
                                else{
                                    password += ch;
                                    cout << "*";
                                }
                            }
                            newhashkey = hashpassword(password,salt);
                            attempts--;
                        }
                    }
                    if(attempts == 0){
                        cout << "\n \033[31m ❌Too many failed attempts!\nAccount locked for 15 seconds.\033[0m" << endl;
                        Sleep(15000);
                    }
                }
            }
        }
        if(!founduser) {
            cout << "\nAccount not found.\nCreate a new account" << endl;
            while(!isstrongpass(password)){
                    cout << "Enter a stronger password!" << endl;
                    password = "";
                    while(true){
                        char ch = _getch();
                        //\r is the carriage return or escape character and \n is new line character
                        if(ch == '\r' || ch == '\n'){
                            break;
                        }
                        //8 is the ascii code for backspace
                        else if(ch == 8){
                            if(!password.empty()){
                                password.pop_back();
                                cout << "\b \b";
                            }
                        }
                        else{
                            password += ch;
                            cout << "*";
                        }
                    }
            }
            string newsalt = generatesalt();
            string newhashkey = hashpassword(password, newsalt);
            createaccount(username,newsalt,newhashkey);
        }

    }

    void changepass(string username){
        ifstream file("login.txt");
        string u,s,h,password;
        cout << "Enter your current password!" << endl;
        while(true){
            char ch = _getch();
            //\r is the carriage return or escape character and \n is new line character
            if(ch == '\r' || ch == '\n'){
                break;
            }
            //8 is the ascii code for backspace
            else if(ch == 8){
                if(!password.empty()){
                    password.pop_back();
                    cout << "\b \b";
                }
            }
             else{
                 password += ch;
                 cout << "*";
             }
        }
        string newpass,newpass2;
        while((getline(file,u,'|')) && (getline(file,s,'|')) && (getline(file,h))){
            string newhashkey = hashpassword(password,s);
            if(newhashkey == h){
                cout << "\nUser confirmed" << endl;
                cout << "\nEnter new password" << endl;
                while(true){
                    char ch = _getch();
                    //\r is the carriage return or escape character and \n is new line character
                    if(ch == '\r' || ch == '\n'){
                        break;
                    }
                    //8 is the ascii code for backspace
                    else if(ch == 8){
                        if(!newpass.empty()){
                            newpass.pop_back();
                            cout << "\b \b";
                        }
                    }
                     else{
                         newpass += ch;
                         cout << "*";
                     }
                }
                cout << "\nEnter new password again to confirm" << endl;
                while(true){
                    char ch = _getch();
                    //\r is the carriage return or escape character and \n is new line character
                    if(ch == '\r' || ch == '\n'){
                        break;
                    }
                    //8 is the ascii code for backspace
                    else if(ch == 8){
                        if(!newpass2.empty()){
                            newpass2.pop_back();
                            cout << "\b \b";
                        }
                    }
                     else{
                         newpass2 += ch;
                         cout << "*";
                     }
                }
                if(newpass == newpass2){
                    ifstream file1("login.txt");
                    char ans;
                    cout << "\n\033[32m ✅ Password matches. \033[0m\n" << endl;
                    cout << "\nConfirmation : Change password?(Y/N) " << endl;
                    cin >> ans;
                    if(tolower(ans) == 'y'){
                        ofstream tempfile("tempfile.txt");
                        while((getline(file1,u,'|')) && (getline(file1,s,'|')) && (getline(file1,h))){
                            if(u == username){
                                string ns = generatesalt();
                                string newhash = hashpassword(newpass,ns);
                                tempfile << u << "|" << ns << "|" << newhash << endl;
                            }
                            else{
                                tempfile << u << "|" << s << "|" << h << endl;
                            }
                        }
                        tempfile.close();
                        file1.close();
                        file.close();
                        remove("login.txt");
                        rename("tempfile.txt" , "login.txt");
                        cout << "\n\033[32m✅ Password changed successfully.\033[0m" << endl;
                        return;

                    }
                    else{
                        cout << "\033[31mChange password process cancelled. \033[0m" << endl;
                        return;
                    }
                }
                else {
                    cout << "\033[31m❌ Password does not match. \033[0m\n" << endl;
                    return;

                }
            }
        }
    }

    void deleteuser(string username){
        ifstream file("login.txt");
        string u,s,h,password;
        bool verified = false;
        cout << "Enter password to confirm account deletion" << endl;
        while(true){
            char ch = _getch();
            //\r is the carriage return or escape character and \n is new line character
            if(ch == '\r' || ch == '\n'){
                break;
            }
            //8 is the ascii code for backspace
            else if(ch == 8){
                if(!password.empty()){
                    password.pop_back();
                    cout << "\b \b";
                }
            }
             else{
                 password += ch;
                 cout << "*";
             }
        }
        while(getline(file,u,'|') && getline(file,s,'|') && getline(file,h)){
            string newhashk = hashpassword(password,s);
            if(newhashk == h){
                cout << "User confirmed" << endl;
                verified = true;
                break;
            }
            file.close();
            if(!verified){
                cout << "Incorrect password" << endl;
            }
            char ans;
            cout << "Confirm account deletion.\n\033[31mThis will permanently delete your account!\033[0m. (Y/N)" << endl;
            cin >> ans;
            ifstream infile("login.txt");
            ofstream tempfile("temp.txt");
            if(tolower(ans) == 'y'){
                while((getline(infile,u,'|')) && (getline(infile,s,'|')) && (getline(infile,h))){
                    if(u == username){
                        continue;
                    }
                    tempfile << u << "|" << s << "|" << h << endl;
                }
                tempfile.close();
                infile.close();
                remove("login.txt");
                rename("temp.txt" , "login.txt");
                m.erase(username);
                cout << "\033[32mUser deleted successfully.\033[0m" << endl;
                return;
            }
            else{
                cout << "Account Deletion cancelled!" << endl;
                return;
            }
        }

    }


    void usermenu(string username){
        int option;
        cout << "\n\nWhat do want to do, " << username << endl;
        while(true){
            cout << "1 - Change password.\n2 - Delete account.\n3 - Logout." << endl;
            cin >> option;
            if(option == 1){
                changepass(username);
                break;
            }
            else if(option == 2){
                deleteuser(username);
                break;
            }
            else if(option == 3){
                cout << "Logged Out successfully!\nVisit again!" << endl;
                break;
            }
            else {
                cout << "Invalid option selected!\nChoose again" << endl;
            }
        }
    }
};


int main()
{
    Login ln;
    SetConsoleOutputCP(CP_UTF8);
    srand(time(0));

    string username,password;
    cout << "====================================================" << endl;
    cout << "\t\tAuth System" << endl;
    cout << "====================================================" << endl;
    cout << "Enter your Username" << endl;
    getline(cin ,username);
    cout << "Enter your Password" << endl;
    cout << "Length of password should be atleast 8.\nIt should contain atleast one digit.\nIt should contain atleast one special character.\nIt should contain atleast one uppercase character." << endl;
    while(true){
        char ch = _getch();
        //\r is the carriage return or escape character and \n is new line character
        if(ch == '\r' || ch == '\n'){
            break;
        }
        //8 is the ascii code for backspace
        else if(ch == 8){
            if(!password.empty()){
                password.pop_back();
                cout << "\b \b";
            }
        }
        else{
            password += ch;
            cout << "*";
        }
    }
    ln.finduser(username,password);


    return 0;
}
