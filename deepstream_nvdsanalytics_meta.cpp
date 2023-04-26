/*
 * Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <gst/gst.h>
#include <glib.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "gstnvdsmeta.h"
#include "nvds_analytics_meta.h"
#include "analytics.h"

/* custom_parse_nvdsanalytics_meta_data
 * and extract nvanalytics metadata */
extern "C" void
analytics_custom_parse_nvdsanalytics_meta_data(NvDsFrameMeta *frame_meta, AnalyticsUserMeta *data)
{	
	NvDsObjectMeta *obj_meta = NULL;
    NvDsMetaList * l_obj = NULL;
	std::stringstream out_string;
	/* Iterate user metadata in frames to search analytics metadata */
	for (NvDsMetaList *l_user = frame_meta->frame_user_meta_list;
		 l_user != NULL; l_user = l_user->next)
	{
		NvDsUserMeta *user_meta = (NvDsUserMeta *)l_user->data;
		if (user_meta->base_meta.meta_type != NVDS_USER_FRAME_META_NVDSANALYTICS)
			continue;

		/* convert to  metadata */
		NvDsAnalyticsFrameMeta *meta =
			(NvDsAnalyticsFrameMeta *)user_meta->user_meta_data;
		/* Get the labels from nvdsanalytics config file */
		for (std::pair<std::string, uint32_t> status : meta->objInROIcnt)
		{
			out_string << " Objs in ROI ";
			out_string << status.first;
			out_string << " = ";
			out_string << status.second;
		}

		// NvDsObjectMeta *meta_obj =
		// 	(NvDsObjectMeta *)user_meta->user_meta_data;
		
		// data->top = meta_obj->rect_params.top;
		// data->left = meta_obj->rect_params.left;
		// data->height = meta_obj->rect_params.height;
		// data->width = meta_obj->rect_params.width;

		// out_string << "Object "<< data->top << "| " << data->left;

	}
	for (l_obj = frame_meta->obj_meta_list; l_obj != NULL;
		 l_obj = l_obj->next)
	{
		obj_meta = (NvDsObjectMeta *)(l_obj->data);
		data->top = obj_meta->rect_params.top;
		data->left = obj_meta->rect_params.left;
		data->height = obj_meta->rect_params.height;
		data->width = obj_meta->rect_params.width;

		out_string << "Object "<< data->top << "| " << data->left;
		// Access attached user meta for each object
		for (NvDsMetaList *l_user_meta = obj_meta->obj_user_meta_list; l_user_meta != NULL;
			 l_user_meta = l_user_meta->next)
		{
			NvDsUserMeta *user_meta = (NvDsUserMeta *)(l_user_meta->data);
			if (user_meta->base_meta.meta_type == NVDS_USER_OBJ_META_NVDSANALYTICS)
			{
				NvDsAnalyticsObjInfo *user_meta_data =
					(NvDsAnalyticsObjInfo *)user_meta->user_meta_data;
				if (user_meta_data->dirStatus.length())
				{
					out_string << " object " << obj_meta->object_id << " is moving in " << user_meta_data->dirStatus;
				}
			}
		}
	}

	if (out_string.str().size())
	{
		g_print("Frame Number = %d of Stream = %d,  %s\n",
				frame_meta->frame_num, frame_meta->pad_index,
				out_string.str().c_str());
	}
}
