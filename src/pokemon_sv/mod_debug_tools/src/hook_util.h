#pragma once

// Used to communicate between UI and hooks
struct PokemonData {
    bool forceShiny = false;
    bool forceAlpha = false;
    bool forceModify = false;  // Does not affect shinyness
    bool allowInvalidForms = false;
    int shinyMultiplier = 1;
    int species = 1;
    int form = 0;
    int sex = 0;
    int ability = 0;
    int nature = 0;
    int level = 1;
    unsigned char iv[6] = {0};
    unsigned char ev[6] = {0};
    int moves[4] = {1, 2, 3, 4};
};
