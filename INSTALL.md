## MacOS

In order to use the supreme-brocolli application template you need to install the following tools:
* XCode
* XCode command line tools (git, clang)
* fswatch (brew install fswatch)

## Build MacOS 

In order to build the MacOS platform layer run: 
```
>./build_macos.sh
```
This builds the main executable **AND** the dynamic load library.

## Instant live coding - MacOS

To start the application in instant live coding mode the script: ```run_macos.hs``` This enables changing application code in the **code** folder while the application is running. Each change triggers a rebuild of the dynamic load library which is reloaded once a second by the platform.

```
>./run_macos.sh
```

## executing shell scripts

In order to run the mentioned scripts make sure they have the neccessary access rights. The easiest is to execute the following for each of the scripts:

```
> chmod 777 >>script name<<
```

This should also be done for the script **build _macos_application.sh** which is triggered during instant live coding to rebuild the dynamic load library.