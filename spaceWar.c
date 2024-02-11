#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
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

struct bullet
{
    char direction; /*u : up    d : down    l : left    r : right   n : nothing*/
    int x;
    int y;
};
typedef struct bullet bullet;

bullet b1 = {'n' , -1 , -1};
bullet b2 = {'n' , -1 , -1};

struct spaceship 
{
    int curx;
    int cury;

    int perx;
    int pery;
};
typedef struct spaceship spaceship;
spaceship sp1 = {0,0,0,0};
spaceship sp2 = {0,0,0,0};

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
void set_bul1();
void set_bul2();
int is_valid_email(char *str);
void* move_bul1();
void* move_bul2();

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;

int main()
{
    // pthread_mutex_init(&mutex1, NULL);
    // pthread_mutex_init(&mutex2, NULL);

    // pthread_t move_bullet1;
    // pthread_t move_bullet2;

    // pthread_create(&move_bullet1, NULL, move_bul1, NULL);
    // pthread_create(&move_bullet2, NULL, move_bul2, NULL);

    // printf("\e[?25l");
    // print_map1();
    // set_obj();
    // play_game(1);


    // printf("\e[?25h");

    // pthread_join(move_bullet1, NULL);
    // pthread_join(move_bullet2, NULL);

    // pthread_mutex_destroy(&mutex1);
    // pthread_mutex_destroy(&mutex2);
    login_menu();

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
        gotoxy(12,11);
        printf(BLUE"\u25A3"RESET);

        /*setup spaceshipe 2*/
        sp2.curx = 98;
        sp2.cury = 11;
        sp2.perx = 98;
        sp2.pery = 11;
        gotoxy(98,11);
        printf(RED"\u25A3"RESET);
        
        

        char input = getch();
        while (input != 'q')
        {
            if(input == 'w' || input == 'a' || input == 's' || input == 'd' || input == 'i' || input == 'l' || input == 'k' || input == 'j')
            {
                move_sp(input);
            }
            else if (input == 'c' && b1.direction == 'n')
            {
                set_bul1();
            }
            else if(input == 'n' && b2.direction == 'n')
            {
                set_bul2();
            }
            input = getch();
        }
        
    }
    
}
void set_bul1()
{
    int deltax;
    int deltay;    
    b1.x = sp1.curx;
    b1.y = sp1.cury;
    deltax = sp1.curx - sp1.perx;
    deltay = sp1.cury - sp1.pery;
    if (deltax == 1)
    {
        b1.direction = 'r';
        b1.x++;
    }
    else if (deltax == -1)
    {
        b1.direction = 'l';
        b1.x--;
    }
    else if (deltay == 1)
    {
        b1.direction = 'd';
        b1.y++;
    }
    else if (deltay == -1)
    {
        b1.direction = 'u';
        b1.y--;
    }
}
void set_bul2()
{
    int deltax;
    int deltay;    
    b2.x = sp2.curx;
    b2.y = sp2.cury;
    deltax = sp2.curx - sp2.perx;
    deltay = sp2.cury - sp2.pery;
    if (deltax == 1)
    {
        b2.direction = 'r';
        b2.x++;
    }
    else if (deltax == -1)
    {
        b2.direction = 'l';
        b2.x--;    
    }
    else if (deltay == 1)
    {
        b2.direction = 'd';
        b2.y++;        
    }
    else if (deltay == -1)
    {
        b2.direction = 'u';
        b2.y--;
    }
}
void* move_bul1()
{
    
    while(1)
    {   
        pthread_mutex_lock(&mutex1); // Lock the mutex
        if(b1.direction == 'u')
        {
            
            while (1)
            {
                gotoxy(b1.x ,b1.y);
                printf(BLUE"\u2022"RESET);
                fflush(stdout);
                usleep(100000);
                gotoxy(b1.x + 1, b1.y);
                printf("\b ");
                fflush(stdout);
                b1.y -= 1;
                if (obj[b1.x][b1.y].hardship == 'h')
                {
                    b1.direction = 'n';
                    break;
                }
                
            }
            b1.direction = 'n';
            
        }
        else if(b1.direction == 'd')
        {
            while (1)
            {
                gotoxy(b1.x ,b1.y);
                printf(BLUE"\u2022"RESET);
                fflush(stdout);
                usleep(100000);
                gotoxy(b1.x + 1, b1.y);
                printf("\b ");
                fflush(stdout);
                b1.y += 1;
                if (obj[b1.x][b1.y].hardship == 'h')
                {
                    b1.direction = 'n';
                    break;
                }
                
            }
            b1.direction = 'n';
        }
        else if(b1.direction == 'l')
        {
            while (1)
            {
                gotoxy(b1.x ,b1.y);
                printf(BLUE"\u2022"RESET);
                fflush(stdout);
                usleep(100000);
                gotoxy(b1.x + 1, b1.y);
                printf("\b ");
                fflush(stdout);
                b1.x -= 1;
                if (obj[b1.x][b1.y].hardship == 'h')
                {
                    b1.direction = 'n';
                    break;
                }
                
            }
            b1.direction = 'n';
        }
        else if(b1.direction == 'r')
        {
            while (1)
            {
                gotoxy(b1.x ,b1.y);
                printf(BLUE"\u2022"RESET);
                fflush(stdout);
                usleep(100000);
                gotoxy(b1.x + 1, b1.y);
                printf("\b ");
                fflush(stdout);
                b1.x += 1;
                if (obj[b1.x][b1.y].hardship == 'h')
                {
                    b1.direction = 'n';
                    break;
                }
                
            }
            b1.direction = 'n';
        }
        pthread_mutex_unlock(&mutex1);
    }
    
    return NULL;
}

void* move_bul2()
{
    while(1)
    {   
        pthread_mutex_lock(&mutex2); // Lock the mutex

        // Perform operations for move_bul2()

        pthread_mutex_unlock(&mutex2); // Unlock the mutex
    }
    return NULL;

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
    //person tempP ;
    person* tmpPtr = (person*)malloc(sizeof(person)); 
    printf(GREEN"Sign up ... \n"RESET);

    printf("please enter your user name ");
    printf(RED"max 20 character "RESET);
    scanf("%s",tmpPtr->userName);

    do
    {
        printf("please enter your email ");
        printf(RED"max 20 character MUST INCLUDE @ and ."RESET);
        scanf("%s",tmpPtr->email);
    } while (!(is_valid_email(tmpPtr->email)));

    printf("please enter your password ");
    printf(RED"max 20 character "RESET);
    scanf("%s",tmpPtr->password);

    fwrite(tmpPtr->userName, sizeof(char), strlen(tmpPtr->userName), file);
    fwrite(tmpPtr->email, sizeof(char), strlen(tmpPtr->email), file);
    fwrite(tmpPtr->password, sizeof(char), strlen(tmpPtr->password), file);
    fclose(file);
    
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