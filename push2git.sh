echo "# CPPEngine5" >> README.md &&
git init &&
git config --global user.email "tommi.ronty@iki.fi" &&
git config --global user.name "Tommi Rönty" &&
git add README.md &&
git commit -m "first commit" &&
git remote add origin https://github.com/CPPEngine/CPPEngine5.git &&
git push -u origin master &&
git add IndieLibX &&
git commit -m "1st commit operation" &&
git add IndieLib_resources &&
git commit -m "1st commit operation" &&
git push -u origin master

