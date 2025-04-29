//
// Created by 최상준 on 2025. 1. 16..
//

#ifndef GRAPH_H
#define GRAPH_H

#include "framework.h"

#include <string>
#include <map>
#include <vector>
#include <unordered_set>
#include <memory>

#define GVDLL
#include <graphviz/gvc.h>

namespace detail {
	struct InfoBase {
		enum class Type { Node, Edge } type;
		virtual ~InfoBase() = default;
	};

	struct NodeInfo : InfoBase {
		Agnode_t* node;
		int logicX;
		int logicY;
		int logicRad;
		NodeInfo() { type = Type::Node; }
		NodeInfo(Agnode_t* node, int logicX, int logicY, int logicRad)
			: node(node), logicX(logicX), logicY(logicY), logicRad(logicRad) {
			type = Type::Node;
		}
	};

	struct EdgeInfo : InfoBase {
		Agedge_t* edge;
		int start_logicX;
		int start_logicY;
		int end_logicX;
		int end_logicY;
		EdgeInfo() { type = Type::Edge; }
		EdgeInfo(Agedge_t* edge, int start_logicX, int start_logicY, int end_logicX, int end_logicY)
			: edge(edge), start_logicX(start_logicX), start_logicY(start_logicY), end_logicX(end_logicX), end_logicY(end_logicY) {
			type = Type::Edge;
		}
	};
}


struct node {
	std::string ElementType;
	std::string Category;
	std::string Priority;
	std::string BBox;
	std::string DisciplinesType;
	std::string GUID;

	node(nlohmann::json json) {
		ElementType = !json.contains("ElementType") ? "null" : json["ElementType"];
		Category = !json.contains("Category") ? "null" : json["Category"];
		Priority = !json.contains("Priority") ? "null" : json["Priority"];
		BBox = !json.contains("BBox") ? "null" : json["BBox"];
		DisciplinesType = !json.contains("DisciplinesType") ? "null" : json["DisciplinesType"];
		GUID = !json.contains("GUID") ? "null" : json["GUID"];
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
		Movable_Space = !property.contains("Movable_Space") ? "null" : property["Movable_Space"];
		Clash_Distance = !property.contains("Clash_Distance") ? "null" : property["Clash_Distance"];
		TopologyXaXis = !property.contains("TopologyXaXis") ? "null" : property["TopologyXaXis"];
		TopologyYaXis = !property.contains("TopologyYaXis") ? "null" : property["TopologyYaXis"];
		TopologyZaXis = !property.contains("TopologyZaXis") ? "null" : property["TopologyZaXis"];
		ClashType = !property.contains("ClashType") ? "null" : property["ClashType"];
		Abs_Volume_Diff = !property.contains("Abs_Volume_Diff") ? "null" : property["Abs_Volume_Diff"];
		Abs_Volume_Sum = !property.contains("Abs_Volume_Sum") ? "null" : property["Abs_Volume_Sum"];
		Severity = !property.contains("Severity") ? "null" : property["Severity"];
		Clearance = !property.contains("Clearance") ? "null" : property["Clearance"];
		Movable_Distance = !property.contains("Movable_Distance") ? "null" : property["Movable_Distance"];
		offset = !property.contains("offset") ? "null" : property["offset"];
		Penetration = !property.contains("Penetration") ? "null" : property["Penetration"];
		HardClash = !property.contains("HardClash") ? "null" : property["HardClash"];
		MovabilityResult = !property.contains("MovabilityResult") ? "null" : property["MovabilityResult"];
		id = json["id"];
	}
};

struct GraphDeleter {
	void operator()(Agraph_t* graph) const {
		if (graph) {
			agclose(graph);
		}
	}
};

// Graphviz 컨텍스트 해제용 커스텀 Deleter
struct GVCDeleter {
	void operator()(GVC_t* gvc) const {
		if (gvc) {
			gvFreeContext(gvc);
		}
	}
};

class Graph {
	std::map<std::string, std::unique_ptr<node>> node_map;
	std::vector<std::unique_ptr<edge>> edge_vec;

	std::unique_ptr<GVC_t, GVCDeleter> gvc;
	std::unique_ptr<Agraph_t, GraphDeleter> g;

public:
	std::unordered_set<detail::EdgeInfo*> visitedEdges;
	std::unordered_set<detail::NodeInfo*> visitedNodes;

public:
	Graph(nlohmann::json json);

	void buildGraph();
	void exportGraphImage();
	void RenderGraph(HDC hdc, double scaleFactor, double offsetX, double offsetY);

	void Release() {
		gvFreeLayout(gvc.get(), g.get());
		for (auto edge : visitedEdges) {
			delete edge;
		}
		for (auto node : visitedNodes) {
			delete node;
		}
	}

private:
	Agnode_t* FindNode(std::string attr) {
		for (Agnode_t* node = agfstnode(g.get()); node; node = agnxtnode(g.get(), node)) {
			if (std::string(agget(node, const_cast<char*>("guid"))) == attr) {
				return node;
			}
		}
		return nullptr;
	}
};

#endif //GRAPH_H