#ifndef FRONTIER_SEARCH_H_
#define FRONTIER_SEARCH_H_

#include "nav2_costmap_2d/costmap_2d_ros.hpp"
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp> 

namespace frontier_exploration
{
/**
 * @brief Represents a frontier
 *
 */
struct Frontier {
  std::uint32_t size;
  double min_distance;
  double cost;
  geometry_msgs::msg::Point initial;
  geometry_msgs::msg::Point centroid;
  geometry_msgs::msg::Point middle;
  std::vector<geometry_msgs::msg::Point> points;
};

/**
 * @brief Thread-safe implementation of a frontier-search task for an input
 * costmap.
 */
class FrontierSearch
{
public:
  FrontierSearch() : logger_(rclcpp::get_logger("frontier_search")) {} 

  /**
   * @brief Constructor for search task
   * @param costmap Reference to costmap data to search.
   */
  FrontierSearch(nav2_costmap_2d::Costmap2D* costmap, double potential_scale,
                 double gain_scale, double orientation_scale, // <-- ADDED: Pass the hysteresis weight
                 double min_frontier_size, rclcpp::Logger logger);

  /**
   * @brief Runs search implementation, outward from the start position
   * @param pose Initial pose to search from
   * @return List of frontiers, if any
   */
  std::vector<Frontier> searchFrom(geometry_msgs::msg::Pose pose); // <-- CHANGED: Accept full Pose instead of just Point

protected:
  /**
   * @brief Starting from an initial cell, build a frontier from valid adjacent
   * cells
   * @param initial_cell Index of cell to start frontier building
   * @param reference Reference index to calculate position from
   * @param frontier_flag Flag vector indicating which cells are already marked
   * as frontiers
   * @return new frontier
   */
  Frontier buildNewFrontier(unsigned int initial_cell, unsigned int reference,
                            std::vector<bool>& frontier_flag);

  /**
   * @brief isNewFrontierCell Evaluate if candidate cell is a valid candidate
   * for a new frontier.
   * @param idx Index of candidate cell
   * @param frontier_flag Flag vector indicating which cells are already marked
   * as frontiers
   * @return true if the cell is frontier cell
   */
  bool isNewFrontierCell(unsigned int idx,
                         const std::vector<bool>& frontier_flag);

  /**
   * @brief computes frontier cost
   * @details cost function is defined by potential_scale and gain_scale
   *
   * @param frontier frontier for which compute the cost
   * @return cost of the frontier
   */
  double frontierCost(const Frontier& frontier);

private:
  nav2_costmap_2d::Costmap2D* costmap_;
  unsigned char* map_;
  unsigned int size_x_, size_y_;
  
  // <-- CHANGED: Added orientation_scale_ to the existing scales
  double potential_scale_, gain_scale_, orientation_scale_; 
  
  double min_frontier_size_;
  rclcpp::Logger logger_;
  
  geometry_msgs::msg::Pose robot_pose_; // <-- ADDED: Stores the robot's pose so frontierCost() can calculate Hysteresis (Yaw)
};
}  // namespace frontier_exploration
#endif