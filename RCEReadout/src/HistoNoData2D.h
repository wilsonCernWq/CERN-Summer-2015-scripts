/*
 *  HistoNoData2D.h
 *  PileupSimu
 *
 *  Created by Jens Dopke on 18/05/2009.
 *  Copyright 2009 University of Wuppertal. All rights reserved.
 *
 */

#include <stdlib.h>
#include <iostream>

class Vec2 {
public:
	Vec2( const double x = 0.0, const double y = 0.0) : x(x), y(y) { }
	Vec2( const Vec2& toBeCopied) : x(toBeCopied.x), y(toBeCopied.y) { }
	~Vec2() {}
	double x, y;
};

using namespace std;

class HistoNoData2D{
public:
	// Constructoren ohne Ende
	HistoNoData2D(string title = " ", int nBinsX = 10, double minX=-1.0, double maxX=1.0, int nBinsY = 10, double minY=-1.0, double maxY=1.0) : title(title), nBinsX(nBinsX), maxX(maxX), minX(minX), nBinsY(nBinsY), maxY(maxY), minY(minY), binWidthX((double) (maxX-minX)/((double) nBinsX)), binWidthY((double) (maxY-minY)/((double) nBinsY)) { 
		histoData = (int *) malloc(sizeof(float) * (nBinsX + 2) * (nBinsY + 2));
		histoDataCount = (int *) malloc(sizeof(int) * (nBinsX + 2) * (nBinsY + 2));
		summX = (double*) malloc(sizeof(double) * (nBinsY + 2));
		summY = (double*) malloc(sizeof(double) * (nBinsX + 2));
		this->clear();
	}
	HistoNoData2D(int nBinsX = 10, double minX=-1.0, double maxX=1.0, int nBinsY = 10, double minY=-1.0, double maxY=1.0) : title(" "), nBinsX(nBinsX), maxX(maxX), minX(minX), nBinsY(nBinsY), maxY(maxY), minY(minY), binWidthX((double) (maxX-minX)/((double) nBinsX)), binWidthY((double) (maxY-minY)/((double) nBinsY)) { 
		histoData = (int *) malloc(sizeof(float) * (nBinsX + 2) * (nBinsY + 2));
		histoDataCount = (int *) malloc(sizeof(int) * (nBinsX + 2) * (nBinsY + 2));
		summX = (double*) malloc(sizeof(double) * (nBinsY + 2));
		summY = (double*) malloc(sizeof(double) * (nBinsX + 2));
		this->clear();
	}
	HistoNoData2D(double minX=-1.0, double maxX=1.0, double minY=-1.0, double maxY=1.0) : title(" "), nBinsX(10), maxX(maxX), minX(minX), nBinsY(10), maxY(maxY), minY(minY), binWidthX((double) (maxX-minX)/((double) 10.0)), binWidthY((double) (maxY-minY)/((double) 10.0)) { 
		histoData = (int *) malloc(sizeof(float) * (nBinsX + 2) * (nBinsY + 2));
		histoDataCount = (int *) malloc(sizeof(int) * (nBinsX + 2) * (nBinsY + 2));
		summX = (double*) malloc(sizeof(double) * (nBinsY + 2));
		summY = (double*) malloc(sizeof(double) * (nBinsX + 2));
		this->clear();
	}
	HistoNoData2D(const HistoNoData2D& toBeCopied) : title(toBeCopied.title), nBinsX(toBeCopied.nBinsX), minX(toBeCopied.minX), maxX(toBeCopied.maxX), nBinsY(toBeCopied.nBinsY), minY(toBeCopied.minY), maxY(toBeCopied.maxY), binWidthX(toBeCopied.binWidthX), binWidthY(toBeCopied.binWidthY) { }
	// Default Destructor
	~HistoNoData2D() { 
		free(summX);
		free(summY);
		free(histoData);
	}
	
	void readFromStreamNoBorders(istream& readStream = std::cin) {
		char* dummyCString;
		float dummyX, dummyY;
		dummyCString = (char *) malloc(100);
		for(int yBin=0; yBin < nBinsY; yBin++) {
			for(int xBin=0; xBin < nBinsX; xBin++) {
				readStream >> dummyX >> dummyY >> histoData[(xBin + 1) + ((yBin + 1) * (nBinsX + 2))]; 
			}
			readStream.getline(dummyCString, 100);
		}
		free(dummyCString);
	}
	
	void fill(const Vec2& newData) {
		int sortx, sorty;
		if (newData.x < minX) sortx = 0;
		else if (newData.x >= maxX) sortx = nBinsX + 1;
		else sortx = (((newData.x - minX)/ binWidthX) + 1);
		
		if (newData.y < minY) sorty = 0;
		else if (newData.y >= maxY) sorty = nBinsY + 1;
		else sorty = (((newData.y - minY)/ binWidthY) + 1);
		
		histoData[sortx + sorty * (nBinsX + 2)] += 1.0;
		histoDataCount[sortx + sorty * (nBinsX + 2)]++;
		if ((sortx != 0) && (sortx != nBinsX+1)) summX[sorty] += newData.x;
		if ((sorty != 0) && (sorty != nBinsY+1)) summY[sortx] += newData.y;
	}

	void fill(const Vec2& newData, float value) {
		int sortx, sorty;
		if (newData.x < minX) sortx = 0;
		else if (newData.x >= maxX) sortx = nBinsX + 1;
		else sortx = (((newData.x - minX)/ binWidthX) + 1);
		
		if (newData.y < minY) sorty = 0;
		else if (newData.y >= maxY) sorty = nBinsY + 1;
		else sorty = (((newData.y - minY)/ binWidthY) + 1);
		
		histoData[sortx + sorty * (nBinsX + 2)] += value;
		histoDataCount[sortx + sorty * (nBinsX + 2)]++;
		if ((sortx != 0) && (sortx != nBinsX+1)) summX[sorty] += newData.x;
		if ((sorty != 0) && (sorty != nBinsY+1)) summY[sortx] += newData.y;
	}

	void clear() {
		for(int i=0; i < ((nBinsX + 2) * (nBinsY + 2)); i++) histoData[i] = 0;
		for(int i=0; i < (nBinsY + 2); i++) summX[i] = 0.0;
		for(int i=0; i < (nBinsX + 2); i++) summY[i] = 0.0;
	}
	
	void HistoTable (ostream& os = std::cout) {
		for(int yBin=0; yBin < (nBinsY + 2); yBin++) {
			for(int xBin=0; xBin < (nBinsX + 2); xBin++) {
				os << (minX + (((double) xBin - 0.5) * binWidthX)) << "\t" << (minY + (((double) yBin - 0.5) * binWidthY)) << "\t" << histoData[xBin + (yBin * (nBinsX + 2))] << std::endl; 
			}
			os << std::endl;
		}
	}
	
	void HistoTableNoBorder (ostream& os = std::cout) {
		for(int yBin=0; yBin < nBinsY; yBin++) {
			for(int xBin=0; xBin < nBinsX; xBin++) {
				os << (minX + (((double) xBin + 0.5) * binWidthX)) << "\t" << (minY + (((double) yBin + 0.5) * binWidthY)) << "\t" << histoData[(xBin + 1) + ((yBin + 1) * (nBinsX + 2))] << std::endl; 
			}
			os << std::endl;
		}
	}
	
	float getBinContent(int xBin, int yBin) {
		return histoData[xBin + yBin * (nBinsX + 2)];
	}
	
	float getBinMean(int xBin, int yBin) {
		return (histoData[xBin + yBin * (nBinsX + 2)] / ((float) histoDataCount[xBin + yBin * (nBinsX + 2)]));
	}
	
	Vec2 getBinAddress(Vec2 location) {
		int sortx, sorty;
		if (location.x < minX) sortx = 0;
		else if (location.x >= maxX) sortx = nBinsX + 1;
		else sortx = (((location.x - minX)/ binWidthX) + 1);
		
		if (location.y < minY) sorty = 0;
		else if (location.y >= maxY) sorty = nBinsY + 1;
		else sorty = (((location.y - minY)/ binWidthY) + 1);
		
		return Vec2((double) sortx, (double) sorty);
	}
	
	// Not summing with outliers
	float projectionX(int yBin) {
		float projectionSumm=0;
		for(int i=1; i<(nBinsX + 1); i++) projectionSumm += histoData[i + yBin * (nBinsX + 2)];
		return projectionSumm;
	}
	
	// Not summing with outliers
	float projectionY(int xBin) {
		float projectionSumm=0;
		for(int i=1; i<(nBinsY + 1); i++) projectionSumm += histoData[xBin + i * (nBinsX + 2)];
		return projectionSumm;
	}
	
	double meanX(int yBin) {
		return (summX[yBin] / ((double) projectionX(yBin)));
	}
	
	double meanY(int xBin) {
		return (summY[xBin] / ((double) projectionY(xBin)));		
	}
	
	double averageX(int yBin) {
		return ((double) projectionX(yBin) / ((double) nBinsX));
	}
	
	double averageY(int xBin) {		
		return ((double) projectionY(xBin) / ((double) nBinsY));
	}
	
	void ProjectionTableX(ostream& os = std::cout) {
	
	}
	
	void ProjectionTableY(ostream& os = std::cout) {
		
	}
	
	void MeanTableX(ostream& os = std::cout) {
		
	}
	
	void MeanTableY(ostream& os = std::cout) {
		for(int xBin=0; xBin < nBinsX; xBin++) {
			os << (minX + (((double) xBin + 0.5) * binWidthX)) << "\t" << summY[xBin] << "\t" << projectionY(xBin) << "\t" << meanY(xBin)  << "\t" << averageY(xBin) << std::endl; 
		}
	}
	
private:
	string title;
	double minX, maxX, minY, maxY;
	int nBinsX, nBinsY;
	
	double binWidthX, binWidthY;
	double *summX, *summY;
	int *histoData, *histoDataCount;	
};
