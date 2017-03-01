# RotMG_Clientless

This is a project i have started just for my own enjoyment. I will be updating this repo as progress is made. 

02/10/2017 - Github repo created and project pushed, almost all inbound packet classes are finished at this point. Some of the core code will most likely be changing, i plan to make each "Client" class handle its own socket connection. That would allow 1 program to run multiple bots. I will keep this log updated with the latest changes and misc plans.

02/14/2017 - All inbound packet classes have been created

02/20/2017 - Modified the program to support multiple clients/accounts at once. The program will load and run as many valid `<Client>` nodes as it finds in the settings.xml file.

02/21/2017- All packet classes have been created. Program also supports a more graceful shutdown now, if you have clients running and you want to exit, press ctrl-c and it will force all the clients threads to stop and exit.

3/1/2017 - created new dailylogin branch for a daily login reward bot

## How to add Build Macros
- Rename `BuildMacros.props-sample` to `BuildMacros.props`
- Open the renamed file in an smart editor.
- Change the path in the `CURL_INCLUDE` element to your `include` folder for `curl`.
- Change the path in the `CURL_LIB` element to your `lib` folder for `curl`.
- Change the path in the `OPENSSL_INCLUDE` element to your `include` folder for `OpenSSL`.
- Change the path in the `OPENSSL_LIB` element to your `lib` folder for `OpenSSL`.
- Save the file and reload the project if you had it opened.


## Daily Login Bot
This branch is just about the daily login rewards. The bot right now will login, go to the quest room, and then attempt to claim both rewards. Because this is only the first day of the rewards im still not too sure of the xml values for the daily rewards, so ill be keeping my eyes on this for the next few days.
