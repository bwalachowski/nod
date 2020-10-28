#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <regex>

struct compareNumber {
	bool operator()(const std::string& str1, const std::string& str2) const {
		if(str1.length() == str2.length()) 
		{
			
			if (str1.compare(1, std::string::npos, str2, 1, std::string::npos) == 0)
			{
				return str1[0] < str2[0];
			}
			return str1.compare(1, std::string::npos, str2, 1, std::string::npos) < 0;
		}
		return str1.length() < str2.length();
	}
};

struct comma final : std::numpunct<char>
{
	char do_decimal_point() const override
	{
		return ',';
	}
};

struct startedRoadDetails {
    std::string roadId;
    long double roadPoint;
    unsigned long long lineNumber;
    std::string lineText;
};

// contains cars' entries that need to be paired
std::map<std::string, struct startedRoadDetails> startedRoads;
// contains cars' total driven roads lengths, ordinarily A and S
std::map<std::string, std::pair<long double, long double> > totalCarDistances;
// contains roads' total driven lengths
std::map<std::string, long double, compareNumber> totalRoadDistances;


int main()
{
	std::string inputLine;
	unsigned long long lineNumber = 0;

	//configure cout
	std::cout << std::fixed;
	std::cout.precision(1);
	std::cout.imbue(std::locale(std::cout.getloc(), new comma));

	while (getline(std::cin, inputLine, '\n'))
	{
	    ++lineNumber;

		if (inputLine.length() == 0)
		{
			continue;
		}

		
		// car id in group 1, road id in group 3, road point in group 4
		std::regex roadJoint("\\W*((\\d|[a-z]|[A-Z]){3,11})\\W+([AS][1-9]\\d{0,2})\\W+((0|([1-9]\\d*)),\\d0*)\\W*");
		// car id or road id or both are in group 1
		std::regex command("\\W*\\?\\W*(((\\d|[a-z]|[A-Z]){3,11})|([AS][1-9]\\d{0,2})|.{0})\\W*");
		std::cmatch m;

		
		if (std::regex_match(inputLine.c_str(), roadJoint)) 
		{
			std::regex_match(inputLine.c_str(), m, roadJoint);

			std::string carId = m[1];
			std::string roadId = m[3];
			std::string roadPointStr = m[4];
			roadPointStr[roadPointStr.size() - 2] = '.';

			long double roadPoint = stod(roadPointStr);
			bool isA = roadId[0] == 'A';

			// if this car started a road before
			if (startedRoads.find(carId) != startedRoads.end())
			{
				// road is paired
				if (startedRoads.find(carId)->second.roadId == roadId)
				{
					long double startPoint = startedRoads.find(carId)->second.roadPoint;
					long double endPoint = roadPoint;
					long double distanceDriven = startPoint - endPoint;
					if (distanceDriven < 0)
					{
						distanceDriven = -distanceDriven;
					}

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

					startedRoads.erase(carId);
				}
				else // startedRoads.find(carId)->second.first != roadId
				{
					std::cerr << "Error in line " << startedRoads.find(carId)->second.lineNumber << ": "
						<< startedRoads.find(carId)->second.lineText << std::endl;

					struct startedRoadDetails editedStartedRoad;
					editedStartedRoad.roadId = roadId;
					editedStartedRoad.roadPoint = roadPoint;
					editedStartedRoad.lineNumber = lineNumber;
					editedStartedRoad.lineText = inputLine;

					startedRoads.find(carId)->second = editedStartedRoad;
				}

			}
			else // startedRoads.find(carId) == startedRoads.end()
			{
				struct startedRoadDetails newStartedRoad;
				newStartedRoad.roadId = roadId;
				newStartedRoad.roadPoint = roadPoint;
				newStartedRoad.lineNumber = lineNumber;
				newStartedRoad.lineText = inputLine;

				startedRoads.insert(std::make_pair(carId, newStartedRoad));
			}
		}
	

		else if (std::regex_match(inputLine.c_str(), command)) 
		{

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
			//but if road number is not correct, then it should not be found in map
			//but when it tried to find it, it used compare funtion and stoi threw exception, fixed compare function

			if (totalCarDistances.find(parameter) != totalCarDistances.end())
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
		
		else 
		{
			std::cerr << "Error in line " << lineNumber << ": " << inputLine << std::endl;
		}
		
	}

	return 0;
}
