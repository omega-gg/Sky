@echo off
setlocal

call LivePortrait_env\Scripts\activate

if not "%~4"=="" (
    start /B /W python inference.py --source "%1" --driving "%2" --output-dir "%3" --flag_crop_driving_video --flag_normalize_lip --vx_ratio "%4"
) else (
    start /B /W python inference.py --source "%1" --driving "%2" --output-dir "%3" --flag_crop_driving_video --flag_normalize_lip
)
