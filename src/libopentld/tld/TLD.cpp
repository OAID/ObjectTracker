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
 * TLD.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay

 * + Author: Klaus Haag
 *  * Replace Median Flow tracker with KCFcpp or DSSTcpp
 *  * Change to self-learning approach instead of semi-supervised
 */

#include "TLD.h"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <random>
#include "kcf_tracker.hpp"
#include "dsst_tracker.hpp"

#include "NNClassifier.h"
#include "TLDUtil.h"

using namespace std;
using namespace cv;
using namespace cf_tracking;

namespace tld
{
    TLD::TLD() :
        hasImageDimensions(false),
        isTrackerValid(false),
        runTracker(true),
        tracker(0)
    {
        trackerEnabled = true;
        detectorEnabled = true;
        learningEnabled = true;
        alternating = false;
        valid = false;
        learning = false;
        currBB = NULL;
        seed = 0;
        detectorCascade = new DetectorCascade();
        nnClassifier = detectorCascade->nnClassifier;
    }

    void TLD::init(bool useDsstTracker)
    {
        if (!tracker)
        {
            if (useDsstTracker)
            {
                DsstParameters dsstParameters;
                dsstParameters.enableTrackingLossDetection = true;
                tracker.reset(new DsstTracker(dsstParameters));
            }
            else
            {
                KcfParameters kcfParameters;
                kcfParameters.enableTrackingLossDetection = true;
                tracker.reset(new KcfTracker(kcfParameters));
            }
        }

        std::cout << "\n\n\n-----------CF_TLD---------------\n";
        std::cout << "---------- " << tracker->getId() << " --------------" << "\n\n\n";
    }

    TLD::~TLD()
    {
        storeCurrentData();
        deleteCurrentBB();

        if (detectorCascade)
        {
            delete detectorCascade;
            detectorCascade = NULL;
        }
    }

    void TLD::release()
    {
        detectorCascade->release();
        deleteCurrentBB();
    }

    void TLD::storeCurrentData()
    {
        isTrackerValid = false;
        detectorCascade->cleanPreviousData(); //Reset detector results
    }

    void TLD::selectObject(const Mat &img, Rect *bb)
    {
        if (!tracker)
            return;

        if (!rng)
        {
            std::random_device rd;
            std::shared_ptr<std::mt19937> g(new std::mt19937(rd()));
            g->seed((unsigned long)seed);
            rng = g;
        }

        rng->seed((unsigned long)seed);

        Mat grayFrame;
        cvtColor(img, grayFrame, CV_BGR2GRAY);

        // initialize the image dimensions once
        if (!hasImageDimensions)
        {
            detectorCascade->imgWidth = grayFrame.cols;
            detectorCascade->imgHeight = grayFrame.rows;
            detectorCascade->imgWidthStep = static_cast<int>(grayFrame.step);
        }

        runTracker = true;
        //Delete old object
        detectorCascade->release();

        detectorCascade->objWidth = bb->width;
        detectorCascade->objHeight = bb->height;

        if (bb->height < detectorCascade->minSize)
            detectorCascade->minSize = bb->height;

        if (bb->width < detectorCascade->minSize)
            detectorCascade->minSize = bb->width;

        //Init detector cascade
        detectorCascade->init(rng);
        tracker->reinit(img, *bb);

        currImg = grayFrame;
        deleteCurrentBB();
        currBB = tldCopyRect(bb);
        currConf = 1;
        valid = true;

        initialLearning();
    }

    void TLD::processImage(cv::Mat &img)
    {
        if (!tracker)
            return;

        storeCurrentData();

        Mat grayFrame;
        cvtColor(img, grayFrame, CV_BGR2GRAY);
        currImg = grayFrame; // Store new image , right after storeCurrentData();

        if (trackerEnabled && runTracker)
        {
            isTrackerValid = tracker->update(img, trackerBB);

            if (!isTrackerValid)
                runTracker = false;
        }

        if (detectorEnabled && (!alternating || !isTrackerValid))
            detectorCascade->detect(grayFrame);

        fuseHypotheses(img);

        learn();
    }

    void TLD::fuseHypotheses(const Mat& colorImg)
    {
        int numClusters = detectorCascade->detectionResult->numClusters;
        Rect *detectorBB = detectorCascade->detectionResult->detectorBB;
        deleteCurrentBB();

        currConf = 0;
        valid = false;

        if (!detectorEnabled)
        {
            if (!isTrackerValid)
                return;

            currBB = tldCopyRect(&trackerBB);
            valid = true;
            return;
        }

        float confDetector = 0;

        if (numClusters == 1)
            confDetector = nnClassifier->classifyBB(currImg, detectorBB);

        if (isTrackerValid)
        {
            float confTracker = nnClassifier->classifyBB(currImg, &trackerBB);
            currBB = tldCopyRect(&trackerBB);
            valid = true;
            currConf = confTracker;
        }
        else if (numClusters == 1)
        {
            // reinit tracker
            if (trackerEnabled)
            {
                if (tracker->updateAt(colorImg, *detectorBB))
                {
                    currConf = nnClassifier->classifyBB(currImg, detectorBB);
                    currBB = tldCopyRect(detectorBB);
                    valid = true;
                    runTracker = true;
                }
                else
                {
                    deleteCurrentBB();
                    currConf = 0;
                    valid = false;
                }
            }
        }
    }

    void TLD::initialLearning()
    {
        learning = true; //This is just for display purposes

        DetectionResult *detectionResult = detectorCascade->detectionResult;

        detectorCascade->detect(currImg);

        //This is the positive patch
        NormalizedPatch initPatch;
        tldExtractNormalizedPatchRect(currImg, currBB, initPatch.values);
        initPatch.positive = 1;

        float initVar = tldCalcVariance(initPatch.values, TLD_PATCH_SIZE * TLD_PATCH_SIZE);
        detectorCascade->varianceFilter->minVar = initVar / 2;

        float *overlap = new float[detectorCascade->numWindows]{};
        tldOverlapRect(detectorCascade->windows, detectorCascade->numWindows, currBB, overlap);

        //Add all bounding boxes with high overlap
        vector< pair<int, float> > positiveIndices;
        vector<int> negativeIndices;

        //First: Find overlapping positive and negative patches
        for (int i = 0; i < detectorCascade->numWindows; i++)
        {
            if (overlap[i] > 0.7)
            {
                positiveIndices.push_back(pair<int, float>(i, overlap[i]));
            }

            if (overlap[i] < 0.2)
            {
                float variance = detectionResult->variances[i];

                if (!detectorCascade->varianceFilter->enabled || variance > detectorCascade->varianceFilter->minVar)   //TODO: This check is unnecessary if minVar would be set before calling detect.
                {
                    negativeIndices.push_back(i);
                }
            }
        }

        sort(positiveIndices.begin(), positiveIndices.end(), tldSortByOverlapDesc);

        vector<NormalizedPatch> patches;

        patches.push_back(initPatch); //Add first patch to patch list

        size_t numIterations = std::min<size_t>(positiveIndices.size(), 10); //Take at most 10 bounding boxes (sorted by overlap)

        for (int i = 0; i < numIterations; i++)
        {
            int idx = positiveIndices.at(i).first;
            //Learn this bounding box
            //TODO: Somewhere here image warping might be possible
            detectorCascade->ensembleClassifier->learn(&detectorCascade->windows[TLD_WINDOW_SIZE * idx], true, &detectionResult->featureVectors[detectorCascade->numTrees * idx]);
        }

        std::shuffle(negativeIndices.begin(), negativeIndices.end(), *rng);

        //Choose 100 random patches for negative examples
        for (size_t i = 0; i < std::min<size_t>(100, negativeIndices.size()); i++)
        {
            int idx = negativeIndices.at(i);

            NormalizedPatch patch;
            tldExtractNormalizedPatchBB(currImg, &detectorCascade->windows[TLD_WINDOW_SIZE * idx], patch.values);
            patch.positive = 0;
            patches.push_back(patch);
        }

        detectorCascade->nnClassifier->learn(patches);
        delete[] overlap;
    }

    //Do this when current trajectory is valid
    void TLD::learn()
    {
        if (!learningEnabled || !valid || !detectorEnabled)
        {
            learning = false;
            return;
        }

        learning = true;

        DetectionResult *detectionResult = detectorCascade->detectionResult;

        if (!detectionResult->containsValidData)
            detectorCascade->detect(currImg);

        //This is the positive patch
        NormalizedPatch patch;
        tldExtractNormalizedPatchRect(currImg, currBB, patch.values);

        float *overlap = new float[detectorCascade->numWindows]{};
        tldOverlapRect(detectorCascade->windows, detectorCascade->numWindows, currBB, overlap);

        //Add all bounding boxes with high overlap
        vector<pair<int, float> > positiveIndices;
        vector<int> negativeIndices;
        vector<int> negativeIndicesForNN;

        //First: Find overlapping positive and negative patches

        for (int i = 0; i < detectorCascade->numWindows; i++)
        {
            if (overlap[i] > 0.7)
            {
                positiveIndices.push_back(pair<int, float>(i, overlap[i]));
            }

            if (overlap[i] < 0.2)
            {
                if (!detectorCascade->ensembleClassifier->enabled || detectionResult->posteriors[i] > 0.5)   //Should be 0.5 according to the paper
                {
                    negativeIndices.push_back(i);
                }

                if (!detectorCascade->ensembleClassifier->enabled || detectionResult->posteriors[i] > 0.5)
                {
                    negativeIndicesForNN.push_back(i);
                }
            }
        }

        sort(positiveIndices.begin(), positiveIndices.end(), tldSortByOverlapDesc);

        vector<NormalizedPatch> patches;

        patch.positive = 1;
        patches.push_back(patch);
        //TODO: Flip

        size_t numIterations = std::min<size_t>(positiveIndices.size(), 10); //Take at most 10 bounding boxes (sorted by overlap)

        for (size_t i = 0; i < negativeIndices.size(); i++)
        {
            int idx = negativeIndices.at(i);
            //TODO: Somewhere here image warping might be possible
            detectorCascade->ensembleClassifier->learn(&detectorCascade->windows[TLD_WINDOW_SIZE * idx], false, &detectionResult->featureVectors[detectorCascade->numTrees * idx]);
        }

        //TODO: Randomization might be a good idea
        for (int i = 0; i < numIterations; i++)
        {
            int idx = positiveIndices.at(i).first;
            //TODO: Somewhere here image warping might be possible
            detectorCascade->ensembleClassifier->learn(&detectorCascade->windows[TLD_WINDOW_SIZE * idx], true, &detectionResult->featureVectors[detectorCascade->numTrees * idx]);
        }

        for (size_t i = 0; i < negativeIndicesForNN.size(); i++)
        {
            int idx = negativeIndicesForNN.at(i);

            NormalizedPatch patch;
            tldExtractNormalizedPatchBB(currImg, &detectorCascade->windows[TLD_WINDOW_SIZE * idx], patch.values);
            patch.positive = 0;
            patches.push_back(patch);
        }

        detectorCascade->nnClassifier->learn(patches);

        //cout << "NN has now " << detectorCascade->nnClassifier->truePositives->size() << " positives and " << detectorCascade->nnClassifier->falsePositives->size() << " negatives.\n";

        delete[] overlap;
    }

    inline void TLD::deleteCurrentBB()
    {
        if (currBB)
        {
            delete currBB;
            currBB = NULL;
        }
    }
} /* namespace tld */
