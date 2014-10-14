#include <windows.h>
#include "voronoiViewer.h"
#include <QApplication>
#include "areaVoronoi.h"
#include "areaAllSubsets.h"
namespace poissonOrdered
{
	void registerQTPluginDir()
	{
		static bool pluginDir = false;
		if(!pluginDir)
		{
			WCHAR pathArray[500];
			GetModuleFileName(NULL, pathArray, 500);
			int error = GetLastError();
			if(error != ERROR_SUCCESS) 
			{
				exit(-1);
			}
			std::wstring path(&(pathArray[0]));
			
			path.erase(std::find(path.rbegin(), path.rend(), '\\').base(), path.end());
			QApplication::addLibraryPath(QString::fromStdWString(path));
			pluginDir = true;
		}
	}
	int main()
	{
		std::vector<geometry::Circle> circles;
		
		std::vector<geometry::SemiCircle> semicircles;
		/*semicircles.push_back(geometry::SemiCircle(cml::vector2f(0.160087630f, 1.75316739f), cml::vector2f(0.0f, -1.0f), 3.0f));
		semicircles.push_back(geometry::SemiCircle(cml::vector2f(2.86960697f, 0.440185726f), cml::vector2f(0.0f, -1.0f), 3.0f));*/
//circles.push_back(geometry::Circle(6.605171203613281e+000, 4.899177551269531e-001, 1.362487792968750e+000));
//circles.push_back(geometry::Circle(8.257032394409180e+000, 9.626249074935913e-001, 2.422500550746918e-001));
circles.push_back(geometry::Circle(2.060036659240723e+000, 5.804502487182617e+000, 1.631540179252625e+000));
semicircles.push_back(geometry::SemiCircle(cml::vector2f(4.568477153778076e+000, 5.091173648834229e+000), cml::vector2f(9.759211540222168e-001, -2.181236147880554e-001),1.494345903396606e+000));

		std::vector<geometry::Line> lines;
		lines.push_back(geometry::Line(cml::vector2f(10.0f, -0.25f), cml::vector2f(0.0f, 1.0f)));
		lines.push_back(geometry::Line(cml::vector2f(1.25f, 10.0f), cml::vector2f(-1.0f, 0.0f)));
		lines.push_back(geometry::Line(cml::vector2f(0.0f, 1.25f), cml::vector2f(0.0f, -1.0f)));
		lines.push_back(geometry::Line(cml::vector2f(-0.25f, 0.0f), cml::vector2f(1.0f, 0.0f)));
		
		float areaVoronoi = geometry::areaOfUnionVoronoi(circles, lines, semicircles);
		float areaSubsets = geometry::areaOfUnionAllSubsets(circles, lines, semicircles);

		registerQTPluginDir();
		int argc = 0;
		QApplication app(argc, (char**)NULL);
		voronoiViewer viewer(circles, lines, semicircles);
		viewer.show();
		app.exec();

		return 0;
	}
}
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return poissonOrdered::main();
}