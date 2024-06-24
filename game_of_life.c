#include <stdio.h>
#include <unistd.h>
#define ROW 25
#define COL 80

void startSpeed();  // меню выбора скорости
int speedTransform(int *speed);  // изменение скорости
int gameSpeed(int *speed);  // получение скорости и перерисовка
void teamBlock();  // информация о команде
void lifePrint(int a[ROW][COL]);  // отрисовка игрового поля и клеток на нем
void borderPrint(int len);  // отрисовка границ
void spaceBar(int n);  // для отрисовки пустых симоволов - отступов
void gameStatus(int speed, int alive, int dead);  // отображание состояния игры
int input(int a[ROW][COL]);  // считываем конфиг из файла
int countAlive(int a[][COL]);  // подсчет живых клеток
int countDead(int a[][COL]);  // подсчет мертвых клеток
int mod(int num, int size);  // смена краевых значений
int neigCount(int masMain[ROW][COL], int x, int y);  // подсчет кол-ва соседей
// меняет состояние клетки в зависимости от кол-ва соседей в первом массиве
void neigChange(int masMain[ROW][COL], int masSave[ROW][COL]);
int main() {
    int speed = 1;
    int parentArray[ROW][COL] = {0};
    input(parentArray);
    gameSpeed(&speed);
    if (speed != 0) {
        int childArray[ROW][COL] = {0};
        int dead = 0;
        while (dead != 2000) {
        int alive = countAlive(parentArray);
        dead = countDead(parentArray);
        gameStatus(speed, alive, dead);
        lifePrint(parentArray);
        neigChange(parentArray , childArray);
        usleep(speedTransform(&speed));
        }
        printf("Game Over");
    } else {
        printf("Game Over! Goodbye!");
    }
    return 0;
}
// орисовка игрового поля и клеток на нем
void lifePrint(int a[ROW][COL]) {
    borderPrint(COL + 1);
    for (int i = 0; i < ROW; i++) {
        printf("|");
        for (int j = 0; j < COL; j++) {
          if (a[i][j] == 1) {
                printf("#");
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }
    borderPrint(COL + 1);
}
// отрисовка рамок-контуров
void borderPrint(int len) {
    char mask = '-';
    printf(" ");
    for (int i = 0; i < len - 1; i++) {
        printf("%c", mask);
    }
    printf("\n");
}

// перенос точки для прорировки
void spaceBar(int n) {
    for (int i = 0; i <= n; i++) {
        printf(" ");
    }
}

// шапка с состоянием игры и ее отрисовка
void gameStatus(int speed, int alive, int dead) {
    printf("\033[0d\033[2J");  // очищаем экран
    borderPrint(80);
    spaceBar(32);
    printf("THE GAME STATUS:\n");
    printf("\n");
    printf("        Living cells: %4d         Dead cells: %4d        ", alive, dead);
    printf(" Set speed: %2d\n", speed);
}

// считывание из потока конфигурации
int input(int a[ROW][COL]) {
    char ch;
    int flag = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            scanf("%c", &ch);
            if (ch != '\n') {
                if (ch == '1') {
                    a[i][j] = 1;
                }
            }
        }
    }
    if (freopen("/dev/tty", "r", stdin) == 0) {
        flag++;
    }
    return flag;
}

// количество живых
int countAlive(int a[ROW][COL]) {
    int count = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (a[i][j] == 1) {
                count++;
            }
        }
    }
    return count;
}

// количество мертвых
int countDead(int a[ROW][COL]) {
    int count = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (a[i][j] == 0) {
                count++;
            }
        }
    }
    return count;
}

// замена координаты, выходящий за границы
int mod(int num, int size) {
    int res = (num + size) % size;
    return res;
}

// считаем кол-во соседений у клетки
int neigCount(int mas[ROW][COL], int x, int y) {
    int N = 0;
    N = mas[mod(y + 1, ROW)][mod(x - 1, COL)]
        + mas[mod(y + 1, ROW)][mod(x, COL)]
        + mas[mod(y + 1, ROW)][mod(x + 1, COL)]
        + mas[mod(y, ROW)][mod(x - 1, COL)]
        + mas[mod(y, ROW)][mod(x + 1, COL)]
        + mas[mod(y - 1, ROW)][mod(x - 1, COL)]
        + mas[mod(y - 1, ROW)][mod(x, COL)]
        + mas[mod(y - 1, ROW)][mod(x + 1, COL)];
    return N;
}

// пересчет и заполнение следующего поколения
void neigChange(int a[ROW][COL], int b[ROW][COL]) {
    for (int y = 0; y < ROW; y++) {
            for (int x = 0; x < COL; x++) {
                if (a[y][x] == 1) {
                    if (neigCount(a, x, y) == 2) b[y][x] = a[y][x];
                    if (neigCount(a, x, y) == 3) b[y][x] = a[y][x];
                    if (neigCount(a, x, y) > 3) b[y][x] = 0;
                    if (neigCount(a, x, y) < 2) b[y][x] = 0;
                } else {
                    if (neigCount(a, x, y) == 3) {
                        b[y][x] = 1;
                    }
                }
            }
        }
        for (int y = 0; y < ROW; y++) {
            for (int x = 0; x < COL; x++) {
                a[y][x] = b[y][x];
            }
        }
}
// Стартовый бор скорости
int gameSpeed(int *speed) {
    startSpeed();
    char key;  // Проверка на корректность считанных клавиш
    while (((key = getchar()) != '0') && (key != '1') && (key != '2')
            && (key != '3') && (key != '4')) {
        startSpeed();
    }
    switch (key) {
    case '1':
        *speed = 1;
        break;
    case '2':
        *speed = 2;
        break;
    case '3':
        *speed = 3;
        break;
    case '4':
        *speed = 4;
        break;
    case '0':
        *speed = 0;
        break;
    }
    return *speed;
}
// отрисовка меню выбора скорости
void startSpeed() {
    printf("\033[0d\033[2J");  // очищаем экран
    teamBlock();
    printf("\n");
    printf(" 0 - Exit Game\n");
        for (int i = 1; i <= 4; i++) {
            printf(" %d - Game speed №%d\n", i, i);
        }
    printf(" Your choice:\n");
}

// отрисовка информации о команде
void teamBlock() {
    borderPrint(80);
    spaceBar(31);
    printf("THE GAME OF LIFE\n");
    printf("\n");
    spaceBar(35);
    printf("Team 54:\n");
    spaceBar(28);
    printf("CELESTAC (TL) - Graphic\n");
    spaceBar(28);
    printf("FOURWALT - Mechanics\n");
    spaceBar(28);
    printf("PERROSNO - Configuration\n");
    printf("\n");
    borderPrint(80);
}

// Пересчет скорости
int speedTransform(int *speed) {
    int newSpeed;
    switch (*speed) {
    case 1:
        newSpeed = 500000;
        break;
    case 2:
        newSpeed = 400000;
        break;
    case 3:
        newSpeed = 300000;
        break;
    case 4:
        newSpeed = 200000;
        break;
    }
    return newSpeed;
}
