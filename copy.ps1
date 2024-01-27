& $PSScriptRoot/build.ps1

if ($?) {
    & adb push build/libcustom-json-data.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/libcustom-json-data.so
    & adb shell am force-stop com.beatgames.beatsaber
    & adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
    adb logcat -c; adb logcat > test.log
}
