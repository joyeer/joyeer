cmake build

mkdir -p build/xcode

pushd build/xcode
cmake -G Xcode ../../
popd