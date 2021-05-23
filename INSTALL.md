## MacOS

In order to use the supreme-brocolli application template you need to install the following tools:
* XCode
* XCode command line tools (git, clang)
* fswatch (brew install fswatch)

## Build MacOS 

In order to build the MacOS platform layer run: ```./build_macos.sh```
This build the main executable **AND** the dynamic load library.

## Instant live coding - MacOS

To start the application in instant live coding mode first start the **supreme-broccoli** app and then the script: ```run_macos.hs``` This enables changing application code in the **code** folder while the application is running. Each change triggers a rebuild of the dynamic load library which is reloaded once a second by the platform.

```
>./supreme-boroccoli
>./run_macos.sh
```