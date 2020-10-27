#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <regex>

struct compareNumber {
	bool operator()(const std::string& str1, const std::string& str2) const {
		int a = stoi(str1.substr(1));
		int b = stoi(str2.substr(1));
		if (a == b)
		{
			return str1[0] < str2[0];
		}
		return a < b;
	}
};

struct comma final : std::numpunct<char>
{
	char do_decimal_point() const override 
	{ 
		return ','; 
	}
};

// contains cars' entries that need to be paired
std::map<std::string, std::pair<std::string, double> > startedRoads;
// contains cars' total driven roads lengths, ordinarily A and S
std::map<std::string, std::pair<double, double> > totalCarDistances;
// contains roads' total driven lengths
std::map<std::string, double, compareNumber> totalRoadDistances;


int main()
{
	std::string inputLine;
	// becomes true if line starts with ? and false otherwise
	bool isCommand;

	//configure cout
	std::cout << std::fixed;
	std::cout.precision(1);
	std::cout.imbue(std::locale(std::cout.getloc(), new comma));

	while (getline(std::cin, inputLine, '\n'))
	{
		if (inputLine.length() == 0)
		{
			continue;
		}

		isCommand = inputLine[0] == '?';

		if (isCommand == false)
		{
			// car id in group 1, road id in group 3, road point in group 4
			std::regex roadJoint("\\W*((\\d|[a-z]|[A-Z]){3,11})\\W+([AS][1-9]\\d{0,2})\\W+((0|([1-9]\\d*)),\\d0*)\\W*");
			std::cmatch m;

			// TODO: check if it is enough
			if (!std::regex_match(inputLine.c_str(), roadJoint))
			{
				std::cerr << "wrong line, error" << std::endl;
				continue;
			}

			std::regex_match(inputLine.c_str(), m, roadJoint);

			std::string carId = m[1];
			std::string roadId = m[3];
			//TODO: check if it works correctly on students idk it doesn't on my windows csuse it excpects number to be separated by "." not by ","
			std::string roadPointStr = m[4];
			roadPointStr[roadPointStr.size() - 2] = '.';

			double roadPoint = stod(roadPointStr);
			bool isA = roadId[0] == 'A';

			// if this car started a road before
			if (startedRoads.find(carId) != startedRoads.end())
			{
				// road is paired
				if (startedRoads.find(carId)->second.first == roadId)
				{
					double startPoint = startedRoads.find(carId)->second.second;
					double endPoint = roadPoint;
					double distanceDriven = abs(startPoint - endPoint);
					totalRoadDistances[roadId] += distanceDriven;

					if (totalCarDistances.find(carId) == totalCarDistances.end())
					{
						if (isA)
						{
							totalCarDistances.insert(std::make_pair(carId, std::make_pair(distanceDriven, 0)));
						}

						else
						{
							totalCarDistances.insert(std::make_pair(carId, std::make_pair(0, distanceDriven)));
						}
					}
					else // totalCarDistances.find(carId) != totalCarDistances.end()
					{
						if (isA)
						{
							totalCarDistances.find(carId)->second.first += distanceDriven;
						}
						else
						{
							totalCarDistances.find(carId)->second.second += distanceDriven;
						}
					}
				}
				else // startedRoads.find(carId)->second.first != roadId
				{
					std::cerr << "road not paired, error" << std::endl;
				}

				// I remove current road, even if the road was not paired, possible fault
				startedRoads.erase(carId);

			}
			else // startedRoads.find(carId) == startedRoads.end()
			{
				startedRoads.insert(std::make_pair(carId, std::make_pair(roadId, roadPoint)));
			}
		}
		else // isCommand == true
		{
			// car id or road id or both are in group 1
			// TODO: line '?' is not processed although it is correct IMO, it is because \W+ not \W*
			std::regex command("\\W*\\?\\W+(((\\d|[a-z]|[A-Z]){3,11})|([AS][1-9]\\d{0,2})|.{0})\\W*");
			std::cmatch m;

			if (!std::regex_match(inputLine.c_str(), command))
			{
				std::cerr << "wrong line, error" << std::endl;
				continue;
			}

			std::regex_match(inputLine.c_str(), m, command);
			std::string parameter = m[1];

			if (parameter.empty())
			{
				for (auto &it : totalCarDistances) {
					std::cout << it.first;
					if (it.second.first > 0)
					{
						std::cout << " A " << it.second.first;
					}
					if (it.second.second > 0)
					{
						std::cout << " S " << it.second.second;
					}
					std::cout << std::endl;
				}

				for (auto &it : totalRoadDistances) {
					std::cout << it.first << " ";
					std::cout << it.second << std::endl;
				}
				continue;
			}
			//need to check if it's valid road number because custom compare doesn't work on invalid ones
			std::regex road("[AS][1-9]\\d{0,2}");


			if (std::regex_match(parameter, road) && 
				totalCarDistances.find(parameter) != totalCarDistances.end())
			{
				std::cout << parameter;
				if (totalCarDistances.find(parameter)->second.first > 0)
				{
					std::cout << " A " << totalCarDistances.find(parameter)->second.first;
				}
				if (totalCarDistances.find(parameter)->second.second > 0)
				{
					std::cout << " S " << totalCarDistances.find(parameter)->second.second;
				}
				std::cout << std::endl;
			}
			if (totalRoadDistances.find(parameter) != totalRoadDistances.end())
			{
				std::cout << parameter << " ";
				std::cout << totalRoadDistances.find(parameter)->second << std::endl;
			}
		}
	}

	return 0;
}