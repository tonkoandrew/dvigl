# dvigl - my first game engine in C++

to build this project you need to install some dev libraries


```
sudo apt-get install libsdl2-dev libsdl2-mixer-dev  libsdl2-ttf-dev  libsdl2-net-dev  libsdl2-image-dev libassimp-dev  libbullet-dev libglm-dev libopus-dev libtheora-dev libogg-dev libwebp-dev libirrlicht-dev libyaml-cpp-dev libfreetype6-dev qtbase5-dev libvorbis-dev libvpx-dev libjpeg-dev libflac-dev libfluidsynth-dev libmodplug-dev libmpg123-dev libopusfile-dev g++ cmake

```

And then use cmake to build

```
git clone git@github.com:tonkoandrew/dvigl.git
git submodule update --init --recursive
git submodule foreach -q --recursive 'branch="$(git config -f <path>.gitmodules submodule.$name.branch)"; git checkout $branch'



mkdir -p build; cd build

# in windows I use CMAKE_GENERATOR="MinGW Makefiles" cmake .. -Wno-dev
cmake .. -Wno-dev

make -j 8

```

Now try to run

```
./bin/game

```

## Demo

![Demo](https://raw.githubusercontent.com/tonkoandrew/dvigl/master/res/demo.gif?cache=false)

## some useful debug commands

```
valgrind  --tool=massif  --time-unit=ms  ./bin/game
massif-visualizer massif.out.${pid}

cd ..
for f in $(find includes/dvigl -type f); do clang-format -i $f; done
for f in $(find src/dvigl -type f); do clang-format -i $f; done
```
