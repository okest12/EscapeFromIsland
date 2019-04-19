#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

#define NUMBER_ROW 9
#define NUMBER_COLUMN 9
#define NUMBER_ROUND 1000
#define MAX_STEPS 1000000
#define PF(a) ((a)*(a))

typedef struct _Position
{
    int x;
    int y;
}Position;

typedef struct _ExplorerResult
{
    float probability[NUMBER_ROW][NUMBER_COLUMN];
    float meanPath[NUMBER_ROW][NUMBER_COLUMN];
    float standardDeviation[NUMBER_ROW][NUMBER_COLUMN];
}ExplorerResult;

void explorer(const char IslandMap[NUMBER_ROW][NUMBER_COLUMN], ExplorerResult* result);
float calStandardDeviation(const float mean, const int num, const int *values);
void randomStep(const Position *directionMap, Position *current);
int status(const char map[NUMBER_ROW][NUMBER_COLUMN], const Position *current);
void printResults(const char *header, float data[NUMBER_ROW][NUMBER_COLUMN]);

int main()
{
    const char JurassicIslandMap[NUMBER_ROW][NUMBER_COLUMN] = {
        {'B','W','W','B','B','W','B','W','W'},
        {'B','L','L','V','L','L','L','L','B'},
        {'W','L','L','L','L','D','L','L','B'},
        {'B','L','L','D','L','L','L','L','W'},
        {'B','L','D','L','L','L','L','L','W'},
        {'W','L','L','L','L','L','V','L','B'},
        {'W','V','L','L','L','L','L','L','W'},
        {'W','L','L','L','D','L','L','L','W'},
        {'B','B','W','B','W','B','B','W','B'}};
    /**const char JurassicIslandMap_test1[NUMBER_ROW][NUMBER_COLUMN] = {
        {'B','W','W','B','B','W','B','W','W'},
        {'W','W','L','V','L','L','L','L','B'},
        {'W','L','L','L','L','D','L','L','B'},
        {'B','L','L','D','L','L','L','L','W'},
        {'B','L','D','L','L','L','L','L','W'},
        {'W','L','L','L','L','L','V','L','B'},
        {'W','V','L','L','L','L','L','L','W'},
        {'W','L','L','L','D','L','L','L','W'},
        {'B','B','W','B','W','B','B','W','B'}};**/

    /**const char JurassicIslandMap[NUMBER_ROW][NUMBER_COLUMN] = {
        {'B','B'},
        {'B','B'}};**/

    ExplorerResult result;
    for(int i =0; i < NUMBER_ROW; i++)
    {
        for(int j =0; j < NUMBER_COLUMN; j++)
        {
            result.probability[i][j] = 0.0;
            result.meanPath[i][j] = 0.0;
            result.standardDeviation[i][j] = 0.0;
        }
    }

    srand(time(NULL));

    explorer(JurassicIslandMap, &result);

    printResults("Percentage (probability) of getting off Jurassic Island", result.probability);
    printResults("Mean path length when escaping", result.meanPath);
    printResults("Standard deviation of path length when escaping", result.standardDeviation);
    return 0;
}

void explorer(const char IslandMap[NUMBER_ROW][NUMBER_COLUMN], ExplorerResult* result)
{
    const Position DirectionMap[8] = {
        {-1, -1},
        {-1, 0},
        {-1, 1},
        {0, -1},
        {0, 1},
        {1,-1},
        {1,0},
        {1,1}};

    for(int i =0; i < NUMBER_ROW; i++)
    {
        for(int j =0; j < NUMBER_COLUMN; j++)
        {
            if((IslandMap[i][j] == 'L') || (IslandMap[i][j] == 'B'))
            {
                int sumSW = 0, totalSteps = 0;
                int steps[NUMBER_ROUND];
                for(int r = 0; r < NUMBER_ROUND; r++)
                {
                    Position current = {i, j};
                    steps[sumSW] = 0;
                    int ret = 2;
                    for(int s = 0; (s < MAX_STEPS) && (2 == ret); s++,steps[sumSW]++)
                    {
                        randomStep(DirectionMap, &current);
                        ret = status(IslandMap, &current);
                    }

                    if(0 == ret)//"successfully made it off the island"
                    {
                        //printf("i:%d, j:%d, r:%d, steps:%d\n", i, j, r, steps[sum_sw]);
                        totalSteps += steps[sumSW++];
                    }
                    else if(2 == ret)
                    {
                        printf("Warning: still walking after %d steps, start cell is (%d,%d)\n", MAX_STEPS, i, j);
                    }
                }
                if(sumSW > 0)
                {
                    result->probability[i][j] = ((float)sumSW/NUMBER_ROUND)*100;
                    result->meanPath[i][j]  = (float)totalSteps/sumSW;
                    result->standardDeviation[i][j] = calStandardDeviation(result->meanPath[i][j], sumSW, steps);
                }
            }
        }
    }
}

float calStandardDeviation(const float mean, const int num, const int *values)
{
    float total_deviation = 0.0;
    for(int i = 0; i < num; i++)
    {
        total_deviation += PF(values[i] - mean);
    }
    return sqrt(total_deviation/num);
}

void randomStep(const Position *directionMap, Position *current)
{
    const int directionIndex = rand()%8;
    const Position *direction = &directionMap[directionIndex];
    current->x = current->x + direction->x;
    current->y = current->y + direction->y;
}

int status(const char map[NUMBER_ROW][NUMBER_COLUMN], const Position *current)
{
    int result = 0;//"successfully made it off the island"
    const int x = current->x, y = current->y;
    if((x >= 0 && x < NUMBER_ROW)  && (y >= 0 && y < NUMBER_COLUMN))
    {
        if((map[x][y] == 'L') || (map[x][y] == 'B'))
        {
            result = 2;//"continue walk"
        }
        else
        {
            result = 1;//"failure, terminate walk"
        }
    }
    return result;
}

void printResults(const char *header, float data[NUMBER_ROW][NUMBER_COLUMN])
{
    printf("\n%s\n", header);
    for(int i = 0; i < NUMBER_ROW; i++)
    {
        for(int j = 0; j < NUMBER_COLUMN; j++)
        {
            printf("%-5.2f\t", data[i][j]);
        }
        printf("\n");
    }
}