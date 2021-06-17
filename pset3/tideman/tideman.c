#include <cs50.h>
#include <stdio.h>

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
bool comparePairs(pair pair1, pair pair2);
void lock_pairs(void);
void print_winner(void);

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
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // find the candidate matching the given name
    chosenCandidateIndex = -1;
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
            pair thisResult;
            int numVotersPreferI = preferences[j][i];
            int numVotersPreferJ = preferences[i][j];
            if (numVotersPreferI > numVotersPreferJ)
            {
                thisResult.winner = i;
                thisResult.loser = j;
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
    // bubble sort
    for (int i = 1; i < pair_count; i++)
    {
        for (int j = 0; j < pair_count - i; j++)
        {
            if (comparePairs(pairs[j], pairs[j + 1]) > 0)
            {
                pair temp = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = temp;
            }
        }

    }
    return;
}

// Compare pairs. Works like strcmp:
//  -zero is equal
//  -negative means firts item is smaller
//  -positive means first item is larger.
bool comparePairs(pair pair1, pair pair2)
{
    // because every voter has to rank every candidate,
    // the number of voters that prefer candidate i over candidate j
    // is all the information needed to sort victory strength
    // because the number of voters that prefer candidate j over candidate i is:
    // preferences[j][i] = numVoters - preferences[i][j]
    int strength1 = preferences[pair1.winner][pair1.loser];
    int strength2 = preferences[pair2.winner][pair2.loser];
    return strength1 - strength2;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // TODO
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // TODO
    return;
}

