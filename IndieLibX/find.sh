cd $HOME;
find ./IndieLibX -name "*.h" -type f -print0 | \
grep -R "ifndef SWIG";
find ./IndieLibX -name "*.hxx" -type f -print0 | \
grep -R "ifndef SWIG";
find ./IndieLibX -name "*.inl" -type f -print0 | \
grep -R "ifndef SWIG";


