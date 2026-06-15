
if [ ! -f  "build" ]; then
    mkdir "build"
fi

cd "build"

cmake "../"

make 