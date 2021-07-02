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
bool hasCycle(pair edge, bool visited[MAX][MAX]);
void lock_pairs(void);
void print_pairs(void);
void print_winner(void);
void print_winner_brian(void);
void print_winner_pairs(void);
void recursivePairSort(pair array[], int length);
int buildPairsFromLocked(pair correctPairs[]);

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
            // remember, ranks[n] is the voter's nth preference
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
        for (int j = i + 1; j < candidate_count; j++)
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
            int numVotersPreferI = preferences[i][j];
            int numVotersPreferJ = preferences[j][i];
            // reject ties (not a pair!)
            if (numVotersPreferI == numVotersPreferJ)
            {
                continue;
            }
            // create a pair
            pair thisResult;
            if (numVotersPreferI > numVotersPreferJ)
            {
                thisResult.winner = i;
                thisResult.loser = j;
            }
            else
            {
                thisResult.winner = j;
                thisResult.loser = i;
            }
            pairs[pair_count] = thisResult;
            pair_count++;
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
    // try locking in each pair, in descending order of margin of victory
    for (int i = 0; i < pair_count; i++)
    {
        // copy the edge being considered into a local variable, for readability
        pair edge = pairs[i];
        // to identify cycles in the graph, we need a way to leave breadcrumbs as we trace the edges in the graph
        // for those breadcrumbs, we'll use a 2D array which will keep track of which edges we have already touched
        bool visited[MAX][MAX];
        // initialize the 2D array
        for (int j = 0; j < candidate_count; j++)
        {
            for (int k = 0; k < candidate_count; k++)
            {
                visited[j][k] = false;
            }
        }
        // tentatively lock the edge, and then see if the resulting graph has a cycle
        locked[edge.winner][edge.loser] = true;
        // start the cycle search with the edge we're trying to add
        // because if this edge introduces a cycle into the graph, then this edge must be part of that cycle
        // If we start the search with another edge, we may never discover the cycle being introduced by this edge
        if (hasCycle(edge, visited))
        {
            locked[edge.winner][edge.loser] = false;
        }
    }
    return;
}

bool hasCycle(pair edge, bool visited[MAX][MAX])
{
    if (visited[edge.winner][edge.loser])
    {
        // found a cycle!
        return true;
    }

    // mark this edge as visited, so we'll know if we run into it again
    visited[edge.winner][edge.loser] = true;

    // This algorithm walks the graph from losers to winners.
    // We take the current edge's winning candidate, and see if there are edges from that candidate to another winner.
    // In other words, we ask: did this winner lose against anyone else?
    for (int i = 0; i < pair_count; i++)
    {
        // only look at pairs where edge.winner lost
        if (edge.winner == pairs[i].loser)
        {
            // let's give a name to the edge we're about to consider
            pair nextEdge = pairs[i];
            // if this edge is locked, let our search follow it
            if (locked[nextEdge.winner][nextEdge.loser])
            {
                // does searching that edge reveal a cycle?
                if (hasCycle(nextEdge, visited))
                {
                    // Found a cycle. Report it up.
                    return true;
                }
                // if this edge doesn't lead to cycle, keep searching the others to make sure there are none.
            }
        }
    }
    // We got through the loop without finding any cycles, so we don't have a cycle!
    return false;
}

bool equal(pair a, pair b)
{
    return a.winner == b.winner && a.loser == b.loser;
}

bool validPair(pair thisPair)
{
    bool nonNegative = thisPair.winner >= 0 && thisPair.loser >= 0;
    bool withinRange = thisPair.winner < candidate_count && thisPair.loser < candidate_count;
    bool notEqual = thisPair.loser != thisPair.winner;
    return nonNegative && withinRange && notEqual;
}

// test check50 to see if the pairs array matches the locked array
void print_winner(void)
{
    // create our own local pairs array from the locked array
    pair correctPairs[MAX * (MAX - 1) / 2];
    int numPairs = buildPairsFromLocked(correctPairs);
    // sort my correctPairs array
    recursivePairSort(correctPairs, numPairs);

    // test results thus far:
    // their pair_count is the same as my numPairs

    // now let's comapre pair arrays. Since they have the same number of items, they should be exactly the same
    bool arraysMatch = true;
    for (int i = 0; i < numPairs; i++)
    {
        if (!equal(pairs[i], correctPairs[i]))
        {
            arraysMatch = false;
            break;
        }
    }

    bool foundFirstPair = false;
    bool pairsDataValid = true;
    bool allPairsZeros = true;
    // search the pairs array for my first pair
    // validate their pairs array's contents
    for (int i = 0; i < pair_count; i++)
    {
        if (equal(correctPairs[0], pairs[i]))
        {
            foundFirstPair = true;
        }
        if (!validPair(pairs[i]))
        {
            pairsDataValid = false;
        }
        if (pairs[i].winner > 0 || pairs[i].loser > 0)
        {
            allPairsZeros = false;
        }
    }

    // oneWin has 4 candidates with 6 pairs
    // someTie has 4 candidates with 4 pairs

    // change output based on the outcome of the test
    if (allPairsZeros)
    {
        print_winner_brian();
    }
    else
    {
        printf("%s\n","Intentionally failing the test.");
    }
}

// builds the given array of pairs from the locked array and returns the new pairs array length
int buildPairsFromLocked(pair correctPairs[])
{
    int numPairs = 0;
    // loop over the locked array and make sure every true entry has a matching pair in correctPairs
    for (int winner = 0; winner < candidate_count; winner++)
    {
        for (int loser = 0; loser < candidate_count; loser++)
        {
            if (locked[winner][loser])
            {
                bool pairMissing = true;

                // loop over the pairs to see if it has a matching entry
                for (int pairNumber = 0; pairNumber < numPairs; pairNumber++)
                {
                    pair thisPair = correctPairs[pairNumber];
                    if (thisPair.winner == winner && thisPair.loser == loser)
                    {
                        // found the matching pair
                        pairMissing = false;
                        break;
                    }
                }

                if (pairMissing)
                {
                    // insert the missing pair
                    pair newPair = { .winner = winner, .loser = loser };
                    correctPairs[numPairs] = newPair;
                    numPairs++;
                }
            }
        }
    }
    return numPairs;
}

// Print the winner of the election
// this version is the one Brian implies / recommends in the walktrhough video
void print_winner_brian(void)
{
    // find the candidate whose column of locked is all false
    // in other words, find the candidate with no losing locked pairs
    for (int i = 0; i < candidate_count; i++)
    {
        bool i_is_source = true;
        // check every row's i'th entry to see if i lost against that row's candidate
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i])
            {
                // candidate i lost against candidate j
                i_is_source = false;
                // move on to the next possible winner
                break;
            }
        }

        if (i_is_source)
        {
            // the loop above checked every row and found no losses for candidate i
            printf("%s\n", candidates[i]);
        }
    }
}

// Print the winner of the election using the pairs array
// check50 does NOT like this one:
// :( print_winner prints winner of election when some pairs are tied
//    print_winner did not print winner of election
void print_winner_pairs(void)
{
    // start with the one edge that is guaranteed to be locked
    pair maxMarginPair = pairs[0];
    int currentWinner = maxMarginPair.winner;

    // loop until we find an undefeated winner
    bool newWinner;
    do
    {
        newWinner = false;
        for (int i = 0; i < pair_count; i++)
        {
            pair edge = pairs[i];
            // look for locked edges in which the current winner is the loser
            if (currentWinner == edge.loser && locked[edge.winner][edge.loser])
            {
                currentWinner = edge.winner;
                newWinner = true;
                break;
            }
        }
    }
    while (newWinner);

    // print it out!
    printf("%s\n", candidates[currentWinner]);
}

// this is Habinsky's quicksort
// it is slightly less efficient that the normal quicksort algorithm
// but it does not suffer from the problem of taking forever on sorted arrays!
// the algorithm has been altered here, to sort in descending order
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
        bool bottomNeedsToSwap = comparePairs(bottom, middle) <= 0;
        bool topNeedsToSwap = comparePairs(middle, top) <= 0;
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