/**************************************
 * Автор: Тенц Ангелина               *
 * Вариант: 27                        *
 * Название: Функции                  *
 **************************************/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

struct provider {
  string name;
  string phoneNumber;
  string connectionType;
  double averageSpeed;
  double costPerHour;
};

vector<string> splitFields(string line) {
  vector<string> fields;
  int start;
  int separator;

  start = 0;

  while (true) {
    separator = line.find(';', start);
	if (separator == string::npos) {
	  fields.push_back(line.substr(start));
	  break;
	}
	fields.push_back(line.substr(start, separator-start));
	start = separator + 1;
  }

  return fields;
}

bool readDouble(string text, double& value) {
  istringstream parser(text);
  char extra;
  if (!(parser >> value) || !isfinite(value)) {
    return false;
  }

  return !(parser >> extra);
}

bool readProviders(string inputPath, vector<provider>& list) {
  ifstream input(inputPath);
  string line; 
  vector<string> fields;
  provider currentProvider;
  int expectedFieldsCount;
  int lineNumber;

  expectedFieldsCount = 5;
  lineNumber = 0;
  
  if (!input) {
    cout << "ERROR input\n";
	return false;
  }

  while (getline(input, line)) {
	lineNumber++;
    fields = splitFields(line);
	if (fields.size() != expectedFieldsCount) {
      cout << "ERROR line " << lineNumber << "\n";
	  return false;
	}
	currentProvider.name = fields[0];
	currentProvider.phoneNumber = fields[1];
	currentProvider.connectionType = fields[2];
	if (!readDouble(fields[3], currentProvider.averageSpeed) || currentProvider.averageSpeed <= 0.0 || !readDouble(fields[4], currentProvider.costPerHour) || currentProvider.costPerHour < 0.0) {
	  cout << "Invalid number\n";
	  return false;
	}
	list.push_back(currentProvider);
  }

  return true;
}

double costPerMb(provider currentProvider) {
  double bitsInByte;
  double secondsInHour;
  double mbPerHour;
  double costPerMb;

  bitsInByte = 8.0;
  secondsInHour = 3600.0;

  mbPerHour = (currentProvider.averageSpeed * secondsInHour) / bitsInByte;
  costPerMb = currentProvider.costPerHour / mbPerHour;

  return costPerMb;
}

vector<provider> filterByConnectionType(vector<provider> list, string type) {
  vector<provider> matching;
  for (auto& provider : list) {
    if (provider.connectionType == type) {
	  matching.push_back(provider);
	}
  }

  return matching;
}

int findMaxSpeedIndex(vector<provider> list) {
  int maxIndex;

  if (list.empty()) {
    return -1;
  }

  maxIndex = 0;

  for (int providerIndex = 1; providerIndex < list.size(); ++providerIndex) {
	if (list[providerIndex].averageSpeed > list[maxIndex].averageSpeed) {
	  maxIndex = providerIndex;
	}
  }

  return maxIndex;
}

int findMinCostPerMbIndex(vector<provider> list) {
  int minIndex;
  double minCost;
  double currentCost;

  if (list.empty()) {
    return -1;
  }

  minIndex = 0;
  minCost = costPerMb(list[0]);

  for (int providerIndex = 1; providerIndex < list.size(); ++providerIndex) {
    currentCost = costPerMb(list[providerIndex]);
	if (currentCost < minCost) {
	  minCost = currentCost;
	  minIndex = providerIndex;
	}
  }

  return minIndex;
}

bool writeReport(string& outputPath, vector<provider> list, vector<provider> matching, string type, int maxSpeedIndex, int minCostIndex) {
  ostringstream report;
  report << "Providers (" << type << "):\n";
  if (matching.empty()) {
    report << "EMPTY\n";
  } else {
    for (auto& provider : matching) {
	  report << "Provider: " << provider.name
			 << ";Phone number: " << provider.phoneNumber
			 << ";Speed (Mbps): " << provider.averageSpeed
			 << ";Cost Per Hour: " << provider.costPerHour
			 << "; Cost for 1 MB: " << costPerMb(provider) << "\n";
	}

  }
  report << "\nHighest speed provider: ";
  if (maxSpeedIndex == -1) {
    report << "NONE\n";
  } else { 
    report << list[maxSpeedIndex].name << "(" << list[maxSpeedIndex].averageSpeed << ")\n";
  }

  report << "Lowest 1 Mb cost provider: ";
  if (minCostIndex == -1) {
    report << "NONE\n";
  } else {
    report << list[minCostIndex].name << "(" << costPerMb(list[minCostIndex]) << ")\n";
  }
  ofstream output(outputPath);
  if (!output) {
	cout << "Cannot open output file\n";
	return false;
  }
  output << report.str();
  cout << report.str();
  return true;
}

int main() {
  string inputPath;
  string outputPath;
  string type;
  vector<provider> list;
  vector<provider> matching;
  int maxSpeedIndex;
  int minCostIndex;

  cout << "Enter input file name: ";
  getline(cin, inputPath);

  cout << "Enter output file name: ";
  getline(cin, outputPath);

  cout << "Enter connection type: ";
  getline(cin, type);

  if (!readProviders(inputPath, list)) {
    return 1;
  }

  matching = filterByConnectionType(list, type);
  maxSpeedIndex = findMaxSpeedIndex(list);
  minCostIndex = findMinCostPerMbIndex(list);
  writeReport(outputPath, list, matching, type, maxSpeedIndex, minCostIndex);
  return 0;
}