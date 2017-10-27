#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <ctime>
#include <stack>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <algorithm>
#include <climits>
#include <bitset>
#include <set>
#include <sys/time.h>
//#include <unordered_map>
#include <bits/stdc++.h>
using namespace std;

#define print_to_err(x) do { std::cerr << #x << ": " << x << std::endl; } while (0)



string int2str(int num)
{
  ostringstream ss;
  ss << num;
  return(ss.str());
}

int str2int(string str)
{
  int value;
  istringstream (str)>>value;
  return value;
}


string input_file="";
string testcase_name="";
string output_file="";
int dimensions;
int trips_in_one_generation;

double crossover_probability=0.8;
double mutation_probability=0.1;

vector< vector 	<double> > distance_bw;

struct city
{
	string id;
	double x_coordinate;
	double y_coordinate;
};


struct trip
{
	vector<int> list;
	long double trip_length;
};

vector<city> city_list;

vector<trip> current_generation;

trip current_shortest_trip;


mt19937_64 rng;
// initialize a uniform distribution between 0 and 1
uniform_real_distribution<double> unif(0, 1);
// initialize the random number generator with time-dependent seed
uint64_t timeSeed = chrono::high_resolution_clock::now().time_since_epoch().count();
seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};


void readfile()
{
	ifstream infile;
	// cout<<input_file<<endl;

	char input_file_array[input_file.length()+1];
	for(int i=0;i<input_file.length();i++)
	{
		input_file_array[i]=input_file.at(i);
	}
	input_file_array[input_file.length()]='\0';

	infile.open(input_file_array);


	//TestCase Name Line
	string name_line;
	getline(infile,name_line);
	// print_to_err(name_line);
	int name_line_length=int(name_line.length());
	int start_name_line;
	for(int i=5;i<name_line_length;i++)
	{
		if(name_line.at(i)!=' ')
		{
			start_name_line=i;
			break;
		}
	}
	testcase_name=name_line.substr(start_name_line,(name_line_length- start_name_line));

	//Dimension Line
	string dimension_line;
	getline(infile,dimension_line);
	// print_to_err(dimension_line);
	int dimension_line_length=int(dimension_line.length());
	int start_dimension_line;
	for(int i=10;i<dimension_line_length;i++)
	{
		if(dimension_line.at(i)!=' ')
		{
			start_dimension_line=i;
			break;
		}
	}
	string dimension_string=dimension_line.substr(start_dimension_line,(dimension_line_length- start_dimension_line));
	// print_to_err(dimension_string);
	dimensions=str2int(dimension_string);

	// dimensions=10;

	//NODE_COORD_SECTION garbage line
	string garbage_line;
	getline(infile,garbage_line);
	// print_to_err(garbage_line);

	//Below lines
	for(int i=0;i<dimensions;i++)
	{
		city new_city;
		infile>>new_city.id>>new_city.x_coordinate>>new_city.y_coordinate;
		city_list.push_back(new_city);
	}
	infile.close();
}

void outfile()
{
	ofstream outfile;

	char output_file_array[output_file.length()+1];
	for(int i=0;i<output_file.length();i++)
	{
		output_file_array[i]=output_file.at(i);
	}
	output_file_array[output_file.length()]='\0';


	outfile.open (output_file_array);


	outfile<<"DIMENSION : "<<dimensions<<endl;
	outfile<<"TOUR_LENGTH : "<<current_shortest_trip.trip_length<<endl;
	outfile<<"TOUR_SECTION"<<endl;

	for(int i=0;i<dimensions;i++)
	{
		outfile<< city_list[ current_shortest_trip.list[i] ].id<<" ";
	}
	outfile<<endl;
	outfile<<"-1"<<endl;
	outfile<<"EOF";


	for(int i=0;i<dimensions;i++)
	{
		cerr<<"distance b/w "<<current_shortest_trip.list[i]<<" and "<<current_shortest_trip.list[(i+1)%dimensions]<< " is "<<distance_bw[current_shortest_trip.list[i]][current_shortest_trip.list[(i+1)%dimensions]]<<endl;
	}

	outfile.close();
}

void make_distance_vector()
{
	distance_bw.resize(dimensions, vector<double>(dimensions,0));
	for(int i=0;i<dimensions;i++)
	{
		for(int j=i;j<dimensions;j++)
		{
			distance_bw[i][j]= pow((pow((city_list[i].x_coordinate - city_list[j].x_coordinate),2)+pow((city_list[i].y_coordinate - city_list[j].y_coordinate),2)),0.5);
			distance_bw[j][i]=distance_bw[i][j]; 
		}
	}
}



vector<int> pmx_crossover(vector<int>& p1, vector<int>& p2)
{
	int start_swath=(dimensions-1);
	int end_swath=(0);

	while(start_swath>=end_swath)
	{
		start_swath=int(floor(unif(rng)*dimensions))%dimensions;
		end_swath=int(floor(unif(rng)*dimensions))%dimensions;
	}

	// start_swath=3;
	// end_swath=7;


	set<int> swath;
	vector<int> child(int(p1.size()),-1);
	for(int i=start_swath;i<=end_swath;i++)
	{
		child[i]=p1[i];
		swath.insert(child[i]);
	}

	for(int i=start_swath;i<=end_swath;i++)
	{
		if(swath.find(p2[i])==swath.end())
		{
			//p2[i] was not found in swath so find its place in the child

			//find number in p1 at same index so that is p1[i]

			//find index of p1[i] in p2 call that Z

			//check if child[Z] is (-1)

			//if it is (-1) then put p2[i] at child[Z]

			//else we now start again at p2[i]=p1[i]


			// ALGO can be seen at http://www.rubicite.com/Tutorials/GeneticAlgorithms/CrossoverOperators/PMXCrossoverOperator.aspx

			int temp_pmx=p2[i];
			int same_in_p1=p1[i];
			int index_in_p2;
			LABEL_CASEY:
			// cerr<<"Below casey"<<endl;
			for(int j=0;j<dimensions;j++)
			{
				if(p2[j]==same_in_p1)
				{
					index_in_p2=j;
					break;
				}
			}

			if(child[index_in_p2]==(-1))
			{
				// cerr<<"found"<<endl;
				child[index_in_p2]=p2[i];
				swath.insert(p2[i]);
			}
			else
			{
				temp_pmx=p2[index_in_p2];
				same_in_p1=p1[index_in_p2];
				goto LABEL_CASEY;
			}
		}
	}

	for(int i=0;i<dimensions;i++)
	{
		if(child[i]==(-1))
		{
			child[i]=p2[i];
		}
	}

	return child;
}


vector<int> greedy_crossover(vector<int>& p1, vector<int>& p2)
{
	set<int> already_taken;
	set<int> not_taken;
	for(int i=0;i<dimensions;i++)
	{
		not_taken.insert(i);
	}
	vector<int> child(dimensions,(-1));
	child[0]=p1[0];
	not_taken.erase(child[0]);
	already_taken.insert(child[0]);

	int pos_in_p1;
	int pos_in_p2;
	bool already_p1;
	bool already_p2;
	bool random_help;
	int random_num;
	for(int i=1;i<dimensions;i++)
	{
		for(int j=0;j<dimensions;j++)
		{
			if(p1[j]==child[i-1])
			{
				pos_in_p1=j;
				break;
			}
		}
		for(int j=0;j<dimensions;j++)
		{
			if(p2[j]==child[i-1])
			{
				pos_in_p2=j;
				break;
			}
		}
		already_p1=(already_taken.find(p1[(pos_in_p1+1)%dimensions])!=already_taken.end());
		already_p2=(already_taken.find(p2[(pos_in_p2+1)%dimensions])!=already_taken.end());
		if(already_p1 && already_p2)
		{
			random_help=false;
			//select a random number from not_taken
			for(int j=0;j<dimensions;j++)
			{
				if(not_taken.find(j)!=not_taken.end())
				{
					random_num=j;
					if(unif(rng)<0.3)
					{
						random_help=true;
						child[i]=j;
						not_taken.erase(j);
						already_taken.insert(j);
						break;
					}
				}
			}

			if(!random_help)
			{
				child[i]=random_num;
				not_taken.erase(random_num);
				already_taken.insert(random_num);
			}

		}
		else if(already_p1 && !already_p2)
		{
			child[i]=p2[(pos_in_p2+1)%dimensions];
			not_taken.erase(child[i]);
			already_taken.insert(child[i]);
		}
		else if(!already_p1 && already_p2)
		{
			child[i]=p1[(pos_in_p1+1)%dimensions];
			not_taken.erase(child[i]);
			already_taken.insert(child[i]);
		}
		else if(!already_p1 && !already_p2)
		{
			if(distance_bw[child[i-1]][p1[(pos_in_p1+1)%dimensions]]<distance_bw[child[i-1]][p2[(pos_in_p2+1)%dimensions]])
			{
				child[i]=p1[(pos_in_p1+1)%dimensions];
				not_taken.erase(child[i]);
				already_taken.insert(child[i]);
			}
			else
			{
				child[i]=p2[(pos_in_p2+1)%dimensions];
				not_taken.erase(child[i]);
				already_taken.insert(child[i]);	
			}
		}		
	}


	return child;
}


void generate_initial_population()
{
	current_generation.clear();
	current_generation.resize(trips_in_one_generation);
	//make permutations
	std::vector<int> initial_trip(dimensions);
	for(int i=0;i<dimensions;i++)
	{
		initial_trip[i]=i;
	}

	for(int i=0;i<trips_in_one_generation;i++)
	{
		random_shuffle ( initial_trip.begin(), initial_trip.end() );
		current_generation[i].list=initial_trip;
	}
}

bool sortbytriplength(const trip &a,const trip &b)
{
    return (a.trip_length < b.trip_length);
}


void evaluate_fitness_of_population()
{
	double temp_distance=0;
	for(int i=0;i<trips_in_one_generation;i++)
	{
		temp_distance=0;
		for(int j=0;j<(dimensions);j++)
		{
			temp_distance+=distance_bw[current_generation[i].list[j]][current_generation[i].list[(j+1)%dimensions]];
		}
		current_generation[i].trip_length=temp_distance;
	}

	sort(current_generation.begin(), current_generation.end(),sortbytriplength);

	if(current_generation[0].trip_length<current_shortest_trip.trip_length)
	{
		current_shortest_trip=current_generation[0];
	}
}

int main(int argc, char const *argv[])
{

	current_shortest_trip.trip_length=LDBL_MAX;
	string temp_str(argv[1]);
	input_file=temp_str;	
	readfile();
	output_file="output_"+testcase_name+".txt";
	make_distance_vector();
	

	// unit test for crossover_pmx
	// vector<int> p1={8,4,7,3,6,2,5,1,9,0};
	// vector<int> p2={0,1,2,3,4,5,6,7,8,9};
	// start_swath=3;
	// end_swath=7;
	// vector<int> v_test_pmx;
	// cerr<<"before pmx"<<endl;
	// v_test_pmx=pmx_crossover(p1,p2);
	// cerr<<"after pmx"<<endl;
	// for(int i=0;i<dimensions;i++)
	// {
	// 	cout<<v_test_pmx[i]<<" ";
	// }
	// cout<<endl;


	// unit test for crossover_greedy
	// vector<int> p1={8,4,7,3,6,2,5,1,9,0,15,11,13,12,14,10};
	// vector<int> p2={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	// vector<int> v_test_greedy;
	// cerr<<"before greedy"<<endl;
	// v_test_greedy=greedy_crossover(p1,p2);
	// cerr<<"after greedy"<<endl;
	// for(int i=0;i<dimensions;i++)
	// {
	// 	cout<<v_test_greedy[i]<<" ";
	// }
	// cout<<endl;


	trips_in_one_generation=500;
	generate_initial_population();
	evaluate_fitness_of_population();


	int total_generations=100;

	int mutation_first_index;
	int mutation_second_index;
	int mutation_temp;

	for(int i=0;i<total_generations;i++)
	{
		// print_to_err(i);
		int half_trips=trips_in_one_generation/2;
		for(int j=0;j<(half_trips);)
		{
			if(unif(rng)<crossover_probability)
			{
				current_generation[half_trips+j].list=pmx_crossover( current_generation[j].list, current_generation[j+1].list );				
			}
			if(unif(rng)<crossover_probability)
			{
				current_generation[half_trips+j+1].list=greedy_crossover( current_generation[j+1].list, current_generation[j].list );
			}
			j+=2;
		}

		for(int j=half_trips;j<(trips_in_one_generation);j++)
		{
			if(unif(rng)<mutation_probability)
			{
				do
				{
					mutation_first_index=int(floor(unif(rng)*dimensions))%dimensions;
					mutation_second_index=int(floor(unif(rng)*dimensions))%dimensions;
				}
				while(mutation_first_index==mutation_second_index);

				mutation_temp=current_generation[j].list[mutation_first_index];
				current_generation[j].list[mutation_first_index]=current_generation[j].list[mutation_second_index];
				current_generation[j].list[mutation_second_index]=mutation_temp;
			}
			// mutate(current_generation[j].list);
		}


		double temp_distance=0;
		for(int j=half_trips;j<trips_in_one_generation;j++)
		{
			temp_distance=0;
			for(int k=0;k<(dimensions);k++)
			{
				temp_distance+=distance_bw[current_generation[j].list[k]][current_generation[j].list[(k+1)%dimensions]];
			}
			current_generation[j].trip_length=temp_distance;
		}

		sort(current_generation.begin(), current_generation.end(),sortbytriplength);

		if(current_generation[0].trip_length<current_shortest_trip.trip_length)
		{
			current_shortest_trip=current_generation[0];
		}
		print_to_err(current_shortest_trip.trip_length);
	}

	outfile();
	return 0;
}
