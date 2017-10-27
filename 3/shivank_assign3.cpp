#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
using namespace std;

//Global Variables
int num_city;
struct city* cityarr;
double** dist;
struct path* population;

//Define Constants
#define pop_size 500
#define print_to_err(x) do { std::cerr << #x << ": " << x << std::endl; } while (0)

struct city{
	int id;
	string name;
	double xc;
	double yc;
};

struct path{
	vector<city> cityseq; 
	double cost;
};


int getinput(){
	
	ifstream f1;
	string temp_line;
	string test_case_name="";
	string temp_substr="";
	f1.open("input.txt" , ios::in);
	
	if(f1.is_open()){
		
		//Name
		getline(f1,temp_line);
		
		int len = temp_line.length();
		
		for(int i=5;i<len;i++){
			if(temp_line[i] != ' ')test_case_name+=temp_line[i];
		}
		
		//Dimension
		getline(f1,temp_line);
		
		len = temp_line.length();
		
		for(int i=10;i<len;i++){
			if(temp_line[i] != ' ')temp_substr+=temp_line[i];
		}
		
		if(temp_substr != ""){
			stringstream ss;
			ss << temp_substr;
			ss >> num_city;
			temp_substr = "";
			cityarr = new city[num_city];
		}
		
		//3rd line
		getline(f1,temp_line);
		
		//Main Section 
		for(int i=0;i<num_city;i++){
			
			f1>>cityarr[i].name>>cityarr[i].xc>>cityarr[i].yc;
			cityarr[i].id = i;
			/*
			getline(f1,temp_line);
			
			len = temp_line.length();
			
			//extract city name
			int j=0;
			for(j=0;j<len;j++){
				if(temp_line[j] != ' ')temp_substr+=temp_line[j];
				else break;
			}
			cityarr[i].name=temp_substr;
			temp_substr="";
			
			//extract city x coordinate
			while(temp_line[j] == ' ')j++;
			for(;j<len;j++){
				if(temp_line[j] != ' ')temp_substr+=temp_line[j];
				else break;
			}
			if(temp_substr != ""){
				stringstream ss;
				ss << temp_substr;
				ss >> cityarr[i].xc;
				temp_substr = "";
			}
			
			//extract city y coordinate
			while(temp_line[j] == ' ')j++;
			for(;j<len;j++){
				if(temp_line[j] != ' ')temp_substr+=temp_line[j];
				else break;
			}
			if(temp_substr != ""){
				stringstream ss;
				ss << temp_substr;
				ss >> cityarr[i].yc;
				temp_substr = "";
			}*/
			
		}
		
		f1.close();
		return 1;
	}else{     
		cout<<"Couldn't open input file.";
		return 0;
    }
    
}

void calculate_distance(){
	
	//Initialise distance array
	dist = new double*[num_city];
	for(int i=0;i<num_city;i++){
		dist[i] = new double[num_city];
	}
	
	for(int i=0;i<num_city;i++){
		for(int j=i;j<num_city;j++){
			int diffx = cityarr[i].xc - cityarr[j].xc;
			int diffy = cityarr[i].yc - cityarr[j].yc;
			dist[i][j] =  pow( (diffx*diffx) + (diffy*diffy) , 0.5);
			dist[j][i] = dist[i][j];
		}
	}
	
}

double findcost(vector<city> cityv){
	
	double sum = 0.0;
	for(int i=0;i<num_city-1;i++){
		print_to_err(cityv[i].id);
		
		sum+= distance[(cityv[i].id)][(cityv[i+1].id)];
	}
	sum+= distance[cityv[1].id][cityv[num_city-1].id];
	return sum;
	
}

void generate_initial_population(){
	
	population = new path[pop_size];
	vector<city> cityv(cityarr, cityarr + sizeof(cityarr)/sizeof(cityarr[0]));
	for(int i=0;i<pop_size;i++){
		random_shuffle(cityv.begin(),cityv.end());
		population[i].cityseq = cityv;
		population[i].cost = findcost(cityv);
	}
	
}

int main(){
	
	if(getinput()){
	
		/*cout<<num_city<<endl;
		for(int i=0;i<num_city;i++){
			cout<<cityarr[i].name<<" "<<cityarr[i].xc<<" "<<cityarr[i].yc;
			cout<<endl;
		}*/
		
		calculate_distance();
		generate_initial_population();
		
		
		
	}
	
	return 0;
}
