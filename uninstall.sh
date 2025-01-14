echo "You must run this script from the root folder of the game."
if [ $(basename $(pwd)) == 'navaduel' ]; then
rm $HOME/Desktop/Navaduel.desktop
sudo rm $HOME/.local/share/applications/Navaduel.desktop
rm -r $(pwd)
echo finished
else
echo "Uninstall not ran from game's root directory"
fi