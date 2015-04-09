/*
 * This file is part of ALVAR, A Library for Virtual and Augmented Reality.
 *
 * Copyright 2007-2012 VTT Technical Research Centre of Finland
 *
 * Contact: VTT Augmented Reality Team <alvar.info@vtt.fi>
 *          <http://www.vtt.fi/multimedia/alvar.html>
 *
 * ALVAR is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ALVAR; if not, see
 * <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>.
 */

#include "MarkerDetector.h"
#include "rana.h"
#include <android/log.h>
#define  LOG_TAG    "OCV:libnative_activity"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

template class ALVAR_EXPORT alvar::MarkerDetector<alvar::Marker>;
template class ALVAR_EXPORT alvar::MarkerDetector<alvar::MarkerData>;
template class ALVAR_EXPORT alvar::MarkerDetector<alvar::MarkerArtoolkit>;

using namespace std;

namespace alvar {
MarkerDetectorImpl::MarkerDetectorImpl() {
	SetMarkerSize();
	SetOptions();
	labeling = NULL;
}

MarkerDetectorImpl::~MarkerDetectorImpl() {
	if (labeling)
		delete labeling;
}

void MarkerDetectorImpl::TrackMarkersReset() {
	_track_markers_clear();
}

void MarkerDetectorImpl::TrackMarkerAdd(int id, PointDouble corners[4]) {
	Marker *mn = new_M(edge_length, res, margin);
	if (map_edge_length.find(id) != map_edge_length.end()) {
		mn->SetMarkerSize(map_edge_length[id], res, margin);
	}

	mn->SetId(id);
	mn->marker_corners_img.clear();
	mn->marker_corners_img.push_back(corners[0]);
	mn->marker_corners_img.push_back(corners[1]);
	mn->marker_corners_img.push_back(corners[2]);
	mn->marker_corners_img.push_back(corners[3]);
	_track_markers_push_back(mn);
	delete mn;
}

void MarkerDetectorImpl::SetMarkerSize(double _edge_length, int _res,
		double _margin) {
	edge_length = _edge_length;
	res = _res;
	margin = _margin;
	map_edge_length.clear(); // TODO: Should we clear these here?
}

void MarkerDetectorImpl::SetMarkerSizeForId(unsigned long id,
		double _edge_length) {
	map_edge_length[id] = _edge_length;
}

void MarkerDetectorImpl::SetOptions(bool _detect_pose_grayscale) {
	detect_pose_grayscale = _detect_pose_grayscale;
}


// TODO switch to float
bool MarkerDetectorImpl::CropTrack(IplImage *image, Camera *cam, Marker * mark,
		double max_track_error, LabelingMethod labeling_method,
		bool update_pose)
{
	static unsigned int trackCount = 0;

	assert(image->origin == 0);
	// Currently only top-left origin supported
	double error = -1;

	// Swap marker tables
	//_swap_marker_tables();
//	_markers_clear();

	switch (labeling_method) {
	case CVSEQ:

		if (!labeling)
			labeling = new LabelingCvSeq();
		((LabelingCvSeq*) labeling)->SetOptions(detect_pose_grayscale);
		break;
	}

	double diameter = sqrt(max(PointSquaredDistance(mark->marker_corners_img[0], mark->marker_corners_img[2]), PointSquaredDistance(mark->marker_corners_img[1], mark->marker_corners_img[3])));

	int margin = (int)(2.0 * max_track_error * diameter);
	cv::Rect cropROI = ranaCropRectangle(mark->marker_corners_img, margin, image->width, image->height);

	cv::Mat cropedImage = cv::Mat((image))(cropROI);//TODO TOFIX TOFF cv::Mat va iplImage ro cast o in harfash o dorsot konim

	labeling->SetCamera(cam);
	labeling->LabelSquares(cropedImage, cropROI.x, cropROI.y);
	vector<vector<PointDouble> >& blob_corners = labeling->blob_corners;
	IplImage* gray = labeling->gray;

	int orientation;

	LOGD("size of blobs %d", blob_corners.size());
	// When tracking we find the best matching blob and test if it is near enough?
	trackCount++;
//	for (std::size_t ii = 0; ii < _track_markers_size(); ii++) {
		//Marker *mn = _track_markers_at(ii);
		if (mark->GetError(Marker::DECODE_ERROR | Marker::MARGIN_ERROR) > 0)
			return false;//continue; // We track only perfectly decoded markers
		int track_i = -1;
		int track_orientation = 0;
		double track_error = 1e200;
		for (unsigned i = 0; i < blob_corners.size()/*blobs_ret.size()*/; ++i) {
			if (blob_corners[i].empty())
				continue;
			mark->CompareCorners(blob_corners[i], &orientation, &error, cropROI.x, cropROI.y, diameter);
			if (error < track_error) {
				track_i = i;
				track_orientation = orientation;
				track_error = error;
			}
		}
		if (track_error <= max_track_error) {
			mark->SetError(Marker::DECODE_ERROR, 0);
			mark->SetError(Marker::MARGIN_ERROR, 0);
			mark->SetError(Marker::TRACK_ERROR, track_error);
			mark->UpdatePose(blob_corners[track_i], cam, track_orientation,
					update_pose);
			//_markers_push_back(mark);
			//blob_corners[track_i].clear(); // We don't want to handle this again...
			return true;
		}
//	}

	return false;
}

int MarkerDetectorImpl::Detect(IplImage *image, Camera *cam, bool track,
		bool visualize, double max_new_marker_error, double max_track_error,
		LabelingMethod labeling_method, bool update_pose) {

	static unsigned int trackCount = 0;

	assert(image->origin == 0);
	// Currently only top-left origin supported
	double error = -1;

	// Swap marker tables
	_swap_marker_tables();
	_markers_clear();

	switch (labeling_method) {
	case CVSEQ:

		if (!labeling)
			labeling = new LabelingCvSeq();
		((LabelingCvSeq*) labeling)->SetOptions(detect_pose_grayscale);
		break;
	}

	labeling->SetCamera(cam);
	labeling->LabelSquares(image, visualize);
	vector<vector<PointDouble> >& blob_corners = labeling->blob_corners;
	IplImage* gray = labeling->gray;

	int orientation;

	LOGD("size of blobs %d", blob_corners.size());
	// When tracking we find the best matching blob and test if it is near enough?
	trackCount++;
	if (track) {
		for (std::size_t ii = 0; ii < _track_markers_size(); ii++) {
			Marker *mn = _track_markers_at(ii);
			if (mn->GetError(Marker::DECODE_ERROR | Marker::MARGIN_ERROR) > 0)
				continue; // We track only perfectly decoded markers
			int track_i = -1;
			int track_orientation = 0;
			double track_error = 1e200;
			for (unsigned i = 0; i < blob_corners.size()/*blobs_ret.size()*/;
					++i) {
				if (blob_corners[i].empty())
					continue;
				mn->CompareCorners(blob_corners[i], &orientation, &error);
				if (error < track_error) {
					track_i = i;
					track_orientation = orientation;
					track_error = error;
				}
			}
			if (track_error <= max_track_error) {
				mn->SetError(Marker::DECODE_ERROR, 0);
				mn->SetError(Marker::MARGIN_ERROR, 0);
				mn->SetError(Marker::TRACK_ERROR, track_error);
				mn->UpdatePose(blob_corners[track_i], cam, track_orientation,
						update_pose);
				_markers_push_back(mn);
				blob_corners[track_i].clear(); // We don't want to handle this again...
				if (visualize)
					mn->Visualize(image, cam, CV_RGB(255,255,0));
			}
		}
	}

	// Now we go through the rest of the blobs -- in case there are new markers...
	if (trackCount > 30) {
		trackCount = 0;
		return (int) _markers_size();
	}

	for (std::size_t i = 0; i < blob_corners.size(); ++i) {
		if (blob_corners[i].empty())
			continue;

		Marker *mn = new_M(edge_length, res, margin);
		if (mn->UpdateContent(blob_corners[i], gray, cam)
				&& mn->DecodeContent(&orientation)
				&& (mn->GetError(Marker::MARGIN_ERROR | Marker::DECODE_ERROR)
						<= max_new_marker_error)) {
			if (map_edge_length.find(mn->GetId()) != map_edge_length.end()) {
				mn->SetMarkerSize(map_edge_length[mn->GetId()], res, margin);
			}
			mn->UpdatePose(blob_corners[i], cam, orientation, update_pose);
			_markers_push_back(mn);

			if (visualize)
				mn->Visualize(image, cam, CV_RGB(255,0,0));
		}
		delete mn;
	}

	return (int) _markers_size();
}

int MarkerDetectorImpl::DetectAdditional(IplImage *image, Camera *cam,
		bool visualize, double max_track_error) {
	assert(image->origin == 0);
	// Currently only top-left origin supported
	if (!labeling)
		return -1;
	double error = -1;
	int orientation;
	int count = 0;
	vector<vector<PointDouble> >& blob_corners = labeling->blob_corners;

	for (std::size_t ii = 0; ii < _track_markers_size(); ii++) {
		Marker *mn = _track_markers_at(ii);
		if (mn->GetError(Marker::DECODE_ERROR | Marker::MARGIN_ERROR) > 0)
			continue; // We track only perfectly decoded markers
		int track_i = -1;
		int track_orientation = 0;
		double track_error = 1e200;
		for (unsigned i = 0; i < blob_corners.size(); ++i) {
			if (blob_corners[i].empty())
				continue;
			mn->CompareCorners(blob_corners[i], &orientation, &error);
			if (error < track_error) {
				track_i = i;
				track_orientation = orientation;
				track_error = error;
			}
		}
		if (track_error <= max_track_error) {
			mn->SetError(Marker::DECODE_ERROR, 0);
			mn->SetError(Marker::MARGIN_ERROR, 0);
			mn->SetError(Marker::TRACK_ERROR, track_error);
			mn->UpdatePose(blob_corners[track_i], cam, track_orientation);
			_markers_push_back(mn);
			count++;
			blob_corners[track_i].clear(); // We don't want to handle this again...

			if (visualize) {
				mn->Visualize(image, cam, CV_RGB(0,255,255));
			}
		}
	}
	return count;
}

}
