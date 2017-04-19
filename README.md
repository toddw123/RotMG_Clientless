# RotMG_Clientless

This project is mostly for learning purposes. The idea is to create an easy to understand and easy to use c/c++ bot for realm of the mad god.

Please check out the wiki to learn how to set up the 2 required libraries for this project. 

## How to add Build Macros
- Rename `BuildMacros.props-sample` to `BuildMacros.props`
- Open the renamed file in an smart editor.
- Change the path in the `CURL_INCLUDE` element to your `include` folder for `curl`.
- Change the path in the `CURL_LIB` element to your `lib` folder for `curl`.
- Change the path in the `OPENSSL_INCLUDE` element to your `include` folder for `OpenSSL`.
- Change the path in the `OPENSSL_LIB` element to your `lib` folder for `OpenSSL`.
- Save the file and reload the project if you had it opened.
