#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
  


// food structure containing it's position on screen 
// food symbol will be 0
struct food 
{
    int x; 
    int y; 
};

typedef struct bodyPart bodyPart;
struct bodyPart
{
    int x;
    int y;
    struct bodyPart *next;
};


// methods 
int kbhit(void);
void delay(int milisec); // delays by the given amount of milisec
void initialSnake(struct bodyPart *snake); // init the snake body parts 
void getKey(); // get arrow key from the user
struct bodyPart* drawSnake(struct bodyPart* snake, int *foodPicked); // this function draws the snake on the screen and returns the snake's body in case the snake ate his food 
void displayScreen(int type);    // displaying the new screen with the snake. type: 0 = regular, 1 = loose, 2 = SNAKE;
struct bodyPart* moveSnake(struct bodyPart* snake, char lastKey); // move the snake one block in the direction of the last key that was clicked
struct bodyPart* updateSnake(struct bodyPart* snake, int xAdd, int yAdd); // this function changes the head of the snake to the new bosy part at the new location
void loose(); // this function checks wheather the snake has lost in case his head touches his body or alternatively, if he touches the walls.
void generateFood(int *foodPicked); // this function generates snake's food at a random spot on the screen
void addBodyPart(); // this function attaches a new body part to he snake's body in case he have eaten food 

// global variable
char SCREEN[45][93]= { "--------------------------------------------------------------------------------------\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "|                                                                                    |\n",
                        "--------------------------------------------------------------------------------------\n"};        

int run = 1;


char LOOSE[10][300] = {

" ▄██   ▄    ▄██████▄  ███    █▄        ▄█        ▄██████▄     ▄████████     ███     ", 
" ███   ██▄ ███    ███ ███    ███      ███       ███    ███   ███    ███ ▀█████████▄ ", 
" ███▄▄▄███ ███    ███ ███    ███      ███       ███    ███   ███    █▀     ▀███▀▀██ ", 
" ▀▀▀▀▀▀███ ███    ███ ███    ███      ███       ███    ███   ███            ███   ▀ ", 
" ▄██   ███ ███    ███ ███    ███      ███       ███    ███ ▀███████████     ███     ", 
" ███   ███ ███    ███ ███    ███      ███       ███    ███          ███     ███     ", 
" ███   ███ ███    ███ ███    ███      ███▌    ▄ ███    ███    ▄█    ███     ███     ", 
"  ▀█████▀   ▀██████▀  ████████▀       █████▄▄██  ▀██████▀   ▄████████▀     ▄████▀   ", 
"                                      ▀                                             " };
                                                                                                                                                

                                                                                                                                      
char SNAKE[6][200] = {                                                                                                                                          
" ███████╗███╗   ██╗ █████╗ ██╗  ██╗███████╗ ",
" ██╔════╝████╗  ██║██╔══██╗██║ ██╔╝██╔════╝ ",
" ███████╗██╔██╗ ██║███████║█████╔╝ █████╗   ",
" ╚════██║██║╚██╗██║██╔══██║██╔═██╗ ██╔══╝   ",
" ███████║██║ ╚████║██║  ██║██║  ██╗███████╗ ",
" ╚══════╝╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝ "};                                                                                                                               
                                                                                                                                                
                                                                                                                                                
                                                                                                                                                

int main(){
    // OPENING SCREEN 
    displayScreen(2);
    delay(4000000);

    struct bodyPart* snake = malloc(sizeof(struct bodyPart)); // the snake array of body parts
    char lastKey = 's'; // holds the last arrow key that was clicked 'U'/'D'/'L'/'R'

    snake->x = 4;
    snake->y = 4;

    int foodPicked = 1; // for the first generation of food 
    generateFood(&foodPicked);
    initialSnake(snake);

    while(run == 1){
        if (kbhit()){
            char key = getchar();
            if (key == 'a' || key == 'w' || key == 's' || key == 'd'){ // there was a keyborad click
                lastKey = key;
            }
        }

        if (foodPicked)
            generateFood(&foodPicked);
        snake = moveSnake(snake, lastKey);
        snake = drawSnake(snake, &foodPicked);
        displayScreen(0);

        if (lastKey == 'a' || lastKey == 'd')
            delay(200000);  // a 1 / 5 of a second delay
        else // y axis movement needs a higher delay time 
            delay(400000);
    }

    // end of game 
    system("clear");
    displayScreen(1);

}


void initialSnake(struct bodyPart* snake){
    struct bodyPart* body = (struct bodyPart *) malloc(sizeof(struct bodyPart));
    body->x = 4;
    body->y = 5;
    struct bodyPart* tail = (struct bodyPart *) malloc(sizeof(struct bodyPart));
    tail->x = 4;
    tail->y = 6;

    // this part works just fine 
    snake->next = body;
    snake->next->next = tail;
    printf("%d", snake->next->next->x);
}


struct bodyPart* drawSnake(struct bodyPart* snake, int *foodPicked){
    struct bodyPart* current = snake;
    
    // check if snake have eaten food 
    if (SCREEN[current->x][current->y] == 'o'){
        // setting food picked to 1
        *foodPicked = 1;

        // add bodyPart to the snake 
        struct bodyPart* newPart = (struct bodyPart *) malloc(sizeof(struct bodyPart));
        newPart->x = current->x;
        newPart->y = current->y;

        // creating a copy of the current snake's head
        struct bodyPart* snakeCpy = (struct bodyPart *) malloc(sizeof(struct bodyPart)); 
        snakeCpy->x = snake->x;
        snakeCpy->y = snake->y;
        snakeCpy->next = snake->next;
        
        newPart->next = snakeCpy;
        snake = newPart; 
    }

    // first check if snake's is not out of bounds
    if (SCREEN[current->x][current->y] != ' ' && SCREEN[current->x][current->y] != 'o')
        // if the statement is true - the snake has bumped into himself / wall
        loose();

    while(current != NULL)
    {
        SCREEN[current->x][current->y] = '#';
        current = current->next;
    }

    return snake;
    
}

void displayScreen(int type){
    // clear screen
    system("clear");
    
    int height = 20;
    if (type == 1)
        height = 10;
    if (type == 2)
        height = 6; 
    // display the array of characters on the screen
    for (int i = 0; i < height; i++){
        if (type == 0) // regular screen
            printf("%s\n", SCREEN[i]);
        else if(type == 1)
            printf("%s\n", LOOSE[i]);
        else 
            printf("%s\n", SNAKE[i]);
    }
}


struct bodyPart* updateSnake(struct bodyPart* snake, int xAdd, int yAdd){
    // first, we'll add the new body part 
    struct bodyPart* newBdyPrt = (struct bodyPart *) malloc(sizeof(struct bodyPart));
    
    // setting the new bosy part x, y coordinates
    newBdyPrt->x = snake->x + xAdd;
    newBdyPrt->y = snake->y + yAdd;

    // creating a copy of the current snake's head
    struct bodyPart* snakeCpy = (struct bodyPart *) malloc(sizeof(struct bodyPart)); 
    snakeCpy->x = snake->x;
    snakeCpy->y = snake->y;
    snakeCpy->next = snake->next;

    // binding the new bosyPart to the copy of the head
    newBdyPrt->next = snakeCpy;

    // remove the last body part 
    struct bodyPart* current = snake;

    while(current->next->next != NULL)
    {
        current = current->next;
    }

    // delete body part from the screen 
    SCREEN[current->next->x][current->next->y] = ' ';

    // now current holds the one before the last body part 
    current->next = NULL; // removing the last body part
    

    // replacing the older snake's head with the new body part 
    return newBdyPrt;
}


struct bodyPart * moveSnake(struct bodyPart* snake, char lastKey){
    switch(lastKey){ 
        // move left
        case 'a': 
            return updateSnake(snake, 0, -1);
            break;
               
        // move right
        case 'd': 
            return updateSnake(snake, 0, 1);
            break;

        // move upwards
        case 'w': 
            return updateSnake(snake, -1, 0);
            break;

        // move downwards
        case 's': 
            return updateSnake(snake, 1, 0);
            break;
    }   
}

void loose(){
    run = 0;
}


void generateFood(int *foodPicked)
{
    // random coordinates 
    srand(time(NULL)); // SETTING SEED TO BE THE CURRENT SYS TIME 

    int xCor, yCor; 

    do{
        xCor = 1 + (rand() % 84);
        yCor = 1 + (rand() % 19);
    } 
    while(SCREEN[yCor][xCor] != ' '); // while the food's place isn't empty 
    
    SCREEN[yCor][xCor] = 'o';
    *foodPicked = 0;
}

// HELPERS METHODS
// method that check wheather the player had clicked an arrow key 
int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}


void delay(int milisec)
{ 
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milisec);
}