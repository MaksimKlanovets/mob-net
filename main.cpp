#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/select.h>

using namespace std;

int kbhit(void);
int getch(void);

struct COMAND
{
   const string c_register = "register";//arg
   const string c_unregister = "unregister";
   const string c_call = "call";//arg
   const string c_name = "name";//arg
   const string c_answer = "answer";
   const string c_reject = "reject";
   const string c_callEnd = "callEnd";
   const string c_exit = "exit";    
};

int main(){
COMAND menu;

char a = {};
string tempComand ={};
string tempArg = {};
int cnt = {0} ;

while(true)
{
            
    if(!kbhit() )
    {
        a = getch();     
                 
        if (a != '\n' && cnt == 1)
        {
            if (a == 127)
            {
                if (!tempArg.empty())
                {
                   tempArg.pop_back();   
                   system("clear");
                   cout << "string arg now " << tempArg << endl;
                   continue;
                }                    
            }
        tempArg.push_back(a);
        system("clear");
        cout << "string arg now " << tempArg << endl;
        }

        if (a != '\n' && cnt == 0)
        {
             if (a == 127)
            {
                if (!tempComand.empty())
                {
                   tempComand.pop_back(); 
                   system("clear");  
                   cout << "string command now " << tempComand << endl;
                   continue;
                }                    
            }

            if(a == ' ')
            {
                cnt++;
                continue;
            } 
            tempComand.push_back(a);
            system("clear");
            cout << "string command now " << tempComand << endl;
        }
      
        if (a == '\n')
        {            
            if (tempComand == menu.c_register )
            {
                if(tempArg.empty())
                {
                cout << "register must containg an argument" << endl;
                cnt =0;
                tempComand.clear();
                continue;
                }
                cout << "register -> " << tempComand << "  Arg -> " << tempArg << endl;
            }
        }
        else if (tempComand == menu.c_unregister )
        {
            cout << "unregister" <<endl;
        }
        else if (tempComand == menu.c_call )
        {
            if (tempArg.empty())
            {
                cout << "call must contain an argument " << endl;
                cnt = 0;
                tempComand.clear();
                continue;
            }            
            cout << "call" <<tempComand << "  Arg -> " << tempArg << endl;
        }
        else if (tempComand == menu.c_name )
        {
            if (tempArg.empty())
            {
                cout<< "name must contain an argument " << endl;
                cnt = 0;
                tempComand.clear();
                continue;
            }
            
            cout << "name" << tempComand << "  Arg -> " << tempArg << endl;
        }
        else if (tempComand == menu.c_answer )
        {
            cout << "answer" <<endl;
        }
        else if (tempComand == menu.c_reject )
        {
            cout << "reject" <<endl;
        }
        else if (tempComand == menu.c_callEnd )
        {
            cout << "call end" <<endl;
        }
        else if (tempComand == menu.c_exit )
        {
            cout << "exit" <<endl;
            return 0;
        }
        
    }
      
}
  return 0;
}

int kbhit(void)
{
  struct timeval tv;
  fd_set read_fd;

  /* Do not wait at all, not even a microsecond */
  tv.tv_sec=0;
  tv.tv_usec=0;

  /* Must be done first to initialize read_fd */
  FD_ZERO(&read_fd);

  /* Makes select() ask if input is ready:
   * 0 is the file descriptor for stdin    */
  FD_SET(0,&read_fd);

  /* The first parameter is the number of the
   * largest file descriptor to check + 1. */
  if(select(1, &read_fd,NULL, /*No writes*/NULL, /*No exceptions*/&tv) == -1)
    return 0;  /* An error occured */

  /*  read_fd now holds a bit map of files that are
   * readable. We test the entry for the standard
   * input (file 0). */
  
if(FD_ISSET(0,&read_fd))
    /* Character pending on stdin */
    return 1;

  /* no characters were pending */
  return 0;
}

int  getch(void) {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
