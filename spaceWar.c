#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "getch.h"

//printf("\e[?25l"); -> hide cursor
//printf("\e[?25h"); -> re-enabel cursor

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




#define MYXMAX 110
#define MYYMAX 24

struct spaceship 
{
    int curx;
    int cury;

    int perx;
    int pery;

    char color; /*red(r) or blue(b)*/
};
typedef struct spaceship spaceship;
spaceship sp1 = {0,0,0,0,'r'};
spaceship sp2 = {0,0,0,0,'b'};

struct person
{
    char* userName;
    char* password;
    char* email;
};
typedef struct person person;

struct object
{
    char hardship;
    char type;
    int x;
    int y;

};
typedef struct object object;
// hardship : s -> soft  h -> hard
//type : s -> spaceship   w -> wall  m -> mirror   b -> black hole   h -> heart   t -> teleporter

object obj[MYXMAX + 1][MYYMAX + 1];

void login_menu();
void sign_up();
void sign_in();
void gotoxy(int x,int y);
void play_game(int numerOfMap);
void print_map1();
void set_obj();
void makeItWall(int x,int y);
void makeItBlackHole();
void makeItHeart(int x, int y);
void makeItTeleporter(int x , int y);
void move_sp(char move);


int main()
{
    //printf("\e[?25l");
    print_map1();
    set_obj();
    play_game(1);
    //printf("\e[?25h");
}

void makeItTeleporter(int x , int y)
{
    gotoxy(x,y);
    obj[x][y].hardship = 's';
    obj[x][y].type = 't';
    printf(YELLOW"\u2742"RESET);
}
void makeItGodMirror(int x,int y)
{
    gotoxy(x,y);
    obj[x][y].hardship = 'h';
    obj[x][y].type = 'm';
    printf("\u2551");
}
void makeItWall(int x,int y)
{
    gotoxy(x,y);
    obj[x][y].hardship = 'h';
    obj[x][y].type = 'w';
    printf("\u2588");
}
void makeItBlackHole(int x,int y)
{
    gotoxy(x,y);
    obj[x][y].hardship = 's';
    obj[x][y].type = 'b';
    printf("\u0B6D");
}
void makeItHeart(int x, int y)
{
    gotoxy(x,y);
    obj[x][y].hardship = 's';
    obj[x][y].type = 'h';
    printf(RED"\u2665"RESET);
}
void set_obj()
{
    for (int i = 0; i <= MYXMAX; i++)
    {
        for (int j = 0; j <= MYYMAX; j++)
        {
            obj[i][j].x = i;
            obj[i][j].y = j;
            obj[i][j].hardship = 's';
            obj[i][j].type = 'n';
        }
    }
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
    FILE* file = fopen("accounts.bin","wb+");
    if (file == NULL)
    {
        printf(RED "could NOT open file\n"RESET);
        exit(-1);
    }
    system("clear");
    person tempP ;
    printf(GREEN"Sign up ... \n"RESET);

    printf("please enter your user name ");
    printf(RED"max 20 character "RESET);
    scanf("%s",tempP.userName);

    printf("please enter your email ");
    printf(RED"max 20 character "RESET);
    scanf("%s",tempP.email);

    printf("please enter your password ");
    printf(RED"max 20 character "RESET);
    scanf("%s",tempP.password);

    fwrite(&tempP, sizeof(struct person), 1, file);
    
}
int is_valid_email(char *str) {
    if (strstr(str, "@") == NULL)
        return 0; 
    if (strstr(str, ".") == NULL)
        return 0; 
    if (strlen(str) > 20)
        return 0; 
    return 1;
}
void sign_in()
{

}
void gotoxy(int x,int y)
{
    x++;
    y++;
  printf("%c[%d;%df",0x1B,y,x);
}

void play_game(int numerOfMap)
{
    if (numerOfMap == 1)
    {
        print_map1();
        
        /*setup spaceshipe 1*/
        sp1.curx = 12;
        sp1.cury = 11;
        sp1.perx = 12;
        sp1.pery = 11;
        sp1.color = 'b';
        gotoxy(12,11);
        printf(BLUE"\u25A3"RESET);

        /*setup spaceshipe 2*/
        sp2.curx = 98;
        sp2.cury = 11;
        sp2.perx = 98;
        sp2.pery = 11;
        sp2.color = 'r';
        gotoxy(98,11);
        printf(RED"\u25A3"RESET);

        char input = getch();
        
        while (input != 'q')
        {

            move_sp(input);
            input = getch();
        }
    }
    
}
void move_sp(char move)
{
    if (move == 'w')
    {
        // printf("%d new %d\n",sp1.curx,sp1.cury);
        // printf("%d %d\n",obj[sp1.curx][sp1.cury - 1].x,obj[sp1.curx][sp1.cury - 1].y);
        // printf("%c\n",obj[12][10].type);


        //if(obj[sp1.curx][sp1.cury - 1].type == 'n')
        //{      
            sp1.perx = sp1.curx;
            sp1.pery = sp1.cury;
            sp1.cury -= 1;
            gotoxy(sp1.perx + 1 ,sp1.pery);
            printf("\b ");       
            gotoxy(sp1.curx + 1, sp1.cury);  
            printf("\b");
            gotoxy(sp1.curx, sp1.cury);  
            printf(BLUE"\u25A3"RESET);
            
        //}
    }
    else if (move == 'a')
    {
        //if(obj[sp1.curx][sp1.cury - 1].type == 'n')
        //{      
            sp1.perx = sp1.curx;
            sp1.pery = sp1.cury;
            sp1.curx -= 1;
            gotoxy(sp1.perx + 1,sp1.pery);
            printf("\b ");       
            gotoxy(sp1.curx + 1, sp1.cury);  
            printf("\b");
            gotoxy(sp1.curx, sp1.cury);               
            printf(BLUE"\u25A3"RESET);
        //}
    }
    else if (move == 's')
    {
        //if(obj[sp1.curx][sp1.cury - 1].type == 'n')
        //{      
            sp1.perx = sp1.curx;
            sp1.pery = sp1.cury;
            sp1.cury += 1;
            gotoxy(sp1.perx + 1,sp1.pery);
            printf("\b ");  
            gotoxy(sp1.curx + 1, sp1.cury);  
            printf("\b");     
            gotoxy(sp1.curx, sp1.cury);               
            printf(BLUE"\u25A3"RESET);
        //}
    }
    else if (move == 'd')
    {
        //if(obj[sp1.curx][sp1.cury - 1].type == 'n')
        //{      
            sp1.perx = sp1.curx;
            sp1.pery = sp1.cury;
            sp1.curx += 1;
            gotoxy(sp1.perx + 1,sp1.pery);
            printf("\b ");
            gotoxy(sp1.curx + 1, sp1.cury);  
            printf("\b");       
            gotoxy(sp1.curx, sp1.cury);               
            printf(BLUE"\u25A3"RESET);
    }
    else if (move == 'i')
    {
        //if(obj[sp1.curx][sp1.cury - 1].type == 'n')
        //{      
            sp2.perx = sp2.curx;
            sp2.pery = sp2.cury;
            sp2.cury -= 1;
            gotoxy(sp2.perx + 1 ,sp2.pery);
            printf("\b ");   
            gotoxy(sp2.curx + 1, sp2.cury);  
            printf("\b");    
            gotoxy(sp2.curx, sp2.cury);               
            printf(RED"\u25A3"RESET);
            
        //}
    }
    else if (move == 'l')
    {
        //if(obj[sp1.curx][sp1.cury - 1].type == 'n')
        //{      
            sp2.perx = sp2.curx;
            sp2.pery = sp2.cury;
            sp2.curx += 1;
            gotoxy(sp2.perx + 1 ,sp2.pery);
            printf("\b ");   
            gotoxy(sp2.curx + 1, sp2.cury);  
            printf("\b");    
            gotoxy(sp2.curx, sp2.cury);               
            printf(RED"\u25A3"RESET);
        //}
    }
    else if (move == 'k')
    {
        //if(obj[sp1.curx][sp1.cury - 1].type == 'n')
        //{      
            sp2.perx = sp2.curx;
            sp2.pery = sp2.cury;
            sp2.cury += 1;
            gotoxy(sp2.perx + 1 ,sp2.pery);
            printf("\b ");   
            gotoxy(sp2.curx + 1, sp2.cury);  
            printf("\b");    
            gotoxy(sp2.curx, sp2.cury);               
            printf(RED"\u25A3"RESET);
        //}
    }
    else if (move == 'j')
    {
        //if(obj[sp1.curx][sp1.cury - 1].type == 'n')
        //{      
            sp2.perx = sp2.curx;
            sp2.pery = sp2.cury;
            sp2.curx -= 1;
            gotoxy(sp2.perx + 1 ,sp2.pery);
            printf("\b ");   
            gotoxy(sp2.curx + 1, sp2.cury);  
            printf("\b");    
            gotoxy(sp2.curx, sp2.cury);               
            printf(RED"\u25A3"RESET);
        //}
    }
    
}
void print_map1()
{    
    system("clear");
    /*printing up and down walls ... */
    for (int i = 0; i < MYXMAX; i += 1)
    {
        makeItWall(i,0);
    }
    for (int i = 0; i < MYXMAX; i += 1)
    {
        makeItWall(i,MYXMAX);
    }

    
    /*printing side walls*/

    for (int j = 0; j < MYYMAX; j++)
    {
        makeItWall(0,j);
    }  
    for (int j = 0; j < MYYMAX; j++)
    {
        makeItWall(MYXMAX,j);
    }  

    /*printing iner walls*/

    for (int i = 12; i < 21; i += 1)
    {
        makeItWall(i,6);
    }
    for (int j = 6; j < 17; j++)
    {
        makeItWall(20,j);
    }
    for (int i = 12; i < 21; i += 1)
    {
        makeItWall(i,16);
    }
    makeItGodMirror(21,7);
    makeItGodMirror(21,8);

    makeItGodMirror(21,14);
    makeItGodMirror(21,15);

    makeItWall(32 , 3);
    makeItWall(32 , 4);

    makeItGodMirror(33 , 3);
    makeItGodMirror(33 , 4);

    makeItWall(32 , 20);
    makeItWall(32 , 19);

    makeItBlackHole(35,16);

    for (int i = 40; i < 47; i++)
    {
        makeItWall(i,13);
    }  
    for (int j = 7; j < 16; j++)
    {
        makeItWall(55,j);
    }
    makeItGodMirror(55,11);
    for (int i = 64; i < 70; i++)
    {
        makeItWall(i,8);
    }
    for (int i = 90; i < 99; i += 1)
    {
        makeItWall(i,6);
    }
        for (int j = 6; j < 17; j++)
    {
        makeItWall(90,j);
    }
    for (int i = 90; i < 99; i += 1)
    {
        makeItWall(i,16);
    }
    makeItWall(78 , 3);
    makeItWall(78 , 4);  

    makeItWall(78 , 19);
    makeItWall(78 , 20);
    makeItGodMirror(77,19);
    makeItGodMirror(77,20);

    makeItGodMirror(89,7);
    makeItGodMirror(89,8);
    makeItGodMirror(89,14);
    makeItGodMirror(89,15);

    makeItHeart(55,5);
    makeItBlackHole(85,4);

    makeItTeleporter(99,20);
    makeItTeleporter(11,20);
}

