# BlueBox
Yet another tool for encoding/decoding [DTMF][wiki-dtmf] signals.

Also able to encode [Russian ANI][rus-ani-lnk] reply. Decoding (in current implementation)
is possible(at least most correct) only via analyzing by hands
debug energy plot ( *requires gnuplot* ).

# Requirements
Requires [lib-sndfile][libsndfile-lnk]

# Build

Default build type is `Debug`.

## Release

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

## Debug
Same as Release with only exception:

    cmake -DCMAKE_BUILD_TYPE=Debug ..

# License 
Licensed under the terms of GNU GPLv3. Copy of the license may be found
in LICENSE file.

[wiki-dtmf]: http://en.wikipedia.org/wiki/Dtmf
[libsndfile-lnk]: http://www.mega-nerd.com/libsndfile/
[rus-ani-lnk]: http://ru.wikipedia.org/wiki/%D0%90%D0%9E%D0%9D
