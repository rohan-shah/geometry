#include <windows.h>
#include "intersectionAreaViewer2.h"
#include <QApplication>
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
		semicircles.push_back(geometry::SemiCircle(cml::vector2f(6.25295734, 2.30865479), cml::vector2f(0.687743843, -0.725953460), 2.99709177));
		semicircles.push_back(geometry::SemiCircle(cml::vector2f(7.91560078, 3.61652899), cml::vector2f(0.526951671, -0.849895239), 0.95));//0.888043523));

		std::vector<geometry::Line> lines;
		lines.push_back(geometry::Line(cml::vector2f(10.0f, -0.25f), cml::vector2f(0.0f, 1.0f)));
		lines.push_back(geometry::Line(cml::vector2f(1.25f, 10.0f), cml::vector2f(-1.0f, 0.0f)));
		lines.push_back(geometry::Line(cml::vector2f(0.0f, 1.25f), cml::vector2f(0.0f, -1.0f)));
		lines.push_back(geometry::Line(cml::vector2f(-0.25f, 0.0f), cml::vector2f(1.0f, 0.0f)));
		
		registerQTPluginDir();
		int argc = 0;
		QApplication app(argc, (char**)NULL);
		intersectionAreaViewer viewer(circles, lines, semicircles);
		viewer.show();
		app.exec();

		return 0;
	}
}
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return poissonOrdered::main();
}