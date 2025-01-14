echo "Note: This program must be ran from games root directory, ie the navaduel folder"
if [ $(basename $(pwd)) == 'navaduel' ]; then
echo "Building makefiles..."
mkdir ./build
cmake -DCMAKE_C_COMPILER=gcc -S ./ -B build
echo "Done!"
echo "Building executable file..."
cmake --build build
echo "Done!"
echo "Moving executable to root directory..."
mv -f ./build/Navaduel ./Navaduel
echo "Done!"
echo "Making binary executable..."
sudo chmod +x Navaduel
echo "Done!"
echo "Creating Desktop Shortcut. Note: If you move the Navaduel folder, run ./install.sh again to update your Navaduel.desktop file."
touch $HOME/Desktop/Navaduel.desktop
echo "[Desktop Entry]
Version=1.0
Name=Navaduel
Terminal=false
Exec=$(pwd)/Navaduel
Path=$(pwd)/
Icon=$(pwd)/resources/N.png
Type=Application
Categories=Game;" | tee $HOME/Desktop/Navaduel.desktop
chmod +x $HOME/Desktop/Navaduel.desktop
echo "User install..."
sudo cp $HOME/Desktop/Navaduel.desktop $HOME/.local/share/applications/Navaduel.desktop
echo "Finished!"
else 
echo "Install not ran from game's root directory"
fi
#./navaduel
