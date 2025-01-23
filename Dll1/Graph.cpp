#include "Graph.hpp"

Graph::Graph(nlohmann::json json) {
	for (auto& clash : json) {
		node_map[clash["m"]["properties"]["GUID"]] = new node(clash["m"]["properties"]);
		node_map[clash["n"]["properties"]["GUID"]] = new node(clash["n"]["properties"]);
		edge rel(clash["r"]);
		rel.start_node = clash["m"]["properties"]["GUID"];
		rel.end_node = clash["n"]["properties"]["GUID"];
		edge_vec.push_back(rel);
	}
}

void Graph::visualize() {
	GVC_t* gvc = gvContext();  // Graphviz context 생성
	char graph_name[] = "Neo4j Graph";
	Agraph_t* g = agopen(static_cast<char*>(graph_name), Agundirected, NULL);  // 무향 그래프 생성

	//set size
	agsafeset(g, const_cast<char*>("size"), const_cast<char*>("5,5"), const_cast<char*>(""));

	for (auto& clash : edge_vec) {
		node* start_node = node_map[clash.start_node];
		node* end_node = node_map[clash.end_node];

		Agnode_t* n1 = agnode(g, const_cast<char*>(start_node->ElementType.c_str()), TRUE);
		Agnode_t* n2 = agnode(g, const_cast<char*>(end_node->ElementType.c_str()), TRUE);

		agsafeset(n1, const_cast<char*>("color"), const_cast<char*>("orange"), const_cast<char*>("black"));        // 외곽선 색상
		agsafeset(n1, const_cast<char*>("style"), const_cast<char*>("filled"), const_cast<char*>(""));         // 내부 색상 적용
		agsafeset(n1, const_cast<char*>("fillcolor"), const_cast<char*>("orange"), const_cast<char*>("white")); // 내부 색상

		agsafeset(n2, const_cast<char*>("color"), const_cast<char*>("orange"), const_cast<char*>("black"));        // 외곽선 색상
		agsafeset(n2, const_cast<char*>("style"), const_cast<char*>("filled"), const_cast<char*>(""));         // 내부 색상 적용
		agsafeset(n2, const_cast<char*>("fillcolor"), const_cast<char*>("orange"), const_cast<char*>("white")); // 내부 색상


		agedge(g, n1, n2, NULL, TRUE);
	}

	// 그래프 레이아웃 및 렌더링
	gvLayout(gvc, g, "circo");
	gvRenderFilename(gvc, g, "png", "C:/objectinfo/gpt_visualize.png");

	// 리소스 해제
	gvFreeLayout(gvc, g);
	agclose(g);
	gvFreeContext(gvc);
}