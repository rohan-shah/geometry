#include <windows.h>
#include "intersectionViewer.h"
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
		circles.push_back(geometry::Circle(0.0f, 0.0f, 1.0f));
		circles.push_back(geometry::Circle(0.5f, 0.5f, 1.0f));
		circles.push_back(geometry::Circle(1.1f, 1.1f, 1.2f));
		circles.push_back(geometry::Circle(0.6f, 1.1f, 1.2f));

		registerQTPluginDir();
		int argc = 0;
		QApplication app(argc, (char**)NULL);
		intersectionViewer viewer(circles);
		viewer.show();
		app.exec();

		return 0;
	}
}
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return poissonOrdered::main();
}