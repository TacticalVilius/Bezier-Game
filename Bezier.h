#ifndef _ISNAH_BEZIER_H_
#define _ISNAH_BEZIER_H_

#include <vector>
#include <fstream>
#include <iostream>

struct Point3d {
	float x, y, z;
};

class HelperMethods {
public:
	static Point3d* findPoint(const Point3d p1, const Point3d p2, float part);
};

class BezierCurve {
public:
	BezierCurve();
	explicit BezierCurve(std::vector<Point3d*> pointsList);

	~BezierCurve();

	void setBasePoints(std::vector<Point3d*> pointsList);
	std::vector<Point3d*> getBasePoints() const;
	std::vector<Point3d*> getBezierPoints() const;
	Point3d* getBezierPoint(unsigned int pointNo) const;

	void calculateBezier(int points);

private:
	std::vector<Point3d*> bezierPoints;
	std::vector<Point3d*> basePoints;
	Point3d* recBezierHelper(std::vector<Point3d*> points, float part);
};

#endif