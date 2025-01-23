//
// Created by √÷ªÛ¡ÿ on 2025. 1. 16..
//

#ifndef GRAPH_H
#define GRAPH_H

#include "framework.h"

#include <string>
#include <map>
#include <vector>

#define GVDLL
#include <graphviz/gvc.h>

struct node {
	std::string ElementType;
	std::string Category;
	std::string Priority;
	std::string BBox;
	std::string DisciplinesType;
	std::string GUID;

	node(nlohmann::json json) {
		ElementType = json.find("ElementType") == json.end() ? "null" : json["ElementType"];
		Category = json.find("Category") == json.end() ? "null" : json["Category"];
		Priority = json.find("Priority") == json.end() ? "null" : json["Priority"];
		BBox = json.find("BBox") == json.end() ? "null" : json["BBox"];
		DisciplinesType = json.find("DisciplinesType") == json.end() ? "null" : json["DisciplinesType"];
		GUID = json.find("GUID") == json.end() ? "null" : json["GUID"];
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
		auto& property = json["properties"];
		Movable_Space = property.find("Movable_Space") == property.end() ? "null" : property["Movable_Space"];
		Clash_Distance = property.find("Clash_Distance") == property.end() ? "null" : property["Clash_Distance"];
		TopologyXaXis = property.find("TopologyXaXis") == property.end() ? "null" : property["TopologyXaXis"];
		TopologyYaXis = property.find("TopologyYaXis") == property.end() ? "null" : property["TopologyYaXis"];
		TopologyZaXis = property.find("TopologyZaXis") == property.end() ? "null" : property["TopologyZaXis"];
		ClashType = property.find("ClashType") == property.end() ? "null" : property["ClashType"];
		Abs_Volume_Diff = property.find("Abs_Volume_Diff") == property.end() ? "null" : property["Abs_Volume_Diff"];
		Abs_Volume_Sum = property.find("Abs_Volume_Sum") == property.end() ? "null" : property["Abs_Volume_Sum"];
		Severity = property.find("Severity") == property.end() ? "null" : property["Severity"];
		Clearance = property.find("Clearance") == property.end() ? "null" : property["Clearance"];
		Movable_Distance = property.find("Movable_Distance") == property.end() ? "null" : property["Movable_Distance"];
		offset = property.find("offset") == property.end() ? "null" : property["offset"];
		Penetration = property.find("Penetration") == property.end() ? "null" : property["Penetration"];
		HardClash = property.find("HardClash") == property.end() ? "null" : property["HardClash"];
		MovabilityResult = property.find("MovabilityResult") == property.end() ? "null" : property["MovabilityResult"];
		id = json["id"];
	}
};

class Graph {
	std::map<std::string, node*> node_map;
	std::vector<edge> edge_vec;

public:
	Graph(nlohmann::json json);

	void visualize();
};

#endif //GRAPH_H