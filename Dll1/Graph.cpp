#include <unordered_set>

#include "Graph.hpp"
#include "utils.hpp"
#include "WindowClass.hpp"
extern PanelWindow panel;

namespace detail {
	struct EdgeHash {
		size_t operator()(const std::pair<Agnode_t*, Agnode_t*>& edge) const {
			return std::hash<void*>()(edge.first) ^ std::hash<void*>()(edge.second);
		}
	};
}

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
	EnumChildWindows(panel.m_hwnd, +[](HWND hwnd, LPARAM lparam) -> int {
		DestroyWindow(hwnd);
		return TRUE;
		}, 0);

	boxf bbox = GD_bb(g.get());
	float x_min = bbox.LL.x;
	float y_min = bbox.LL.y;
	float x_max = bbox.UR.x;
	float y_max = bbox.UR.y;

	float width = x_max - x_min;
	float height = y_max - y_min;

	std::unordered_set<std::pair<Agnode_t*, Agnode_t*>, detail::EdgeHash> visitedEdges;
	std::unordered_set<Agnode_t*> visitedNodes;

	for (Agnode_t* node = agfstnode(g.get()); node; node = agnxtnode(g.get(), node)) {
		for (Agedge_t* edge = agfstedge(g.get(), node); edge; edge = agnxtedge(g.get(), edge, node)) {
			Agnode_t* start = aghead(edge);
			Agnode_t* end = agtail(edge);
			std::pair<Agnode_t*, Agnode_t*> edgeKey = { min(start, end), max(start, end) };

			if (visitedEdges.find(edgeKey) != visitedEdges.end()) {
				continue;
			}
			visitedEdges.insert(edgeKey);

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

			if (x_s > x_e) {
				int temp = x_s;
				x_s = x_e;
				x_e = temp;

				temp = y_s;
				y_s = y_e;
				y_e = temp;
			}

			// 인치 단위를 픽셀단위로 변환하기 위해 72를 곱함	
			int r_xs = static_cast<int>(ND_width(start) * 72 * scaleFactor * (950 / width) / 2);
			int r_ys = static_cast<int>(ND_height(start) * 72 * scaleFactor * (340 / height) / 2);
			int r_xe = static_cast<int>(ND_width(end) * 72 * scaleFactor * (950 / width) / 2);
			int r_ye = static_cast<int>(ND_height(end) * 72 * scaleFactor * (340 / height) / 2);

			std::string node_name_start = agnameof(start);
			std::string node_name_end = agnameof(end);


			// 선을 먼저 그려야 간선이 노드 위로 그려지지 않음
			DrawLine(edge, x_s, y_s, x_e, y_e);
			if (visitedNodes.find(start) == visitedNodes.end()) {
				DrawNode(start, x_s, y_s, r_xs, r_ys);
				visitedNodes.insert(start);
			}
			if (visitedNodes.find(end) == visitedNodes.end()) {
				DrawNode(end, x_e, y_e, r_xe, r_ye);
				visitedNodes.insert(end);
			}
		}
	}	
}

void Graph::DrawNode(Agnode_t* node, int x, int y, int rx, int ry) {
	size_t rv;
	char* guid = agget(node, const_cast<char*>("guid"));
	errno_t err = mbstowcs_s(&rv, nullptr, 0, guid, _TRUNCATE);

	// 2. 변환할 wchar_t 배열 할당
	wchar_t* wGuid = new wchar_t[rv];
	// 3. 변환 수행
	err = mbstowcs_s(&rv, wGuid, rv, guid, _TRUNCATE);

	// 노드 그리기
	HWND hwnd = CreateWindowEx(0, L"NODECLASS", wGuid,
		WS_CHILD | WS_VISIBLE,
		x - rx, y - ry, 2 * rx, 2 * ry,
		panel.m_hwnd, NULL, GetModuleHandle(NULL), nullptr
	);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(node));
}

void Graph::DrawLine(Agedge_t* edge, int x1,int y1, int x2, int y2) {
	int thickness = 5;

	int anchor_x = x1 < x2 ? x1 : x2;
	int anchor_y = y1 < y2 ? y1 : y2;
	int len = abs(x2 - x1);
	int height = abs(y2 - y1) > thickness ? abs(y2 - y1) : thickness;

	HWND line = CreateWindowEx(
		0,
		L"LINECLASS", L"Line Window", WS_CHILD | WS_VISIBLE,
		anchor_x, anchor_y, len, height, 
		panel.m_hwnd, NULL, GetModuleHandle(NULL), nullptr);
	SetWindowLongPtr(line, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(edge));	

	if (height > thickness)	{
		POINT pt[4] = {
			{x1,y1}, {x1,y1},
			{x2,y2}, {x2, y2}
		};
		if (anchor_y == y1) {
			pt[1].y += thickness;
			pt[3].y -= thickness;
		}
		else {
			pt[0].y -= thickness;
			pt[2].y += thickness;
		}
		
		// 회전된 사각형 형태의 Region을 생성하여 적용
		HRGN hRgn = CreatePolygonRgn(pt, 4, WINDING);
		SetWindowRgn(line, hRgn, TRUE);
		DeleteObject(hRgn);
	}
}