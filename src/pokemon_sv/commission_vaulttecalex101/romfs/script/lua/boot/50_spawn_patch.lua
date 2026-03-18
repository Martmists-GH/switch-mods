---main.encount.EncountCreatePokepara:Create
_trampoline(C2B428A63695FD2C7, "S988D7D0A4EEF693D", function(old, rand, dev, form, sexType, level, pokeData, rareTryUp, option)
    if dev == 641 or dev == 642 or dev == 645 then
        return old(rand, dev, form, 1, level, pokeData, rareTryUp, option)
    else
        return old(rand, dev, form, 2, level, pokeData, rareTryUp, option)
    end
end)
