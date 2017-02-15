# RotMG_Clientless

This is a project i have started just for my own enjoyment. I will be updating this repo as progress is made. 

02/10/2017 - Github repo created and project pushed, almost all inbound packet classes are finished at this point.
             Some of the core code will most likely be changing, i plan to make each "Client" class handle its own
             socket connection. That would allow 1 program to run multiple bots. I will keep this log updated with
             the latest changes and misc plans.

02/14/2017 - All inbound packet classes have been created

## How to add Build Macros
- Rename `BuildMacros.props-sample` to `BuildMacros.props`
- Open the renamed file in an smart editor.
- Change the path in the `CURL_INCLUDE` element to your `include` folder for `curl`.
- Change the path in the `CURL_LIB` element to your `lib` folder for `curl`.
- Change the path in the `OPENSSL_INCLUDE` element to your `include` folder for `OpenSSL`.
- Change the path in the `OPENSSL_LIB` element to your `lib` folder for `OpenSSL`.
- Save the file and reload the project if you had it opened.