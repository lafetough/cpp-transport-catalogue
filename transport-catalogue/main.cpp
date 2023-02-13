#include <iostream>
#include "transport_catalogue.h"
#include "input_reader.h"
#include <vector>
#include <cassert>
#include <unordered_set>
#include "stat_reader.h"
#include <fstream>
#include "log_duration.h"
#include <algorithm>
#include <mutex>
#include <execution>

using namespace std;
using namespace input_processing;
using namespace thread_reading;
using namespace stop_parsing;
using namespace bus_parsing;

std::string ReadLineT(std::ifstream& in) {
	std::string s;
	std::getline(in, s);
	return s;
}

int ReadLineWithNumberT(std::ifstream& in) {
	int result = 0;
	in >> result;
	ReadLineT(in);
	return result;
}

namespace tests {

	void CorrectStopParsing() {
		{
			std::string s = "Stop Tolstopaltsevo: 55.611087, 37.208290"s;
			Stop correct_stop = {
				"Tolstopaltsevo",
				Coordinates {55.611087, 37.208290 } };
			assert(correct_stop == StopParsing(s));
		}
		{
			std::string s = "Stop A: 55.611087, 37.208290"s;
			Stop correct_stop = {
				"A",
				Coordinates {55.611087, 37.208290 } };
			assert(correct_stop == StopParsing(s));
		}
		{
			std::string s = "    Stop Tolstopaltsevo: 55.611087, 37.208290     "s;
			Stop correct_stop = {
				"Tolstopaltsevo",
				Coordinates {55.611087, 37.208290 } };
			assert(correct_stop == StopParsing(s));
		}
		{
			std::string s = "Stop Biryulyovo Zapadnoye: 55.611087, 37.208290"s;
			Stop correct_stop = {
				"Biryulyovo Zapadnoye",
				Coordinates {55.611087, 37.208290 } };
			assert(correct_stop == StopParsing(s));
		}
		{
			std::string s = "Stop One Two Three Four Five: 55.611087, 37.208290"s;
			Stop correct_stop = {
				"One Two Three Four Five",
				Coordinates {55.611087, 37.208290 } };
			assert(correct_stop == StopParsing(s));
		}
		cout << "CorrectStopParsing.........DONE" << endl;
	}

	void CorrectInstructionDetection() {

		{
			string s = "Stop Tolstopaltsevo: 55.611087, 37.208290";
			assert(Instruction::STOP == Identifaer(s));
		}
		{
			string s = "    Stop Tolstopaltsevo: 55.611087, 37.208290";
			assert(Instruction::STOP == Identifaer(s));
		}
		{
			string s = "Bus one: Biryulyovo Zapadnoye > Biryusinka > Biryulyovo Zapadnoye";
			assert(Instruction::BUS_CIRCLE == Identifaer(s));
		}
		{
			string s = "     Bus one: Biryulyovo Zapadnoye > Biryusinka > Biryulyovo Zapadnoye";
			assert(Instruction::BUS_CIRCLE == Identifaer(s));
		}
		{
			string s = "Bus one: Biryulyovo Zapadnoye - Biryusinka - Biryulyovo Zapadnoye";
			assert(Instruction::BUS_RETURNING == Identifaer(s));
		}
		{
			string s = "     Bus one: Biryulyovo Zapadnoye - Biryusinka - Biryulyovo Zapadnoye";
			assert(Instruction::BUS_RETURNING == Identifaer(s));
		}

		try {
			string s = "top Tolstopaltsevo: 55.611087, 37.208290";
			Identifaer(s);
		}
		catch (std::invalid_argument& e) {
			cout << "Excepton catched! " << e.what() << endl;
		}
		try {
			string s = "B us one: Biryulyovo Zapadnoye - Biryusinka - Biryulyovo Zapadnoye";
			Identifaer(s);
		}
		catch (std::invalid_argument& e) {
			cout << "Excepton catched! " << e.what() << endl;
		}

		cout << "CorrectInstructionDetection.........DONE" << endl;

	}

	void CorrectStopAdding() {

		{
			TransportCatalogue t;
			Stop correct_stop = {
				"Tolstopaltsevo",
				Coordinates {55.611087, 37.208290 } };
			t.AddStop(correct_stop);
			assert(t._GetStops().size() == 1);
			assert(t._GetStops()[0] == correct_stop);
			assert(t._GetMap().size() == 1);
			auto map = t._GetMap();
			assert(map.find("Tolstopaltsevo") != map.end());
			assert(*t._GetMap()["Tolstopaltsevo"] == correct_stop);
		}
		{
			TransportCatalogue t;
			Stop correct_stop1 = {
				"Tolstopaltsevo",
				Coordinates {55.611087, 37.208290 } };

			t.AddStop(correct_stop1);

			Stop correct_stop2 = {
				"A",
				Coordinates {55.611087, 37.208290 } };

			t.AddStop(correct_stop2);

			Stop correct_stop3 = {
				"Biryulyovo Zapadnoye",
				Coordinates {55.611087, 37.208290 } };

			t.AddStop(correct_stop3);

			assert(t._GetStops().size() == 3);
			assert(t._GetMap().size() == 3);
			auto map1 = t._GetMap();
			assert(map1.find("Tolstopaltsevo") != map1.end());
			assert(*t._GetMap()["Tolstopaltsevo"] == correct_stop1);
			assert(map1.find("Biryulyovo Zapadnoye") != map1.end());
			assert(*t._GetMap()["Biryulyovo Zapadnoye"] == correct_stop3);
		}

		cout << "CorrectStopAdding.........DONE" << endl;
	}

	bool operator==(
		const std::pair<std::pair<Stop*, Stop*>, int> lhs,
		const std::pair <std::pair<Stop*, Stop*>, int> rhs) {

		return 
			lhs.first.first == rhs.first.first && 
			lhs.first.second == rhs.first.second && 
			lhs.second == rhs.second;

	}

	void CorrectAddDistance() {

		{
			TransportCatalogue t;

			Stop correct_stop1 = {
				"Tolstopaltsevo",
				Coordinates {55.611087, 37.208290 }
			};

			Stop correct_stop2 = {
				"Marushkino",
				Coordinates {55.595884, 37.209755}
			};

			

			t.AddStop(correct_stop1);
			t.AddStop(correct_stop2);

			std::string_view s = "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino"sv;
			std::pair<std::pair<Stop*, Stop*>, int> correct_pair = { {&correct_stop1, &correct_stop2}, 3900 };

			std::pair<std::pair<Stop*, Stop*>, int> tested;
			t.AddDistance(NameParcing(s), DistancesParsing(s));

			int i;

			try {
				i = t._GetStopsToDist().at({ &(t.GetStop(correct_stop1.stop_name)), &(t.GetStop(correct_stop2.stop_name)) });
			}
			catch(...) {
				throw std::invalid_argument("There no this pair");
			}
			assert(i == correct_pair.second);
		}	
		cout << "CorrectAddDistance.........DONE" << endl;
	}

	void StartTests() {
		CorrectStopParsing();
		CorrectInstructionDetection();
		CorrectStopAdding();
		CorrectAddDistance();
	}
}


int main() {


	
	tests::StartTests();

	int num = ReadLineWithNumber();
	vector<string> requests;
	requests.reserve(num);
	vector<string_view> bus_requests;


	for (int i = 0; i < num; ++i) {
		requests.push_back(ReadLine());
	}

	TransportCatalogue catalouge;

	//добавление остановок 
	
	for (std::string_view sv : requests) {
		if (Identifaer(sv) == Instruction::STOP) {
			catalouge.AddStop(StopParsing(sv));
		}
	}

	// добавление расстояний
	for (std::string_view sv : requests) {
		if (Identifaer(sv) == Instruction::STOP) {
			catalouge.AddDistance(NameParcing(sv), DistancesParsing(sv));
		}
	}
	// добавление маршрутов
	for (std::string_view sv : requests) {
		Instruction identifaer = Identifaer(sv);
		if (identifaer != Instruction::STOP) {
			catalouge.AddBus(BusParsing(sv, identifaer, catalouge));
		}
	}

	num = ReadLineWithNumber();

	for (int i = 0; i < num; ++i) {
		using namespace output_processing;
		string request = ReadLine();
		output_processing::Instruction identifaer = output_processing::Identifaer(request);
		if (identifaer == output_processing::Instruction::STOP) {
			std::string_view sv(request);
			stop::Information(std::cout, sv, catalouge);
		}
		else {
			std::string_view sv(request);
			bus::Information(std::cout, catalouge.GetBus(NameParsing(sv)), catalouge);
		}
	}
}
