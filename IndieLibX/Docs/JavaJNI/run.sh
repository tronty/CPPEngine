#export JAVA_INC=/usr/lib/jvm/java-7-oracle/include
export JAVA_INC=/usr/lib/jvm/java-8-openjdk-amd64/include

# step 1: compile the .class file with invocation to a native method
javac HelloJNI.java
# step 2: auto-generate a .h header file from said Java source
javah HelloJNI

# step 3: make the shared library with the name linked in said Java source, and implementing said native method
g++ -std=c++11 -shared -fPIC -I$JAVA_INC -I$JAVA_INC/linux HelloJNIImpl.cpp -o libhello.so

# step 4: run JVM with java.library.path set to include said shared library
java -Djava.library.path=. HelloJNI
