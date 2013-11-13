/*
* Copyright (c) 2013, Marcus Liebhardt, Yujin Robot.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Yujin Robot nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Inspired by the openni_tracker by Tim Field and PrimeSense's NiTE 2.0 - Simple Skeleton Sample
 */

//These includes are not in NiTE example
#include <ros/ros.h>
#include <ros/package.h>
#include <tf/transform_broadcaster.h>
#include <kdl/frames.hpp>

//#include <OpenNI.h>
//#include <XnCodecIDs.h>
//#include <XnCppWrapper.h>
#include <NiTE.h>

using std::string;

//xn::Context        g_Context;
//xn::DepthGenerator g_DepthGenerator;
//xn::UserGenerator  g_UserGenerator;

#define MAX_USERS 10
bool g_visibleUsers[MAX_USERS] = {false};
nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};

#define USER_MESSAGE(msg) \
	{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

//XnBool g_bNeedPose   = FALSE;
//XnChar g_strPose[20] = "";
//
//void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie) {
//	ROS_INFO("New User %d", nId);
//
//	if (g_bNeedPose)
//		g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
//	else
//		g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
//}
//
//void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie) {
//	ROS_INFO("Lost user %d", nId);
//}
//
//void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie) {
//	ROS_INFO("Calibration started for user %d", nId);
//}
//
//void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie) {
//	if (bSuccess) {
//		ROS_INFO("Calibration complete, start tracking user %d", nId);
//		g_UserGenerator.GetSkeletonCap().StartTracking(nId);
//	}
//	else {
//		ROS_INFO("Calibration failed for user %d", nId);
//		if (g_bNeedPose)
//			g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
//		else
//			g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
//	}
//}
//
//void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, XnChar const* strPose, XnUserID nId, void* pCookie) {
//    ROS_INFO("Pose %s detected for user %d", strPose, nId);
//    g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
//    g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
//}
//
//void publishTransform(XnUserID const& user, XnSkeletonJoint const& joint, string const& frame_id, string const& child_frame_id) {
//    static tf::TransformBroadcaster br;
//
//    XnSkeletonJointPosition joint_position;
//    g_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(user, joint, joint_position);
//    double x = -joint_position.position.X / 1000.0;
//    double y = joint_position.position.Y / 1000.0;
//    double z = joint_position.position.Z / 1000.0;
//
//    XnSkeletonJointOrientation joint_orientation;
//    g_UserGenerator.GetSkeletonCap().GetSkeletonJointOrientation(user, joint, joint_orientation);
//
//    XnFloat* m = joint_orientation.orientation.elements;
//    KDL::Rotation rotation(m[0], m[1], m[2],
//    					   m[3], m[4], m[5],
//    					   m[6], m[7], m[8]);
//    double qx, qy, qz, qw;
//    rotation.GetQuaternion(qx, qy, qz, qw);
//
//    char child_frame_no[128];
//    snprintf(child_frame_no, sizeof(child_frame_no), "%s_%d", child_frame_id.c_str(), user);
//
//    tf::Transform transform;
//    transform.setOrigin(tf::Vector3(x, y, z));
//    transform.setRotation(tf::Quaternion(qx, -qy, -qz, qw));
//
//    // #4994
//    tf::Transform change_frame;
//    change_frame.setOrigin(tf::Vector3(0, 0, 0));
//    tf::Quaternion frame_rotation;
//    frame_rotation.setEulerZYX(1.5708, 0, 1.5708);
//    change_frame.setRotation(frame_rotation);
//
//    transform = change_frame * transform;
//
//    br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), frame_id, child_frame_no));
//}
//
//void publishTransforms(const std::string& frame_id) {
//    XnUserID users[15];
//    XnUInt16 users_count = 15;
//    g_UserGenerator.GetUsers(users, users_count);
//
//    for (int i = 0; i < users_count; ++i) {
//        XnUserID user = users[i];
//        if (!g_UserGenerator.GetSkeletonCap().IsTracking(user))
//            continue;
//
//
//        publishTransform(user, XN_SKEL_HEAD,           frame_id, "head");
//        publishTransform(user, XN_SKEL_NECK,           frame_id, "neck");
//        publishTransform(user, XN_SKEL_TORSO,          frame_id, "torso");
//
//        publishTransform(user, XN_SKEL_LEFT_SHOULDER,  frame_id, "left_shoulder");
//        publishTransform(user, XN_SKEL_LEFT_ELBOW,     frame_id, "left_elbow");
//        publishTransform(user, XN_SKEL_LEFT_HAND,      frame_id, "left_hand");
//
//        publishTransform(user, XN_SKEL_RIGHT_SHOULDER, frame_id, "right_shoulder");
//        publishTransform(user, XN_SKEL_RIGHT_ELBOW,    frame_id, "right_elbow");
//        publishTransform(user, XN_SKEL_RIGHT_HAND,     frame_id, "right_hand");
//
//        publishTransform(user, XN_SKEL_LEFT_HIP,       frame_id, "left_hip");
//        publishTransform(user, XN_SKEL_LEFT_KNEE,      frame_id, "left_knee");
//        publishTransform(user, XN_SKEL_LEFT_FOOT,      frame_id, "left_foot");
//
//        publishTransform(user, XN_SKEL_RIGHT_HIP,      frame_id, "right_hip");
//        publishTransform(user, XN_SKEL_RIGHT_KNEE,     frame_id, "right_knee");
//        publishTransform(user, XN_SKEL_RIGHT_FOOT,     frame_id, "right_foot");
//    }
//}
//
//#define CHECK_RC(nRetVal, what)										\
//	if (nRetVal != XN_STATUS_OK)									\
//	{																\
//		ROS_ERROR("%s failed: %s", what, xnGetStatusString(nRetVal));\
//		return nRetVal;												\
//	}

void updateUserState(const nite::UserData& user, unsigned long long ts)
{
	if (user.isNew())
		USER_MESSAGE("New")
	else if (user.isVisible() && !g_visibleUsers[user.getId()])
		USER_MESSAGE("Visible")
	else if (!user.isVisible() && g_visibleUsers[user.getId()])
		USER_MESSAGE("Out of Scene")
	else if (user.isLost())
		USER_MESSAGE("Lost")

	g_visibleUsers[user.getId()] = user.isVisible();


	if(g_skeletonStates[user.getId()] != user.getSkeleton().getState())
	{
		switch(g_skeletonStates[user.getId()] = user.getSkeleton().getState())
		{
		case nite::SKELETON_NONE:
			USER_MESSAGE("Stopped tracking.")
			break;
		case nite::SKELETON_CALIBRATING:
			USER_MESSAGE("Calibrating...")
			break;
		case nite::SKELETON_TRACKED:
			USER_MESSAGE("Tracking!")
			break;
		case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
		case nite::SKELETON_CALIBRATION_ERROR_HANDS:
		case nite::SKELETON_CALIBRATION_ERROR_LEGS:
		case nite::SKELETON_CALIBRATION_ERROR_HEAD:
		case nite::SKELETON_CALIBRATION_ERROR_TORSO:
			USER_MESSAGE("Calibration Failed... :-|")
			break;
		}
	}
}

int main(int argc, char **argv) {
//    string configFilename = ros::package::getPath("openni_tracker") + "/openni_tracker.xml";
//    XnStatus nRetVal = g_Context.InitFromXmlFile(configFilename.c_str());
//    CHECK_RC(nRetVal, "InitFromXml");
//
//    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
//    CHECK_RC(nRetVal, "Find depth generator");
//
//	nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
//	if (nRetVal != XN_STATUS_OK) {
//		nRetVal = g_UserGenerator.Create(g_Context);
//		CHECK_RC(nRetVal, "Find user generator");
//	}
//
//	if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON)) {
//		ROS_INFO("Supplied user generator doesn't support skeleton");
//		return 1;
//	}
//
//    XnCallbackHandle hUserCallbacks;
//	g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
//
//	XnCallbackHandle hCalibrationCallbacks;
//	g_UserGenerator.GetSkeletonCap().RegisterCalibrationCallbacks(UserCalibration_CalibrationStart, UserCalibration_CalibrationEnd, NULL, hCalibrationCallbacks);
//
//	if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration()) {
//		g_bNeedPose = TRUE;
//		if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
//			ROS_INFO("Pose required, but not supported");
//			return 1;
//		}
//
//		XnCallbackHandle hPoseCallbacks;
//		g_UserGenerator.GetPoseDetectionCap().RegisterToPoseCallbacks(UserPose_PoseDetected, NULL, NULL, hPoseCallbacks);
//
//		g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
//	}
//
//	g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
//
//	nRetVal = g_Context.StartGeneratingAll();
//	CHECK_RC(nRetVal, "StartGenerating");

    ros::init(argc, argv, "openni_tracker"); //Different than NiTE sample
    ros::NodeHandle nh, nh_priv("~"); //Different than NiTE sample

    nite::UserTracker userTracker;
    nite::Status niteRc;

    nite::NiTE::initialize();

    ros::Rate r(30); //Different than NiTe sample
    std::string frame_id("openni_depth_frame");//Different than NiTe sample
    nh_priv.getParam("camera_frame_id", frame_id);//Different than NiTe sample

	niteRc = userTracker.create();
	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return 3;
	}
	printf("\nStart moving around to get detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");

	nite::UserTrackerFrameRef userTrackerFrame;

	while (ros::ok()) //ros::ok() Replaces !wasKeyboardHit()
	{
		niteRc = userTracker.readFrame(&userTrackerFrame);
		if (niteRc == nite::STATUS_OK) //Slightly different case handling
		{
			const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
			for (int i = 0; i < users.getSize(); ++i)
			{
				const nite::UserData& user = users[i];
				updateUserState(user,userTrackerFrame.getTimestamp());
				if (user.isNew())
				{
					userTracker.startSkeletonTracking(user.getId());
					ROS_INFO_STREAM("Found a new user.");
				}
				else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
				{
					ROS_INFO_STREAM("Now tracking user " << user.getId());
//					const nite::SkeletonJoint& head = user.getSkeleton().getJoint(nite::JOINT_HEAD);
//					if (head.getPositionConfidence() > .5)
//					printf("%d. (%5.2f, %5.2f, %5.2f)\n", user.getId(), head.getPosition().x, head.getPosition().y, head.getPosition().z);
				}
			}
//			publishTransforms(frame_id);
		}
		else
		{
			ROS_WARN_STREAM("Get next frame failed.");
		}

		r.sleep();
	}

	nite::NiTE::shutdown();
	return 0;
}
