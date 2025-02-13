#include "Graph.hpp"
#include "utils.hpp"
#include "WindowClass.hpp"
extern PanelWindow panel;

Graph::Graph(nlohmann::json& json) {
	for (auto& clash : json) {
		node_map[clash["m"]["properties"]["GUID"]] = std::make_unique<node>(clash["m"]["properties"]);
		node_map[clash["n"]["properties"]["GUID"]] = std::make_unique<node>(clash["n"]["properties"]);
		edge rel(clash["r"]);
		rel.start_node = clash["m"]["properties"]["GUID"];
		rel.end_node = clash["n"]["properties"]["GUID"];
		edge_vec.push_back(std::make_unique<edge>(rel));
	}
}

Graph::~Graph() {
	gvFreeLayout(gvc.get(), g.get());
}

void Graph::buildGraph() {
	gvc = std::unique_ptr<GVC_t, GVCDeleter>(gvContext());  // Graphviz context 생성
	g = std::unique_ptr<Agraph_t, GraphDeleter>(agopen(const_cast<char*>("Neo4jGraph"), Agundirected, nullptr));

	//set size
	agsafeset(g.get(), const_cast<char*>("size"), const_cast<char*>("5,5"), const_cast<char*>(""));

	for (auto& clash : edge_vec) {
		node* start_node = node_map[clash->start_node].get();
		node* end_node = node_map[clash->end_node].get();

		Agnode_t* n1 = agnode(g.get(), const_cast<char*>(start_node->ElementType.c_str()), TRUE);
		agsafeset(n1, const_cast<char*>("guid"), const_cast<char*>(start_node->GUID.c_str()), const_cast<char*>("default_value"));
		Agnode_t* n2 = agnode(g.get(), const_cast<char*>(end_node->ElementType.c_str()), TRUE);
		agsafeset(n2, const_cast<char*>("guid"), const_cast<char*>(end_node->GUID.c_str()), const_cast<char*>("default_value"));

		agsafeset(n1, const_cast<char*>("color"), const_cast<char*>("orange"), const_cast<char*>("black"));        // 외곽선 색상
		agsafeset(n1, const_cast<char*>("style"), const_cast<char*>("filled"), const_cast<char*>(""));         // 내부 색상 적용
		agsafeset(n1, const_cast<char*>("fillcolor"), const_cast<char*>("orange"), const_cast<char*>("white")); // 내부 색상

		agsafeset(n2, const_cast<char*>("color"), const_cast<char*>("orange"), const_cast<char*>("black"));        // 외곽선 색상
		agsafeset(n2, const_cast<char*>("style"), const_cast<char*>("filled"), const_cast<char*>(""));         // 내부 색상 적용
		agsafeset(n2, const_cast<char*>("fillcolor"), const_cast<char*>("orange"), const_cast<char*>("white")); // 내부 색상

		Agedge_t* e = agedge(g.get(), n1, n2, NULL, TRUE);
		agsafeset(e, const_cast<char*>("Movable_Space"), const_cast<char*>(clash->Movable_Space.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("Clash_Distance"), const_cast<char*>(clash->Clash_Distance.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("TopologyXaXis"), const_cast<char*>(clash->TopologyXaXis.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("TopologyYaXis"), const_cast<char*>(clash->TopologyYaXis.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("TopologyZaXis"), const_cast<char*>(clash->TopologyZaXis.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("ClashType"), const_cast<char*>(clash->ClashType.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("Abs_Volume_Diff"), const_cast<char*>(clash->Abs_Volume_Diff.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("Abs_Volume_Sum"), const_cast<char*>(clash->Abs_Volume_Sum.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("Severity"), const_cast<char*>(clash->Severity.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("Clearance"), const_cast<char*>(clash->Clearance.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("Movable_Distance"), const_cast<char*>(clash->Movable_Distance.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("offset"), const_cast<char*>(clash->offset.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("Penetration"), const_cast<char*>(clash->Penetration.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("HardClash"), const_cast<char*>(clash->HardClash.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("MovabilityResult"), const_cast<char*>(clash->MovabilityResult.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("start_node"), const_cast<char*>(clash->start_node.c_str()), const_cast<char*>("default"));
		agsafeset(e, const_cast<char*>("end_node"), const_cast<char*>(clash->end_node.c_str()), const_cast<char*>("default"));
	}
	gvLayout(gvc.get(), g.get(), "circo");
}

void Graph::exportGraphImage() {
	gvRenderFilename(gvc.get(), g.get(), "png", "C:/objectinfo/gpt_visualize.png");
}

void Graph::RenderGraph(HDC hdc, double scaleFactor, double offsetX, double offsetY) {
	boxf bbox = GD_bb(g.get());
	float x_min = bbox.LL.x;
	float y_min = bbox.LL.y;
	float x_max = bbox.UR.x;
	float y_max = bbox.UR.y;

	float width = x_max - x_min;
	float height = y_max - y_min;

	Agnode_t* sample_node = agfstnode(g.get());

	for (Agedge_t* edge = agfstedge(g.get(), sample_node); edge; edge = agnxtedge(g.get(), edge, sample_node)) {
		Agnode_t* start = FindNode(agget(edge, const_cast<char*>("start_node")));
		Agnode_t* end = FindNode(agget(edge, const_cast<char*>("end_node")));

		if (!start) {
			MessageBox(panel.m_hwnd, L"start is null", L" ", MB_OK);
		}
		if (!end) {
			MessageBox(panel.m_hwnd, L"end is null", L" ", MB_OK);
		}

		pointf coord_s = ND_coord(start);
		pointf coord_e = ND_coord(end);
		double rad_s = max(ND_width(start), ND_height(start));
		double rad_e = max(ND_width(end), ND_height(end));
		
		// 중점의 x,y좌표
		int x_s = static_cast<int>(((coord_s.x - x_min) / width) * 950 * scaleFactor + offsetX);
		int y_s = static_cast<int>((1.0 - (coord_s.y - y_min) / height) * 340 * scaleFactor + offsetY);
		int x_e = static_cast<int>(((coord_e.x - x_min) / width) * 950 * scaleFactor + offsetX);
		int y_e = static_cast<int>((1.0 - (coord_e.y - y_min) / height) * 340 * scaleFactor + offsetY);
		// 인치 단위를 픽셀단위로 변환하기 위해 72를 곱함	
		int r_xs = static_cast<int>(ND_width(start) * 72 * scaleFactor * (950 / width) / 2);
		int r_ys = static_cast<int>(ND_height(start) * 72 * scaleFactor * (340 / height) / 2);
		int r_xe = static_cast<int>(ND_width(end) * 72 * scaleFactor * (950 / width) / 2);
		int r_ye = static_cast<int>(ND_height(end) * 72 * scaleFactor * (340 / height) / 2);
		
		std::string node_name_start = agnameof(start);
		std::string node_name_end = agnameof(end);

		DrawNode(start ,x_s, y_s, r_xs, r_ys);
		DrawNode(end, x_e, y_e, r_xe, r_ye);
	}
}

void Graph::DrawNode(Agnode_t* node, int x, int y, int rx, int ry) {
	size_t rv;
	char* guid = agnameof(node);
	errno_t err = mbstowcs_s(&rv, nullptr, 0, guid, _TRUNCATE);

	// 2. 변환할 wchar_t 배열 할당
	wchar_t* wGuid = new wchar_t[rv];
	// 3. 변환 수행
	err = mbstowcs_s(&rv, wGuid, rv, guid, _TRUNCATE);

	MessageBox(panel.m_hwnd, wGuid, L" ", MB_OK);

	// 노드 그리기
	HWND hwnd = CreateWindowEx(0, L"NODECLASS", wGuid,
		WS_CHILD | WS_VISIBLE | SS_WHITEFRAME | SS_NOTIFY,
		x - rx, y - ry, 2 * rx, 2 * ry,
		panel.m_hwnd, (HMENU)0, GetModuleHandle(NULL), nullptr
	);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(node));
}