#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "getch.h"
#include "kbhit.h"

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

int game_running = 1;
int cur_map = 0;
struct bullet
{
    char direction; /*u : up    d : down    l : left    r : right   n : nothing*/
    int x;
    int y;
    int remainBoost;
};
typedef struct bullet bullet;

bullet b1 = {'n' , -1 , -1,0};
bullet b2 = {'n' , -1 , -1,0};

struct person
{
    char* userName[21];
    char* password[21];
    char* email[21];
    int win;
    int lose;
};
typedef struct person person;

struct spaceship 
{
    int startPositionX;
    int startPositiony;

    int heart;
    int curx;
    int cury;

    int perx;
    int pery;

    person* Person;
};
typedef struct spaceship spaceship;
spaceship sp1 = {0, 0, 5, 0, 0, 0, 0, NULL};
spaceship sp2 = {0, 0, 5, 0, 0, 0, 0, NULL};



struct object
{
    char hardship;
    char type;
    int x;
    int y;

};
typedef struct object object;
// hardship : s -> soft  h -> hard
//type : s -> spaceship   w -> wall  m -> mirror   b -> black hole   h -> heart   t -> teleporter   g -> goust  o -> optimizer

object obj[MYXMAX + 1][MYYMAX + 1];

void login_menu();
void get_person_from_file(person* Pptr);
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
void makeItGhost(int x, int y);
void makeItOptimizer(int x, int y);
void move_sp(char move);
void set_bul1();
void set_bul2();
int is_valid_email(char *str);
void* move_bul1();
void* move_bul2();
void* check_ruls1();
void* check_ruls2();
void sp2_wins(int map);
void sp1_wins(int map);
void refresh_sp1_heart(int mapNumber);
void refresh_sp2_heart(int mapNumber);
void print_map2();



pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;


int main()
{
    printf("\e[?25l");
    
    play_game(1);

    //login_menu();

    printf("\e[?25h");
    exit(1);
}
int is_1_dead()
{
    if (sp1.heart <= 0)
    {
        game_running=0;
        system("clear");
        printf("SP1 is dead.\n");
        return 1;
    }   
}
int is_2_dead()
{
    if (sp2.heart <= 0)
    {
        game_running=0;
        system("clear");
        printf("SP2 is dead.\n");
        return 1;
    }   
}
void refresh_sp2_heart(int mapNumber)
{
    gotoxy(115,12);
    printf(RED);
    printf("red player hearts : %d",sp2.heart);
    printf(RESET);
    fflush(stdout);
}
void refresh_sp1_heart(int mapNumber)
{
    gotoxy(115,10);
    printf(BLUE);
    printf("blue player hearts : %d",sp1.heart);
    printf(RESET);
    fflush(stdout);

}
void sp2_wins(int map)
{
    if (map == 1)
    {
        system("clear");
        printf(RED"red player wins the game\n"RESET);
        sleep(2);
        play_game(2);
    }
}
void sp1_wins(int map)
{
    if (map == 1)
    {
        system("clear");
        printf(BLUE"blue player wins the game\n"RESET);
        sleep(2);
        play_game(2);
    }
    
}
void makeItTeleporter(int x , int y)
{
    gotoxy(x,y);
    obj[x][y].hardship = 's';
    obj[x][y].type = 't';
    printf(YELLOW"\u2742"RESET);
}
void makeItGhost(int x, int y)
{
    gotoxy(x,y);
    obj[x][y].hardship = 's';
    obj[x][y].type = 'g';
    printf(CYAN"G"RESET);
}
void makeItOptimizer(int x, int y)
{
    gotoxy(x,y);
    obj[x][y].hardship = 's';
    obj[x][y].type = 'o';
    printf(YELLOW"T"RESET);
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
        set_obj();
        cur_map = 1;
        
        
        /*setup spaceshipe 1*/
        sp1.heart = 5;
        sp1.startPositionX = 12;
        sp1.startPositiony = 11;
        sp1.curx = 12;
        sp1.cury = 11;
        sp1.perx = 12;
        sp1.pery = 11;
        gotoxy(12,11);
        printf(BLUE"\u25A3"RESET);

        /*setup spaceshipe 2*/
        sp2.heart = 5;
        sp2.startPositionX = 98;
        sp2.startPositiony = 11;
        sp2.curx = 98;
        sp2.cury = 11;
        sp2.perx = 98;
        sp2.pery = 11;
        gotoxy(98,11);
        printf(RED"\u25A3"RESET);
        
        print_map1();

        pthread_mutex_init(&mutex1, NULL);
        pthread_mutex_init(&mutex2, NULL);
        pthread_mutex_init(&mutex3, NULL);

        pthread_t move_bullet1;
        pthread_t move_bullet2;
        pthread_t chack_rule1;


        pthread_create(&move_bullet1, NULL, move_bul1, NULL);
        pthread_create(&move_bullet2, NULL, move_bul2, NULL);
        pthread_create(&chack_rule1, NULL, check_ruls1, NULL);

        char input = '\0';
        while (input != 'q' && game_running)
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
        
        game_running = 0;
        pthread_join(move_bullet1, NULL);
        pthread_join(move_bullet2, NULL);
        pthread_join(chack_rule1, NULL);

        pthread_mutex_destroy(&mutex1);
        pthread_mutex_destroy(&mutex2);
        pthread_mutex_destroy(&mutex3);
        numerOfMap++;
        system("clear");
        if (sp1.heart > sp2.heart)
        {
            gotoxy(55,11);
            printf(BLUE"blue "RESET);
            printf("player win");
            fflush(stdout);
            /*work with file*/
        }
        if (sp2.heart > sp1.heart)
        {
            gotoxy(55,11);
            printf(RED"red "RESET);
            printf("player win");
            fflush(stdout);
            /*work with file*/
        }
        sleep(2);
    }
    if (numerOfMap == 2)
    {
        set_obj();
        cur_map = 2;
        game_running = 1;
        
        

        /*setup spaceshipe 1*/
        sp1.heart = 5;
        sp1.startPositionX = 12;
        sp1.startPositiony = 11;
        sp1.curx = 12;
        sp1.cury = 11;
        sp1.perx = 12;
        sp1.pery = 11;
        gotoxy(12,11);
        printf(BLUE"\u25A3"RESET);

        /*setup spaceshipe 2*/
        sp2.heart = 5;
        sp2.startPositionX = 98;
        sp2.startPositiony = 11;
        sp2.curx = 98;
        sp2.cury = 11;
        sp2.perx = 98;
        sp2.pery = 11;
        gotoxy(98,11);
        printf(RED"\u25A3"RESET);
        
        print_map2();

        pthread_mutex_init(&mutex1, NULL);
        pthread_mutex_init(&mutex2, NULL);
        pthread_mutex_init(&mutex3, NULL);

        pthread_t move_bullet1;
        pthread_t move_bullet2;
        pthread_t chack_rule2;


        pthread_create(&move_bullet1, NULL, move_bul1, NULL);
        pthread_create(&move_bullet2, NULL, move_bul2, NULL);
        pthread_create(&chack_rule2, NULL, check_ruls2, NULL);

        char input = '\0';
        while (input != 'q' && game_running)
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
        
        game_running = 0;
        pthread_join(move_bullet1, NULL);
        pthread_join(move_bullet2, NULL);
        pthread_join(chack_rule2, NULL);

        pthread_mutex_destroy(&mutex1);
        pthread_mutex_destroy(&mutex2);
        pthread_mutex_destroy(&mutex3);
        numerOfMap++;
        if (sp1.heart > sp2.heart)
        {
            gotoxy(55,11);
            printf(BLUE"blue "RESET);
            printf("player win");
            fflush(stdout);
            /*work with file*/
        }
        if (sp2.heart > sp1.heart)
        {
            gotoxy(55,11);
            printf(RED"red "RESET);
            printf("player win");
            fflush(stdout);
            /*work with file*/
        }
    }
    
}
void* check_ruls1()
{
    pthread_mutex_lock(&mutex3);
    while (game_running)
    {
        
        if(b2.x == sp1.curx && b2.y == sp1.cury)/*sp1 shuted*/
        {
            sp1.heart--;
            gotoxy(115,10);
            printf(BLUE);
            printf("blue player hearts : %d",sp1.heart);
            printf(RESET);
            fflush(stdout);
            if (sp1.heart <= 0)
            {
                game_running  = 0;
            }

        }
        if (b1.x == sp2.curx && b1.y == sp2.cury)/*sp2 shuted*/
        {
            sp2.heart--;
            gotoxy(115,12);
            printf(RED);
            printf("red player hearts : %d",sp2.heart);
            printf(RESET);
            fflush(stdout);
            if (sp2.heart <= 0)
            {
                game_running  = 0;
            } 
        }
        if (obj[sp1.curx][sp1.cury].type=='h')/*sp1 heart increased*/
        {
            sp1.heart++;
            gotoxy(115,10);
            printf(BLUE);
            printf("blue player hearts : %d",sp1.heart);
            printf(RESET);
            fflush(stdout);
            obj[55][5].type='n';      
        }
        if (obj[sp2.curx][sp2.cury].type=='h')/*sp2 heart increased*/
        {
            sp2.heart++;
            gotoxy(115,12);
            printf(RED);
            printf("red player hearts : %d",sp2.heart);
            printf(RESET);
            fflush(stdout);
            obj[sp2.curx][sp2.cury].type = 'n';
        }
        if (obj[sp1.curx][sp1.cury].type=='t')/*sp1 have teleported*/
        {
            if(sp1.curx == 99 && sp1.cury == 20)/*right teleport*/
            {
                
                gotoxy(sp1.curx + 1 ,sp1.cury);
                printf("\b ");       
                gotoxy(12, 19);  
                printf("\b");
                gotoxy(11, 19);  
                printf(BLUE"\u25A3"RESET);
                fflush(stdout);
                sp1.curx = 11;
                sp1.cury = 19;
                sp1.perx = 11;
                sp1.pery = 20;
                // if(cur_map == 1)
                // {
                print_map1();
                // }
                // if (cur_map == 2)
                // {
                //     print_map2();
                // }
                    
                fflush(stdout);

            }
            if(sp1.curx == 11 && sp1.cury == 20)/*left teleport*/
            {
                gotoxy(sp1.curx + 1 ,sp1.cury);
                printf("\b ");       
                gotoxy(100, 19);  
                printf("\b");
                gotoxy(99, 19);  
                printf(BLUE"\u25A3"RESET);
                fflush(stdout);
                sp1.curx = 99;
                sp1.cury = 19;
                sp1.perx = 99;
                sp1.pery = 20;
                if(cur_map == 1)
                {
                    print_map1();
                }
                if (cur_map == 2)
                {
                    print_map2();
                }
                    
                fflush(stdout);

            }
        }
        if (obj[sp2.curx][sp2.cury].type=='t')/*sp2 have teleported*/
        {
            if(sp2.curx == 99 && sp2.cury == 20)/*right teleport*/
            {
                
                gotoxy(sp2.curx + 1 ,sp2.cury);
                printf("\b ");       
                gotoxy(12, 19);  
                printf("\b");
                gotoxy(11, 19);  
                printf(RED"\u25A3"RESET);
                fflush(stdout);
                sp2.curx = 11;
                sp2.cury = 19;
                sp2.perx = 11;
                sp2.pery = 20;
                
                if(cur_map == 1)
                {
                    print_map1();
                }
                if (cur_map == 2)
                {
                    print_map2();
                }
                    
                fflush(stdout);

            }
            if(sp2.curx == 11 && sp2.cury == 20)/*left teleport*/
            {
                gotoxy(sp2.curx + 1 ,sp2.cury);
                printf("\b ");       
                gotoxy(100, 19);  
                printf("\b");
                gotoxy(99, 19);  
                printf(RED"\u25A3"RESET);
                fflush(stdout);
                sp2.curx = 99;
                sp2.cury = 19;
                sp2.perx = 99;
                sp2.pery = 20;
                
                print_map1();
                    
                fflush(stdout);
            }
        }
        if (obj[sp1.curx][sp1.cury].type == 'b')/*sp1 in black hole*/
        {
            sp1.heart--;
            if (sp1.heart <= 0)
            {
                game_running = 0;
            }
            
            gotoxy(sp1.curx + 1 ,sp1.cury);
            printf("\b ");
            gotoxy(sp1.startPositionX + 1,sp1.startPositiony);
            printf("\b");
            gotoxy(sp1.startPositionX ,sp1.startPositiony);
            printf(BLUE"\u25A3"RESET);
            sp1.curx =sp1.startPositionX ;
            sp1.cury=sp1.startPositiony;
            sp1.perx=sp1.startPositionX;
            sp1.pery=sp1.startPositiony;
            
            print_map1();
           
        }
        if (obj[sp2.curx][sp2.cury].type == 'b')/*sp2 in black hole*/
        {
            sp2.heart--;
            if (sp2.heart <= 0)
            {
                game_running = 0;
            }
            
            gotoxy(sp2.curx + 1 ,sp2.cury);
            printf("\b ");
            gotoxy(sp2.startPositionX + 1,sp2.startPositiony);
            printf("\b");
            gotoxy(sp2.startPositionX ,sp2.startPositiony);
            printf(RED"\u25A3"RESET);
            sp2.curx =sp2.startPositionX;
            sp2.cury=sp2.startPositiony;
            sp2.perx=sp2.startPositionX;
            sp2.pery=sp2.startPositiony;
            
            print_map1();
            
        }

        usleep(90000);
    }
    pthread_mutex_unlock(&mutex3);
    
    
    
    pthread_exit(NULL);
}
void* check_ruls2()
{
    pthread_mutex_lock(&mutex3);
    while (game_running)
    {
        if (obj[sp1.curx][sp1.cury].type == 'o')/*bullet1 boost*/
        {
            
        }
        
        if(b2.x == sp1.curx && b2.y == sp1.cury)/*sp1 shuted*/
        {
            sp1.heart--;
            if (b2.remainBoost > 0)
            {
                sp1.heart--;
            }
            
            gotoxy(115,10);
            printf(BLUE);
            printf("blue player hearts : %d",sp1.heart);
            printf(RESET);
            fflush(stdout);
            if (sp1.heart <= 0)
            {
                game_running  = 0;
            }

        }
        if (b1.x == sp2.curx && b1.y == sp2.cury)/*sp2 shuted*/
        {
            sp2.heart--;
            gotoxy(115,12);
            printf(RED);
            printf("red player hearts : %d",sp2.heart);
            printf(RESET);
            fflush(stdout);
            if (sp2.heart <= 0)
            {
                game_running  = 0;
            } 
        }
        if (obj[sp1.curx][sp1.cury].type=='h')/*sp1 heart increased*/
        {
            sp1.heart++;
            gotoxy(115,10);
            printf(BLUE);
            printf("blue player hearts : %d",sp1.heart);
            printf(RESET);
            fflush(stdout);
            obj[55][11].type='n';      
        }
        if (obj[sp2.curx][sp2.cury].type=='h')/*sp2 heart increased*/
        {
            sp2.heart++;
            gotoxy(115,12);
            printf(RED);
            printf("red player hearts : %d",sp2.heart);
            printf(RESET);
            fflush(stdout);
            obj[55][11].type='n';
        }
        if (obj[sp1.curx][sp1.cury].type=='t')/*sp1 have teleported*/
        {
            if(sp1.curx == 99 && sp1.cury == 20)/*right teleport*/
            {
                
                gotoxy(sp1.curx + 1 ,sp1.cury);
                printf("\b ");       
                gotoxy(12, 19);  
                printf("\b");
                gotoxy(11, 19);  
                printf(BLUE"\u25A3"RESET);
                fflush(stdout);
                sp1.curx = 11;
                sp1.cury = 19;
                sp1.perx = 11;
                sp1.pery = 20;
                // if(cur_map == 1)
                // {
                print_map2();
                // }
                // if (cur_map == 2)
                // {
                //     print_map2();
                // }
                    
                fflush(stdout);

            }
            if(sp1.curx == 11 && sp1.cury == 20)/*left teleport*/
            {
                gotoxy(sp1.curx + 1 ,sp1.cury);
                printf("\b ");       
                gotoxy(100, 19);  
                printf("\b");
                gotoxy(99, 19);  
                printf(BLUE"\u25A3"RESET);
                fflush(stdout);
                sp1.curx = 99;
                sp1.cury = 19;
                sp1.perx = 99;
                sp1.pery = 20;


                print_map2();
                
                    
                fflush(stdout);

            }
        }
        if (obj[sp2.curx][sp2.cury].type=='t')/*sp2 have teleported*/
        {
            if(sp2.curx == 99 && sp2.cury == 20)/*right teleport*/
            {
                
                gotoxy(sp2.curx + 1 ,sp2.cury);
                printf("\b ");       
                gotoxy(12, 19);  
                printf("\b");
                gotoxy(11, 19);  
                printf(RED"\u25A3"RESET);
                fflush(stdout);
                sp2.curx = 11;
                sp2.cury = 19;
                sp2.perx = 11;
                sp2.pery = 20;
                
                print_map2();
                 
                fflush(stdout);

            }
            if(sp2.curx == 11 && sp2.cury == 20)/*left teleport*/
            {
                gotoxy(sp2.curx + 1 ,sp2.cury);
                printf("\b ");       
                gotoxy(100, 19);  
                printf("\b");
                gotoxy(99, 19);  
                printf(RED"\u25A3"RESET);
                fflush(stdout);
                sp2.curx = 99;
                sp2.cury = 19;
                sp2.perx = 99;
                sp2.pery = 20;
                
                print_map2();
                    
                fflush(stdout);
            }
        }
        if (obj[sp1.curx][sp1.cury].type == 'b')/*sp1 in black hole*/
        {
            sp1.heart--;
            if (sp1.heart <= 0)
            {
                game_running = 0;
            }
            gotoxy(sp1.curx + 1 ,sp1.cury);
            printf("\b ");
            gotoxy(sp1.startPositionX + 1,sp1.startPositiony);
            printf("\b");
            gotoxy(sp1.startPositionX ,sp1.startPositiony);
            printf(BLUE"\u25A3"RESET);
            sp1.curx =sp1.startPositionX ;
            sp1.cury=sp1.startPositiony;
            sp1.perx=sp1.startPositionX;
            sp1.pery=sp1.startPositiony;
            
            print_map2();
           
        }
        if (obj[sp2.curx][sp2.cury].type == 'b')/*sp2 in black hole*/
        {
            sp2.heart--;
            if (sp2.heart <= 0)
            {
                game_running = 0;
            }
            
            gotoxy(sp2.curx + 1 ,sp2.cury);
            printf("\b ");
            gotoxy(sp2.startPositionX + 1,sp2.startPositiony);
            printf("\b");
            gotoxy(sp2.startPositionX ,sp2.startPositiony);
            printf(RED"\u25A3"RESET);
            sp2.curx =sp2.startPositionX;
            sp2.cury=sp2.startPositiony;
            sp2.perx=sp2.startPositionX;
            sp2.pery=sp2.startPositiony;
            
            print_map2();
            
        }

        usleep(90000);
    }
    pthread_mutex_unlock(&mutex3);
    
    
    
    pthread_exit(NULL);
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
    if (b1.remainBoost > 0)
    {
        b1.remainBoost--;
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
    
    while(game_running)
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
                if (obj[b1.x][b1.y].hardship == 's' && (obj[b1.x][b1.y].type == 'h' || obj[b1.x][b1.y].type == 't' || obj[b1.x][b1.y].type == 'b'))
                {
                    b1.y -= 1;
                }
                
                
            }
            
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
                if (obj[b1.x][b1.y].hardship == 's' && (obj[b1.x][b1.y].type == 'h' || obj[b1.x][b1.y].type == 't' || obj[b1.x][b1.y].type == 'b'))
                {
                    b1.y += 1;
                }
                
            }
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
                if (obj[b1.x][b1.y].hardship == 'h' && obj[b1.x][b1.y].type == 'w')
                {
                    b1.direction = 'n';
                    break;
                }
                else if (obj[b1.x][b1.y].hardship == 'h' && obj[b1.x][b1.y].type == 'm')/*mirror hit*/
                {   
                    b1.direction = 'r';
                    b1.x += 1;
                    break;
                }
                if (obj[b1.x][b1.y].hardship == 's' && (obj[b1.x][b1.y].type == 'h' || obj[b1.x][b1.y].type == 't' || obj[b1.x][b1.y].type == 'b'))
                {
                    b1.x -= 1;
                }
                
            }
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
                if (obj[b1.x][b1.y].hardship == 'h' && obj[b1.x][b1.y].type == 'w')
                {
                    b1.direction = 'n';
                    break;
                }
                else if (obj[b1.x][b1.y].hardship == 'h' && obj[b1.x][b1.y].type == 'm')/*mirror hit*/
                {   
                    b1.direction = 'l';
                    b1.x -= 1;
                    break;
                }
                if (obj[b1.x][b1.y].hardship == 's' && (obj[b1.x][b1.y].type == 'h' || obj[b1.x][b1.y].type == 't' || obj[b1.x][b1.y].type == 'b'))
                {
                    b1.x += 1;
                }
            }
        }
        pthread_mutex_unlock(&mutex1);
    }
    pthread_exit(NULL);

    return NULL;
}

void* move_bul2()
{
    while(game_running)
    {   
        pthread_mutex_lock(&mutex2); // Lock the mutex
        if(b2.direction == 'u')
        {
            
            while (1)
            {
                gotoxy(b2.x ,b2.y);
                printf(RED"\u2022"RESET);
                fflush(stdout);
                usleep(100000);
                gotoxy(b2.x + 1, b2.y);
                printf("\b ");
                fflush(stdout);
                b2.y -= 1;
                if (obj[b2.x][b2.y].hardship == 'h')
                {
                    b2.direction = 'n';
                    break;
                }
                if (obj[b2.x][b2.y].hardship == 's' && (obj[b2.x][b2.y].type == 'h' || obj[b2.x][b2.y].type == 't' || obj[b2.x][b2.y].type == 'b'))
                {
                    b2.y -= 1;
                }
                
            }
            
        }
        else if(b2.direction == 'd')
        {
            while (1)
            {
                gotoxy(b2.x ,b2.y);
                printf(RED"\u2022"RESET);
                fflush(stdout);
                usleep(100000);
                gotoxy(b2.x + 1, b2.y);
                printf("\b ");
                fflush(stdout);
                b2.y += 1;
                if (obj[b2.x][b2.y].hardship == 'h')
                {
                    b2.direction = 'n';
                    break;
                }
                if (obj[b2.x][b2.y].hardship == 's' && (obj[b2.x][b2.y].type == 'h' || obj[b2.x][b2.y].type == 't' || obj[b2.x][b2.y].type == 'b'))
                {
                    b2.y += 1;
                }
                
            }
        }
        else if(b2.direction == 'l')
        {
            while (1)
            {
                gotoxy(b2.x ,b2.y);
                printf(RED"\u2022"RESET);
                fflush(stdout);
                usleep(100000);
                gotoxy(b2.x + 1, b2.y);
                printf("\b ");
                fflush(stdout);
                b2.x -= 1;
                if (obj[b2.x][b2.y].hardship == 'h' && obj[b2.x][b2.y].type == 'w')
                {
                    b2.direction = 'n';
                    break;
                }
                else if (obj[b2.x][b2.y].hardship == 'h' && obj[b2.x][b2.y].type == 'm')/*mirror hit*/
                {   
                    b2.direction = 'r';
                    b2.x += 1;
                    break;
                }
                if (obj[b2.x][b2.y].hardship == 's' && (obj[b2.x][b2.y].type == 'h' || obj[b2.x][b2.y].type == 't' || obj[b2.x][b2.y].type == 'b'))
                {
                    b2.x -= 1;
                }
            }
        }
        else if(b2.direction == 'r')
        {
            while (1)
            {
                gotoxy(b2.x ,b2.y);
                printf(RED"\u2022"RESET);
                fflush(stdout);
                usleep(100000);
                gotoxy(b2.x + 1, b2.y);
                printf("\b ");
                fflush(stdout);
                b2.x += 1;
                if (obj[b2.x][b2.y].hardship == 'h' && obj[b2.x][b2.y].type == 'w')
                {
                    b2.direction = 'n';
                    break;
                }
                else if (obj[b2.x][b2.y].hardship == 'h' && obj[b2.x][b2.y].type == 'm')/*mirror hit*/
                {   
                    b2.direction = 'l';
                    b2.x -= 1;
                    break;
                }
                if (obj[b2.x][b2.y].hardship == 's' && (obj[b2.x][b2.y].type == 'h' || obj[b2.x][b2.y].type == 't' || obj[b2.x][b2.y].type == 'b'))
                {
                    b2.x += 1;
                }
            }
        }
        pthread_mutex_unlock(&mutex2);
    }
    pthread_exit(NULL);

    return NULL;

}

void move_sp(char move)
{
    if (move == 'w')
    {
        // printf("%d new %d\n",sp1.curx,sp1.cury);
        // printf("%d %d\n",obj[sp1.curx][sp1.cury - 1].x,obj[sp1.curx][sp1.cury - 1].y);
        // printf("%c\n",obj[12][10].type);


        if(obj[sp1.curx][sp1.cury - 1].hardship == 's')
        {      
            sp1.perx = sp1.curx;
            sp1.pery = sp1.cury;
            sp1.cury -= 1;
            gotoxy(sp1.perx + 1 ,sp1.pery);
            printf("\b ");       
            gotoxy(sp1.curx + 1, sp1.cury);  
            printf("\b");
            gotoxy(sp1.curx, sp1.cury);  
            printf(BLUE"\u25A3"RESET);
            
        }
    }
    else if (move == 'a')
    {
        if(obj[sp1.curx - 1][sp1.cury].hardship == 's')
        {      
            sp1.perx = sp1.curx;
            sp1.pery = sp1.cury;
            sp1.curx -= 1;
            gotoxy(sp1.perx + 1,sp1.pery);
            printf("\b ");       
            gotoxy(sp1.curx + 1, sp1.cury);  
            printf("\b");
            gotoxy(sp1.curx, sp1.cury);               
            printf(BLUE"\u25A3"RESET);
        }
    }
    else if (move == 's')
    {
        if(obj[sp1.curx][sp1.cury + 1].hardship == 's')
        {      
            sp1.perx = sp1.curx;
            sp1.pery = sp1.cury;
            sp1.cury += 1;
            gotoxy(sp1.perx + 1,sp1.pery);
            printf("\b ");  
            gotoxy(sp1.curx + 1, sp1.cury);  
            printf("\b");     
            gotoxy(sp1.curx, sp1.cury);               
            printf(BLUE"\u25A3"RESET);
        }
    }
    else if (move == 'd')
    {
        if(obj[sp1.curx + 1][sp1.cury].hardship == 's')
        {      
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
    }
    else if (move == 'i')
    {
        if(obj[sp2.curx][sp2.cury - 1].hardship == 's')
        {      
            sp2.perx = sp2.curx;
            sp2.pery = sp2.cury;
            sp2.cury -= 1;
            gotoxy(sp2.perx + 1 ,sp2.pery);
            printf("\b ");   
            gotoxy(sp2.curx + 1, sp2.cury);  
            printf("\b");    
            gotoxy(sp2.curx, sp2.cury);               
            printf(RED"\u25A3"RESET);
            
        }
    }
    else if (move == 'l')
    {
        if(obj[sp2.curx + 1][sp2.cury].hardship == 's')
        {      
            sp2.perx = sp2.curx;
            sp2.pery = sp2.cury;
            sp2.curx += 1;
            gotoxy(sp2.perx + 1 ,sp2.pery);
            printf("\b ");   
            gotoxy(sp2.curx + 1, sp2.cury);  
            printf("\b");    
            gotoxy(sp2.curx, sp2.cury);               
            printf(RED"\u25A3"RESET);
        }
    }
    else if (move == 'k')
    {
        if(obj[sp2.curx][sp2.cury + 1].hardship == 's')
        {      
            sp2.perx = sp2.curx;
            sp2.pery = sp2.cury;
            sp2.cury += 1;
            gotoxy(sp2.perx + 1 ,sp2.pery);
            printf("\b ");   
            gotoxy(sp2.curx + 1, sp2.cury);  
            printf("\b");    
            gotoxy(sp2.curx, sp2.cury);               
            printf(RED"\u25A3"RESET);
        }
    }
    else if (move == 'j')
    {
        if(obj[sp2.curx - 1][sp2.cury].hardship == 's')
        {      
            sp2.perx = sp2.curx;
            sp2.pery = sp2.cury;
            sp2.curx -= 1;
            gotoxy(sp2.perx + 1 ,sp2.pery);
            printf("\b ");   
            gotoxy(sp2.curx + 1, sp2.cury);  
            printf("\b");    
            gotoxy(sp2.curx, sp2.cury);               
            printf(RED"\u25A3"RESET);
        }
    }
    
}
void print_map1()
{    
    system("clear");
    /*printing up and down walls ... */
    for (int i = 0; i <= MYXMAX; i += 1)
    {
        makeItWall(i,0);
    }
    for (int i = 0; i <= MYXMAX; i += 1)
    {
        makeItWall(i,MYXMAX);
    }

    
    /*printing side walls*/

    for (int j = 0; j <= MYYMAX; j++)
    {
        makeItWall(0,j);
    }  
    for (int j = 0; j <= MYYMAX; j++)
    {
        makeItWall(MYXMAX,j);
    }  

    /*printing iner walls*/
    for (int i = 1; i < MYXMAX; i++)
    {
        if (!(i == 20 || i == 55 || i == 90))
        {
            obj[i][10].hardship = 's';
            obj[i][10].type = 'n';
        }
        
        
    }

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
    for (int j = 7; j < 11; j++)
    {
        makeItWall(55,j);
    }
    makeItGodMirror(55,11);
    for (int j = 12; j < 16; j++)
    {
        makeItWall(55,j);
    }
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
    refresh_sp1_heart(1);
    refresh_sp2_heart(1);

    gotoxy(sp1.curx,sp1.cury);
    printf(BLUE"\u25A3"RESET);
    gotoxy(sp2.curx,sp2.cury);
    printf(RED"\u25A3"RESET);
    fflush(stdout);
}
void print_map2()
{
    system("clear");
    /*printing up and down walls ... */
    for (int i = 0; i <= MYXMAX; i += 1)
    {
        makeItWall(i,0);
    }
    for (int i = 0; i <= MYXMAX; i += 1)
    {
        makeItWall(i,MYXMAX);
    }

    
    /*printing side walls*/

    for (int j = 0; j <= MYYMAX; j++)
    {
        makeItWall(0,j);
    }  
    for (int j = 0; j <= MYYMAX; j++)
    {
        makeItWall(MYXMAX,j);
    }  


    for (int i = 1; i < MYXMAX; i++)
    {
        if (!(i == 20 || i == 55 || i == 90))
        {
            obj[i][10].hardship = 's';
            obj[i][10].type = 'n';
        }
        
        
    }
    /*Printing the map itself*/
    for (int j = 6; j < 17; j++)
    {
        makeItGodMirror(20,j);
    }
    for (int j = 6; j < 17; j++)
    {
        makeItGodMirror(90,j);
    }
    makeItWall(27,3);
    makeItWall(27,4);
    makeItGodMirror(28,3);
    
    makeItWall(27,19);
    makeItWall(27,20);
    makeItGodMirror(28,19);

    makeItTeleporter(99,20);
    makeItTeleporter(11,20);

    makeItWall(83,19);
    makeItWall(83,20);
    makeItGodMirror(82,20);

    makeItWall(83,3);
    makeItWall(83,4);
    makeItGodMirror(82,4);

    makeItWall(36,8);
    makeItWall(35,9);
    makeItWall(34,10);
    makeItWall(33,11);
    makeItWall(34,12);
    makeItWall(35,13);
    makeItWall(36,14);

    makeItWall(74,8);
    makeItWall(75,9);
    makeItWall(76,10);
    makeItWall(77,11);
    makeItWall(76,12);
    makeItWall(75,13);
    makeItWall(74,14);

    for (int i = 53; i < 58; i++)
    {
        makeItWall(i,18);
    }
    for (int i = 53; i < 58; i++)
    {
        makeItWall(i,5);
    }
    makeItWall(55,15);
    makeItWall(55,14);
    for (int i = 55; i < 61; i++)
    {
        makeItWall(i,14);
    }
    makeItWall(60,13);
    for (int i = 60; i < 64; i++)
    {
        makeItWall(i,12);
    }
    makeItHeart(55,11);


    makeItWall(55,7);
    for (int i = 49; i < 56; i++)
    {
        makeItWall(i,8);
    }
    makeItWall(49,9);
    for (int i = 46; i < 50; i++)
    {
        makeItWall(i,10);
    }
    makeItGodMirror(35,12);
    makeItGodMirror(50,10);
    makeItGodMirror(75,10);
    makeItBlackHole(55,16);
    makeItBlackHole(54,12);
    makeItBlackHole(56,10);
    makeItGhost(55,3);
    makeItOptimizer(31,9);
    makeItOptimizer(79,9);

    refresh_sp1_heart(1);
    refresh_sp2_heart(1);

    gotoxy(sp1.curx,sp1.cury);
    printf(BLUE"\u25A3"RESET);
    gotoxy(sp2.curx,sp2.cury);
    printf(RED"\u25A3"RESET);
    fflush(stdout);


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
    FILE* file = fopen("accounts.txt","a");
    if (file == NULL)
    {
        printf(RED "could NOT open file\n"RESET);
        exit(-1);
    }
    system("clear");
    person tempP ;
    //person* tmpPtr = (person*)malloc(sizeof(person)); 
    printf(GREEN"Sign up ... \n"RESET);

    printf("please enter your user name ");
    printf(RED"max 20 character "RESET);
    scanf("%s",tempP.userName);

    do
    {
        printf("please enter your email ");
        printf(RED"max 20 character MUST INCLUDE @ and ."RESET);
        scanf("%s",tempP.email);
    } while (!(is_valid_email(tempP.email)));

    printf("please enter your password ");
    printf(RED"max 20 character "RESET);
    scanf("%s",tempP.password);

    fwrite(&tempP, sizeof(person), 1, file);

    fclose(file);
    
}
// void get_person_from_file(person* Pptr)
// {
//     FILE* file = fopen("accounts.bin","wb+");
//     if (file == NULL)
//     {
//         printf(RED "could NOT open file\n"RESET);
//         exit(-1);
//     }
//     fread();
// }
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
    // FILE* file = fopen("accounts.txt","r");
    // if (file == NULL)
    // {
    //     printf(RED "could NOT open file\n"RESET);
    //     exit(-1);
    // }
    person tempP;
    system("clear");
    
    char user[21];
    do
    {
        printf("please enter your user name ");
        printf(RED"max 20 character "RESET);
        scanf("%s",user);
    } while (!(is_user_found(&tempP,user)));
    char p1[21];  
    scanf("%s",p1);
    if (strcmp(p1,tempP.password))
    {
        printf("success\n");
    }
    else
    {
        sign_in();
    }
    
}
int is_user_found(person * newPerson ,char* username)
{
    person temp;
    FILE* file = fopen("accounts.txt","r");
    if (file == NULL)
    {
        printf(RED "could NOT open file\n"RESET);
        return 0;
    }
    while(fread(&temp,sizeof(person),1,file))
    {
        if (!(strcmp(username, temp.userName)))
        {
            *newPerson = temp;
            fclose(file);
            return 1;
        }
        
    }
    fclose(file);

    return 0;
}