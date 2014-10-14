#ifndef AREA_VORONOI_HEADER_GUARD
#define AREA_VORONOI_HEADER_GUARD
#include <vector>
#include "Shapes.h"
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Regular_triangulation_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Regular_triangulation_vertex_base_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Regular_triangulation_face_base_2.h>
#include <CGAL/Regular_triangulation_euclidean_traits_2.h>
#include <CGAL/Regular_triangulation_filtered_traits_2.h>
#include <CGAL/Regular_triangulation_adaptation_traits_2.h>
#include <CGAL/Regular_triangulation_adaptation_policies_2.h>
namespace geometry
{
	namespace areaOfUnionVoronoiImpl
	{
		typedef CGAL::Cartesian<double> K;
		typedef CGAL::Regular_triangulation_euclidean_traits_2<K> Traits;
		typedef CGAL::Regular_triangulation_face_base_2<Traits> Fb;
		typedef CGAL::Regular_triangulation_vertex_base_2<Traits, CGAL::Triangulation_vertex_base_with_info_2<const Circle*, Traits> > Vb;
		typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
		typedef CGAL::Regular_triangulation_2<Traits,Tds> RG;
		typedef CGAL::Regular_triangulation_adaptation_traits_2<RG> AT;
		typedef CGAL::Regular_triangulation_caching_degeneracy_removal_policy_2<RG> AP;
		typedef CGAL::Voronoi_diagram_2<RG,AT,AP> VD;
		float areaOfUnionVoronoiNoSemicircles(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, VD& vd, bool useExistingVD);
	}
	float areaOfUnionVoronoi(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, const std::vector<SemiCircle>& semicircles);
	float areaOfUnionVoronoiNoSemicircles(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines);
}
#endif