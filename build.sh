mkdir -p build
cd build

if ! vcpkg list | grep -iq crow; then 
    echo "crow not found, installing crow"
    vcpkg install crow
else
    echo "found crow"
fi

cmake .. -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" #cmake needs to look for vcpkg paths
cmake --build .
