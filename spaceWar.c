#include <stdio.h>
#include <termios.h>
#include "getch.h"

#define RESET "\x1b[0m"
#define RED "\x1b[38;2;255;0;0m"
#define GREEN "\x1b[38;2;0;255;0m"
#define YELLOW "\x1b[38;2;255;255;0m"
#define BLUE "\x1b[38;2;0;0;255m"
#define MAGENTA "\x1b[38;2;225;25;225m"
#define CYAN "\x1b[38;2;0;255;255m"
#define WHITE "\x1b[38;2;255;255;255m"
#define GRAY "\x1b[38;2;100;100;100m"
#define ORANGE "\x1b[38;2;255;165;0m"
#define PINK "\x1b[38;2;255;100;190m"

void login_menu();
void sign_up();
void sign_in();



int main()
{
    login_menu();
}

void login_menu()
{
    printf(GREEN "wellcome to space war\n" RESET);
    printf("here you can :\n");
    printf("\t"RED"[1]"RESET"Sign up\n");
    printf("\t"RED"[2]"RESET"Sign in\n");
    printf("\t"RED"[3]"RESET"Exit\n");

    char usin = 'a';
    do
    {
        usin = getch();
    } while (!(usin == '1' || usin == '2' || usin == '3'));
    switch (usin)
    {
    case '1':
        {
            sign_up();
            break;
        }
    
    case '2':
        {
            sign_in();
            break;
        }

    case '3' :
        {
            return;
        }    
    }
}
void sign_up()
{

}
void sign_in()
{

}