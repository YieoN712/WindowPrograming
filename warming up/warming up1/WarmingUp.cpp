#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

#define MAX 60

void convert(char c);
void removeSpace();
void addSpace();
void saveSpace();
void sortSentence(bool order);
void countAlphabet();

std::string sentence;
std::string original;
bool sorted = false;
std::vector<size_t> spacePositions;

int main()
{
	std::cout << "Enter the sentence (max 60 char): ";
	std::getline(std::cin, sentence);

	if (sentence.length() > MAX) {
		std::cout << "Error: Sentence exceeds 60 char\n";
	}

	original = sentence;
	saveSpace();

	char command;

	while (true) {
		std::cout << "command: ";
		std::cin >> command;
		std::cin.ignore();

		if (command >= 'a' and command <= 'z') {
			convert(command);
		}
		else if (command == '1') {
			removeSpace();
		}
		else if (command == '2') {
			addSpace();
		}
		else if (command == '3') {
			sortSentence(true);
		}
		else if (command == '4') {
			sortSentence(false);
		}
		else if (command == '5') {
			countAlphabet();
		}
		else if (command == '0') {
			std::cout << "Exit program\n";
			break;
		}
		else {
			std::cout << "wrong command\n";
		}
	}
}

void convert(char c) {
	for (char& ch : sentence) {
		if (tolower(ch) == c) {
			ch = isupper(ch) ? tolower(ch) : toupper(ch);
		}
	}

	std::cout << "Result: " << sentence << "\n";
}

void removeSpace() {
	for (size_t i = 0; i < sentence.size(); i++) {
		if (sentence[i] == ' ') {
			if (sentence[i + 1] != ' ') {
				sentence.erase(i, 1);
			}
		}
	}

	std::cout << "Result: " << sentence << "\n";
}

void addSpace() {
	if (spacePositions.empty()) return;

	std::string newSent;
	size_t spaceIndex = 0, oriIndex = 0;

	for (size_t i = 0; i < sentence.size(); i++) {
		newSent += sentence[i];
		if (sentence[i] == ' ' and sentence[i - 1] != ' ') {
			newSent += ' ';
			oriIndex++;
		}
		else if (spaceIndex < spacePositions.size() and oriIndex == spacePositions[spaceIndex]) {
			newSent += ' ';
			spaceIndex++;
			oriIndex++;
		}

		oriIndex++;
	}

	sentence = newSent;

	saveSpace();

	std::cout << "Result: " << sentence.substr(0, MAX) << "\n";
}

void saveSpace() {
	spacePositions.clear();

	for (size_t i = 0; i < sentence.size(); i++) {
		if (sentence[i] == ' ') {
			if (sentence[i - 1] != ' ') {
				spacePositions.push_back(i - 1);
			}
		}
	}
}

void sortSentence(bool order) {
	std::string temp;

	if (!sorted) {
		for (char c : sentence) {
			if (isalpha(c))
				temp += c;
		}

		if (order) {
			std::sort(temp.begin(), temp.end());
		}
		else {
			std::sort(temp.begin(), temp.end(), std::greater<char>());
		}

		std::cout << "Result: " << temp << "\n";

		sorted = true;
	}
	else {
		std::cout << "Result: " << sentence << "\n";
		sorted = false;
	}
}

void countAlphabet() {
	int freq[26] = { 0 };

	for (char c : sentence) {
		if (isalpha(c))
			freq[tolower(c) - 'a']++;
	}

	for (int i = 0; i < 26; i++) {
		std::cout << (char)('a' + i) << ": " << freq[i] << " ";
	}
	std::cout << "\n";
}

// This is Window programming problem.