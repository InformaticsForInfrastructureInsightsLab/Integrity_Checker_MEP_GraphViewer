//
// Created by 최상준 on 2025. 1. 16..
//

#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <map>
#include <vector>
#include "json.hpp"

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
	Graph(nlohmann::json json) {
		for (auto& clash : json) {
			node_map[clash["m"]["properties"]["GUID"]] = new node(clash["m"]["properties"]);
			node_map[clash["n"]["properties"]["GUID"]] = new node(clash["n"]["properties"]);
			edge rel(clash["r"]);
			rel.start_node = clash["m"]["properties"]["GUID"];
			rel.end_node = clash["n"]["properties"]["GUID"];
			edge_vec.push_back(rel);
		}
	}

	void visualize() {
		GVC_t* gvc = gvContext();  // Graphviz context 생성
		char graph_name[] = "Neo4j Graph";
		Agraph_t* g = agopen(static_cast<char *>(graph_name), Agundirected, NULL);  // 무향 그래프 생성

		//set size
		agsafeset(g, const_cast<char*>("size"), const_cast<char*>("5,5"), const_cast<char*>(""));

		for (auto& clash : edge_vec) {
			node* start_node = node_map[clash.start_node];
			node* end_node = node_map[clash.end_node];

			Agnode_t* n1 = agnode(g, static_cast<char*>(start_node->ElementType.data()), TRUE);
			Agnode_t* n2 = agnode(g, static_cast<char*>(end_node->ElementType.data()), TRUE);

			agsafeset(n1, const_cast<char*>("color"), const_cast<char*>("orange"), const_cast<char*>("black"));        // 외곽선 색상
			agsafeset(n1, const_cast<char*>("style"), const_cast<char*>("filled"), const_cast<char*>(""));         // 내부 색상 적용
			agsafeset(n1, const_cast<char*>("fillcolor"), const_cast<char*>("orange"), const_cast<char*>("white")); // 내부 색상

			agsafeset(n2, const_cast<char*>("color"), const_cast<char*>("orange"), const_cast<char*>("black"));        // 외곽선 색상
			agsafeset(n2, const_cast<char*>("style"), const_cast<char*>("filled"), const_cast<char*>(""));         // 내부 색상 적용
			agsafeset(n2, const_cast<char*>("fillcolor"), const_cast<char*>("orange"), const_cast<char*>("white")); // 내부 색상


			agedge(g, n1, n2, NULL, TRUE);
		}

		// 그래프 레이아웃 및 렌더링
		gvLayout(gvc, g, "neato");
		gvRenderFilename(gvc, g, "png", "C:/objectinfo/gpt_visualize.png");

		// 리소스 해제
		gvFreeLayout(gvc, g);
		agclose(g);
		gvFreeContext(gvc);
	}
};

#endif //GRAPH_H
