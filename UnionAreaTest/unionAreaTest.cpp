#include <windows.h>
#include "Shapes.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include "areaAllSubsets.h"
#include "areaVoronoi.h"
namespace poissonOrdered
{
	int main()
	{
		const int nObjects = 4;
		std::vector<geometry::Line> lines;
		lines.push_back(geometry::Line(cml::vector2f(10.0f, -0.25f), cml::vector2f(0.0f, 1.0f)));
		lines.push_back(geometry::Line(cml::vector2f(1.25f, 10.0f), cml::vector2f(-1.0f, 0.0f)));
		lines.push_back(geometry::Line(cml::vector2f(0.0f, 1.25f), cml::vector2f(0.0f, -1.0f)));
		lines.push_back(geometry::Line(cml::vector2f(-0.25f, 0.0f), cml::vector2f(1.0f, 0.0f)));
		
		boost::mt19937 randomSource;
		randomSource.seed(1);
		boost::random::uniform_real_distribution<float> uniformPosition(0.0f, 10.0f);
		boost::random::uniform_real_distribution<float> uniformRadius(0.0f, 3.0f);

		boost::random::uniform_real_distribution<float> angleDistribution(0.0f, (float)(2*M_PI));
		
		boost::random::bernoulli_distribution<float> isSemiCircleDist(0.5f);

		for(int i = 0; i < 1000; i++)
		{
			std::vector<geometry::Circle> circles;
			std::vector<geometry::SemiCircle> semicircles;
			for(int j = 0; j < nObjects; j++)
			{
				if(isSemiCircleDist(randomSource))
				{
					cml::vector2f pos(uniformPosition(randomSource), uniformPosition(randomSource));
					float angle = angleDistribution(randomSource);
					cml::vector2f direction(cos(angle), sin(angle));
					semicircles.push_back(geometry::SemiCircle(pos, direction, uniformRadius(randomSource)));
				}
				else
				{
					circles.push_back(geometry::Circle(uniformPosition(randomSource), uniformPosition(randomSource), uniformRadius(randomSource)));
				}
			}
			float areaVoronoi;
			try
			{
				areaVoronoi = geometry::areaOfUnionVoronoi(circles, lines, semicircles);
			}
			catch(...)
			{
				std::cout << "Exception with i = " << i << std::endl;
				goto printObjects;
			}
			float areaAllSubsets = geometry::areaOfUnionAllSubsets(circles, lines, semicircles);
			if(fabs(areaVoronoi - areaAllSubsets) > 0.01)
			{
				std::cout << "i : " << i << ", difference between exact methods was " << fabs(areaVoronoi - areaAllSubsets) << std::endl;
				goto printObjects;
			}
			int sum = 0;
			for(int j = 0; j < 100000; j++)
			{
				cml::vector2f randomPoint(uniformPosition(randomSource), uniformPosition(randomSource));
				bool keep = false;
				for(int k = 0; k < circles.size(); k++)
				{
					if((randomPoint - circles[k].pos).length_squared() <= circles[k].radius * circles[k].radius) keep = true;
				}
				for(int k = 0; k < semicircles.size(); k++)
				{
					if((randomPoint - semicircles[k].circle.pos).length_squared() <= semicircles[k].circle.radius * semicircles[k].circle.radius && semicircles[k].straightLine.correctSide(randomPoint))
					{
						keep = true;
					}
				}
				if(keep) sum++;
			}
			float areaEstimate = ((float)sum / 100000.0f) * 100;
			if(areaVoronoi != areaVoronoi || fabs(areaVoronoi - areaEstimate) > 0.6f)
			{
				std::cout << "i : " << i << ",  difference was " << fabs(areaVoronoi - areaEstimate) << std::endl;
			printObjects:
				std::cout.precision(std::numeric_limits<double>::digits10);
				std::cout << std::scientific;
				for(int j = 0; j < circles.size(); j++)
				{
					std::cout << "circles.push_back(Circle(" << circles[j].pos[0] << ", " << circles[j].pos[1] << ", " << circles[j].radius << "));" << std::endl;
				}
				for(int j = 0; j < semicircles.size(); j++)
				{
					std::cout << "semicircles.push_back(SemiCircle(cml::vector2f(" << semicircles[j].circle.pos[0] << ", " << semicircles[j].circle.pos[1] << "), cml::vector2f(" << semicircles[j].straightLine.unitDirection[0] << ", " << semicircles[j].straightLine.unitDirection[1] << "), " << semicircles[j].circle.radius << "));" << std::endl;
				}
				std::cout.flush();
				return 0;
			}
		}
		return 0;
	}
}
int main(int argc, char **argv)
{
	return poissonOrdered::main();
}