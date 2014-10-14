#include <windows.h>
#include "convexBoundaryViewer.h"
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
		circles.push_back(geometry::Circle(5.38817f, 9.35539f, 1.1903f));
		circles.push_back(geometry::Circle(6.8522f, 3.13274f, 1.25758f));
		
		std::vector<geometry::SemiCircle> semicircles;
		semicircles.push_back(geometry::SemiCircle(cml::vector2f(7.20325f, 9.97185f), cml::vector2f(0.911552f, -0.411186f), 0.000343143f));
		semicircles.push_back(geometry::SemiCircle(cml::vector2f(9.99041f, 3.02333f), cml::vector2f(0.604152f, 0.796869f), 0.708267f));
		semicircles.push_back(geometry::SemiCircle(cml::vector2f(1.8626f, 3.96581f), cml::vector2f(-0.76208f, 0.647483f), 1.03668f));

		std::vector<geometry::Line> lines;
		lines.push_back(geometry::Line(cml::vector2f(10.0f, -0.25f), cml::vector2f(0.0f, 1.0f)));
		lines.push_back(geometry::Line(cml::vector2f(1.25f, 10.0f), cml::vector2f(-1.0f, 0.0f)));
		lines.push_back(geometry::Line(cml::vector2f(0.0f, 1.25f), cml::vector2f(0.0f, -1.0f)));
		lines.push_back(geometry::Line(cml::vector2f(-0.25f, 0.0f), cml::vector2f(1.0f, 0.0f)));
		
		registerQTPluginDir();
		int argc = 0;
		QApplication app(argc, (char**)NULL);
		convexBoundaryViewer viewer(circles, lines, semicircles);
		viewer.show();
		app.exec();

		return 0;
	}
}
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return poissonOrdered::main();
}