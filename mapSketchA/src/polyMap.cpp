#include "polyMap.h"
// 2.15
#define LINE_SIMPLIFY 3.55
#define CV_BLUR 3

PolyMap::PolyMap() {}

void PolyMap::loadMap(ofImage &terrain_, int steps_) {

  levels.clear();
  float layerSteps = (255.0 / steps_);
  ofImage tempImg;
  tempImg.allocate(terrain_.getWidth(), terrain_.getHeight(), OF_IMAGE_COLOR);
  cvColorImg.allocate(terrain_.getWidth(), terrain_.getHeight());
  cvGrayImg.allocate(terrain_.getWidth(), terrain_.getHeight());

  // convert hight info to 0-255
  convertBW(terrain_);

  unsigned char *terrainImgData = terrain_.getPixels().getData();
  unsigned char *tempImgData = tempImg.getPixels().getData();
  cout << terrain_.getPixels().size() << endl;
  cout << tempImg.getPixels().size() << endl;

  for (int i = 1; i < steps_; i++) {
    for (int j = 0; j < terrain_.getWidth() * terrain_.getHeight(); j++) {
      if ((terrainImgData[j * 3] >= (layerSteps * i))) {
        tempImgData[(j * 3)] = 255;
        tempImgData[(j * 3) + 1] = 255;
        tempImgData[(j * 3) + 2] = 255;
      } else {
        tempImgData[(j * 3)] = 0;
        tempImgData[(j * 3) + 1] = 0;
        tempImgData[(j * 3) + 2] = 0;
      }
    }

    MapLevel tempM;

    cvColorImg = tempImg;
    cvGrayImg.setFromColorImage(cvColorImg);
    cvGrayImg.blur(CV_BLUR);
    tempM.levelImg.setFromPixels(cvGrayImg.getPixels());

    contourFinder.findContours(
        cvGrayImg, 10, (terrain_.getWidth() * terrain_.getHeight()) * 0.90, 220,
        false);

    for (int k = 0; k < contourFinder.blobs.size(); k++) {

      ofPolyline tempLineFirst;
      tempLineFirst.curveTo(contourFinder.blobs.at(k).pts.front());
      for (int l = 0; l < contourFinder.blobs.at(k).nPts; l++) {
        tempLineFirst.curveTo(contourFinder.blobs.at(k).pts.at(l));
      }
      tempLineFirst.curveTo(contourFinder.blobs.at(k).pts.back());

      tempLineFirst.close();
      tempLineFirst.simplify(LINE_SIMPLIFY);
      // tempLineFirst = tempLineFirst.getSmoothed(2,0.5);

      ofPath tempPath;
      // tempPath.setCurveResolution(60);
      tempPath.moveTo(tempLineFirst.getVertices().front());
      tempPath.curveTo(tempLineFirst.getVertices().front());

      for (int l = 0; l < tempLineFirst.getVertices().size(); l++) {
        tempPath.curveTo(tempLineFirst.getVertices().at(l));
      }
      tempPath.lineTo(tempLineFirst.getVertices().back());

      tempPath.setFillColor(ofColor(layerSteps * i));
      tempPath.setStrokeColor(ofColor(ofColor::white));
      tempPath.setStrokeWidth(22.50);
      tempPath.setFilled(false);
      tempPath.close();

      tempM.lines.push_back(tempPath);
    }

    tempM.height = layerSteps * i;
    levels.push_back(tempM);
    cout << "added " << levels.back().lines.size() << " polylines at height "
         << layerSteps * i << endl;
  }
}

void PolyMap::convertBW(ofImage &imgTerrain_) {
  float minV = 60000;
  float maxV = 0;

  int w = imgTerrain_.getWidth();
  int h = imgTerrain_.getHeight();
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      ofColor c = imgTerrain_.getColor(i, j);
      float ele = (c.r * 256 + c.g + c.b / 256) - 32768;
      if (minV > ele) {
        minV = ele;
      }
      if (maxV < ele) {
        maxV = ele;
      }
    }
  }

  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      ofColor c = imgTerrain_.getColor(i, j);
      float ele = (c.r * 256 + c.g + c.b / 256) - 32768;
      imgTerrain_.setColor(i, j, ofMap(ele, minV, maxV, 0, 255));
    }
  }

  imgTerrain_.update();
}
