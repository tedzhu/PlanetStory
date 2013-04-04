//#include"stdafx.h"
#include <iostream>
#include "CreateLevel.h"
using namespace Ogre;
using namespace std;

class generateLevelsClass{
public:
	generateLevelsClass(string _outputFile)
	{
		if(_outputFile.c_str()=="")
			outputFile="D:\\Dare2011\\Sln\\media\\level.xml";
		else
			outputFile=_outputFile;
		PI=3.1415927;
		r=150;
		total=0;
		minDistance=10;
		maxtry=10;
	}


	double frand()
	{
		return (rand()%100)*0.01;
	}

	double arcDistance(Vector3 v1,Vector3 v2)
	{	
		return r*acos(v1.dotProduct(v2)/(r*r));
	}

	class Island{
	public:
		Vector3 position;		
		int r;
		char type;
		string name;

		Island()
		{
		}

		Island(char _type):type(_type)
		{
			switch(type){
			case 'a':r=40;break;
			case 'b':r=30;break;
			case 'c':r=20;break;
			}
		}

		void output()
		{
			cout<<"		<island name=\""<<name<<"\" type=\""<<type<<"\">"<<endl;
			cout<<"			<position x=\""<<position.x<<"\" y=\""<<position.y<<"\" z=\""<<position.z<<"\"/>"<<endl;
			cout<<"		</island>"<<endl;
		}
	};

	void generateIsland(int n,char type)
	{
		for(int i=0;i<n;i++){

			for(int j=0;j<maxtry;j++){
				Island island(type);
				std::stringstream st;
				st << "island"<<total;			
				island.name=st.str();

				double theta=frand()*2*PI;
				double phi=frand()*PI;
				island.position.x=r*sin(theta)*cos(phi);
				island.position.y=r*sin(theta)*sin(phi);
				island.position.z=r*cos(theta);

				bool ok=true;
				for(int k=0;k<total;k++){			
					if(arcDistance(islands[k].position,island.position)<island.r+islands[k].r+minDistance){
						ok=false;
						break;
					}
				}

				if(ok){					
					islands[total++]=island;
					break;
				}
			}
		}
	}


	int generateLevels(int clevel)
	{	
		srand((unsigned)time(NULL));			

		freopen(outputFile.c_str(),"w",stdout);
		cout<<"<levels num=\""<<clevel<<"\">"<<endl;

		for(int level=1;level<=clevel;level++){
			total=0;

			generateIsland(2+frand()*5,'a');
			generateIsland(frand()*5,'b');
			generateIsland(frand()*5,'c');		

			cout.precision(2);
			cout.setf(ios::fixed);

			cout<<"	<level value=\""<<level<<"\">"<<endl;

			cout<<"		<islands num=\""<<total<<"\">"<<endl;
			for(int i=0;i<total;i++){
				islands[i].output();
			}
			cout<<"		</islands>"<<endl;

			cout<<"		<gases num=\""<<total<<"\">"<<endl;
			for(int i=0;i<total;i++){
				string type;
				int num;
				switch(i){
				case 0:
					type="player";
					num=50+frand()*10;
					break;
				case 1:
					type="enemy1";
					num=50+frand()*10+level*10*frand();
					break;
				case 2:
					if(level>4){
						type="enemy2";
						num=50+frand()*10+level*10*frand();
						break;
					}
				default:
					type="middle";
					num=10+frand()*20;
				}
				cout<<"		<gas name=\"gas"<<i<<"\" type=\""+type+"\" num=\""<<num<<"\" belong=\""<<islands[i].name<<"\"/>"<<endl;
			}
			cout<<"		</gases>"<<endl;

			cout<<"	</level>"<<endl;
		}

		cout<<"</levels>"<<endl;
		fclose(stdout);
		return 0;
	}

	double PI;
	double r;
	int total;
	int maxtry;
	double minDistance;
	string outputFile;
	Island islands[100];
};

int main()
{
	generateLevelsClass glc("D:\\Dare2011\\Sln\\media\\level.xml");
	glc.generateLevels(10);
	return 0;
}