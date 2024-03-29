/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2013, SRI International
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of SRI International nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Sachin Chitta, Dave Coleman, Mike Lautman */

#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>

#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>

#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>

#include <moveit_visual_tools/moveit_visual_tools.h>

#include <geometry_msgs/PoseStamped.h>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "learning");
  ros::NodeHandle node_handle;
  ros::AsyncSpinner spinner(1);
  spinner.start();

  // BEGIN_TUTORIAL
  //
  // Setup
  // ^^^^^
  //
  // MoveIt! operates on sets of joints called "planning groups" and stores them in an object called
  // the `JointModelGroup`. Throughout MoveIt! the terms "planning group" and "joint model group"
  // are used interchangably.
  static const std::string PLANNING_GROUP = "manipulator";

  // The :move_group_interface:`MoveGroup` class can be easily
  // setup using just the name of the planning group you would like to control and plan for.
  moveit::planning_interface::MoveGroupInterface move_group(PLANNING_GROUP);

  // We will use the :planning_scene_interface:`PlanningSceneInterface`
  // class to add and remove collision objects in our "virtual world" scene
  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

  // Raw pointers are frequently used to refer to the planning group for improved performance.
  const robot_state::JointModelGroup *joint_model_group =
      move_group.getCurrentState()->getJointModelGroup(PLANNING_GROUP);

  // Visualization
  // ^^^^^^^^^^^^^
  //
  // The package MoveItVisualTools provides many capabilties for visualizing objects, robots,
  // and trajectories in RViz as well as debugging tools such as step-by-step introspection of a script
  namespace rvt = rviz_visual_tools;
  moveit_visual_tools::MoveItVisualTools visual_tools("base_link");
  visual_tools.deleteAllMarkers();

  // Remote control is an introspection tool that allows users to step through a high level script
  // via buttons and keyboard shortcuts in RViz
  visual_tools.loadRemoteControl();

  // RViz provides many types of markers, in this demo we will use text, cylinders, and spheres
  Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
  text_pose.translation().z() = 1.75;
  visual_tools.publishText(text_pose, "MoveGroupInterface Demo", rvt::WHITE, rvt::XLARGE);

  // Batch publishing is used to reduce the number of messages being sent to RViz for large visualizations
  visual_tools.trigger();

  // Getting Basic Information
  // ^^^^^^^^^^^^^^^^^^^^^^^^^
  //
  // We can print the name of the reference frame for this robot.
  ROS_INFO_NAMED("tutorial", "Planning frame: %s", move_group.getPlanningFrame().c_str());

  // We can also print the name of the end-effector link for this group.
  ROS_INFO_NAMED("tutorial", "End effector link: %s", move_group.getEndEffectorLink().c_str());

  // We can get a list of all the groups in the robot:
  ROS_INFO_NAMED("tutorial", "Available Planning Groups:");
  std::copy(move_group.getJointModelGroupNames().begin(), move_group.getJointModelGroupNames().end(),
            std::ostream_iterator<std::string>(std::cout, ", "));

  // ADDING COLLISSION OBJECTS OF WORLD
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  moveit_msgs::CollisionObject object;
  object.header.frame_id = move_group.getPlanningFrame();

  // The id of the object is used to identify it.
  object.id = "Floor";

  // Define a box to add to the world.
  shape_msgs::SolidPrimitive primitive;
  primitive.type = primitive.BOX;
  primitive.dimensions.resize(3);
  primitive.dimensions[0] = 1.47;
  primitive.dimensions[1] = 0.97;
  primitive.dimensions[2] = 0;

  // Define a pose for the box (specified relative to frame_id)
  geometry_msgs::Pose box_pose;
  box_pose.orientation.w = 1.0;
  box_pose.position.x = 0.460; //  Base is ofset by -(0.1470/2-.275)
  box_pose.position.y = 0;
  box_pose.position.z = -0.01;

  object.primitives.push_back(primitive);
  object.primitive_poses.push_back(box_pose);
  object.operation = object.ADD;

  std::vector<moveit_msgs::CollisionObject> collision_objects;
  collision_objects.push_back(object);

  // The id of the object is used to identify it.
  object.id = "Left Wall";

  // Define a box to add to the world.
  primitive.dimensions[0] = 1.47;
  primitive.dimensions[1] = 0;
  primitive.dimensions[2] = 1.1;

  // Define a pose for the box (specified relative to frame_id
  box_pose.orientation.w = 1.0;
  box_pose.position.x = 0.460;
  box_pose.position.y = 0.485;
  box_pose.position.z = 0.55;

  object.primitives.push_back(primitive);
  object.primitive_poses.push_back(box_pose);
  object.operation = object.ADD;

  collision_objects.push_back(object);

  // The id of the object is used to identify it.
  object.id = "Right Wall";

  // Define a box to add to the world.
  primitive.dimensions[0] = 1.47;
  primitive.dimensions[1] = 0;
  primitive.dimensions[2] = 1.1;

  // Define a pose for the box (specified relative to frame_id
  box_pose.orientation.w = 1.0;
  box_pose.position.x = 0.460;
  box_pose.position.y = -0.485;
  box_pose.position.z = 0.55;

  object.primitives.push_back(primitive);
  object.primitive_poses.push_back(box_pose);
  object.operation = object.ADD;

  collision_objects.push_back(object);

  // The id of the object is used to identify it.
  object.id = "Back Wall";

  // Define a box to add to the world.
  primitive.dimensions[0] = 0;
  primitive.dimensions[1] = 0.97;
  primitive.dimensions[2] = 1.1;

  // Define a pose for the box (specified relative to frame_id
  box_pose.orientation.w = 1.0;
  box_pose.position.y = 0;
  box_pose.position.x = -0.275;
  box_pose.position.z = 0.55;

  object.primitives.push_back(primitive);
  object.primitive_poses.push_back(box_pose);
  object.operation = object.ADD;

  collision_objects.push_back(object);

  // Now, let's add the collision object into the world
  ROS_INFO_NAMED("tutorial", "Add an object into the world");
  planning_scene_interface.addCollisionObjects(collision_objects);

  // Show text in RViz of status
  visual_tools.publishText(text_pose, "Add object", rvt::WHITE, rvt::XLARGE);
  visual_tools.trigger();
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  // MOVE TO UP POSITION
  // ^^^^^^^^^^^^^^^^^^^^^^^^^
  visual_tools.prompt("Moving to starting pose up, press Next to continue");

  move_group.setNamedTarget("up");

  moveit::planning_interface::MoveGroupInterface::Plan my_plan;
  move_group.plan(my_plan);
  move_group.move();
  // ^^^^^^^^^^^^^^^^^^^^^^^^^

  visual_tools.prompt("Attempting plan 1, Press Next to continue");

  // Planning to a Pose goal
  // ^^^^^^^^^^^^^^^^^^^^^^^
  // We can plan a motion for this group to a desired pose for the
  // end-effector.
  bool success = false;
  int trials = 0;
  int MAX_TRIALS = 3;
  geometry_msgs::Pose target_pose1;

  while (trials++ < MAX_TRIALS)
  {

    target_pose1.position.x = 0.25;
    target_pose1.position.y = 0.25;
    target_pose1.position.z = 0.25;
    target_pose1.orientation.x = 0;
    target_pose1.orientation.y = 0;
    target_pose1.orientation.z = 0;
    target_pose1.orientation.w = 1.0;

    move_group.setPoseTarget(target_pose1);

    // Now, we call the planner to compute the plan and visualize it.
    // Note that we are just planning, not asking move_group
    // to actually move the robot.

    success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);

    ROS_INFO_NAMED("tutorial", "Visualizing plan 1 (pose goal)");

    // Visualizing plans
    // ^^^^^^^^^^^^^^^^^
    // We can also visualize the plan as a line with markers in RViz.
    //    ROS_INFO_NAMED("tutorial", "Visualizing plan 1 as trajectory line");
    visual_tools.publishAxisLabeled(target_pose1, "pose1");
    visual_tools.publishText(text_pose, "Pose Goal", rvt::WHITE, rvt::XLARGE);
    visual_tools.publishTrajectoryLine(my_plan.trajectory_, joint_model_group);
    visual_tools.trigger();
    ROS_INFO("Plan Attemp %d of %d : %s", trials, MAX_TRIALS, success ? "SUCCESS" : "FAILED");
    if (success)
    {
      ROS_INFO_STREAM("Succeeded plan, continuing to execute");
      break;
    }
    else if (trials < MAX_TRIALS)
    {
      ROS_WARN_STREAM("Planning failed, reattempting");
    }
    else
    {
      ROS_ERROR_STREAM("Planning failed, consider quitting");
    }
  }

  bool exec_succeed = false;
  trials = 0;

  while (trials++ < MAX_TRIALS)
  {
    exec_succeed = (move_group.move() == moveit::planning_interface::MoveItErrorCode::SUCCESS);
    ROS_INFO("Execution Attemp %d of %d : %s", trials, MAX_TRIALS, exec_succeed ? "SUCCESS" : "FAILED");
    if (exec_succeed)
    {
      ROS_INFO_STREAM("Execution succeeded");
      break;
    }
    else if (trials < MAX_TRIALS)
    {
      ROS_WARN_STREAM("Execution failed, reattempting");
    }
    else
    {
      ROS_ERROR_STREAM("Execution failed, consider quitting");
    }
  }
  success = false;
  trials = 0;
  // Move down in Z axis
  visual_tools.prompt("Attempting plan 2, Press Next to continue");

  while (trials++ < MAX_TRIALS)
  {

    geometry_msgs::PoseStamped current_pose = move_group.getCurrentPose();
    geometry_msgs::PoseStamped new_pose = current_pose;
    new_pose.pose.position.z -= 0.2;

    target_pose1 = new_pose.pose;
    move_group.setPoseTarget(target_pose1);
    success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
    ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (pose goal z down)");

    // Visualizing plans
    // ^^^^^^^^^^^^^^^^^
    // We can also visualize the plan as a line with markers in RViz.
    //    ROS_INFO_NAMED("tutorial", "Visualizing plan 2 as trajectory line");
    visual_tools.publishAxisLabeled(target_pose1, "pose1");
    visual_tools.publishText(text_pose, "Pose Goal", rvt::WHITE, rvt::XLARGE);
    visual_tools.publishTrajectoryLine(my_plan.trajectory_, joint_model_group);
    visual_tools.trigger();
    ROS_INFO("Attemp %d of %d : %s", trials, MAX_TRIALS, success ? "SUCCESS" : "FAILED");
    if (success)
    {
      ROS_INFO_STREAM("Succeeded plan, continuing to execute");
      break;
    }
    else if (trials < MAX_TRIALS)
    {
      ROS_WARN_STREAM("Planning failed, reattempting");
    }
    else
    {
      ROS_ERROR_STREAM("Planning failed, consider quitting");
    }
  }

  exec_succeed = false;
  trials = 0;

  while (trials++ < MAX_TRIALS)
  {
    exec_succeed = (move_group.move() == moveit::planning_interface::MoveItErrorCode::SUCCESS);
    ROS_INFO("Execution Attemp %d of %d : %s", trials, MAX_TRIALS, exec_succeed ? "SUCCESS" : "FAILED");
    if (exec_succeed)
    {
      ROS_INFO_STREAM("Execution succeeded");
      break;
    }
    else if (trials < MAX_TRIALS)
    {
      ROS_WARN_STREAM("Execution failed, reattempting");
    }
    else
    {
      ROS_ERROR_STREAM("Execution failed, consider quitting");
    }
  }

  /*

  // Planning to a joint-space goal
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  //
  // Let's set a joint space goal and move towards it.  This will replace the
  // pose target we set above.
  //
  // To start, we'll create an pointer that references the current robot's state.
  // RobotState is the object that contains all the current position/velocity/acceleration data.
  moveit::core::RobotStatePtr current_state = move_group.getCurrentState();
  //
  // Next get the current set of joint values for the group.
  std::vector<double> joint_group_positions;
  current_state->copyJointGroupPositions(joint_model_group, joint_group_positions);

  // Now, let's modify one of the joints, plan to the new joint space goal and visualize the plan.
  joint_group_positions[0] = -1.0;  // radians
  move_group.setJointValueTarget(joint_group_positions);

  success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
  ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");

  // Visualize the plan in RViz
  visual_tools.deleteAllMarkers();
  visual_tools.publishText(text_pose, "Joint Space Goal", rvt::WHITE, rvt::XLARGE);
  visual_tools.publishTrajectoryLine(my_plan.trajectory_, joint_model_group);
  visual_tools.trigger();
  visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo");

  move_group.move();*/

  ros::shutdown();
  return 0;
}
