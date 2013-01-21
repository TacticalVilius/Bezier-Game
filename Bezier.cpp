#include "Bezier.h"

BezierCurve::BezierCurve() 
{
}

BezierCurve::BezierCurve(std::vector<Point3d*> pointsList) 
	: basePoints(pointsList)
{
}

BezierCurve::~BezierCurve() 
{
  for(unsigned int i = 0; i < basePoints.size(); ++i) {
    delete basePoints[i];
  }

  for(unsigned int i = 0; i < bezierPoints.size(); ++i) {
    delete bezierPoints[i];
  }
}

void BezierCurve::setBasePoints(std::vector<Point3d*> pointsList) 
{
	if(!basePoints.empty())	basePoints.clear();
	basePoints = pointsList;
}

std::vector<Point3d*> BezierCurve::getBasePoints() const 
{
	return basePoints;
}

std::vector<Point3d*> BezierCurve::getBezierPoints() const 
{
	return bezierPoints;
}

Point3d* BezierCurve::getBezierPoint(unsigned int pointNo) const 
{
	if(bezierPoints.size() > pointNo && pointNo >= 0) {
		return bezierPoints[pointNo];
	}

	std::cerr << "Error retrieving point from list. Asked for index " << pointNo << ", size of array: " << bezierPoints.size() << ". Returned faux point." << std::endl;
	Point3d* point = new Point3d;
	point->x = 0;
	point->y = 0;
	point->z = 0;

	return point;
}

void BezierCurve::calculateBezier(int points) 
{
	points--;

	std::cout << "Creating curve";

	for(int i = 0; i <= points; ++i) {
		bezierPoints.push_back(recBezierHelper(basePoints, ((float)i)/points));
		std::cout << ".";
	}

	std::cout << std::endl;
}

Point3d* BezierCurve::recBezierHelper(std::vector<Point3d*> points, float part) 
{
	if(points.size() == 2) {															
		return HelperMethods::findPoint(*points[0], *points[1], part);				
	}

	std::vector<Point3d*> points2;														
	for(size_t i = 0; i < (points.size() - 1); ++i) {										
		points2.push_back(HelperMethods::findPoint(*points[i], *points[i+1], part));   
	}
	return recBezierHelper(points2, part);

}

Point3d* HelperMethods::findPoint(const Point3d p1, const Point3d p2, float part) 
{
	Point3d* point = new Point3d;
	point->x = p2.x - p1.x;  
	point->y = p2.y - p1.y;   
	point->z = p2.z - p1.z;   

	point->x *= part;         
	point->y *= part;         
	point->z *= part;

	point->x += p1.x;
	point->y += p1.y;
	point->z += p1.z;

	return point;
}
/*
int main() 
{
	std::cout << "How many basePoints do you want? ";
	int basePointAmount;
	std::cin >> basePointAmount;
	std::cout << std::endl;

	std::vector<Point3d*> basePoints;

	for(int i = 0; i < basePointAmount; ++i) {
		float x, y, z;
		std::cout << "X: ";
		std::cin >> x;
		std::cout << "Y: ";
		std::cin >> y;
		std::cout << "Z: ";
		std::cin >> z;

		Point3d *point = new Point3d;
		point->x = x;
		point->y = y;
		point->z = z;

		basePoints.push_back(point);
	}

	BezierCurve curve(basePoints);
	int bezierPointAmount;

	std::cout << "How many bezierPoints do you want? ";
	std::cin >> bezierPointAmount;
	std::cout << std::endl;

	curve.calculateBezier(bezierPointAmount);

	int displayAmount;
	std::cout << "How many points do you want to display? ";
	std::cin >> displayAmount;

	std::cout << "x\ty\tz" << std::endl;
	for(int i = 0; i < displayAmount; ++i) {
		Point3d* point = curve.getBezierPoint(bezierPointAmount*i/displayAmount);
		std::cout << point->x << "\t" << point->y << "\t" << point->z << std::endl;
	}
	return 0;
}*/