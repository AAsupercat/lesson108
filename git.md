# git学习指南(版本控制系统):

git创建本地仓库：git init 

## 配置本地仓库:
列举出当前本地仓库所有配置项：git config -l
配置用户名： git config user.name "your name"
查看用户名： git config user.name
配置邮箱：   git config user.email your email
查看邮箱：   git config user.email 

重置配置项： git config --unset 配置项
eg:  git config --unset user.name
    
全局生效：git config --global user.name "your name"
全局重置：git config --global --unset 配置项

## 几个概念

1. .git文件时仓库的版本库，而与其同文件目录的文件时处于工作区的文件

![alt text](image.png)

