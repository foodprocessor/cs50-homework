#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
int comparePairs(pair pair1, pair pair2);
bool hasCycle(int newEdgeWinner, int newEdgeLoser, bool traversedEdges[MAX][MAX]);
void lock_pairs(void);
void print_pairs(void);
void print_winner(void);
void recursivePairSort(pair array[], int length);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[j] is voter's jth preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    print_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // find the candidate matching the given name
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            // remember, rank[i] is the voter's ith preference
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // iterate over the list of ranks
    for (int i = 0; i < candidate_count - 1; i++)
    {
        // i < j, so ranks[i] is prefered (wins) over ranks[j]
        int winner = ranks[i];
        for (int j = i + 1; j < candidate_count; i++)
        {
            int loser = ranks[j];
            preferences[winner][loser]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // iterate over the preferences to create pairs for each one
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            int numVotersPreferI = preferences[j][i];
            int numVotersPreferJ = preferences[i][j];
            // ignore cases where i is the loser
            // this loop will hit both cases eventually
            if (numVotersPreferI > numVotersPreferJ)
            {
                pair thisResult;
                thisResult.winner = i;
                thisResult.loser = j;
                pairs[pair_count] = thisResult;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    recursivePairSort(pairs, pair_count);
    // // bubble sort (for fallback)
    // for (int i = 1; i < pair_count; i++)
    // {
    //     for (int j = 0; j < pair_count - i; j++)
    //     {
    //         if (comparePairs(pairs[j], pairs[j + 1]) > 0)
    //         {
    //             pair temp = pairs[j];
    //             pairs[j] = pairs[j + 1];
    //             pairs[j + 1] = temp;
    //         }
    //     }

    // }
    return;
}

// Compare pairs. Works like strcmp:
//  -zero is equal
//  -negative means firts item is smaller
//  -positive means first item is larger.
int comparePairs(pair pair1, pair pair2)
{
    // because every voter has to rank every candidate, that means:
    //  pref[i][j] = numVoters - pref[j][i]
    //  so,
    //  voteMargin = pref[i][j] - pref[j][i] = pref[i][j] - (numVoters - pref[i][j]) = 2*pref[i][j] - numVoters
    // so the number of people that prefer the winner to the loser is linearly related to the winning margin
    // so the two are interchangeable for the purposes of sorting
    int strength1 = preferences[pair1.winner][pair1.loser];
    int strength2 = preferences[pair2.winner][pair2.loser];
    return strength1 - strength2;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        // get the information for this pair
        pair topPair = pairs[i];
        int edgeWinner = topPair.winner;
        int edgeLoser = topPair.loser;
        // to identify cycles in the graph, we need a way to leave breadcrumbs as we trace the edges in the graph
        // for those breadcrumbs, we'll use a 2D array which will keep track of which edges we have already touched
        bool traversedEdges[MAX][MAX];
        // initialize the 2D array (this is boring!)
        for (int j = 0; j < candidate_count; j++)
        {
            for (int k = 0; k < candidate_count; k++)
            {
                traversedEdges[j][k] = false;
            }
        }
        // check if adding this edge would cause a cycle
        if (!hasCycle(edgeWinner, edgeLoser, traversedEdges))
        {
            locked[edgeWinner][edgeLoser] = true;
        }
    }
    return;
}

bool hasCycle(int newEdgeWinner, int newEdgeLoser, bool traversedEdges[MAX][MAX])
{
    if (traversedEdges[newEdgeWinner][newEdgeLoser])
    {
        // found a cycle!
        return true;
    }
    // mark this edge as traversed (we're about to traverse it)
    traversedEdges[newEdgeWinner][newEdgeLoser] = true;
    // now find locked edges for which the current pair's winner is a loser
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[i][newEdgeWinner])
        {
            if (hasCycle(i, newEdgeWinner, traversedEdges))
            {
                return true;
            }
        }
    }
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    // TODO
    return;
}

// this is Habinsky's quicksort
// it is slightly less efficient that the normal quicksort algorithm
// but it does not suffer from the problem of taking forever on sorted arrays!
void recursivePairSort(pair array[], int length)
{
    // base case
    if (length <= 1)
    {
        return;
    }
    // pick a random value for a guessed median
    pair middle = array[length / 2];
    // loop from the outside in
    int bottomIndex = 0;
    int topIndex = length - 1;
    // this loop exit condition is designed to get the indices to just pass each other before exiting
    while (bottomIndex <= topIndex)
    {
        pair bottom = array[bottomIndex];
        pair top = array[topIndex];
        bool bottomNeedsToSwap = comparePairs(bottom, middle) >= 0;
        bool topNeedsToSwap = comparePairs(middle, top) >= 0;
        if (!bottomNeedsToSwap)
        {
            bottomIndex++;
        }
        if (!topNeedsToSwap)
        {
            topIndex--;
        }
        else if (bottomNeedsToSwap && topNeedsToSwap)
        {
            if (bottomIndex != topIndex)
            {
                // swap them!
                array[bottomIndex] = top;
                array[topIndex] = bottom;
                // move the indices
                bottomIndex++;
                topIndex--;
            }
            else // topIndex == bottomIndex
            {
                // if the indices are the same,
                // incrementing both will break the recursion math below
                topIndex--;
            }
        }
    }
    // recurse on bottom and top sections
    // the bottom section is from index 0 with length topIndex + 1
    recursivePairSort(array, topIndex + 1);
    // the top section is from bottomIndex with length length - bottomIndex
    recursivePairSort(array + bottomIndex, length - bottomIndex);
}

void print_pairs(void)
{
    printf("Printing pairs...\n");
    for (int i = 0; i < pair_count; i++)
    {
        int winner = pairs[i].winner;
        int loser = pairs[i].loser;
        string winnerName = candidates[winner];
        string loserName = candidates[loser];
        printf("%s (%i) won against %s (%i) with %i votes.\n",
            winnerName, winner, loserName, loser, preferences[winner][loser]);
    }
}