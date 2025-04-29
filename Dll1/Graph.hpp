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
			: edge(edge), start_logicX(start_logicX), end_logicX(end_logicX), end_logicY(end_logicY) {
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

	void DrawNode(Agnode_t* name, int x, int y, int rx, int ry);
	void DrawLine(Agedge_t* edge, int x1, int y1, int x2, int y2);
};

#endif //GRAPH_H