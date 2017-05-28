## Building Ferocious on Linux 

*Tested on kde Ubuntu 16.04 LTS*

#### Overview
- Install Qt development environment
- Install openGL library (dependency of Qt)
- clone this repo
- Build inside Qt Creator

#### Install Qt development environment

Download latest Qt package:

[download page](https://www.qt.io/download-open-source/#section-2)

*tested with Offline installer*

make the installer file executable, eg:
~~~
chmod +x qt-opensource-linux-x64-5.8.0
~~~

run the installer as sudo
~~~
sudo ./qt-opensource-linux-x64-5.8.0.run
~~~

#### Install OpenGL library

If QtCreator gives an error such as "cannot find -lGL" when you try to build project,
it means that the OpenGL library is not installed. This is a fundamental dependency of Qt 5.x

~~~
sudo apt-get install libgl1-mesa-dev
~~~

#### clone this repo
~~~
cd ~/wherever-you-put-your-projects
git clone https://github.com/jniemann66/ferocious.git
~~~

#### build inside QtCreator
- Start QtCreator
- Choose 'Open File or Project' from the File menu
- Open 'ferocious.pro'
- Build the project from the Build menu