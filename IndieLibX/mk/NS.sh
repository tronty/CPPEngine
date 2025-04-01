
rm -fr ../Samples/NatureScene2/src/Application\ Nature/Data
rm ../Samples/NatureScene2/src/Application\ Nature/SceneGraph.exe
rm ../Samples/NatureScene2/src/Application\ Nature/SceneGraph.suo
rm ../Samples/NatureScene2/src/Application\ Nature/SceneGraph.sln
rm ../Samples/NatureScene2/src/Application\ Nature/SceneGraph.vcproj

rm ../Samples/NatureScene2/src/Nodes/TransformGroup.cpp
rm ../Samples/NatureScene2/src/Nodes/TransformGroup.h

mv ../Samples/NatureScene2/src/Application\ Nature/Terrain ../Samples/NatureScene2/src
mv ../Samples/NatureScene2/src/Application\ Nature/SceneFrame.cpp ../Samples/NatureScene2/src
mv ../Samples/NatureScene2/src/Application\ Nature/SceneFrame.h ../Samples/NatureScene2/src
mv ../Samples/NatureScene2/src/Application\ Nature/Main.cpp ../Samples/NatureScene2/src

rm -fr ../Samples/NatureScene2/src/Application\ Nature
