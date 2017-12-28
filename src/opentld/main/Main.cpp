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
 * MainX.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Georg Nebehay
 */

#include "Main.h"

#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"
#include "TLDUtil.h"
#include "Trajectory.h"
#include "opencv2/imgproc/imgproc.hpp"

using namespace tld;
using namespace cv;

void Main::doWork()
{
    Trajectory trajectory;
    IplImage *img = imAcqGetImg(imAcq);
    Mat colorImage = cvarrToMat(img, true);

    if (colorImage.channels() == 1)
        cv::cvtColor(colorImage, colorImage, cv::COLOR_GRAY2BGR);

    if (showTrajectory)
    {
        trajectory.init(trajectoryLength);
    }

    if (selectManually)
    {
        CvRect box;

        if (getBBFromUser(img, box, gui) == PROGRAM_EXIT)
        {
            return;
        }

        if (initialBB == NULL)
        {
            initialBB = new int[4];
        }

        initialBB[0] = box.x;
        initialBB[1] = box.y;
        initialBB[2] = box.width;
        initialBB[3] = box.height;
    }

    FILE *resultsFile = NULL;

    if (printResults != NULL)
    {
        resultsFile = fopen(printResults, "w");
        if (!resultsFile)
        {
            fprintf(stderr, "Error: Unable to create results-file \"%s\"\n", printResults);
            exit(-1);
        }
    }

    bool reuseFrameOnce = false;
    bool skipProcessingOnce = false;
    bool paused = false;
    bool step = false;
    double tic = 0;
    double toc = 0;

    if (initialBB != NULL)
    {
        Rect bb = tldArrayToRect(initialBB);

        printf("Starting at %d %d %d %d\n", bb.x, bb.y, bb.width, bb.height);
        tic = static_cast<double>(getTickCount());
        tld->selectObject(colorImage, &bb);
        toc = getTickCount() - tic;
        skipProcessingOnce = true;
        reuseFrameOnce = true;
    }

    while (imAcqHasMoreFrames(imAcq))
    {
        if (!reuseFrameOnce && (!paused || step))
        {
            cvReleaseImage(&img);
            img = imAcqGetImg(imAcq);
            colorImage = cvarrToMat(img, true);

            if (colorImage.channels() == 1)
                cv::cvtColor(colorImage, colorImage, cv::COLOR_GRAY2BGR);

            if (img == NULL)
            {
                printf("current image is NULL, assuming end of input.\n");
                break;
            }
        }

        if (!skipProcessingOnce && (!paused || step))
        {
            tic = static_cast<double>(getTickCount());
            tld->processImage(colorImage);
            toc = getTickCount() - tic;
        }
        else
        {
            skipProcessingOnce = false;
        }

        float fps = static_cast<float>(getTickFrequency() / toc);

        if (printResults != NULL)
        {
            if (tld->currBB != NULL)
            {
                fprintf(resultsFile, "%d, %.2d, %.2d, %.2d, %.2d, %f, %f\n", imAcq->currentFrame - 1,
                    tld->currBB->x, tld->currBB->y, tld->currBB->width, tld->currBB->height, tld->currConf,
                    fps);
            }
            else
            {
                fprintf(resultsFile, "%d, NaN, NaN, NaN, NaN, NaN, %f\n", imAcq->currentFrame - 1, fps);
            }
        }

        if (showOutput || saveDir != NULL)
        {
            char string[128];
            char learningString[10] = "";

            if (paused && step)
                step = false;

            if (tld->learning)
            {
                strcpy(learningString, "Learning");
            }

            sprintf(string, "#%d, fps: %.2f, #numwindows:%d, %s", imAcq->currentFrame - 1,
                fps, tld->detectorCascade->numWindows, learningString);
            CvScalar yellow = CV_RGB(255, 255, 0);
            CvScalar blue = CV_RGB(0, 0, 255);
            CvScalar black = CV_RGB(0, 0, 0);
            CvScalar white = CV_RGB(255, 255, 255);
            CvScalar red = CV_RGB(255, 0, 0);

            if (tld->currBB != NULL)
            {
                CvScalar rectangleColor = red;
                cvRectangle(img, tld->currBB->tl(), tld->currBB->br(), rectangleColor, 2, 8, 0);
            }

            CvFont font;
            cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 2, 8);
            //cvRectangle(img, cvPoint(0, 0), cvPoint(img->width, 50), black, CV_FILLED, 8, 0);
            cvPutText(img, string, cvPoint(25, 25), &font, red);

            if (showOutput)
            {
                gui->showImage(img);
                char key = gui->getKey();

                if (key == 'q')
                    break;

                if (key == 'p')
                    paused = !paused;

                if (paused && key == 's')
                    step = true;

                if (key == 'c')
                {
                    //clear everything
                    tld->release();
                }

                if (key == 'l')
                {
                    tld->learningEnabled = !tld->learningEnabled;
                    printf("LearningEnabled: %d\n", tld->learningEnabled);
                }

                if (key == 'a')
                {
                    tld->alternating = !tld->alternating;
                    printf("alternating: %d\n", tld->alternating);
                }

                if (key == 'r')
                {
                    CvRect box;

                    if (getBBFromUser(img, box, gui) == PROGRAM_EXIT)
                    {
                        break;
                    }

                    Rect r = Rect(box);
                    tld->selectObject(colorImage, &r);
                }
            }

            if (saveDir != NULL)
            {
                char fileName[256];
                sprintf(fileName, "%s/%.5d.png", saveDir, imAcq->currentFrame - 1);

                cvSaveImage(fileName, img);
            }
        }

        if (reuseFrameOnce)
        {
            reuseFrameOnce = false;
        }
    }

    cvReleaseImage(&img);
    img = NULL;

    if (resultsFile)
    {
        fclose(resultsFile);
    }
}
