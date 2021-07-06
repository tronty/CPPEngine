#include "lsystem.h"
#include <fstream>
using namespace std;
void LSystem::replaceAll(string& str, const string& from, const string& to) 
{
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != string::npos) 
	{
		str.replace(start_pos, from.length(), to);
    	start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}
string LSystem::produce(const string axiom, const AssociativeArray rules)
{
	string t=axiom;
	AssociativeArray::const_iterator iter;
	for (iter=rules.begin(); iter!=rules.end();++iter)
	{
		string key=iter->first;
		vector<string> value=iter->second;
		int index=rand()%value.size();
		// printf("Selected %d out of %d : %s\n",index,value.size(),value[index].c_str());
		replaceAll(t,key,value[index]);
	}
	return t;
}
string LSystem::reproduce(const string axiom,const AssociativeArray rules, const int iterations)
{
	if (iterations>0)
		return reproduce(produce(axiom,rules),rules,iterations-1);
	return axiom;
}
string LSystem::generateFromFile(const char * filename,const int iterationsOverride )
{
	cout <<"Generating L-System data..."<<endl;
	ifstream file(filename);
	if (!file)
	{
		cout <<"Could not open L file "<<filename<<endl;
		return "";
	}
	int numbersRead=0;
	float numbers[3];
	string axiom;
	AssociativeArray rules;
	while (!file.eof())
	{
		string temp;
		char c;
		file>>c;
		if (c=='#') 
		{
			getline(file,temp);
			continue;
		}
		file.putback(c);
		if (numbersRead<3)
		{
			file >>numbers[numbersRead++];
			continue;
		}
		if (c=='@') break;
		getline(file,temp);
		int equalSignPos=temp.find("=");
		if (equalSignPos==string::npos)
		{
			axiom=temp;
		}
		else
		{

			rules[temp.substr(0,equalSignPos)].push_back(temp.substr(equalSignPos+1));
			// cout <<temp.substr(0,equalSignPos)<<"  "<<temp.substr(equalSignPos+1)<<endl;
		}
	}

	int iterations=(int)numbers[0];
	if (iterationsOverride)
		iterations=iterationsOverride;
	defaultCoefficient=numbers[1];
	float thickness=numbers[2];
	turtle.thickness=thickness/100;
	return reproduce(axiom,rules,iterations);

}
void LSystem::run(const char command,const float param)
{
	float co=defaultCoefficient;
	float num=param;
	if (num==1)
		num*=co;
	switch (command)
	{
		case '+':
			turtle.turnLeft(num);
			break;
		case '-':
			turtle.turnRight(num);
			break;
		case '&':
			turtle.pitchDown(num);
			break;
		case '^':
			turtle.pitchUp(num);
			break;
		case '<': //increase diameter
		case '\\':
			turtle.rollLeft(num);
			break;
		case '/':
		case '>':
			turtle.rollRight(num);
			break;
		case '|':
			turtle.turn180(param);
			break;
		case 'F':
		case 'f':
			turtle.draw(param);
		// case 'G':
		case 'g':
			turtle.move(param);
			break;
		case '[':
			turtle.save();
			break;
		case ']':
			turtle.restore();
			break;
		default:
		;
	}



}
void LSystem::draw(const string tree)
{
	char paramBuf[1024];
	int bufIndex=0;
	string data=tree;
	float param=0;
	bool getParam=false,checkParam=false;
	char command;
	for (int i=0;i<data.size();++i)
	{
		char c=data[i];
		if (getParam)
		{
			if (c==')')
			{
				paramBuf[bufIndex]=0;
				bufIndex=0;
				param=atof(paramBuf);
				getParam=false;
				run(command,param);
			}
			else
				paramBuf[bufIndex++]=c;
			continue;
		}
		if (checkParam)
		{
			checkParam=false;
			if (c=='(')
			{
				param=0;	
				getParam=true;
				continue;
			}
			run(command,1);

		}
		command=c;
		checkParam=true;
	}
	if (checkParam)
		run(command,1);

	cout <<data<<endl;
}