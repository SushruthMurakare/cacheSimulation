//
// This file contains all of the implementations of the replacement_policy
// constructors from the replacement_policies.h file.
//
// It also contains stubs of all of the functions that are added to each
// replacement_policy struct at construction time.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// replacement_policies.h file for further context on what each function is
// for.
// ============================================================================
//

#include "replacement_policies.h"

// LRU Replacement Policy
// ============================================================================
// TODO feel free to create additional structs/enums as necessary

struct array_lru
{
    int index;
    unsigned char present : 1;
};

void lru_cache_access(struct replacement_policy *replacement_policy,
                      struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    // TODO update the LRU replacement policy state given a new memory access
    int associativity = cache_system->associativity;
    int index_to_start = associativity * set_idx;
    int index_of_cache_line;

    for (int i = 0; i < associativity; i++)
    {
        struct cache_line *cache_line = &cache_system->cache_lines[index_to_start + i];
        if (tag == cache_line->tag && cache_line->status != INVALID)
        {
            index_of_cache_line = i;
            break;
        }
    }

    struct array_lru *info = (struct array_lru *)cache_system->replacement_policy->data;
    struct array_lru *start_in_array = info + (index_to_start);

    struct array_lru *pointer;
    struct array_lru *next;
    for (int i = 0; i < associativity; i++)
    {
        pointer = i + start_in_array;
        next = pointer + 1;
        if (!pointer->present)
        {
            pointer->index = index_of_cache_line;
            pointer->present = true;
            return;
        }

        if (pointer->present && (pointer->index == index_of_cache_line))
        {
            if(i == associativity -1){ 
                return;
            }
            if (!next->present)
            {
                pointer->index = index_of_cache_line;
                return;
            }
            else
            {
                struct array_lru temp = *pointer;
                *pointer = *next;
                *next = temp;
            }
        }
    }

}

uint32_t lru_eviction_index(struct replacement_policy *replacement_policy,
                            struct cache_system *cache_system, uint32_t set_idx)
{
    // TODO return the index within the set that should be evicted.
    uint32_t associativity = cache_system->associativity;
    uint32_t index_to_start = associativity * set_idx;
    struct array_lru *evict_first_data = (struct array_lru *)cache_system->replacement_policy->data + (index_to_start);
    return evict_first_data->index;
}

void lru_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // lru_replacement_policy_new function.
    free(replacement_policy->data);
}

struct replacement_policy *lru_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    struct replacement_policy *lru_rp = calloc(1, sizeof(struct replacement_policy));
    lru_rp->cache_access = &lru_cache_access;
    lru_rp->eviction_index = &lru_eviction_index;
    lru_rp->cleanup = &lru_replacement_policy_cleanup;
    // TODO allocate any additional memory to store metadata here and assign to
    // lru_rp->data.
    uint32_t index_to_start = sets * associativity;
    lru_rp->data = calloc(index_to_start, sizeof(struct array_lru));
    return lru_rp;
}

// RAND Replacement Policy
// ============================================================================
void rand_cache_access(struct replacement_policy *replacement_policy,
                       struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    // TODO update the RAND replacement policy state given a new memory access
}

uint32_t rand_eviction_index(struct replacement_policy *replacement_policy,
                             struct cache_system *cache_system, uint32_t set_idx)
{
    // TODO return the index within the set that should be evicted.
    return rand() % cache_system->associativity;
}

void rand_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // rand_replacement_policy_new function.
    free(replacement_policy->data);
}

struct replacement_policy *rand_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    // Seed randomness
    srand(time(NULL));

    struct replacement_policy *rand_rp = malloc(sizeof(struct replacement_policy));
    rand_rp->cache_access = &rand_cache_access;
    rand_rp->eviction_index = &rand_eviction_index;
    rand_rp->cleanup = &rand_replacement_policy_cleanup;

    // TODO allocate any additional memory to store metadata here and assign to
    // rand_rp->data.

    return rand_rp;
}

// LRU_PREFER_CLEAN Replacement Policy
// ============================================================================
void lru_prefer_clean_cache_access(struct replacement_policy *replacement_policy,
                                   struct cache_system *cache_system, uint32_t set_idx,
                                   uint32_t tag)
{
    // TODO update the LRU_PREFER_CLEAN replacement policy state given a new
    // memory access
    // NOTE: you may be able to share code with the LRU policy
    lru_cache_access(replacement_policy, cache_system, set_idx, tag);
}

uint32_t lru_prefer_clean_eviction_index(struct replacement_policy *replacement_policy,
                                         struct cache_system *cache_system, uint32_t set_idx)
{
    // TODO return the index within the set that should be evicted.
    int associativity = cache_system->associativity;
    int index_to_start = associativity * set_idx;
    struct cache_line *cache_line = &cache_system->cache_lines[index_to_start];
    struct array_lru *first_block = (struct array_lru *)cache_system->replacement_policy->data + (associativity * set_idx);

    for (int i = 0; i < associativity; i++)
    {

        if (cache_line[first_block->index + i].status == EXCLUSIVE)
        {
            return (uint32_t)first_block->index + i;
        }
    }
    return (uint32_t)first_block->index;
}

void lru_prefer_clean_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // lru_prefer_clean_replacement_policy_new function.
    free(replacement_policy->data);
}

struct replacement_policy *lru_prefer_clean_replacement_policy_new(uint32_t sets,
                                                                   uint32_t associativity)
{
    struct replacement_policy *lru_prefer_clean_rp = malloc(sizeof(struct replacement_policy));
    lru_prefer_clean_rp->cache_access = &lru_prefer_clean_cache_access;
    lru_prefer_clean_rp->eviction_index = &lru_prefer_clean_eviction_index;
    lru_prefer_clean_rp->cleanup = &lru_prefer_clean_replacement_policy_cleanup;

    // TODO allocate any additional memory to store metadata here and assign to
    // lru_prefer_clean_rp->data.
    uint32_t index_to_start = sets * associativity;
    lru_prefer_clean_rp->data = calloc(index_to_start, sizeof(struct array_lru));
    return lru_prefer_clean_rp;
}
