echo "Building makefiles..."
mkdir ./build
cmake CmakeLists.txt -B ./build
echo "Done!"
echo "Building executable file..."
cmake --build ./build
echo "Done!"
echo "Moving executable to root directory..."
mv -f ./build/Navaduel ./Navaduel
echo "Done!"
echo "Making binary executable..."
sudo chmod +x navaduel
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
#./navaduel
