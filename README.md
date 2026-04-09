# P1 monitor
Shows charts for last hour and last day
For 1 and 3 phase systems.
Build for S2 minimodule.

Interface: data (pin 5 RJ12 )pullup 1k from 3V3 to uart RX pin
DR (pin 2 RJ12) 3V3

RGBLED shows status of connection and flashes upon receive frames from P1 port

Connects with station from wifisettings ssd and pwd.
Starts MDNS name from usersettings.moduleName.

if fail: starts WPS 
if fail: ( timeout) starts Softapp mode
if fail: ( timeout) start over.



