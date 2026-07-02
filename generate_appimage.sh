#!/bin/bash

# If the script is NOT running inside a terminal, reopen itself in one
if [ ! -t 0 ]; then
    # Try to find a terminal emulator installed on your system
    for term in gnome-terminal konsole xfce4-terminal alacritty kitty xterm; do
        if command -v "$term" >/dev/null 2>&1; then
            # Launch the terminal, execute this script, and pause at the end
            exec "$term" -e "bash -c '\"$0\" \"$@\"; echo; echo \"Press Enter to close...\"; read'"
        fi
    done
    echo "No suitable terminal emulator found." >&2
    exit 1
fi

# ==========================================
# YOUR ACTUAL BUILD CODE STARTS HERE
# ==========================================

echo "Starting AppImage Build..."

# 1. Clean up old AppImage
rm -f Info_Overlay-x86_64.AppImage

# 2. Copy ALL files and subdirectories
cp -ra build/. AppDir/usr/bin/

# 3. Run appimagetool
./appimagetool-x86_64.AppImage AppDir/

echo "Build complete!"
