#include "ahocorasick.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int add_state(AhoCorasick *ac)
{
    if (ac->num_states == ac->capacity)
    {
        ac->capacity *= 2;
        ac->states = (ACState *)realloc(ac->states, ac->capacity * sizeof(ACState));
    }
    int s = ac->num_states++;
    for (int i = 0; i < MAX_CHARS; i++)
    {
        ac->states[s].next[i] = -1;
    }
    ac->states[s].fail = 0;
    ac->states[s].dict_link = -1;
    ac->states[s].out = -1;
    return s;
}

void init_aho(AhoCorasick *ac)
{
    ac->num_states = 0;
    ac->capacity = 16;
    ac->patterns = 0;
    ac->states = (ACState *)malloc(ac->capacity * sizeof(ACState));
    add_state(ac); // state 0 is the root
}

void insert_pattern(AhoCorasick *ac, const unsigned char *pattern, int index, int case_insensitive)
{
    int current = 0;
    for (int i = 0; pattern[i] != '\0'; i++)
    {
        unsigned char c = pattern[i];
        if (case_insensitive)
        {
            c = (unsigned char)tolower(c);
        }
        if (ac->states[current].next[c] == -1)
        {
            ac->states[current].next[c] = add_state(ac);
        }
        current = ac->states[current].next[c];
    }
    ac->states[current].out = index;
    ac->patterns++;
}

void build_failure_links(AhoCorasick *ac)
{
    int *queue = (int *)malloc(ac->num_states * sizeof(int));
    int front = 0, rear = 0;

    // Initialize transitions for root
    for (int c = 0; c < MAX_CHARS; c++)
    {
        int nxt = ac->states[0].next[c];
        if (nxt != -1)
        {
            ac->states[nxt].fail = 0;
            queue[rear++] = nxt;
        }
        else
        {
            ac->states[0].next[c] = 0; // loop back to root
        }
    }

    // BFS to build failure links
    while (front < rear)
    {
        int s = queue[front++];
        int f = ac->states[s].fail;

        // If fail state is an output, inherit dict_link
        if (ac->states[f].out != -1 && ac->states[s].out == -1)
        {
            ac->states[s].dict_link = f;
        }

        for (int c = 0; c < MAX_CHARS; c++)
        {
            int nxt = ac->states[s].next[c];
            if (nxt != -1)
            {
                ac->states[nxt].fail = ac->states[f].next[c];
                // Inherit dict_link if fail state leads to a pattern
                if (ac->states[ac->states[nxt].fail].out != -1 && ac->states[nxt].out == -1)
                {
                    ac->states[nxt].dict_link = ac->states[nxt].fail;
                }
                else if (ac->states[ac->states[nxt].fail].dict_link != -1 && ac->states[nxt].out == -1)
                {
                    ac->states[nxt].dict_link = ac->states[ac->states[nxt].fail].dict_link;
                }
                queue[rear++] = nxt;
            }
            else
            {
                // Precompute next transition for missing edge
                ac->states[s].next[c] = ac->states[f].next[c];
            }
        }
    }

    free(queue);
}

int aho_find_matches(AhoCorasick *ac, const unsigned char *text, int **matchIndices, int case_insensitive)
{
    int s = 0;
    int length = (int)strlen((const char *)text);
    int capacity = 16;
    int count = 0;
    *matchIndices = (int *)malloc(capacity * sizeof(int));

    for (int i = 0; i < length; i++)
    {
        unsigned char c = text[i];
        if (case_insensitive)
        {
            c = (unsigned char)tolower(c);
        }
        s = ac->states[s].next[c];

        // Check if this state or any dict_linked state is an output
        if (ac->states[s].out != -1)
        {
            if (count == capacity)
            {
                capacity *= 2;
                *matchIndices = (int *)realloc(*matchIndices, capacity * sizeof(int));
            }
            (*matchIndices)[count++] = ac->states[s].out;
        }
        int dl = ac->states[s].dict_link;
        while (dl != -1)
        {
            if (ac->states[dl].out != -1)
            {
                if (count == capacity)
                {
                    capacity *= 2;
                    *matchIndices = (int *)realloc(*matchIndices, capacity * sizeof(int));
                }
                (*matchIndices)[count++] = ac->states[dl].out;
            }
            dl = ac->states[dl].dict_link;
        }
    }

    return count;
}

void free_aho(AhoCorasick *ac)
{
    free(ac->states);
    ac->states = NULL;
    ac->num_states = 0;
    ac->capacity = 0;
    ac->patterns = 0;
}
