#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#define MAX 20

struct Student {
	std::string name;
	int studentID;
	int grade;
	int birth;
	int attendance[12];

	void printInfo() {
		std::cout << "이름: " << name << " | ";
		std::cout << "ID: " << studentID << ", 학년: " << grade << ", ";
		std::cout << "생일: " << birth / 100 << "/" << birth % 100;
		std::cout << "출석 현황: ";
		for (int i = 0; i < 12; i++) {
			std::cout << (i + 1) << "월 " << attendance[i] << "회, ";
		}
		std::cout << "\n";
	}
};

void addStudent();
void printAll();
bool isLeapyear(int year);
bool isValidAttendance(int month, int count, int year = 2024);
void searchStudent(std::string name);
void deleteStudent(std::string name);
void modifyStudent(std::string name);
void sortStudents(int type);

std::vector<Student> students;

int main()
{
	std::string command, name;

	while (true) {
		std::cin >> command;

		if (command == "a") {
			addStudent();
		}
		else if (command == "p") {
			printAll();
		}
		else if (command == "s") {
			std::cin >> name;
			searchStudent(name);
		}
		else if (command == "d") {
			std::cin >> name;
			deleteStudent(name);
		}
		else if (command == "p") {
			std::cin >> name;
			modifyStudent(name);
		}
		else if (command == "1" or command == "2" or command == "3" or command == "4") {
			sortStudents(stoi(command));
		}
		else if (command == "q") {
			std::cout << "exit program\n";
			break;
		}
		else {
			std::cout << "wrong command\n";
		}
	}
}

void printAll() {
	std::cout << "===== 학생 목록 =====\n";
	for (auto& student : students) {
		student.printInfo();
	}
}

void addStudent() {

}

bool isLeapyear(int year) {
	return (year % 4 == 0 and year % 100 != 0) or (year % 400 == 0);
}

bool isValidAttendance(int month, int count, int year = 2024) {
	int maxDay[] = { 31,(isLeapyear(year) ? 29 : 28),31,30,31,30,31,31,30,31,30,31 };
	return (month >= 1 and month <= 12) and (count >= 1 and count <= maxDay[month - 1]);
}

void searchStudent(std::string name) {

}

void deleteStudent(std::string name) {

}

void modifyStudent(std::string name) {

}

void sortStudents(int type) {

}