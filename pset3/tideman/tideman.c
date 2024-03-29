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
bool hasCycle(pair edgeBeingLocked, pair currentEdge);
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
        printf("Voter %i\n", i + 1);
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
        // tentatively lock the edge, and then see if the resulting graph has a cycle
        pair edge = pairs[i];
        locked[edge.winner][edge.loser] = true;
        // start the cycle search with the edge we're trying to add
        // because if this edge introduces a cycle into the graph, then this edge must be part of that cycle
        // If we start the search with another edge, we may not discover the cycle being introduced by this edge
        if (hasCycle(edge, edge))
        {
            locked[edge.winner][edge.loser] = false;
        }
    }
    return;
}

// to use this recursive function, lock the pair under consideration,
// then call this function with that pair for both parameters
bool hasCycle(pair edgeBeingLocked, pair currentEdge)
{
    // this function traverses the graph from winners to losers
    // We take the currentEdge's losing candidate, and search for locked match-ups in which they won
    // if the currentEdge's loser is the winner of the edgeBeingLocked, then we've run into ourselves and found a cycle
    if (currentEdge.loser == edgeBeingLocked.winner)
    {
        return true;
    }
    // find locked pairs in which the currentEdge's loser won
    for (int pairIndex = 0; pairIndex < pair_count; pairIndex++)
    {
        // get the pair
        pair thisPair = pairs[pairIndex];
        // we're only interested in locked pairs
        if (locked[thisPair.winner][thisPair.loser])
        {
            // did the currentEdge's loser win in this locked pair?
            if (currentEdge.loser == thisPair.winner)
            {
                // does this path lead to a cycle?
                if (hasCycle(edgeBeingLocked, thisPair))
                {
                    return true;
                }
            }
        }
    }
    // We got through the loop without finding any cycles, so we don't have a cycle.
    return false;
}

// Print the winner of the election
// there are two working versions of print_winner in this code: print_winner_brian and print_winner_pairs
// this function just calls print_winner_pairs, after rewriting pairs as a workaround to a check50 bug
void print_winner(void)
{
    // check50 does not write the pairs array when testing this function
    // let's rebuild the pairs array ourselves
    pair_count = buildPairsFromLocked(pairs);
    // sort the pairs
    sort_pairs();
    // now we can run my favorite pairs-based print_winner algorithm
    print_winner_pairs();
}

// Build the given array of pairs from the locked array and return the resulting pairs array length
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
                for (int pairIndex = 0; pairIndex < numPairs; pairIndex++)
                {
                    pair thisPair = correctPairs[pairIndex];
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
// check50 does NOT work with this algorithm because check50 does not fill out the pairs array
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