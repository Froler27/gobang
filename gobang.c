#include <stdio.h>
#include <stdlib.h>
#define N 15
#define S 12
#define BLACK -1
#define WHITE 1

void initializeCheckerBoard(int board[][N]);
void initializeWeight(int weight[][S]);
void showBoard(int board[][N]);


// 判断棋形的四个函数
void getNinePoint(int ninePoint[],int board[][N], int x,int y, int lineDir, int player);
int coreJudge(int sevenPoint[]);
int judgeType(int ninePoint[], int pnum, int enemyL, int enemyR, int selfL, int selfR);
int getLineType(int ninePoint[]);

int isOverBoard(int x, int y, int direction, int dist);
int getContent(int board[][N], int x, int y, int direction, int dist);
int getPointScore(int board[][N], int site[], int weight[][S], int player);
void calculateExpValue(int score[][N], int board[][N], int weight[][S], int player);

int white(int board[][N], int weight[][S], int site[]);
int black(int board[][N], int weight[][S], int site[]);
int downPiece(int board[][N], int site[], int score, int player);

int main()
{
    int checkerboard[N][N];
    int weight[2][S];
    int i, scoreB, scoreW;
    int site[2];

    system("color 0f");

    initializeCheckerBoard(checkerboard);
    initializeWeight(weight);getchar();
    showBoard(checkerboard);

    for(i = N*N; i>0; --i)
    {
        //scoreW = black(checkerboard, weight, site);
        printf("请输入坐标：");
        scanf("%d %d", &site[0], &site[1]);
        scoreW = getPointScore(checkerboard, site, weight, -1);
        if(scoreW < 0)
        {
            printf("该点不能落子！\n");
            ++i;
            continue;
        }
        if(downPiece(checkerboard, site, scoreW, BLACK))
            break;
        showBoard(checkerboard);
        scoreB = white(checkerboard, weight, site);
        if(downPiece(checkerboard, site, scoreB, WHITE))
            break;
        showBoard(checkerboard);
        if(i%10 == 1)
            getchar();
    }
    printf("\n******************************\n");
    printf("\n           游戏结束           \n");
    printf("\n******************************\n");
    return 0;
}

int getPointScore(int board[][N], int site[], int weight[][S], int player)
{
    int x, y, i;
    int p_type, temp_p_type;
    int enemy_type = 0, temp_enemy_type = 0;
    int ninePoint[9];

    x = site[0];
    y = site[1];
    if(isOverBoard(x, y, 0, 0))
        return -1;
    if(board[x][y] == 0)
    {
        for(i=0, p_type=0; i<4; ++i)
        {
            getNinePoint(ninePoint, board, x, y, i,player);
            temp_p_type = getLineType(ninePoint);
            if(temp_p_type >= 7 && p_type >= 7)
                p_type = 10;
            else if(temp_p_type > p_type)
                p_type = temp_p_type;
        }
        for(i=0, enemy_type=0; i<4; ++i)
        {
            getNinePoint(ninePoint, board, x, y, i,0-player);
            temp_enemy_type = getLineType(ninePoint);
            if(temp_enemy_type >= 7 && enemy_type >= 7)
                enemy_type = 10;
            else if(temp_enemy_type > enemy_type)
                enemy_type = temp_enemy_type;
        }
        return p_type < enemy_type ? weight[1][enemy_type] : weight[0][p_type];
    }
    else
        return -1;
}

void initializeCheckerBoard(int board[][N])
{
    int i,j;
    for(i=0; i<N; i++)
    {
        for(j=0; j<N; j++)
        {
            board[i][j] = 0;
        }
    }
}

void initializeWeight(int weight[][S])
{
    FILE *fp;
    int i;
    if(access("evaluation_form.txt",0) < 0)
    {
        fp = fopen("evaluation_form.txt", "w");
        fprintf(fp, "0 0\n");
        fprintf(fp, "15 10\n");
        fprintf(fp, "50 40\n");
        fprintf(fp, "100 80\n");
        fprintf(fp, "230 200\n");
        fprintf(fp, "300 240\n");
        fprintf(fp, "600 480\n");
        fprintf(fp, "1200 1000\n");
        fprintf(fp, "1800 1200\n");
        fprintf(fp, "2100 1800\n");
        fprintf(fp, "5000 3000\n");
        fprintf(fp, "100000 10000\n");
        fclose(fp);
    }
    fp = fopen("evaluation_form.txt", "r");
    for(i=0; i<S; ++i)
        fscanf(fp, "%d %d", &weight[0][i], &weight[1][i]);
    fclose(fp);
}

void showBoard(int board[][N])
{
    int i,j;
    printf("\t");
    for(i=0; i<N; ++i)
    {
        printf("%d\t",i);
    }
    printf("\n\n\n");
    for(i=0; i<N; ++i)
    {
        printf("  %-2d\t", i);
        for(j=0; j<N; ++j)
        {
            switch(board[i][j])
            {
            case 0:
                printf(".\t");
                break;
            case -1:
                printf("O\t");
                break;
            case 1:
                printf("X\t");
                break;
            }
        }
        printf("\n\n\n");
    }
}

//判断相对于点（x,y）在direction方向上dist处是否在界限之外
int isOverBoard(int x, int y, int direction, int dist)
{
    switch(direction)
    {
        case 0: return y-dist < 0 || y-dist >= N ? 1:0;
        case 1: return y-dist < 0 || y-dist >= N || x+dist >= N || x+dist < 0 ? 1:0;
        case 2: return x+dist >= N || x+dist < 0 ? 1:0;
        case 3: return y+dist >= N || y+dist < 0 || x+dist >= N || x+dist < 0 ? 1:0;
        case 4: return y+dist >= N || y+dist < 0 ? 1:0;
        case 5: return y+dist >= N || y+dist < 0 || x-dist < 0 || x-dist >= N ? 1:0;
        case 6: return x-dist < 0 || x-dist >= N ? 1:0;
        case 7: return y-dist < 0 || y-dist >= N || x-dist < 0 || x-dist >= N ? 1:0;
        default: return 1;
    }
}

int getContent(int board[][N], int x, int y, int direction, int dist)
{
    switch(direction)
    {
        case 0: return board[x][y-dist];
        case 1: return board[x+dist][y-dist];
        case 2: return board[x+dist][y];
        case 3: return board[x+dist][y+dist];
        case 4: return board[x][y+dist];
        case 5: return board[x-dist][y+dist];
        case 6: return board[x-dist][y];
        case 7: return board[x-dist][y-dist];
        default: return 0;
    }
}

void getNinePoint(int ninePoint[],int board[][N], int x,int y, int lineDir, int player)
{
    int i;              //循环变量
    for(i=4; i>=-4; --i)
    {
        if(isOverBoard(x,y,lineDir,i))
        {
            ninePoint[4-i] = -1;
        }else if(getContent(board, x, y, lineDir, i) == player)
            ninePoint[4-i] = 1;
        else if(getContent(board, x, y, lineDir, i) == 0)
        {
            ninePoint[4-i] = 0;
        }
        else
            ninePoint[4-i] = -1;
    }
    ninePoint[4] = 1;
}

int downPiece(int board[][N], int site[], int score, int player)
{
    char playerName[2][10] = {"黑方", "白方"};
    if(site[0] < 0)
    {
        printf("平局！\n");
        return 1;
    }
    else
    {
        board[site[0]][site[1]] = player;
        printf("%s在（%2d，%2d）落子\n", playerName[player>0], site[0], site[1]);
        if(score >= 100000)
        {
            printf("%s获胜！\n", playerName[player>0]);
            return 1;
        }
    }
    return 0;
}

int coreJudge(int sevenPoint[])
{
    int i;          // 循环变量
    int tag = 0;    // 等于 2 说明发生了间断
    int num = 0;    // 记录落子个数
    for(i=1; i<6; ++i)
    {
        if(sevenPoint[i] == 1)
        {
            if(tag)
                tag = 2;
            ++num;
        }
        else if(num && tag != 2)
                tag = 1;
    }
    switch(num)
    {
    case 1:
        return 2;       // 活一
    case 2:
        if((sevenPoint[0] == -1 && sevenPoint[1] == 1) || (sevenPoint[6] == -1 && sevenPoint[5] == 1))
            return 3;   // 眠二
        else if(tag == 2)
            return 4;   // 跳二
        else
            return 5;   // 连二
    case 3:
        if((sevenPoint[0] == -1 && sevenPoint[1] == 1) || (sevenPoint[6] == -1 && sevenPoint[5] == 1))
            return 6;   // 眠三
        else if(tag == 2)
            return 7;   // 跳三
        else
            return 8;   // 连三
    case 4:
        if((sevenPoint[0] == -1 && sevenPoint[1] == 1) || (sevenPoint[6] == -1 && sevenPoint[5] == 1) || tag == 2)
            return 9;   // 冲四
        else
            return 10;  // 活四
    case 5:
        return 11;      // 成五
    }
    return -1;
}
// pnum 为大于等于1的时候才调用
int judgeType(int ninePoint[], int pnum, int enemyL, int enemyR, int selfL, int selfR)
{
    int res1, res2;
    int i;
    switch(pnum)
    {
    case 1:
        if(enemyL+1 == selfL || enemyR-1 == selfR)
            return 3;           // 眠二
        else if(selfL == 3 || selfR == 5)
            return 5;           // 连二
        else
            return 4;           // 跳二
        break;
    case 2:
        res1 = judgeType(ninePoint, 1, enemyL, selfR + 1, selfL, 4);
        res2 = judgeType(ninePoint, 1, selfL - 1, enemyR, 4, selfR);
        return res1 > res2 ? res1 : res2;
    default:
        for(i = 0, res1 = res2 = -1; i < enemyR - enemyL - 5; ++i)
        {
            int j;
            int sevenPoint[7];
            sevenPoint[0] = enemyL + i == -1 ? -1 : ninePoint[enemyL + i];
            for(j = 1 ; j < 6; ++j)
            {
                sevenPoint[j] = ninePoint[i + j + enemyL];
            }
            sevenPoint[6] = enemyL + 6 + i == 9 ? -1 : ninePoint[enemyL + 6 + i];
            res1 = coreJudge(sevenPoint);
            res2 = res2 > res1 ? res2 : res1;
        }
        return res2;
    }
}

int getLineType(int ninePoint[])
{
    int i, j;
    int pnum = 0;           //线上我方已落子数
    int enemyL = -1;        //左界限
    int enemyR = 9;         //右界限
    int selfL = 4;          //我方落子开始的地方
    int selfR = 4;          //我方落子结束的地方

    for(i=0; i<4; ++i)
    {
        if(ninePoint[i] == -1)
            enemyL = i;
        else if(ninePoint[i] == 1)
        {
            ++pnum;
            selfL = i;
        }
    }
    for(j=5; j<9; j++)
    {
        if(ninePoint[j] == -1)
        {
            enemyR = j;
            break;
        }
        else if(ninePoint[j] == 1)
        {
            ++pnum;
            selfR = j;
        }
    }
    if(enemyR - enemyL - 1 < 5)
        return 0;

    switch(pnum)
    {
    case 0:
        if(enemyL == 3 || enemyR == 5)
            return 1;           // 眠一
        else
            return 2;           // 活一
        break;
    case 1:
        if(enemyL+1 == selfL || enemyR-1 == selfR)
            return 3;           // 眠二
        else if(selfL == 3 || selfR == 5)
            return 5;           // 连二
        else
            return 4;           // 跳二
        break;
    case 2:
        if(selfL + 4 < selfR)
            return judgeType(ninePoint, pnum, enemyL, enemyR, selfL, selfR);
        else
        {
            if(enemyL+1 == selfL || enemyR-1 == selfR)
                return 6;       // 眠三
            else if(ninePoint[selfL+1] == 0 || ninePoint[selfR-1] == 0)
            {
                if(ninePoint[selfL+1] == 0 && ninePoint[selfR-1] == 0)
                    return 6;   // 眠三   0-0-0
                return 7;       // 跳三
            }
            else
                return 8;       // 连三
        }
        break;
    case 3:
        if(selfL + 4 < selfR)
            return judgeType(ninePoint, pnum, enemyL, enemyR, selfL, selfR);
        else
        {
            if(enemyL+1 == selfL || enemyR-1 == selfR)
                return 9;       // 冲四
            else if(selfR - selfL + 1 == 4)
                return 10;      // 活四
            else
                return 9;       // 冲四
        }
        break;
    case 4:
        if(selfL + 4 < selfR)
            return judgeType(ninePoint, pnum, enemyL, enemyR, selfL, selfR);
        else
            return 11;          // 成五
        break;
    default:
        return judgeType(ninePoint, pnum, enemyL, enemyR, selfL, selfR);
        break;
    }

}

void calculateExpValue(int score[][N], int board[][N], int weight[][S], int player)
{
    int x, y, i;
    int p_type, temp_p_type;
    int enemy_type = 0, temp_enemy_type = 0;
    int ninePoint[9];
    for(x=0; x<N; ++x)
    {
        for(y=0; y<N; ++y)
        {
            if(board[x][y] == 0)
            {
                for(i=0, p_type=0; i<4; ++i)
                {
                    getNinePoint(ninePoint, board, x, y, i,player);
                    temp_p_type = getLineType(ninePoint);
                    if(temp_p_type >= 7 && p_type >= 7)
                        p_type = 10;
                    else if(temp_p_type > p_type)
                        p_type = temp_p_type;
                }
                for(i=0, enemy_type=0; i<4; ++i)
                {
                    getNinePoint(ninePoint, board, x, y, i,0-player);
                    temp_enemy_type = getLineType(ninePoint);
                    if(temp_enemy_type >= 7 && enemy_type >= 7)
                        enemy_type = 10;
                    else if(temp_enemy_type > enemy_type)
                        enemy_type = temp_enemy_type;
                }
                score[x][y] = p_type < enemy_type ? weight[1][enemy_type] : weight[0][p_type];
            }
        }
    }
}

int white(int board[][N], int weight[][S], int site[])      // 棋盘上的子为 1
{
    int i, j;
    int score[N][N] = {};
    int max = -1;
    site[0] = site[1] = -1;
    calculateExpValue(score, board, weight, 1);
    for(i=0; i<N; ++i)
    {
        for(j=0; j<N; ++j)
        {
            if(board[i][j] == 0)
            {
                if(max < score[i][j])
                {
                    max = score[i][j];
                    site[0] = i;
                    site[1] = j;
                }
            }
        }
    }
    return max;
}

int black(int board[][N], int weight[][S], int site[])      // 棋盘上的子为 -1
{
    int i, j;
    int score[N][N] = {};
    int max = -1;
    site[0] = site[1] = -1;
    calculateExpValue(score, board, weight, -1);
    for(i=0; i<N; ++i)
    {
        for(j=0; j<N; ++j)
        {
            if(board[i][j] == 0)
            {
                if(max < score[i][j])
                {
                    max = score[i][j];
                    site[0] = i;
                    site[1] = j;
                }
            }
        }
    }
    return max;
}
