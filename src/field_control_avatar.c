#include "global.h"
#include "battle_setup.h"
#include "bike.h"
#include "coord_event_weather.h"
#include "daycare.h"
#include "debug.h"
#include "faraway_island.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "fieldmap.h"
#include "field_control_avatar.h"
#include "field_player_avatar.h"
#include "field_poison.h"
#include "field_screen_effect.h"
#include "field_specials.h"
#include "fldeff_misc.h"
#include "item_menu.h"
#include "link.h"
#include "match_call.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "pokemon.h"
#include "safari_zone.h"
#include "script.h"
#include "secret_base.h"
#include "sound.h"
#include "start_menu.h"
#include "trainer_see.h"
#include "trainer_hill.h"
#include "wild_encounter.h"
#include "follow_me.h"
#include "constants/event_bg.h"
#include "constants/event_objects.h"
#include "constants/field_poison.h"
#include "constants/map_types.h"
#include "constants/songs.h"
#include "constants/trainer_hill.h"

#include "rogue_controller.h"
#include "rogue_debug.h"
#include "rogue_ridemon.h"

static EWRAM_DATA u8 sWildEncounterImmunitySteps = 0;
static EWRAM_DATA u16 sPreviousPlayerMetatileBehavior = 0;

u8 gSelectedObjectEvent;

static void GetPlayerPosition(struct MapPosition *);
static void GetInFrontOfPlayerPosition(struct MapPosition *);
static u16 GetPlayerCurMetatileBehavior(int);
static bool8 TryStartInteractionScript(struct MapPosition*, u16, u8);
static const u8 *GetInteractionScript(struct MapPosition*, u8, u8);
static const u8 *GetInteractedObjectEventScript(struct MapPosition *, u8, u8);
static const u8 *GetInteractedBackgroundEventScript(struct MapPosition *, u8, u8);
static const u8 *GetInteractedMetatileScript(struct MapPosition *, u8, u8);
static const u8 *GetInteractedWaterScript(struct MapPosition *, u8, u8);
static bool32 TrySetupDiveDownScript(void);
static bool32 TrySetupDiveEmergeScript(void);
static bool8 TryStartStepBasedScript(struct MapPosition *, u16, u16);
static bool8 CheckStandardWildEncounter(u16);
static bool8 TryArrowWarp(struct MapPosition *, u16, u8);
static bool8 IsWarpMetatileBehavior(u16);
static bool8 IsArrowWarpMetatileBehavior(u16, u8);
static s8 GetWarpEventAtMapPosition(struct MapHeader *, struct MapPosition *);
static void SetupWarp(struct MapHeader *, s8, struct MapPosition *);
static bool8 TryDoorWarp(struct MapPosition *, u16, u8);
static s8 GetWarpEventAtPosition(struct MapHeader *, u16, u16, u8);
static const u8 *GetCoordEventScriptAtPosition(struct MapHeader *, u16, u16, u8);
static const struct BgEvent *GetBackgroundEventAtPosition(struct MapHeader *, u16, u16, u8);
static bool8 TryStartCoordEventScript(struct MapPosition *);
static bool8 TryStartWarpEventScript(struct MapPosition *, u16);
static bool8 TryStartMiscWalkingScripts(u16);
static bool8 TryStartStepCountScript(u16);
static void UpdateFriendshipStepCounter(void);
#if 0 // RogueNote: overworld poison damage
static bool8 UpdatePoisonStepCounter(void);
#endif

void FieldClearPlayerInput(struct FieldInput *input)
{
    input->pressedAButton = FALSE;
    input->checkStandardWildEncounter = FALSE;
    input->pressedStartButton = FALSE;
    input->pressedSelectButton = FALSE;
    input->heldDirection = FALSE;
    input->heldDirection2 = FALSE;
    input->tookStep = FALSE;
    input->pressedBButton = FALSE;
    input->input_field_1_0 = FALSE;
    input->input_field_1_1 = FALSE;
    input->input_field_1_2 = FALSE;
    input->input_field_1_3 = FALSE;
    input->dpadDirection = 0;
}

void FieldGetPlayerInput(struct FieldInput *input, u16 newKeys, u16 heldKeys)
{
    u8 tileTransitionState = gPlayerAvatar.tileTransitionState;
    u8 runningState = gPlayerAvatar.runningState;
    bool8 forcedMove = MetatileBehavior_IsForcedMovementTile(GetPlayerCurMetatileBehavior(runningState));

    if ((tileTransitionState == T_TILE_CENTER && forcedMove == FALSE) || tileTransitionState == T_NOT_MOVING)
    {
        if (GetPlayerSpeed() != PLAYER_SPEED_FASTEST)
        {
            if (newKeys & START_BUTTON)
                input->pressedStartButton = TRUE;
            if (newKeys & SELECT_BUTTON)
                input->pressedSelectButton = TRUE;
            if (newKeys & A_BUTTON)
                input->pressedAButton = TRUE;
            if (newKeys & B_BUTTON)
                input->pressedBButton = TRUE;
        }

        if (heldKeys & (DPAD_UP | DPAD_DOWN | DPAD_LEFT | DPAD_RIGHT))
        {
            input->heldDirection = TRUE;
            input->heldDirection2 = TRUE;
        }
    }

    if (forcedMove == FALSE)
    {
        if (tileTransitionState == T_TILE_CENTER && runningState == MOVING)
            input->tookStep = TRUE;
        if (forcedMove == FALSE && tileTransitionState == T_TILE_CENTER)
            input->checkStandardWildEncounter = TRUE;
    }

    if (heldKeys & DPAD_UP)
        input->dpadDirection = DIR_NORTH;
    else if (heldKeys & DPAD_DOWN)
        input->dpadDirection = DIR_SOUTH;
    else if (heldKeys & DPAD_LEFT)
        input->dpadDirection = DIR_WEST;
    else if (heldKeys & DPAD_RIGHT)
        input->dpadDirection = DIR_EAST;

#if DEBUG_OVERWORLD_MENU == TRUE && DEBUG_OVERWORLD_IN_MENU == FALSE
    if ((heldKeys & DEBUG_OVERWORLD_HELD_KEYS) && input->DEBUG_OVERWORLD_TRIGGER_EVENT)
    {
        input->input_field_1_2 = TRUE;
        input->DEBUG_OVERWORLD_TRIGGER_EVENT = FALSE;
    }
#endif
}

int ProcessPlayerFieldInput(struct FieldInput *input)
{
    struct MapPosition position;
    u8 playerDirection;
    u16 metatileBehavior;

    gSpecialVar_LastTalked = 0;
    gSelectedObjectEvent = 0;

    playerDirection = GetPlayerFacingDirection();
    GetPlayerPosition(&position);
    metatileBehavior = MapGridGetMetatileBehaviorAt(position.x, position.y);

    START_TIMER(OVERWORLD_TRAINER_SIGHT_LINE);
    if (CheckForTrainersWantingBattle() == TRUE)
    {
        STOP_TIMER(OVERWORLD_TRAINER_SIGHT_LINE);
        return TRUE;
    }
    STOP_TIMER(OVERWORLD_TRAINER_SIGHT_LINE);

    START_TIMER(OVERWORLD_MAP_SCRIPT);
    if (TryRunOnFrameMapScript() == TRUE)
    {
        STOP_TIMER(OVERWORLD_MAP_SCRIPT);
        return TRUE;
    }
    STOP_TIMER(OVERWORLD_MAP_SCRIPT);

    START_TIMER(OVERWORLD_ROGUE_PLAYER_FIELD_INPUT_CB);
    if (Rogue_OnProcessPlayerFieldInput() == TRUE)
    {
        STOP_TIMER(OVERWORLD_ROGUE_PLAYER_FIELD_INPUT_CB);
        return TRUE;
    }
    STOP_TIMER(OVERWORLD_ROGUE_PLAYER_FIELD_INPUT_CB);

    if (input->pressedBButton && TrySetupDiveEmergeScript() == TRUE)
        return TRUE;
    if (input->tookStep)
    {
        IncrementGameStat(GAME_STAT_STEPS);
        IncrementBirthIslandRockStepCount();
        if (TryStartStepBasedScript(&position, metatileBehavior, playerDirection) == TRUE)
            return TRUE;
    }
    if (input->checkStandardWildEncounter && CheckStandardWildEncounter(metatileBehavior) == TRUE)
        return TRUE;
    if (input->heldDirection && input->dpadDirection == playerDirection)
    {
        if (TryArrowWarp(&position, metatileBehavior, playerDirection) == TRUE)
        {
            if(gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_RIDING)
                Rogue_OnRideMonWarp();

            return TRUE;
        }
    }

    GetInFrontOfPlayerPosition(&position);
    metatileBehavior = MapGridGetMetatileBehaviorAt(position.x, position.y);
    if (input->pressedAButton && !Rogue_IsRideMonFlying() && TryStartInteractionScript(&position, metatileBehavior, playerDirection) == TRUE)
        return TRUE;

    if (input->heldDirection2 && input->dpadDirection == playerDirection)
    {
        if (TryDoorWarp(&position, metatileBehavior, playerDirection) == TRUE)
        {
            if(gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_RIDING)
                Rogue_OnRideMonWarp();

            return TRUE;
        }
    }
    if (input->pressedAButton && TrySetupDiveDownScript() == TRUE)
        return TRUE;
    if (input->pressedStartButton)
    {
        PlaySE(SE_WIN_OPEN);
        ShowStartMenu();
        return TRUE;
    }
    if (input->pressedSelectButton && UseRegisteredKeyItemOnField() == TRUE)
        return TRUE;

#if DEBUG_OVERWORLD_MENU == TRUE && DEBUG_OVERWORLD_IN_MENU == FALSE
    if (input->input_field_1_2)
    {
        PlaySE(SE_WIN_OPEN);
        FreezeObjectEvents();
        Debug_ShowMainMenu();
        return TRUE;
    }
#endif

    return FALSE;
}

static void GetPlayerPosition(struct MapPosition *position)
{
    PlayerGetDestCoords(&position->x, &position->y);
    position->elevation = PlayerGetElevation();
}

static void GetInFrontOfPlayerPosition(struct MapPosition *position)
{
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&position->x, &position->y);
    PlayerGetDestCoords(&x, &y);
    if (MapGridGetElevationAt(x, y) != 0)
        position->elevation = PlayerGetElevation();
    else
        position->elevation = 0;
}

static u16 GetPlayerCurMetatileBehavior(int runningState)
{
    s16 x, y;

    PlayerGetDestCoords(&x, &y);
    return MapGridGetMetatileBehaviorAt(x, y);
}

static bool8 TryStartInteractionScript(struct MapPosition *position, u16 metatileBehavior, u8 direction)
{
    const u8 *script = GetInteractionScript(position, metatileBehavior, direction);

    script = Rogue_ModifyOverworldInteractionScript(position, metatileBehavior, direction, script);

    if (script == NULL)
        return FALSE;

    // Don't play interaction sound for certain scripts.
    if (script != EventScript_PC)
        PlaySE(SE_SELECT);

    ScriptContext_SetupScript(script);
    return TRUE;
}

static const u8 *GetInteractionScript(struct MapPosition *position, u8 metatileBehavior, u8 direction)
{
    const u8 *script = GetInteractedObjectEventScript(position, metatileBehavior, direction);
    if (script != NULL)
        return script;

    script = GetInteractedBackgroundEventScript(position, metatileBehavior, direction);
    if (script != NULL)
        return script;

    script = GetInteractedMetatileScript(position, metatileBehavior, direction);
    if (script != NULL)
        return script;

    script = GetInteractedWaterScript(position, metatileBehavior, direction);
    if (script != NULL)
        return script;

    return NULL;
}

const u8 *GetInteractedLinkPlayerScript(struct MapPosition *position, u8 metatileBehavior, u8 direction)
{
    u8 objectEventId;
    s32 i;

    if (!MetatileBehavior_IsCounter(MapGridGetMetatileBehaviorAt(position->x, position->y)))
        objectEventId = GetObjectEventIdByPosition(position->x, position->y, position->elevation);
    else
        objectEventId = GetObjectEventIdByPosition(position->x + gDirectionToVectors[direction].x, position->y + gDirectionToVectors[direction].y, position->elevation);

    if (objectEventId == OBJECT_EVENTS_COUNT || gObjectEvents[objectEventId].localId == OBJ_EVENT_ID_PLAYER)
        return NULL;

    for (i = 0; i < 4; i++)
    {
        if (gLinkPlayerObjectEvents[i].active == TRUE && gLinkPlayerObjectEvents[i].objEventId == objectEventId)
            return NULL;
    }

    gSelectedObjectEvent = objectEventId;
    gSpecialVar_LastTalked = gObjectEvents[objectEventId].localId;
    gSpecialVar_Facing = direction;
    return GetObjectEventScriptPointerByObjectEventId(objectEventId);
}

static const u8 *GetInteractedObjectEventScript(struct MapPosition *position, u8 metatileBehavior, u8 direction)
{
    u8 objectEventId;
    const u8 *script;
    s16 currX = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.x;
    s16 currY = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.y;
    u8 currBehavior = MapGridGetMetatileBehaviorAt(currX, currY);

    switch (direction)
    {
    case DIR_EAST:
        if (MetatileBehavior_IsSidewaysStairsLeftSideAny(metatileBehavior))
            // sideways stairs left-side to your right -> check northeast
            objectEventId = GetObjectEventIdByPosition(currX + 1, currY - 1, position->elevation);
        else if (MetatileBehavior_IsSidewaysStairsRightSideAny(currBehavior))
            // on top of right-side stairs -> check southeast
            objectEventId = GetObjectEventIdByPosition(currX + 1, currY + 1, position->elevation);
        else
            // check in front of player
            objectEventId = GetObjectEventIdByPosition(position->x, position->y, position->elevation);
        break;
    case DIR_WEST:
        if (MetatileBehavior_IsSidewaysStairsRightSideAny(metatileBehavior))
            // facing sideways stairs right side -> check northwest
            objectEventId = GetObjectEventIdByPosition(currX - 1, currY - 1, position->elevation);
        else if (MetatileBehavior_IsSidewaysStairsLeftSideAny(currBehavior))
            // on top of left-side stairs -> check southwest
            objectEventId = GetObjectEventIdByPosition(currX - 1, currY + 1, position->elevation);
        else
            // check in front of player
            objectEventId = GetObjectEventIdByPosition(position->x, position->y, position->elevation);
        break;
    default:
        objectEventId = GetObjectEventIdByPosition(position->x, position->y, position->elevation);
        break;
    }

    if (objectEventId == OBJECT_EVENTS_COUNT || gObjectEvents[objectEventId].localId == OBJ_EVENT_ID_PLAYER)
    {
        if (MetatileBehavior_IsCounter(metatileBehavior) != TRUE)
            return NULL;

        // Look for an object event on the other side of the counter.
        objectEventId = GetObjectEventIdByPosition(position->x + gDirectionToVectors[direction].x, position->y + gDirectionToVectors[direction].y, position->elevation);
        if (objectEventId == OBJECT_EVENTS_COUNT || gObjectEvents[objectEventId].localId == OBJ_EVENT_ID_PLAYER)
            return NULL;
    }

    gSelectedObjectEvent = objectEventId;
    gSpecialVar_LastTalked = gObjectEvents[objectEventId].localId;
    gSpecialVar_Facing = direction;

    if (InTrainerHill() == TRUE)
        script = GetTrainerHillTrainerScript();
    else if (objectEventId == GetFollowerObjectId())//(gObjectEvents[objectEventId].localId == OBJ_EVENT_ID_FOLLOWER)
        script = GetFollowerScriptPointer();
    else
        script = GetObjectEventScriptPointerByObjectEventId(objectEventId);

    script = GetRamScript(gSpecialVar_LastTalked, script);
    return script;
}

static const u8 *GetInteractedBackgroundEventScript(struct MapPosition *position, u8 metatileBehavior, u8 direction)
{
    const struct BgEvent *bgEvent = GetBackgroundEventAtPosition(&gMapHeader, position->x - MAP_OFFSET, position->y - MAP_OFFSET, position->elevation);

    if (bgEvent == NULL)
        return NULL;
    if (bgEvent->bgUnion.script == NULL)
        return EventScript_TestSignpostMsg;

    switch (bgEvent->kind)
    {
    case BG_EVENT_PLAYER_FACING_ANY:
    default:
        return bgEvent->bgUnion.script;
    case BG_EVENT_PLAYER_FACING_NORTH:
        if (direction != DIR_NORTH)
            return NULL;
        break;
    case BG_EVENT_PLAYER_FACING_SOUTH:
        if (direction != DIR_SOUTH)
            return NULL;
        break;
    case BG_EVENT_PLAYER_FACING_EAST:
        if (direction != DIR_EAST)
            return NULL;
        break;
    case BG_EVENT_PLAYER_FACING_WEST:
        if (direction != DIR_WEST)
            return NULL;
        break;
    case 5:
    case 6:
    case BG_EVENT_HIDDEN_ITEM:
        gSpecialVar_0x8004 = ((u32)bgEvent->bgUnion.script >> 16) + FLAG_HIDDEN_ITEMS_START;
        gSpecialVar_0x8005 = (u32)bgEvent->bgUnion.script;
        if (FlagGet(gSpecialVar_0x8004) == TRUE)
            return NULL;
        return EventScript_HiddenItemScript;
    case BG_EVENT_SECRET_BASE:
        return NULL;
    }

    return bgEvent->bgUnion.script;
}

static const u8 *GetInteractedMetatileScript(struct MapPosition *position, u8 metatileBehavior, u8 direction)
{
    s8 elevation;

    if (MetatileBehavior_IsPlayerFacingTVScreen(metatileBehavior, direction) == TRUE)
        return EventScript_TV;
    if (MetatileBehavior_IsPC(metatileBehavior) == TRUE)
        return EventScript_PC;
    if (MetatileBehavior_IsCableBoxResults1(metatileBehavior) == TRUE)
        return EventScript_CableBoxResults;
    if (MetatileBehavior_IsPokeblockFeeder(metatileBehavior) == TRUE)
        return EventScript_PokeBlockFeeder;
    if (MetatileBehavior_IsRegionMap(metatileBehavior) == TRUE)
        return EventScript_RegionMap;
    if (MetatileBehavior_IsPictureBookShelf(metatileBehavior) == TRUE)
        return EventScript_PictureBookShelf;
    if (MetatileBehavior_IsBookShelf(metatileBehavior) == TRUE)
        return EventScript_BookShelf;
    if (MetatileBehavior_IsPokeCenterBookShelf(metatileBehavior) == TRUE)
        return EventScript_PokemonCenterBookShelf;
    if (MetatileBehavior_IsVase(metatileBehavior) == TRUE)
        return EventScript_Vase;
    if (MetatileBehavior_IsTrashCan(metatileBehavior) == TRUE)
        return EventScript_EmptyTrashCan;
    if (MetatileBehavior_IsShopShelf(metatileBehavior) == TRUE)
        return EventScript_ShopShelf;
    if (MetatileBehavior_IsBlueprint(metatileBehavior) == TRUE)
        return EventScript_Blueprint;
    if (MetatileBehavior_IsPlayerFacingWirelessBoxResults(metatileBehavior, direction) == TRUE)
        return EventScript_WirelessBoxResults;
    if (MetatileBehavior_IsCableBoxResults2(metatileBehavior, direction) == TRUE)
        return EventScript_CableBoxResults;
    if (MetatileBehavior_IsQuestionnaire(metatileBehavior) == TRUE)
        return EventScript_Questionnaire;
    if (MetatileBehavior_IsTrainerHillTimer(metatileBehavior) == TRUE)
        return EventScript_TrainerHillTimer;

    elevation = position->elevation;
    if (elevation == MapGridGetElevationAt(position->x, position->y))
    {
        if (MetatileBehavior_IsSecretBaseDecorationBase(metatileBehavior) == TRUE)
        {
            CheckInteractedWithFriendsFurnitureBottom();
            return NULL;
        }
        if (MetatileBehavior_HoldsLargeDecoration(metatileBehavior) == TRUE)
        {
            CheckInteractedWithFriendsFurnitureMiddle();
            return NULL;
        }
        if (MetatileBehavior_HoldsSmallDecoration(metatileBehavior) == TRUE)
        {
            CheckInteractedWithFriendsFurnitureTop();
            return NULL;
        }
    }
    else if (MetatileBehavior_IsSecretBasePoster(metatileBehavior) == TRUE)
    {
        CheckInteractedWithFriendsPosterDecor();
        return NULL;
    }

    return NULL;
}

static const u8 *GetInteractedWaterScript(struct MapPosition *unused1, u8 metatileBehavior, u8 direction)
{
    // RogueNote: Always allow surf for follower
    if (PartyHasMonWithSurf() == TRUE && IsPlayerFacingSurfableFishableWater() == TRUE) // CheckFollowerFlag(FOLLOWER_FLAG_CAN_SURF)
        return EventScript_UseSurf;

    if (MetatileBehavior_IsWaterfall(metatileBehavior) == TRUE) // CheckFollowerFlag(FOLLOWER_FLAG_CAN_WATERFALL)
    {
        if (IsPlayerSurfingNorth() == TRUE)
            return EventScript_UseWaterfall;
        else
            return EventScript_CannotUseWaterfall;
    }
    return NULL;
}

static bool32 TrySetupDiveDownScript(void)
{
    // RogueNote: always allow dive
    //if (!CheckFollowerFlag(FOLLOWER_FLAG_CAN_DIVE))
    //    return FALSE;

    if (TrySetDiveWarp() == 2)
    {
        ScriptContext_SetupScript(EventScript_UseDive);
        return TRUE;
    }
    return FALSE;
}

static bool32 TrySetupDiveEmergeScript(void)
{
    // RogueNote: always allow dive
    //if (!CheckFollowerFlag(FOLLOWER_FLAG_CAN_DIVE))
    //    return FALSE;

    if (gMapHeader.mapType == MAP_TYPE_UNDERWATER && TrySetDiveWarp() == 1)
    {
        ScriptContext_SetupScript(EventScript_UseDiveUnderwater);
        return TRUE;
    }
    return FALSE;
}

static bool8 TryStartStepBasedScript(struct MapPosition *position, u16 metatileBehavior, u16 direction)
{
    if (TryStartCoordEventScript(position) == TRUE)
        return TRUE;
    if (TryStartWarpEventScript(position, metatileBehavior) == TRUE)
    {
        if(gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_RIDING)
            Rogue_OnRideMonWarp();
        return TRUE;
    }
    if (TryStartMiscWalkingScripts(metatileBehavior) == TRUE)
        return TRUE;
    if (TryStartStepCountScript(metatileBehavior) == TRUE)
        return TRUE;
    if (UpdateRepelCounter() == TRUE)
        return TRUE;
    return FALSE;
}

static bool8 TryStartCoordEventScript(struct MapPosition *position)
{
    const u8 *script = GetCoordEventScriptAtPosition(&gMapHeader, position->x - MAP_OFFSET, position->y - MAP_OFFSET, position->elevation);

    if (script == NULL)
        return FALSE;
    ScriptContext_SetupScript(script);
    return TRUE;
}

static bool8 TryStartMiscWalkingScripts(u16 metatileBehavior)
{
    s16 x, y;

    if (MetatileBehavior_IsCrackedFloorHole(metatileBehavior))
    {
        ScriptContext_SetupScript(EventScript_FallDownHole);
        return TRUE;
    }
    else if (MetatileBehavior_IsBattlePyramidWarp(metatileBehavior))
    {
        return TRUE;
    }
    else if (MetatileBehavior_IsSecretBaseGlitterMat(metatileBehavior) == TRUE)
    {
        DoSecretBaseGlitterMatSparkle();
        return FALSE;
    }
    else if (MetatileBehavior_IsSecretBaseSoundMat(metatileBehavior) == TRUE)
    {
        PlayerGetDestCoords(&x, &y);
        PlaySecretBaseMusicNoteMatSound(MapGridGetMetatileIdAt(x, y));
        return FALSE;
    }
    return FALSE;
}

static bool8 TryStartStepCountScript(u16 metatileBehavior)
{
    if (InUnionRoom() == TRUE)
    {
        return FALSE;
    }

    IncrementRematchStepCounter();
    UpdateFriendshipStepCounter();
    UpdateFarawayIslandStepCounter();

    if (!(gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_FORCED_MOVE) && !MetatileBehavior_IsForcedMovementTile(metatileBehavior))
    {
    #if 0 // RogueNote: overworld poison damage
        if (UpdatePoisonStepCounter() == TRUE)
        {
            ScriptContext_SetupScript(EventScript_FieldPoison);
            return TRUE;
        }
    #endif
        if (ShouldEggHatch())
        {
            //IncrementGameStat(GAME_STAT_HATCHED_EGGS);
            ScriptContext_SetupScript(EventScript_EggHatch);
            return TRUE;
        }
    }

    if (SafariZoneTakeStep() == TRUE)
        return TRUE;

    if (TryStartMatchCall())
        return TRUE;
    return FALSE;
}

// Unused
static void ClearFriendshipStepCounter(void)
{
    VarSet(VAR_FRIENDSHIP_STEP_COUNTER, 0);
}

static void UpdateFriendshipStepCounter(void)
{
    u16 *ptr = GetVarPointer(VAR_FRIENDSHIP_STEP_COUNTER);
    int i;

    (*ptr)++;
    (*ptr) %= 128;
    if (*ptr == 0)
    {
        struct Pokemon *mon = gPlayerParty;
        for (i = 0; i < PARTY_SIZE; i++)
        {
            AdjustFriendship(mon, FRIENDSHIP_EVENT_WALKING);
            mon++;
        }
    }
}

void ClearPoisonStepCounter(void)
{
    VarSet(VAR_POISON_STEP_COUNTER, 0);
}

#if 0 // RogueNote: overworld poison damage
static bool8 UpdatePoisonStepCounter(void)
{
    u16 *ptr;

    if (gMapHeader.mapType != MAP_TYPE_SECRET_BASE)
    {
        ptr = GetVarPointer(VAR_POISON_STEP_COUNTER);
        (*ptr)++;
        (*ptr) %= 4;
        if (*ptr == 0)
        {
            switch (DoPoisonFieldEffect())
            {
            case FLDPSN_NONE:
                return FALSE;
            case FLDPSN_PSN:
                return FALSE;
            case FLDPSN_FNT:
                return TRUE;
            }
        }
    }
    return FALSE;
}
#endif

void RestartWildEncounterImmunitySteps(void)
{
    // Starts at 0 and counts up to 4 steps.
    sWildEncounterImmunitySteps = 0;
}

static bool8 CheckStandardWildEncounter(u16 metatileBehavior)
{
    if (FlagGet(OW_FLAG_NO_ENCOUNTER))
        return FALSE;

    if (sWildEncounterImmunitySteps < 4)
    {
        sWildEncounterImmunitySteps++;
        sPreviousPlayerMetatileBehavior = metatileBehavior;
        return FALSE;
    }

    if (StandardWildEncounter(metatileBehavior, sPreviousPlayerMetatileBehavior) == TRUE)
    {
        sWildEncounterImmunitySteps = 0;
        sPreviousPlayerMetatileBehavior = metatileBehavior;
        return TRUE;
    }

    sPreviousPlayerMetatileBehavior = metatileBehavior;
    return FALSE;
}

static bool8 TryArrowWarp(struct MapPosition *position, u16 metatileBehavior, u8 direction)
{
    s8 warpEventId = GetWarpEventAtMapPosition(&gMapHeader, position);

    if (IsArrowWarpMetatileBehavior(metatileBehavior, direction) == TRUE && warpEventId != WARP_ID_NONE)
    {
        //if(Rogue_IsRideMonFlying())
        //    return FALSE;

        StoreInitialPlayerAvatarState();
        SetupWarp(&gMapHeader, warpEventId, position);
        DoWarp();
        return TRUE;
    }
    return FALSE;
}

static bool8 TryStartWarpEventScript(struct MapPosition *position, u16 metatileBehavior)
{
    s8 warpEventId = GetWarpEventAtMapPosition(&gMapHeader, position);

    if (warpEventId != WARP_ID_NONE && IsWarpMetatileBehavior(metatileBehavior) == TRUE)
    {
        StoreInitialPlayerAvatarState();
        SetupWarp(&gMapHeader, warpEventId, position);
        if (MetatileBehavior_IsEscalator(metatileBehavior) == TRUE)
        {
            DoEscalatorWarp(metatileBehavior);
            return TRUE;
        }
        if (MetatileBehavior_IsLavaridgeB1FWarp(metatileBehavior) == TRUE)
        {
            DoLavaridgeGymB1FWarp();
            return TRUE;
        }
        if (MetatileBehavior_IsLavaridge1FWarp(metatileBehavior) == TRUE)
        {
            DoLavaridgeGym1FWarp();
            return TRUE;
        }
        if (MetatileBehavior_IsAquaHideoutWarp(metatileBehavior) == TRUE)
        {
            DoTeleportTileWarp();
            return TRUE;
        }
        if (MetatileBehavior_IsUnionRoomWarp(metatileBehavior) == TRUE)
        {
            DoSpinExitWarp();
            return TRUE;
        }
        if (MetatileBehavior_IsMossdeepGymWarp(metatileBehavior) == TRUE)
        {
            DoMossdeepGymWarp();
            return TRUE;
        }
        DoWarp();
        return TRUE;
    }
    return FALSE;
}

static bool8 IsWarpMetatileBehavior(u16 metatileBehavior)
{
    if (MetatileBehavior_IsWarpDoor(metatileBehavior) != TRUE
     && MetatileBehavior_IsLadder(metatileBehavior) != TRUE
     && MetatileBehavior_IsEscalator(metatileBehavior) != TRUE
     && MetatileBehavior_IsNonAnimDoor(metatileBehavior) != TRUE
     && MetatileBehavior_IsLavaridgeB1FWarp(metatileBehavior) != TRUE
     && MetatileBehavior_IsLavaridge1FWarp(metatileBehavior) != TRUE
     && MetatileBehavior_IsAquaHideoutWarp(metatileBehavior) != TRUE
     && MetatileBehavior_IsMtPyreHole(metatileBehavior) != TRUE
     && MetatileBehavior_IsMossdeepGymWarp(metatileBehavior) != TRUE
     && MetatileBehavior_IsUnionRoomWarp(metatileBehavior) != TRUE)
        return FALSE;
    return TRUE;
}

static bool8 IsArrowWarpMetatileBehavior(u16 metatileBehavior, u8 direction)
{
    switch (direction)
    {
    case DIR_NORTH:
        return MetatileBehavior_IsNorthArrowWarp(metatileBehavior);
    case DIR_SOUTH:
        return MetatileBehavior_IsSouthArrowWarp(metatileBehavior);
    case DIR_WEST:
        return MetatileBehavior_IsWestArrowWarp(metatileBehavior);
    case DIR_EAST:
        return MetatileBehavior_IsEastArrowWarp(metatileBehavior);
    }
    return FALSE;
}

static s8 GetWarpEventAtMapPosition(struct MapHeader *mapHeader, struct MapPosition *position)
{
    return GetWarpEventAtPosition(mapHeader, position->x - MAP_OFFSET, position->y - MAP_OFFSET, position->elevation);
}

static void SetupWarp(struct MapHeader *unused, s8 warpEventId, struct MapPosition *position)
{
    struct WarpEvent warpEvent;

    u8 trainerHillMapId = GetCurrentTrainerHillMapId();

    if (trainerHillMapId)
    {
        if (trainerHillMapId == GetNumFloorsInTrainerHillChallenge())
        {
            if (warpEventId == 0)
                warpEvent = gMapHeader.events->warps[0];
            else
                warpEvent = *SetWarpDestinationTrainerHill4F();
        }
        else if (trainerHillMapId == TRAINER_HILL_ROOF)
        {
            warpEvent = *SetWarpDestinationTrainerHillFinalFloor(warpEventId);
        }
        else
        {
            warpEvent = gMapHeader.events->warps[warpEventId];
            Rogue_ModifyMapWarpEvent(&gMapHeader, warpEventId, &warpEvent);
        }
    }
    else
    {
        warpEvent = gMapHeader.events->warps[warpEventId];
        Rogue_ModifyMapWarpEvent(&gMapHeader, warpEventId, &warpEvent);
    }

    if (warpEvent.mapNum == MAP_NUM(NONE))
    {
        SetWarpDestinationToDynamicWarp(warpEvent.warpId);
    }
    else
    {
        const struct MapHeader *mapHeader;

        SetWarpDestinationToMapWarp(warpEvent.mapGroup, warpEvent.mapNum, warpEvent.warpId);
        UpdateEscapeWarp(position->x, position->y);
        mapHeader = Overworld_GetMapHeaderByGroupAndId(warpEvent.mapGroup, warpEvent.mapNum);
        if (mapHeader->events->warps[warpEvent.warpId].mapNum == MAP_NUM(NONE))
            SetDynamicWarp(mapHeader->events->warps[warpEventId].warpId, gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum, warpEventId);
    }
}

static bool8 TryDoorWarp(struct MapPosition *position, u16 metatileBehavior, u8 direction)
{
    s8 warpEventId;

    if (direction == DIR_NORTH)
    {
        //if(Rogue_IsRideMonFlying())
        //    return FALSE;

        if (MetatileBehavior_IsOpenSecretBaseDoor(metatileBehavior) == TRUE)
        {
            StoreInitialPlayerAvatarState();
            SetWarpDestinationToMapWarp(MAP_GROUP(ROGUE_ADVENTURE_PATHS), MAP_NUM(ROGUE_ADVENTURE_PATHS), 0);
            DoWarp();
            return TRUE;
        }

        if (MetatileBehavior_IsWarpDoor(metatileBehavior) == TRUE)
        {
            warpEventId = GetWarpEventAtMapPosition(&gMapHeader, position);
            if (warpEventId != WARP_ID_NONE && IsWarpMetatileBehavior(metatileBehavior) == TRUE)
            {
                StoreInitialPlayerAvatarState();
                SetupWarp(&gMapHeader, warpEventId, position);
                DoDoorWarp();
                return TRUE;
            }
        }
    }
    return FALSE;
}

static s8 GetWarpEventAtPosition(struct MapHeader *mapHeader, u16 x, u16 y, u8 elevation)
{
    s32 i;
    const struct WarpEvent *warpEvent = mapHeader->events->warps;
    u8 warpCount = mapHeader->events->warpCount;

    for (i = 0; i < warpCount; i++, warpEvent++)
    {
        if ((u16)warpEvent->x == x && (u16)warpEvent->y == y)
        {
            if (warpEvent->elevation == elevation || warpEvent->elevation == 0)
                return i;
        }
    }
    return WARP_ID_NONE;
}

static const u8 *TryRunCoordEventScript(const struct CoordEvent *coordEvent)
{
    if (coordEvent != NULL)
    {
        if (coordEvent->script == NULL)
        {
            DoCoordEventWeather(coordEvent->trigger);
            return NULL;
        }
        if (coordEvent->trigger == 0)
        {
            RunScriptImmediately(coordEvent->script);
            return NULL;
        }
        if (VarGet(coordEvent->trigger) == (u8)coordEvent->index)
            return coordEvent->script;
    }
    return NULL;
}

static const u8 *GetCoordEventScriptAtPosition(struct MapHeader *mapHeader, u16 x, u16 y, u8 elevation)
{
    s32 i;
    const struct CoordEvent *coordEvents = mapHeader->events->coordEvents;
    u8 coordEventCount = mapHeader->events->coordEventCount;

    for (i = 0; i < coordEventCount; i++)
    {
        if ((u16)coordEvents[i].x == x && (u16)coordEvents[i].y == y)
        {
            if (coordEvents[i].elevation == elevation || coordEvents[i].elevation == 0)
            {
                const u8 *script = TryRunCoordEventScript(&coordEvents[i]);
                if (script != NULL)
                    return script;
            }
        }
    }
    return NULL;
}

const u8 *GetCoordEventScriptAtMapPosition(struct MapPosition *position)
{
    return GetCoordEventScriptAtPosition(&gMapHeader, position->x - MAP_OFFSET, position->y - MAP_OFFSET, position->elevation);
}

static const struct BgEvent *GetBackgroundEventAtPosition(struct MapHeader *mapHeader, u16 x, u16 y, u8 elevation)
{
    u8 i;
    const struct BgEvent *bgEvents = mapHeader->events->bgEvents;
    u8 bgEventCount = mapHeader->events->bgEventCount;

    for (i = 0; i < bgEventCount; i++)
    {
        if ((u16)bgEvents[i].x == x && (u16)bgEvents[i].y == y)
        {
            if (bgEvents[i].elevation == elevation || bgEvents[i].elevation == 0)
                return &bgEvents[i];
        }
    }
    return NULL;
}

bool8 TryDoDiveWarp(struct MapPosition *position, u16 metatileBehavior)
{
    if (gMapHeader.mapType == MAP_TYPE_UNDERWATER && !MetatileBehavior_IsUnableToEmerge(metatileBehavior))
    {
        if (SetDiveWarpEmerge(position->x - MAP_OFFSET, position->y - MAP_OFFSET))
        {
            StoreInitialPlayerAvatarState();
            DoDiveWarp();
            PlaySE(SE_M_DIVE);
            return TRUE;
        }
    }
    else if (MetatileBehavior_IsDiveable(metatileBehavior) == TRUE)
    {
        if (SetDiveWarpDive(position->x - MAP_OFFSET, position->y - MAP_OFFSET))
        {
            StoreInitialPlayerAvatarState();
            DoDiveWarp();
            PlaySE(SE_M_DIVE);
            return TRUE;
        }
    }
    return FALSE;
}

u8 TrySetDiveWarp(void)
{
    s16 x, y;
    u8 metatileBehavior;

    PlayerGetDestCoords(&x, &y);
    metatileBehavior = MapGridGetMetatileBehaviorAt(x, y);
    if (gMapHeader.mapType == MAP_TYPE_UNDERWATER && !MetatileBehavior_IsUnableToEmerge(metatileBehavior))
    {
        if (SetDiveWarpEmerge(x - MAP_OFFSET, y - MAP_OFFSET) == TRUE)
            return 1;
    }
    else if (MetatileBehavior_IsDiveable(metatileBehavior) == TRUE)
    {
        if (SetDiveWarpDive(x - MAP_OFFSET, y - MAP_OFFSET) == TRUE)
            return 2;
    }
    return 0;
}

const u8 *GetObjectEventScriptPointerPlayerFacing(void)
{
    u8 direction;
    struct MapPosition position;

    direction = GetPlayerMovementDirection();
    GetInFrontOfPlayerPosition(&position);
    return GetInteractedObjectEventScript(&position, MapGridGetMetatileBehaviorAt(position.x, position.y), direction);
}

int SetCableClubWarp(void)
{
    struct MapPosition position;

    GetPlayerMovementDirection();  //unnecessary
    GetPlayerPosition(&position);
    MapGridGetMetatileBehaviorAt(position.x, position.y);  //unnecessary
    SetupWarp(&gMapHeader, GetWarpEventAtMapPosition(&gMapHeader, &position), &position);
    return 0;
}
