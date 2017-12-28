/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/*
 * TLD.h
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */

#ifndef TLD_H_
#define TLD_H_

#include <opencv/cv.h>
#include <memory>
#include <random>
#include "opencv2/core/core.hpp"
#include "cf_tracker.hpp"
#include "DetectorCascade.h"

namespace tld
{
    class TLD
    {
    private:
        void storeCurrentData();
        void fuseHypotheses(const cv::Mat& colorImg);
        void learn();
        void initialLearning();
        void deleteCurrentBB();
        std::shared_ptr<cf_tracking::CfTracker> tracker;
    public:
        DetectorCascade *detectorCascade;
        NNClassifier *nnClassifier;
        cv::Mat currImg;
        cv::Rect *currBB;
        cv::Rect trackerBB;
        bool valid;
        bool isTrackerValid;
        bool runTracker;
        float currConf;
        bool learning;

        bool hasImageDimensions;
        bool trackerEnabled;
        bool detectorEnabled;
        bool learningEnabled;
        bool alternating;
        std::shared_ptr<std::mt19937> rng;
        int seed;

        TLD();
        virtual ~TLD();
        void init(bool useDsstTracker);
        void release();
        void selectObject(const cv::Mat &img, cv::Rect *bb);
        void processImage(cv::Mat &img);
    };
} /* namespace tld */
#endif /* TLD_H_ */
