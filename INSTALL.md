## MacOS

In order to use the supreme-brocolli application template you need to install the following tools:
* XCode
* XCode command line tools (git, clang)

## Build MacOS 

In order to build for the MacOS platform layer run: 
```
>./build_macos.sh
```
This builds the main executable **AND** the dynamic load library.

## Instant live coding - MacOS

As long as you only ever change code in the application part of the system you can just recompile only the application and the system will reload the new compilation result while the system is still running. For this do not use the build_>>platform<<.sh script but the **build_>>platform<<_application.sh** script.

>./build_macos_application.sh

## executing shell scripts

In order to run the mentioned scripts make sure they have the neccessary access rights. The easiest is to execute the following for each of the scripts:
```
> chmod 777 >>script name<<
```