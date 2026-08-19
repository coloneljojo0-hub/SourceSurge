#!/bin/bash
set -e

cd "D:/.mygame/source-sdk-2013"

# Step 1: Restore backup and remove carriage returns
cp game/mod_tf/scripts/hudlayout.res.backup game/mod_tf/scripts/hudlayout.res
tr -d '\r' < game/mod_tf/scripts/hudlayout.res > game/mod_tf/scripts/hudlayout.res.tmp && mv game/mod_tf/scripts/hudlayout.res.tmp game/mod_tf/scripts/hudlayout.res

# Step 2: Remove trailing empty lines and ensure the file ends with a single '}'
sed -i -e :a -e '/^\n*$/{$d;ba' -e '}' game/mod_tf/scripts/hudlayout.res
if [ "$(tail -1 game/mod_tf/scripts/hudlayout.res)" != "}" ]; then
    echo "" >> game/mod_tf/scripts/hudlayout.res
    echo "}" >> game/mod_tf/scripts/hudlayout.res
fi

# Step 3: List of missing HUD elements (from the error messages)
missing=(HudScope HudScopeCharge AchievementNotificationPanel CriticalPanel QueueHUDStatus MatchSummary QuestNotificationPanel ItemQuickSwitchPanel ItemTestHUDPanel HudUpgradePanel HudKothTimeStatus HudWeaponSelection HudTrainingMsg HudStopWatch HudTournamentSetup HudTournament HudTeamSwitch HudTeamGoalTournament HudTeamGoal CSecondaryTargetID CSpectatorTargetID CMainTargetID HudStalemate HudSpectatorExtras HudObjectiveStatus NotificationPanel HudMiniGame HudMenuTauntSelection HudMenuSpyDisguise HudMedicCharge HudMatchStatus HudMannVsMachineStatus CurrencyStatusPanel HudSpeedometer HudFPS ItemAttributeTracker HudInspectPanel CTFFlagCalloutPanel DisguiseStatus HudDemomanPipes HudDemomanCharge HudDeathNotice CTFStreakNotice HudDamageIndicator HudCrosshair HudChat BuildingStatus_Engineer BuildingStatus_Spy HudBowCharge HudBossHealth HudArenaVsPanel HudArenaPlayerCount HudArenaNotification HudArenaClassLayout HudArenaCapPointCountdown HudWeaponAmmo HudAlert HudAchievementTracker CDamageAccountPanel CHealthAccountPanel CHudAccountPanel CoachedByPanel NotificationQueuePanel HudTrain HudMessage HudMenu HudWeapon CHudVote HudVoiceStatus HudVoiceSelfStatus VguiScreenCursor HudVehicle HudPredictionDump HudHintKeyDisplay HudHintDisplay HudHDRDemo HudFilmDemo HudWaveModeGameOver HudWaveModeReady HudWave2Status HudControlPointIcons HudCloseCaption HudHealth HUDAutoAim HudAnimationInfo HudHistoryResource HudGeiger HudCommentary ReplayReminder HudEurekaEffectTeleportMenu HudMenuEngyDestroy HudMenuEngyBuild WinPanel WaitingForPlayersPanel TrainingComplete StatPanel FreezePanel AnnotationsPanel)

# Step 4: For each missing element, if it's not present as a top-level key (line starting with tab and then the name), add it before the last line.
for name in "${missing[@]}"; do
    if ! grep -q "^\t$name$" game/mod_tf/scripts/hudlayout.res; then
        # We'll insert the block before the last line by using a temporary file.
        # But to avoid too many sed calls, we'll append to a temporary file and then replace.
        # However, we are in a loop, so let's do it by building the entire file in one go? 
        # Instead, let's create a temporary file for the missing blocks and then insert them all at once.
        :
    fi
done

# Let's do it differently: collect all missing blocks and then insert them before the last line.
> /tmp/missing_blocks.txt
for name in "${missing[@]}"; do
    if ! grep -q "^\t$name$" game/mod_tf/scripts/hudlayout.res; then
        echo "" >> /tmp/missing_blocks.txt
        echo -e "\t$name" >> /tmp/missing_blocks.txt
        echo -e "\t{" >> /tmp/missing_blocks.txt
        echo -e "\t\t\"fieldName\"\t\t\"$name\"" >> /tmp/missing_blocks.txt
        echo -e "\t\t\"visible\"\t\t\"1\"" >> /tmp/missing_blocks.txt
        echo -e "\t\t\"enabled\"\t\t\"1\"" >> /tmp/missing_blocks.txt
        echo -e "\t\t\"xpos\"\t\t\t\"0\"" >> /tmp/missing_blocks.txt
        echo -e "\t\t\"ypos\"\t\t\t\"0\"" >> /tmp/missing_blocks.txt
        echo -e "\t\t\"wide\"\t\t\t\"0\"" >> /tmp/missing_blocks.txt
        echo -e "\t\t\"tall\"\t\t\t\"0\"" >> /tmp/missing_blocks.txt
        echo -e "\t}" >> /tmp/missing_blocks.txt
    fi
done

# Now insert the missing blocks before the last line of the main file.
head -n -1 game/mod_tf/scripts/hudlayout.res > /tmp/head
cat /tmp/missing_blocks.txt >> /tmp/head
tail -n 1 game/mod_tf/scripts/hudlayout.res >> /tmp/head
mv /tmp/head game/mod_tf/scripts/hudlayout.res

# Step 5: Ensure the file ends with a closing brace (again) and remove any trailing empty lines.
sed -i -e :a -e '/^\n*$/{$d;ba' -e '}' game/mod_tf/scripts/hudlayout.res
if [ "$(tail -1 game/mod_tf/scripts/hudlayout.res)" != "}" ]; then
    echo "" >> game/mod_tf/scripts/hudlayout.res
    echo "}" >> game/mod_tf/scripts/hudlayout.res
fi

# Clean up
rm -f /tmp/head /tmp/missing_blocks.txt

echo "Done fixing hudlayout.res"