
#include "constants/event_objects.h"
#include "constants/layouts.h"
#include "constants/maps.h"
#include "constants/weather.h"

#define ROUTE_MAP(map) { .layout=LAYOUT_##map, .group=MAP_GROUP(map), .num=MAP_NUM(map), }
#define ENCOUNTER_MAP(id, map) { .encounterId=id, .layout=LAYOUT_##map, .group=MAP_GROUP(map), .num=MAP_NUM(map) }


static const struct RogueRouteEncounter sRogueRouteTable[] = {
    // Hoenn
    //
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_FIELD0),
        .wildTypeTable = { TYPE_GRASS, TYPE_NORMAL, TYPE_FLYING }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_FIELD1),
        .wildTypeTable = { TYPE_ELECTRIC, TYPE_STEEL, TYPE_ICE }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .dropRarity = 1,
        .map = ROUTE_MAP(ROGUE_ROUTE_FOREST0),
        .wildTypeTable = { TYPE_BUG, TYPE_GHOST, TYPE_POISON }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .dropRarity = 2,
        .map = ROUTE_MAP(ROGUE_ROUTE_CAVE0),
        .wildTypeTable = { TYPE_ROCK, TYPE_ICE, TYPE_DRAGON }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_MOUNTAIN0),
        .wildTypeTable = { TYPE_GROUND, TYPE_FIRE, TYPE_FIGHTING }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_MOUNTAIN1),
#ifdef ROGUE_EXPANSION
        .wildTypeTable = { TYPE_FAIRY, TYPE_DARK, TYPE_PSYCHIC }
#else
        .wildTypeTable = { TYPE_FIGHTING, TYPE_DARK, TYPE_PSYCHIC }
#endif
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_MOUNTAIN2),
        .wildTypeTable = { TYPE_GROUND, TYPE_GHOST, TYPE_FIRE }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_MOUNTAIN3),
        .wildTypeTable = { TYPE_ROCK, TYPE_DRAGON, TYPE_POISON }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_WATER_FRONT0),
        .wildTypeTable = { TYPE_WATER, TYPE_FLYING, TYPE_GRASS }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_WATER_FRONT1),
        .wildTypeTable = { TYPE_NORMAL, TYPE_FIGHTING, TYPE_DARK }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_URBAN0),
        .wildTypeTable = { TYPE_STEEL, TYPE_ELECTRIC, TYPE_PSYCHIC }
    },
    {
        .mapFlags = ROUTE_FLAG_HOENN,
        .map = ROUTE_MAP(ROGUE_ROUTE_URBAN1),
#ifdef ROGUE_EXPANSION
        .wildTypeTable = { TYPE_FAIRY, TYPE_ICE, TYPE_BUG }
#else
        .wildTypeTable = { TYPE_WATER, TYPE_ICE, TYPE_BUG }
#endif
    },

    // Kanto
    //
    {
        .mapFlags = ROUTE_FLAG_KANTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_KANTO_FIELD0),
        .wildTypeTable = { TYPE_NORMAL, TYPE_FLYING, TYPE_GRASS }
    },
    {
        .mapFlags = ROUTE_FLAG_KANTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_KANTO_MOUNTAIN0),
        .wildTypeTable = { TYPE_FIGHTING, TYPE_ROCK, TYPE_ELECTRIC }
    },
    {
        .mapFlags = ROUTE_FLAG_KANTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_KANTO_URBAN0),
#ifdef ROGUE_EXPANSION
        .wildTypeTable = { TYPE_WATER, TYPE_FAIRY, TYPE_STEEL }
#else
        .wildTypeTable = { TYPE_WATER, TYPE_PSYCHIC, TYPE_STEEL }
#endif
    },
    {
        .mapFlags = ROUTE_FLAG_KANTO,
        .dropRarity = 1,
        .map = ROUTE_MAP(ROGUE_ROUTE_KANTO_FOREST0),
        .wildTypeTable = { TYPE_BUG, TYPE_POISON, TYPE_DARK }
    },
    {
        .mapFlags = ROUTE_FLAG_KANTO,
        .dropRarity = 2,
        .map = ROUTE_MAP(ROGUE_ROUTE_KANTO_CAVE0),
        .wildTypeTable = { TYPE_GROUND, TYPE_FIRE, TYPE_DRAGON }
    },
    {
        .mapFlags = ROUTE_FLAG_KANTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_KANTO_URBAN1),
        .wildTypeTable = { TYPE_GHOST, TYPE_ICE, TYPE_PSYCHIC }
    },

    // Johto
    //
    {
        .mapFlags = ROUTE_FLAG_JOHTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_JOHTO_FIELD0),
        .wildTypeTable = { TYPE_ELECTRIC, TYPE_GRASS, TYPE_DARK }
    },
    {
        .mapFlags = ROUTE_FLAG_JOHTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_JOHTO_FIELD1),
        .wildTypeTable = { TYPE_BUG, TYPE_NORMAL, TYPE_POISON }
    },
    {
        .mapFlags = ROUTE_FLAG_JOHTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_JOHTO_CAVE0),
        .dropRarity = 2,
#ifdef ROGUE_EXPANSION
        .wildTypeTable = { TYPE_ICE, TYPE_FAIRY, TYPE_GROUND }
#else
        .wildTypeTable = { TYPE_ICE, TYPE_DARK, TYPE_GROUND }
#endif
    },
    {
        .mapFlags = ROUTE_FLAG_JOHTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_JOHTO_WATER_FRONT0),
        .wildTypeTable = { TYPE_WATER, TYPE_DRAGON, TYPE_GHOST }
    },
    {
        .mapFlags = ROUTE_FLAG_JOHTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_JOHTO_URBAN0),
        .wildTypeTable = { TYPE_STEEL, TYPE_PSYCHIC, TYPE_FLYING }
    },
    {
        .mapFlags = ROUTE_FLAG_JOHTO,
        .map = ROUTE_MAP(ROGUE_ROUTE_JOHTO_MOUNTAIN0),
        .wildTypeTable = { TYPE_FIRE, TYPE_ROCK, TYPE_FIGHTING }
    },

    // Sinnoh
    //
    {
        .mapFlags = ROUTE_FLAG_SINNOH,
        .map = ROUTE_MAP(ROGUE_ROUTE_SINNOH_205),
        .wildTypeTable = { TYPE_FLYING, TYPE_WATER, TYPE_ROCK }
    },
    {
        .mapFlags = ROUTE_FLAG_SINNOH,
        .map = ROUTE_MAP(ROGUE_ROUTE_SINNOH_ETERNA_FOREST),
        .wildTypeTable = { TYPE_GRASS, TYPE_BUG, TYPE_GHOST }
    },
    {
        .mapFlags = ROUTE_FLAG_SINNOH,
        .map = ROUTE_MAP(ROGUE_ROUTE_SINNOH_207),
        .wildTypeTable = { TYPE_ROCK, TYPE_FIRE, TYPE_DRAGON }
    },
    {
        .mapFlags = ROUTE_FLAG_SINNOH,
        .map = ROUTE_MAP(ROGUE_ROUTE_SINNOH_215),
        .wildTypeTable = { TYPE_POISON, TYPE_NORMAL, TYPE_ICE }
    },
};

const struct RogueRouteData gRogueRouteTable = 
{
    .routeCount = ARRAY_COUNT(sRogueRouteTable),
    .routes = sRogueRouteTable,
};

static const struct RogueEncounterMap sRogueLegendaryEncounters[] = 
{
    ENCOUNTER_MAP(SPECIES_ARTICUNO, ROGUE_SPECIAL_ARTICUNO),
    ENCOUNTER_MAP(SPECIES_ZAPDOS, ROGUE_SPECIAL_ZAPDOS),
    ENCOUNTER_MAP(SPECIES_MOLTRES, ROGUE_SPECIAL_MOLTRES),
    
    ENCOUNTER_MAP(SPECIES_MEWTWO, ROGUE_SPECIAL_MEWTWO),
    ENCOUNTER_MAP(SPECIES_MEW, ROGUE_SPECIAL_MEW),

    ENCOUNTER_MAP(SPECIES_RAIKOU, ROGUE_SPECIAL_RAIKOU),
    ENCOUNTER_MAP(SPECIES_ENTEI, ROGUE_SPECIAL_ENTEI),
    ENCOUNTER_MAP(SPECIES_SUICUNE, ROGUE_SPECIAL_SUICUNE),

    ENCOUNTER_MAP(SPECIES_HO_OH, ROGUE_SPECIAL_HOOH),
    ENCOUNTER_MAP(SPECIES_LUGIA, ROGUE_SPECIAL_LUGIA),

    ENCOUNTER_MAP(SPECIES_CELEBI, ROGUE_SPECIAL_CELEBI),

    ENCOUNTER_MAP(SPECIES_REGICE, ROGUE_SPECIAL_REGI),
    ENCOUNTER_MAP(SPECIES_REGIROCK, ROGUE_SPECIAL_REGI),
    ENCOUNTER_MAP(SPECIES_REGISTEEL, ROGUE_SPECIAL_REGI),

    ENCOUNTER_MAP(SPECIES_GROUDON, ROGUE_SPECIAL_GROUDON),
    ENCOUNTER_MAP(SPECIES_KYOGRE, ROGUE_SPECIAL_KYOGRE),
    ENCOUNTER_MAP(SPECIES_RAYQUAZA, ROGUE_SPECIAL_RAYQUAZA),

    ENCOUNTER_MAP(SPECIES_LATIAS, ROGUE_SOUTHERN_ISLAND_LATIAS),
    ENCOUNTER_MAP(SPECIES_LATIOS, ROGUE_SOUTHERN_ISLAND_LATIOS),

    ENCOUNTER_MAP(SPECIES_JIRACHI, ROGUE_SPECIAL_JIRACHI),
    ENCOUNTER_MAP(SPECIES_DEOXYS, ROGUE_SPECIAL_DEOXYS),

#ifdef ROGUE_EXPANSION
    ENCOUNTER_MAP(SPECIES_UXIE, ROGUE_SPECIAL_LAKE_GUARDIAN),
    ENCOUNTER_MAP(SPECIES_MESPRIT, ROGUE_SPECIAL_LAKE_GUARDIAN),
    ENCOUNTER_MAP(SPECIES_AZELF, ROGUE_SPECIAL_LAKE_GUARDIAN),
    ENCOUNTER_MAP(SPECIES_DIALGA, ROGUE_SPECIAL_GEN4_MON),
    ENCOUNTER_MAP(SPECIES_PALKIA, ROGUE_SPECIAL_GEN4_MON),
    ENCOUNTER_MAP(SPECIES_HEATRAN, ROGUE_SPECIAL_GROUDON), // map is fitting
    ENCOUNTER_MAP(SPECIES_REGIGIGAS, ROGUE_SPECIAL_REGI),
    ENCOUNTER_MAP(SPECIES_GIRATINA, ROGUE_SPECIAL_GEN4_MON),
    ENCOUNTER_MAP(SPECIES_CRESSELIA, ROGUE_SOUTHERN_ISLAND_LATIOS), // map is fitting
    ENCOUNTER_MAP(SPECIES_PHIONE, ROGUE_SPECIAL_KYOGRE), // map is fitting
    ENCOUNTER_MAP(SPECIES_MANAPHY, ROGUE_SPECIAL_KYOGRE), // map is fitting
    ENCOUNTER_MAP(SPECIES_DARKRAI, ROGUE_SOUTHERN_ISLAND_LATIOS), // map is fitting
    ENCOUNTER_MAP(SPECIES_SHAYMIN, ROGUE_SPECIAL_CELEBI), // map is fitting
    ENCOUNTER_MAP(SPECIES_ARCEUS, ROGUE_SPECIAL_ARCEUS),

    ENCOUNTER_MAP(SPECIES_VICTINI, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_COBALION, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_TERRAKION, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_VIRIZION, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_TORNADUS, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_THUNDURUS, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_RESHIRAM, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_ZEKROM, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_LANDORUS, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_KYUREM, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_KELDEO, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_MELOETTA, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_GENESECT, ROGUE_SPECIAL_GEN5_MON),

    ENCOUNTER_MAP(SPECIES_XERNEAS, ROGUE_SPECIAL_GEN6_MON),
    ENCOUNTER_MAP(SPECIES_YVELTAL, ROGUE_SPECIAL_GEN6_MON),
    ENCOUNTER_MAP(SPECIES_ZYGARDE, ROGUE_SPECIAL_GEN6_MON),
    ENCOUNTER_MAP(SPECIES_DIANCIE, ROGUE_SPECIAL_GEN6_MON),
    ENCOUNTER_MAP(SPECIES_HOOPA, ROGUE_SPECIAL_GEN6_MON),
    ENCOUNTER_MAP(SPECIES_VOLCANION, ROGUE_SPECIAL_GEN6_MON),

    ENCOUNTER_MAP(SPECIES_TYPE_NULL, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_SILVALLY, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_TAPU_KOKO, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_TAPU_LELE, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_TAPU_BULU, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_TAPU_FINI, ROGUE_SPECIAL_GEN7_MON),
    //ENCOUNTER_MAP(SPECIES_COSMOG),
    //ENCOUNTER_MAP(SPECIES_COSMOEM),
    ENCOUNTER_MAP(SPECIES_SOLGALEO, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_LUNALA, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_NIHILEGO, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_BUZZWOLE, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_PHEROMOSA, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_XURKITREE, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_CELESTEELA, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_KARTANA, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_GUZZLORD, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_NECROZMA, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_MAGEARNA, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_MARSHADOW, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_POIPOLE, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_NAGANADEL, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_STAKATAKA, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_BLACEPHALON, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_ZERAORA, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_MELTAN, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_MELMETAL, ROGUE_SPECIAL_GEN7_MON),

    ENCOUNTER_MAP(SPECIES_ZACIAN, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_ZAMAZENTA, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_ETERNATUS, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_KUBFU, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_URSHIFU, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_ZARUDE, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_REGIELEKI, ROGUE_SPECIAL_REGI),
    ENCOUNTER_MAP(SPECIES_REGIDRAGO, ROGUE_SPECIAL_REGI),
    ENCOUNTER_MAP(SPECIES_GLASTRIER, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_SPECTRIER, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_CALYREX, ROGUE_SPECIAL_GEN8_MON),

    ENCOUNTER_MAP(SPECIES_ENAMORUS, ROGUE_SPECIAL_GEN5_MON),

    ENCOUNTER_MAP(SPECIES_WO_CHIEN, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_CHIEN_PAO, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_TING_LU, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_CHI_YU, ROGUE_SPECIAL_GEN9_MON),

    ENCOUNTER_MAP(SPECIES_KORAIDON, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_MIRAIDON, ROGUE_SPECIAL_GEN9_MON),

    ENCOUNTER_MAP(SPECIES_WALKING_WAKE, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_IRON_LEAVES, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_GOUGING_FIRE, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_RAGING_BOLT, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_IRON_BOULDER, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_IRON_CROWN, ROGUE_SPECIAL_GEN9_MON),

    ENCOUNTER_MAP(SPECIES_TERAPAGOS, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_PECHARUNT, ROGUE_SPECIAL_GEN9_MON),

    ENCOUNTER_MAP(SPECIES_OKIDOGI, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_MUNKIDORI, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_FEZANDIPITI, ROGUE_SPECIAL_GEN9_MON),
    ENCOUNTER_MAP(SPECIES_OGERPON, ROGUE_SPECIAL_GEN9_MON),
    
    // Forms
    ENCOUNTER_MAP(SPECIES_KYUREM_WHITE, ROGUE_SPECIAL_GEN5_MON),
    ENCOUNTER_MAP(SPECIES_KYUREM_BLACK, ROGUE_SPECIAL_GEN5_MON),

    ENCOUNTER_MAP(SPECIES_NECROZMA_DUSK_MANE, ROGUE_SPECIAL_GEN7_MON),
    ENCOUNTER_MAP(SPECIES_NECROZMA_DAWN_WINGS, ROGUE_SPECIAL_GEN7_MON),
    
    ENCOUNTER_MAP(SPECIES_URSHIFU_RAPID_STRIKE_STYLE, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_CALYREX_ICE_RIDER, ROGUE_SPECIAL_GEN8_MON),
    ENCOUNTER_MAP(SPECIES_CALYREX_SHADOW_RIDER, ROGUE_SPECIAL_GEN8_MON),
    
#endif
};

const struct RogueEncounterData gRogueLegendaryEncounterInfo = 
{
    .mapCount = ARRAY_COUNT(sRogueLegendaryEncounters),
    .mapTable = sRogueLegendaryEncounters
};

static const struct RogueEncounterMap sRogueTeamEncounters[] = 
{
    ENCOUNTER_MAP(TEAM_NUM_KANTO_ROCKET, ROGUE_ENCOUNTER_ROCKET_HIDEOUT),
    ENCOUNTER_MAP(TEAM_NUM_KANTO_ROCKET + TEAM_PRE_LEGEND_MAP_OFFSET, ROGUE_ENCOUNTER_SILPH_CO),

    ENCOUNTER_MAP(TEAM_NUM_JOHTO_ROCKET, ROGUE_ENCOUNTER_ROCKET_HIDEOUT),
    ENCOUNTER_MAP(TEAM_NUM_JOHTO_ROCKET + TEAM_PRE_LEGEND_MAP_OFFSET, ROGUE_ENCOUNTER_SILPH_CO),
    
    ENCOUNTER_MAP(TEAM_NUM_AQUA, ROGUE_ENCOUNTER_AQUA_HIDEOUT),
    ENCOUNTER_MAP(TEAM_NUM_AQUA + TEAM_PRE_LEGEND_MAP_OFFSET, ROGUE_ENCOUNTER_SEAFLOOR_CAVERN),

    ENCOUNTER_MAP(TEAM_NUM_MAGMA, ROGUE_ENCOUNTER_MT_CHIMNEY),
    ENCOUNTER_MAP(TEAM_NUM_MAGMA + TEAM_PRE_LEGEND_MAP_OFFSET, ROGUE_ENCOUNTER_MAGMA_HIDEOUT),
};

const struct RogueEncounterData gRogueTeamEncounterInfo = 
{
    .mapCount = ARRAY_COUNT(sRogueTeamEncounters),
    .mapTable = sRogueTeamEncounters
};

static const struct RogueEncounterMap sRouteMapsRestStop[] = 
{
    [ADVPATH_SUBROOM_RESTSTOP_FULL] = ENCOUNTER_MAP(OBJ_EVENT_GFX_NURSE, ROGUE_ENCOUNTER_REST_STOP),
    [ADVPATH_SUBROOM_RESTSTOP_SHOP] = ENCOUNTER_MAP(OBJ_EVENT_GFX_MART_EMPLOYEE, ROGUE_ENCOUNTER_SHOPS),
    [ADVPATH_SUBROOM_RESTSTOP_DAYCARE] = ENCOUNTER_MAP(OBJ_EVENT_GFX_LINK_RECEPTIONIST, ROGUE_ENCOUNTER_DAY_CARE),
    [ADVPATH_SUBROOM_RESTSTOP_BATTLE] = ENCOUNTER_MAP(OBJ_EVENT_GFX_MYSTERY_GIFT_MAN, ROGUE_ENCOUNTER_TUTORS),
};

const struct RogueEncounterData gRogueRestStopEncounterInfo = 
{
    .mapCount = ARRAY_COUNT(sRouteMapsRestStop),
    .mapTable = sRouteMapsRestStop
};

#undef ROUTE_MAP
#undef ENCOUNTER_MAP