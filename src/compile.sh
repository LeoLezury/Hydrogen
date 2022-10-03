#!/bin/bash
cd $(dirname "$0")
echo -e '\e[38;2;0;192;128m清理文件\e[0m'
echo -e "\e[38;2;0;0;255m# rm -r ./build/*\e[0m"
rm -r ./build/*
echo -e '\e[38;2;0;192;128m开始编译\e[0m'

for file in ./*.c
do
    file=$(basename "$file" .c)
    echo -e "\e[38;2;0;0;255m# gcc -c ./$file.c <xxx>\e[0m"
    gcc -c ./$file.c -o ./build/$file.o -O3 -fPIC
done

echo -e '\e[38;2;0;192;128m开始链接\e[0m'

cd ./build
echo -e "\e[38;2;0;0;255m# gcc <xxx> -o ./audio\e[0m"
gcc -o ../audio\
  ./audio-main.o ./pcm-gen.o ./audio.o ./basic.o\
  -lasound -lm
echo -e "\e[38;2;0;0;255m# gcc <xxx> -o ./main\e[0m"
gcc -o ../main\
  ./main.o ./display.o ./pcm-gen.o ./audio.o ./basic.o\
  -lGL -lGLX -lX11 -lXfixes -lXext -lasound

# gcc ./main.c ./basic.c -o ./main -lGL -lGLX -lX11 -lXfixes -lXext
echo -e '\e[38;2;0;192;128m构建完成\e[30m'