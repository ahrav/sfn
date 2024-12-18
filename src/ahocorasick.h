#ifndef AHOCORASICK_H
#define AHOCORASICK_H

#include <stddef.h>
#include <stdint.h>

/* Maximum number of characters (full ASCII set) */
#define MAX_CHARS 256

/* The ACState represents a single state in the Aho-Corasick automaton */
typedef struct
{
  int next[MAX_CHARS]; // Transitions for each character
  int fail;            // Failure link
  int dict_link;       // Dictionary link for suffix matches
  int out;             // Pattern index if this state corresponds to an end-of-pattern (-1 if none)
} ACState;

/* AhoCorasick structure holds the entire automaton */
typedef struct
{
  ACState *states;
  int num_states; // Number of states currently in use
  int capacity;   // Current allocated capacity for states
  int patterns;   // Number of patterns inserted
} AhoCorasick;

/**
 * Initialize Aho-Corasick automaton
 */
void init_aho(AhoCorasick *ac);

/**
 * Insert a pattern into the Aho-Corasick automaton.
 * index: pattern index (usually the order in which pattern was added)
 * case_insensitive: if non-zero, pattern is converted to lowercase.
 */
void insert_pattern(AhoCorasick *ac, const unsigned char *pattern, int index, int case_insensitive);

/**
 * Build failure and dictionary links after all patterns are inserted.
 */
void build_failure_links(AhoCorasick *ac);

/**
 * Find matches in the given text using Aho-Corasick automaton.
 * Returns number of matches found.
 * matchIndices is an out-parameter: array of pattern indices matched.
 * Caller must free(matchIndices).
 */
int aho_find_matches(AhoCorasick *ac, const unsigned char *text, int **matchIndices, int case_insensitive);

/**
 * Free resources associated with the AhoCorasick automaton.
 */
void free_aho(AhoCorasick *ac);

#endif // AHOCORASICK_H
