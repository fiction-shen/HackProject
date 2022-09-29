## HackProject
玩转x3派
##

##
后续的功能包在此分支上增加
##

在ubuntu20.04 + ros2环境下运行
ros2安装步骤：https://zhuanlan.zhihu.com/p/149187701
安装sfml依赖
sudo apt install libsfml-dev


## compile
mkdir build
colcon build
##

## Run
需要提前下载好游戏文件，下载地址：http://nesfc.ysepan.com/，放于/Gmae 文件夹
以《雪人兄弟为例》
source install/setup.bash
cd build
ros2 run playgame PLay ../Game/snowbrother.nes
##
