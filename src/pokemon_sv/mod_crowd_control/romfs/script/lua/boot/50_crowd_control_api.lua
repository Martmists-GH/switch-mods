local species = {1,2,3,4,5,6,7,8,9,23,24,25,26,27,28,35,36,37,38,39,40,43,44,45,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,69,70,71,72,73,74,75,76,79,80,81,82,84,85,86,87,88,89,90,91,92,93,94,96,97,100,101,102,103,106,107,109,110,111,112,113,116,117,123,125,126,128,129,130,131,132,133,134,135,136,137,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,167,168,170,171,172,173,174,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,203,204,205,206,207,209,210,211,212,214,215,216,217,218,219,220,221,225,227,228,229,230,231,232,233,234,235,236,237,239,240,242,243,244,245,246,247,248,249,250,252,253,254,255,256,257,258,259,260,261,262,270,271,272,273,274,275,278,279,280,281,282,283,284,285,286,287,288,289,296,297,298,299,302,307,308,311,312,313,314,316,317,322,323,324,325,326,328,329,330,331,332,333,334,335,336,339,340,341,342,349,350,353,354,355,356,357,358,361,362,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,401,402,403,404,405,408,409,410,411,415,416,417,418,419,422,423,424,425,426,429,430,433,434,435,436,437,438,440,442,443,444,445,446,447,448,449,450,453,454,456,457,459,460,461,462,464,466,467,469,470,471,472,473,474,475,476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,492,493,495,496,497,498,499,500,501,502,503,522,523,529,530,532,533,534,540,541,542,546,547,548,549,550,551,552,553,559,560,570,571,572,573,574,575,576,577,578,579,580,581,585,586,590,591,594,595,596,602,603,604,607,608,609,610,611,612,613,614,615,619,620,622,623,624,625,627,628,629,630,633,634,635,636,637,638,639,640,641,642,643,644,645,646,647,648,650,651,652,653,654,655,656,657,658,661,662,663,664,665,666,667,668,669,670,671,672,673,677,678,686,687,690,691,692,693,700,701,702,703,704,705,706,707,708,709,712,713,714,715,719,720,721,722,723,724,725,726,727,728,729,730,731,732,733,734,735,736,737,738,739,740,741,742,743,744,745,747,748,749,750,751,752,753,754,757,758,761,762,763,764,765,766,769,770,774,775,778,779,782,783,784,789,790,791,792,800,801,810,811,812,813,814,815,816,817,818,819,820,821,822,823,833,834,837,838,839,840,841,842,843,844,845,846,847,848,849,854,855,856,857,858,859,860,861,863,868,869,870,871,872,873,874,875,876,877,878,879,884,885,886,887,888,889,890,891,892,893,894,895,896,897,898,899,900,901,902,903,904,905,906,907,908,909,910,911,912,913,914,915,916,917,918,919,920,921,922,923,924,925,926,927,928,929,930,931,932,933,934,935,936,937,938,939,940,941,942,943,944,945,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,961,962,963,964,965,966,967,968,969,970,971,972,973,974,975,976,977,978,979,980,981,982,983,984,985,986,987,988,989,990,991,992,993,994,995,996,997,998,999,1000,1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,1012,1013,1014,1015,1016,1017,1018,1019,1020,1021,1022,1023,1024,1025}
local moves = {1, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 42, 43, 44, 45, 46, 47, 48, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 97, 98, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 113, 114, 115, 116, 118, 120, 122, 123, 124, 126, 127, 129, 133, 135, 136, 137, 138, 139, 141, 143, 144, 147, 150, 151, 152, 153, 154, 156, 157, 160, 161, 162, 163, 164, 165, 166, 167, 168, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 186, 187, 188, 189, 191, 192, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 217, 219, 220, 221, 223, 224, 225, 226, 227, 229, 230, 231, 232, 234, 235, 236, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 259, 260, 261, 262, 263, 264, 266, 268, 269, 270, 271, 272, 273, 275, 276, 278, 280, 281, 282, 283, 284, 285, 286, 291, 292, 294, 295, 296, 297, 298, 299, 303, 304, 305, 306, 307, 308, 309, 310, 311, 313, 314, 315, 317, 319, 321, 322, 323, 325, 326, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 359, 360, 361, 362, 364, 365, 366, 367, 368, 369, 370, 371, 372, 374, 379, 380, 383, 384, 385, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 427, 428, 430, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 444, 446, 447, 449, 450, 451, 452, 453, 454, 455, 457, 458, 459, 460, 461, 462, 463, 464, 465, 467, 468, 469, 470, 471, 472, 473, 474, 476, 478, 479, 482, 483, 484, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 499, 500, 501, 502, 503, 504, 505, 506, 508, 509, 510, 511, 512, 513, 514, 515, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529, 532, 533, 534, 535, 538, 539, 540, 541, 542, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, 562, 564, 565, 566, 568, 570, 571, 572, 573, 574, 575, 576, 577, 580, 581, 583, 584, 585, 586, 587, 589, 590, 591, 592, 593, 594, 595, 596, 597, 598, 602, 603, 604, 605, 606, 607, 608, 609, 610, 611, 618, 619, 620, 621, 659, 660, 661, 662, 663, 664, 665, 666, 667, 668, 669, 670, 672, 675, 676, 678, 679, 680, 681, 682, 683, 684, 686, 688, 689, 690, 691, 692, 693, 694, 705, 706, 707, 709, 710, 711, 713, 714, 715, 716, 722, 744, 745, 746, 747, 748, 749, 750, 751, 752, 756, 775, 776, 777, 778, 780, 781, 782, 783, 784, 785, 786, 787, 788, 789, 790, 791, 793, 796, 797, 798, 799, 800, 801, 802, 803, 804, 805, 806, 807, 808, 809, 810, 811, 812, 813, 814, 815, 816, 817, 818, 819, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 830, 831, 832, 833, 834, 835, 836, 837, 838, 839, 840, 841, 842, 843, 844, 845, 846, 847, 848, 849, 850, 851, 852, 853, 854, 855, 856, 857, 858, 859, 860, 861, 862, 863, 864, 865, 866, 867, 868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 878, 879, 880, 881, 882, 883, 884, 885, 886, 887, 888, 889, 890, 891, 892, 893, 894, 895, 896, 897, 898, 899, 900, 901, 902, 903, 904, 905, 906, 907, 908, 909, 910, 911, 912, 913, 914, 915, 916, 917, 918, 919}

function API:CC_SPAWN()
  local sp, fm = API:randomPokemonSpecies()
  print("Spawning species", sp, "and form", fm)
  ---main.encount.EncountManager.s_controlObject:getWorldTranslation
  local x, y, z = C4EE52E49562F8277.SBF5E0EFFB821427F:f7360ED03()
  API:spawn(sp, fm, 90, x, y, z)
end

function API:CC_RESET_MONEY()
  ---main.management.currency.CurrencyManager.Instance:SetOkudokai
  C41AF1C042AB05442.S264F26F6894F3392:F1588D0D28E9CCA94(0)
end

local filterTimer = 0
local funFilters = {2, 5, 6, 8, 11, -1}
function API:CC_RANDOM_FILTER(time)
  local filt = funFilters[math.random(#funFilters)]

  ---main.event.command.PostEffectCommand.StopScreenBlur
  C633B39AA77B60DA3.SA811BB295BC57024()
  ---main.event.command.PostEffectCommand.DisablePhotoFilter
  C633B39AA77B60DA3.S386D3DCE2989826F()

  if filt == -1 then
    ---main.event.command.PostEffectCommand.StartScreenBlur
    C633B39AA77B60DA3.S168DFBCB35622E1E({0, 0, 0, 0}, {100, 0, 0, 0}, 0)
  else
    ---main.ui.photomode.main.PhotoFilterController.get_Instance:SetFilter
    CAEA636D5598C61A0.S385504EFF7E842C3():F8E7AA61914DEB22D(filt, false)
  end

  local shouldEnable = filterTimer == 0

  filterTimer = cc_Tick() + time

  if (shouldEnable) then
    local removeFn
    removeFn = API:addTickListener(function()
      while (cc_Tick() < filterTimer) and (filterTimer ~= 0) do
        coroutine.yield()
      end

      filterTimer = 0

      ---main.event.command.PostEffectCommand.StopScreenBlur
      C633B39AA77B60DA3.SA811BB295BC57024()
      ---main.event.command.PostEffectCommand.DisablePhotoFilter
      C633B39AA77B60DA3.S386D3DCE2989826F()

      removeFn()
    end)
  end
end

function API:CC_METRONOME()
  ---generated.cmn.GameData.GetPlayerParty
  local party = c1A1CBE3B.fFCE06E04()
  ---generated.pml.PokeParty:GetMemberCount
  for i = 0, party:f62782BA1() - 1 do
    ---generated.pml.PokeParty:GetMemberPointer
    local poke = party:f0FDE774D(i)
    for i = 0, 3 do
      ---generated.pml.pokepara.PokemonParam:SetWazaInt
      poke:fC482D86E(i, 118)
    end
  end
end

function API:CC_RANDOMIZE_MOVES()
  ---generated.cmn.GameData.GetPlayerParty
  local party = c1A1CBE3B.fFCE06E04()
  ---generated.pml.PokeParty:GetMemberCount
  for i = 0, party:f62782BA1() - 1 do
    ---generated.pml.PokeParty:GetMemberPointer
    local poke = party:f0FDE774D(i)

    ---generated.pml.pokepara.PokemonParam:GetWazaCount
    for i = 0, poke:f6269FACE()-1 do
      ---generated.pml.pokepara.PokemonParam:SetWazaInt
      poke:fC482D86E(i, moves[math.random(#moves)])
    end
  end
end

function API:CC_RANDOMIZE_TEAM()
  math.randomseed(cc_Tick())

  ---generated.cmn.GameData.GetPlayerParty
  local party = c1A1CBE3B.fFCE06E04()
  ---generated.pml.PokeParty:GetMemberCount
  for i = 0, party:f62782BA1() - 1 do
    ---generated.pml.PokeParty:GetMemberPointer
    local poke = party:f0FDE774D(i)

    ---generated.pml.pokepara.PokemonParam:GetLevel
    local param = API:randomPokemon(poke:fD2EF1BBB())

    ---generated.pml.PokeParty:ReplaceMember
    party:f36E12C71(i, param)
  end
end

local disableTimer = 0
function API:CC_ABILITY_DISABLED()
  return disableTimer ~= 0
end

function API:CC_DISABLE_ABILITIES(time)
  local shouldEnable = disableTimer == 0

  disableTimer = cc_Tick() + time

  if (shouldEnable) then
    local removeFn
    removeFn = API:addTickListener(function()
      while (cc_Tick() < disableTimer) and (disableTimer ~= 0) do
        coroutine.yield()
      end

      disableTimer = 0

      removeFn()
    end)
  end
end

local randomSpawnTimer = 0
function API:CC_RANDOM_SPAWNS_ENABLED()
  return randomSpawnTimer ~= 0
end
function API:CC_ENABLE_RANDOM_SPAWNS(time)
  local shouldEnable = randomSpawnTimer == 0

  randomSpawnTimer = cc_Tick() + time

  if (shouldEnable) then
    local removeFn
    removeFn = API:addTickListener(function()
      while (cc_Tick() < randomSpawnTimer) and (randomSpawnTimer ~= 0) do
        coroutine.yield()
      end

      randomSpawnTimer = 0

      removeFn()
    end)
  end
end

local spawnBoostTimer = 0
function API:CC_SPAWN_BOOST_ENABLED()
  return spawnBoostTimer ~= 0
end
function API:CC_ENABLE_SPAWN_BOOST(time)
  local shouldEnable = spawnBoostTimer == 0

  spawnBoostTimer = cc_Tick() + time

  if (shouldEnable) then
    local removeFn
    removeFn = API:addTickListener(function()
      while (cc_Tick() < spawnBoostTimer) and (spawnBoostTimer ~= 0) do
        coroutine.yield()
      end

      spawnBoostTimer = 0

      removeFn()
    end)
  end
end

local shinyBoostTimer = 0
function API:CC_SHINY_BOOST_ENABLED()
  return shinyBoostTimer ~= 0
end
function API:CC_ENABLE_SHINY_BOOST(time)
  local shouldEnable = shinyBoostTimer == 0

  shinyBoostTimer = cc_Tick() + time

  if (shouldEnable) then
    local removeFn
    removeFn = API:addTickListener(function()
      while (cc_Tick() < shinyBoostTimer) and (shinyBoostTimer ~= 0) do
        coroutine.yield()
      end

      shinyBoostTimer = 0

      removeFn()
    end)
  end
end
