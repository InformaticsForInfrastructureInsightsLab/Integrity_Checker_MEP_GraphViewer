//
// Created by √÷ªÛ¡ÿ on 2025. 1. 16..
//

#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <map>
#include <vector>
#include "json.hpp"

struct node {
	std::string ElementType;
	std::string Category;
	std::string Priority;
	std::string BBox;
	std::string DisciplinesType;
	std::string GUID;

	node(nlohmann::json json) {
		ElementType = json["ElementType"];
		Category = json["Category"];
		Priority = json["Priority"];
		BBox = json["BBox"];
		DisciplinesType = json["DisciplinesType"];
		GUID = json["GUID"];
	}
};

struct edge {
	std::string Movable_Space;
	std::string Clash_Distance;
	std::string TopologyXaXis;
	std::string TopologyYaXis;
	std::string TopologyZaXis;
	std::string ClashType;
	std::string Abs_Volume_Diff;
	std::string Abs_Volume_Sum;
	std::string Severity;
	std::string Clearance;
	std::string Movable_Distance;
	std::string offset;
	std::string Penetration;
	std::string HardClash;
	std::string MovabilityResult;

	std::string start_node;
	std::string end_node;
	std::string id;

	edge(nlohmann::json json) {
		Movable_Space = json["properties"]["Movable_Space"];
		Clash_Distance = json["properties"]["Clash_Distance"];
		TopologyXaXis = json["properties"]["TopologyXaXis"];
		TopologyYaXis = json["properties"]["TopologyYaXis"];
		TopologyZaXis = json["properties"]["TopologyZaXis"];
		ClashType = json["properties"]["ClashType"];
		Abs_Volume_Diff = json["properties"]["Abs_Volume_Diff"];
		Abs_Volume_Sum = json["properties"]["Abs_Volume_Sum"];
		Severity = json["properties"]["Severity"];
		Clearance = json["properties"]["Clearance"];
		Movable_Distance = json["properties"]["Movable_Distance"];
		offset = json["properties"]["offset"];
		Penetration = json["properties"]["Penetration"];
		HardClash = json["properties"]["HardClash"];
		MovabilityResult = json["properties"]["MovabilityResult"];

		start_node = json["start"]["properties"]["GUID"];
		end_node = json["end"]["properties"]["GUID"];
		id = json["id"];
	}
};

class Graph {
	std::map<std::string, node*> node_map;
	std::vector<edge> edge_vec;

public:
	Graph(nlohmann::json json) {
		for (auto& clash : json) {
			node_map[clash["m"]["properties"]["GUID"]] = new node(clash["m"]["properties"]);
			node_map[clash["n"]["properties"]["GUID"]] = new node(clash["n"]["properties"]);
			edge rel(clash["r"]);
			edge_vec.push_back(rel);
		}
	}
};

#endif //GRAPH_H
