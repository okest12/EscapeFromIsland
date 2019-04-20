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

const Position DirectionMap[8] = {//Direction map for a random step
    {0, -1},//N
    {1, -1},//NE
    {1, 0},//E
    {1, 1},//SE
    {0, 1},//S
    {-1,1},//SW
    {-1,0},//w
    {-1,-1}};//NW

void explorer(const char IslandMap[NUMBER_ROW][NUMBER_COLUMN], ExplorerResult* result);
float calStandardDeviation(const float mean, const int num, const int *values);
void randomStep(Position *current);
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
    for(int i =0; i < NUMBER_ROW; i++)
    {
        for(int j =0; j < NUMBER_COLUMN; j++)
        {
            //Starting from cell (i, j), only perform the random walks when the starting cell is valid('L' or 'B')
            if((IslandMap[i][j] == 'L') || (IslandMap[i][j] == 'B'))
            {
                int sumSW = 0;//Number of successful walks, initialized as 0
				int totalSteps = 0;//Number of the total steps for all of the successful walks, used to calculate the mean path length
                int steps[NUMBER_ROUND];//Steps of each successful walks, the maximum considiton is the number of round
                for(int r = 0; r < NUMBER_ROUND; r++)
                {
                    Position current = {i, j};//Initialize the starting cell as (i, j) for each random walks
                    int ret = 2;//Initialize the status as "continue walk"
                    steps[sumSW] = 0;//Initialize the steps of this random walks as 0

                    for(int s = 0; (s < MAX_STEPS) && (2 == ret); s++,steps[sumSW]++)
                    {//Perform a random step until the status is not "continue walk" or the maximum steps reached
                        randomStep(&current);
                        ret = status(IslandMap, &current);
                    }

                    if(0 == ret)//"successfully made it off the island"
                    {//Add the steps of this successful walks to the total steps, then increase the number of successful walks
                        totalSteps += steps[sumSW++];
                    }
                    else if(2 == ret)
                    {//Abnormal condition, the random walk continues for a maximum steps
                        printf("Warning: still walking after %d steps, starting cell is (%d,%d)\n", MAX_STEPS, i, j);
                    }
                }
                if(sumSW > 0)
                {
                    result->probability[i][j] = ((float)sumSW/NUMBER_ROUND)*100;//Calculate the probability, using the number of successful walks divide the total random walks
                    result->meanPath[i][j]  = (float)totalSteps/sumSW;//Calculate the mean path length, using the number of the total steps divide the number of successful walks
                    result->standardDeviation[i][j] = calStandardDeviation(result->meanPath[i][j], sumSW, steps);//Calculate the standard deviation of path lengths of successful walks
                }
            }
        }
    }
}

//Calculate the standard deviation of path lengths of successful walks
float calStandardDeviation(const float mean, const int num, const int *values)
{
    float totalDeviation = 0.0;
    for(int i = 0; i < num; i++)
    {
        totalDeviation += PF(values[i] - mean);
    }
    return sqrt(totalDeviation/num);
}

//Perform a random step to change the current position
void randomStep(Position *current)
{
    const int directionIndex = rand()%8;
    const Position *direction = &DirectionMap[directionIndex];
    current->x += direction->x;
    current->y += direction->y;
}

//Check the current status
int status(const char map[NUMBER_ROW][NUMBER_COLUMN], const Position *current)
{
    int result = 0;//"successfully made it off the island"
    const int x = current->x, y = current->y;
    if((x >= 0 && x < NUMBER_ROW)  && (y >= 0 && y < NUMBER_COLUMN))
    {//Still in the island
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

//Out put the result
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
