# Installing a custom version of CMake

## Motivation

GROMACS 2023 requires 3.18, Ubuntu 20.04 has 3.16

## Tutorials

[Installing from source](https://zhuanlan.zhihu.com/p/519732843)

[Installing the binary](https://askubuntu.com/questions/829310/how-to-upgrade-cmake-in-ubuntu)

## Steps

1. Download the latest version of CMake (3.26.4 at the time being)

    [Files](https://cmake.org/files/)

    [Download page](https://cmake.org/download/)

    ```sh
    wget https://cmake.org/files/v3.26/cmake-3.26.4-linux-x86_64.tar.gz
    ```

2. Decompress

    ```sh
    tar -zxvf cmake-3.26.4.tar.gz
    cd cmake-3.26.4
    ```

3. Move the directory to user

    ```sh
    mv cmake-3.26.4-linux-x86_64 $HOME/app/cmake-3.26
    ```
