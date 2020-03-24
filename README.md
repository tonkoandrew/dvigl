# dvigl - my first game engine in C++

to build this project you need to install some dev libraries


```
sudo apt-get install libsdl2-dev libsdl2-mixer-dev  libsdl2-ttf-dev  libsdl2-net-dev  libsdl2-image-dev libassimp-dev  libbullet-dev libglm-dev libopus-dev libtheora-dev libogg-dev libwebp-dev libirrlicht-dev g++ cmake

```

And then use cmake to build

```

mkdir -p build; cd build

cmake ..

make -j 8

```

Now try to run

```
./bin/game

```

## Demo

![Demo](https://github.com/tonkoandrew/dvigl/raw/master/res/demo.gif)