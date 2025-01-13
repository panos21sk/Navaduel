make config=debug_x86
echo 'Moving navaduel binary'
mv -f ./bin/Debug/navaduel ./navaduel
echo "Make binary executable"
sudo chmod +x navaduel
echo "Creating Desktop Shortcut. Note: If you move the Navaduel folder, run ./build.sh again to update your Navaduel.desktop file."
touch $HOME/Desktop/Navaduel.desktop
echo "[Desktop Entry]
Version=1.0
Name=Navaduel
Terminal=false
Exec=/home/panos/Documents/pGames/navaduel/navaduel
Icon=/home/panos/Documents/pGames/navaduel/resources/N.png
Type=Application
Categories=Game;" | tee $HOME/Desktop/Navaduel.desktop
chmod +x $HOME/Desktop/Navaduel.desktop
echo "User install..."
sudo cp $HOME/Desktop/Navaduel.desktop $HOME/.local/share/applications/Navaduel.desktop
echo "Finished!"
#./navaduel
