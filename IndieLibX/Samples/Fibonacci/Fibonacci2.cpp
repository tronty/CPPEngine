// https://hbfs.wordpress.com/2014/12/23/fibonacci-rabbits-as-a-rewrite-system/
/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#include <map>
#include <string>
#include <iostream>
#include <iomanip>
 
typedef std::map<const std::string,const std::string> rules;
 
std::string next(const std::string & prev,
                 const rules & r)
 {
  std::string result;
 
 // for (const std::string p : prev)
for (unsigned int i = 0;i<prev.length();i++)
{
	const std::string p(1, prev[i]);
    // hack to ensure const-ness on rules
    rules::const_iterator x=r.find(p);
    if (x!=r.end())
     result+=x->second;
    else
     result+='X'; // oh noes!
   }
 
  return result;
 }
 
int ApplicationLogic()
{
 #if 1
  // rules for the classic problem 
  rules immortal;
  immortal.insert(std::pair<const std::string,const std::string>("1","2"));
  immortal.insert(std::pair<const std::string,const std::string>("2","21"));
 
  // rules if the rabbits die when they turn 5
  rules mortal1;
  mortal1.insert(std::pair<const std::string,const std::string>("1","2"));
  mortal1.insert(std::pair<const std::string,const std::string>("2","31"));
  mortal1.insert(std::pair<const std::string,const std::string>("3","41"));
  mortal1.insert(std::pair<const std::string,const std::string>("4","1"));
 
  // rules if the rabbits die when they turn 6
  // and lived to be 5 a whole generation
  rules mortal2;
  mortal2.insert(std::pair<const std::string,const std::string>("1","2"));
  mortal2.insert(std::pair<const std::string,const std::string>("2","31"));
  mortal2.insert(std::pair<const std::string,const std::string>("3","41"));
  mortal2.insert(std::pair<const std::string,const std::string>("4","51"));
  mortal2.insert(std::pair<const std::string,const std::string>("5",""));
 
  std::string population="1";
 
  for (int i=0;i<15;i++)
   {
    std::cout
     << std::setw(3) << population.size()
     << " "
     << population << std::endl;
    population=next(population,immortal);
   }
#endif
	return 0;
}

