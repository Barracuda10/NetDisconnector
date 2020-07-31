# NetDisconnector
This is a tool can disconnect internet use hotkey<br>
<br>
[<ins>[Version 4] Download here...</ins>](https://github.com/Barracuda10/NetDisconnector/releases/download/1.3/NetDisconnector.exe)<br>
[<ins>Download newer but not yet release version here...</ins>](https://raw.githubusercontent.com/Barracuda10/NetDisconnector/master/NetDisconnector/x64/Release/NetDisconnector.exe)<br>
<br>
<img src="https://raw.githubusercontent.com/Barracuda10/others/master/NetDisconnector/netdisconnector_main.png?token=AHWAOFFFNR4TIK4XGH3ESJK6EESXA"><br>
<h3><ins>Usage:</ins></h3>
&nbsp;&nbsp;&nbsp;&nbsp;Run NetDisconnector.exe<br>
&nbsp;&nbsp;&nbsp;&nbsp;Check or select target network adapter name<br>
&nbsp;&nbsp;&nbsp;&nbsp;It'll disconnect immediately when pressing down the hotkey<br>
<br>
<h3><ins>Things this tool can do:</ins></h3>
<h4>Spin the Lucky Wheel</h4>
&nbsp;&nbsp;-&nbsp;&nbsp;Start GTA5 with VPN or proxy enabled<br>
&nbsp;&nbsp;-&nbsp;&nbsp;Wait game completing Initializing Social Club<br>
&nbsp;&nbsp;-&nbsp;&nbsp;Join a Invite Only Session and go to Lucky Wheel<br>
&nbsp;&nbsp;-&nbsp;&nbsp;Wait for 4s / 4.65s to press S after the game show Use S to Spin prompt<br>
&nbsp;&nbsp;-&nbsp;&nbsp;Press hotkey (default PAUSE key) to disconnect internet if it's not car<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* Notice this method doesn't work very well now, consider wait 4.65 seconds instead<br>
<!--&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* Wait 4.65 seconds also work<br>-->
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* Another way to get more chance is to use the left stick of the controller<br>
<h4>Make solo Public Session</h4>
&nbsp;&nbsp;-&nbsp;&nbsp;Press hotkey (default PAUSE key) to disconnect<br>
&nbsp;&nbsp;-&nbsp;&nbsp;Offline around 12s (count the reconnecting time)<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* This 12s count the reconnecting time, it means this time must subtract around 3s reconnecting time<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;* Game will force sign out after offline 15s<br>
<h4>Make solo Public Session with UDP Block</h4>
&nbsp;&nbsp;-&nbsp;&nbsp;Change to Firewall Block UDP method<br>
&nbsp;&nbsp;-&nbsp;&nbsp;Press hotkey (default PAUSE key) to disconnect<br>
&nbsp;&nbsp;-&nbsp;&nbsp;Offline around 10s (game will not sign out)<br>
<h4>Other Things</h4>
&nbsp;&nbsp;-&nbsp;&nbsp;Disconnect the internet to prevent game saving unwanted progress<br>
&nbsp;&nbsp;-&nbsp;&nbsp;This program work when using a VPN or proxy<br>
&nbsp;&nbsp;-&nbsp;&nbsp;The Block UDP and Block TCP methods require system firewall enabled, Auto reconnect will not apply to these methods<br>
<br>
<h3><ins>Note:</ins></h3>
&nbsp;&nbsp;&nbsp;&nbsp;The default hotkey is set to PAUSE key<br>
&nbsp;&nbsp;&nbsp;&nbsp;It saves last time settings in Registry<details><summary>See registry path</summary>
&nbsp;&nbsp;&nbsp;&nbsp;HKEY_CURRENT_USER\Software\NetDisconnector\NetDisconnector\Settings<br>
</details>
&nbsp;&nbsp;&nbsp;&nbsp;No extra setting file needed<br>
<br>
<h2>Features</h2>
<h4>Change Hotkey</h4>
&nbsp;&nbsp;&nbsp;&nbsp;Check and change current hotkey here<br>
&nbsp;&nbsp;&nbsp;&nbsp;Notice there are some keys may not work<br>
<br>
<img src="https://raw.githubusercontent.com/Barracuda10/others/master/NetDisconnector/netdisconnector_feature_1.png" width="410" height="310"><br>
<h4>Auto Reconnect</h4>
&nbsp;&nbsp;&nbsp;&nbsp;Enable this option to let it automatically reconnect after disconnect 15s (default)<br>
&nbsp;&nbsp;&nbsp;&nbsp;Change the waiting period in the delay box<br>
&nbsp;&nbsp;&nbsp;&nbsp;And you can still manually reconnect by pressing the hotkey again<br>
<br>
<img src="https://raw.githubusercontent.com/Barracuda10/others/master/NetDisconnector/netdisconnector_feature_2.png" width="410" height="310"><br>
<h4>Disconnect by Firewall</h4>
&nbsp;&nbsp;&nbsp;&nbsp;Switch to firewall disconnect mode to let it disconnect by firewall<br>
&nbsp;&nbsp;&nbsp;&nbsp;Firewall Block UDP mode can make solo Public Session, <br>
&nbsp;&nbsp;&nbsp;&nbsp;It can disconnect all players in the session, and block anyone from joining this session<br>
&nbsp;&nbsp;&nbsp;&nbsp;Firewall Block TCP mode can disconnect all connections to Game Server<br>
&nbsp;&nbsp;&nbsp;&nbsp;Firewall Block ALL mode can disconnect both of them<br>
&nbsp;&nbsp;&nbsp;&nbsp;Game will not force sign out by using these methods<br>
&nbsp;&nbsp;&nbsp;&nbsp;Notice these methods require system firewall enabled, and Auto reconnect will not apply to these methods<br>
<details>
      <summary>See how to enable system firewall</summary>
      <br>
      <img src="https://raw.githubusercontent.com/Barracuda10/others/master/NetDisconnector/netdisconnector_feature_help_2.png" width=267 height=216><br>
      &nbsp;&nbsp;-&nbsp;&nbsp;Choose Open Firewall Settings in Help<br>
      <img src="https://raw.githubusercontent.com/Barracuda10/others/master/NetDisconnector/netdisconnector_fw.png" width=267 height=216><br>
      &nbsp;&nbsp;-&nbsp;&nbsp;Choose Turn Windows Defender Firewall on or off<br>
      <img src="https://raw.githubusercontent.com/Barracuda10/others/master/NetDisconnector/netdisconnector_fw_on.png" width=267 height=216><br>
      &nbsp;&nbsp;-&nbsp;&nbsp;Turn both network settings to on (If know which network profile is using, only need to turn currently using one on)<br>
</details>
<br>
<img src="https://raw.githubusercontent.com/Barracuda10/others/master/NetDisconnector/netdisconnector_feature_3-5.png" width="410" height="310"><br>
<h4>Extra features</h4>
<h5>&nbsp;&nbsp;&nbsp;&nbsp;Click these buttons to operate the network through the menu</h5>
<img src="https://raw.githubusercontent.com/Barracuda10/others/master/NetDisconnector/netdisconnector_manul.png" width="410" height="310"><br>
<h5>&nbsp;&nbsp;&nbsp;&nbsp;Choose different disconnect approach</h5>
&nbsp;&nbsp;&nbsp;&nbsp;There are two different approaches to disconnect<br>
&nbsp;&nbsp;&nbsp;&nbsp;The default method is set to release IP address, it's better to use, and faster to disconnect<br>
&nbsp;&nbsp;&nbsp;&nbsp;The disable adapter method shown below is a backup method if the default one not work<br>
<br>
<img src="https://raw.githubusercontent.com/Barracuda10/others/master/NetDisconnector/netdisconnector_method.png" width="410" height="310"><br>
