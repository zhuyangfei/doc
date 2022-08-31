# Download wsl image file
$dl_url = "https://github.com/sakai135/wsl-vpnkit/releases/download/v0.2.4/wsl-vpnkit.tar.gz"
$file = "C:\WSL\$(Split-Path -Path $dl_url -Leaf)"
#Start-BitsTransfer -Source $dl_url -Destination $file

# Import WSL
wsl --unregister wsl-vpnkit
wsl --import wsl-vpnkit ${env:USERPROFILE}\wsl-vpnkit $file --version 2

# Check distro for additional instructions
wsl -d wsl-vpnkit

# Start vpnkit service
wsl -d wsl-vpnkit service wsl-vpnkit start

# check service log
wsl -d wsl-vpnkit tail -f /var/log/wsl-vpnkit.log