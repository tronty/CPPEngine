OS="Ubuntu"

usage() { echo "Usage: $0 [<>] [<NDK|iOS|LNX|OSX|NMAKE|VC|MW|PRO|CMake>] -> Builds an solution." 1>&2; echo "Usage: $0 -> Removes Temporary Files." 1>&2;exit 1; }

echo "$OS"
if [ $OS == "Darwin" ]; then
echo "Darwin"
else
echo "Ubuntu"
fi

slns=( Test SampleApps TerrainApps Tanks IndieLibApps AllLibs )
targets=( NDK iOS LNX OSX NMAKE VC MW PRO CMake )

if [[ ${slns[*]} =~ "$1" ]]
 then
	echo ""
 else
	usage
fi

if [[ ${targets[*]} =~ "$2" ]]
 then
	echo ""
 else
	usage
fi

