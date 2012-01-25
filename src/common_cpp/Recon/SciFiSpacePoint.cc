/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "src/common_cpp/Recon/SciFiSpacePoint.hh"

SciFiSpacePoint::SciFiSpacePoint() {
  _tracker = -1;
}

SciFiSpacePoint::SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2, SciFiCluster *clust3) {
  make_triplet(clust1, clust2, clust3);
  _type = "triplet";
  _channels.push_back(clust1);
  _channels.push_back(clust2);
  _channels.push_back(clust3);
  _tracker = -1;
}

SciFiSpacePoint::SciFiSpacePoint(SciFiCluster *clust1, SciFiCluster *clust2) {
  make_duplet(clust1, clust2);
  _type = "duplet";
  _channels.push_back(clust1);
  _channels.push_back(clust2);
  _tracker = -1;
}

SciFiSpacePoint::~SciFiSpacePoint() {}


// Given 3 input clusters, this function computes all the triplet variables,
// like position and respective standard deviation
void SciFiSpacePoint::make_triplet(SciFiCluster* vcluster,
                                            SciFiCluster* xcluster,
                                            SciFiCluster* wcluster) {
  // Set clusters as used
  vcluster->setUsed();
  xcluster->setUsed();
  wcluster->setUsed();
/*
  channels = Json::Value(Json::arrayValue);
  channels.clear();
  Json::Value one;
  Json::Value two;
  Json::Value three;
  assert((*vcluster).isMember("ChanNoW"));
  assert((*vcluster).isMember("plane"));

  one["plane_number"]    =  (*vcluster)["plane"].asInt();
  one["channel_number"]  =  (*vcluster)["ChanNoW"].asDouble();
  one["npe"]             =  (*vcluster)["nPE"].asDouble();
  two["plane_number"]    =  (*xcluster)["plane"].asInt();
  two["channel_number"]  =  (*xcluster)["ChanNoW"].asDouble();
  two["npe"]             =  (*xcluster)["nPE"].asDouble();
  three["plane_number"]  =  (*wcluster)["plane"].asInt();
  three["channel_number"]=  (*wcluster)["ChanNoW"].asDouble();
  three["npe"]           =  (*wcluster)["nPE"].asDouble();

  channels.append(one);
  channels.append(two);
  channels.append(three);
*/
  _npe = vcluster->get_npe()+xcluster->get_npe()+wcluster->get_npe();

  Hep3Vector p1 = crossing_pos(vcluster, xcluster);
  Hep3Vector p2 = crossing_pos(vcluster, wcluster);
  Hep3Vector p3 = crossing_pos(xcluster, wcluster);

  // This is the position of the space-point
  _position = (p1+p2+p3)/3.0;

  // Vector p stores the crossing position of views v and w.
  Hep3Vector p = p2;

  // Now, determine the perpendicular distance from the hit on the X view
  // to the intersection of the V and W views
  Hep3Vector x_dir(xcluster->get_direction());
  Hep3Vector x_pos(xcluster->get_position());

  // Assume that the station is perpendicular to the Z axis
  // get_chi_squared(x_pos,p);
  double x1 = x_pos.x();
  double y1 = x_pos.y();
  double x2 = x_pos.x() + 10.0*x_dir.x();
  double y2 = x_pos.y() + 10.0*x_dir.y();
  double x0 = p.x();
  double y0 = p.y();

  double dist = ((x2-x1) * (y1-y0) - (x1-x0) * (y2-y1)) /
                 sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));

  _chi2 = (dist*dist)/0.064;

  _tracker = vcluster->get_tracker();
  _station = vcluster->get_station();

  // Determine time
  // get_time(vcluster, xcluster, wcluster);
  double time_A = vcluster->get_time();
  double time_B = xcluster->get_time();
  double time_C = wcluster->get_time();

  _time = (time_A+time_B+time_C)/3.0;
  _time_error = 0.0;
  _time_error += (time_A-_time)*time_A;
  _time_error += (time_B-_time)*time_B;
  _time_error += (time_C-_time)*time_C;
  _time_error = sqrt(_time_error);

  _time_res = time_A - _time;
}

void SciFiSpacePoint::make_duplet
     (SciFiCluster* clusterA, SciFiCluster* clusterB) {
  // Set clusters as used
  // Set clusters as used
  clusterA->setUsed();
  clusterB->setUsed();
/*
  channels = Json::Value(Json::arrayValue);
  channels.clear();
  Json::Value one;
  Json::Value two;
  assert((*clusterA).isMember("ChanNoW"));
  assert((*clusterA).isMember("plane"));

  one["plane_number"]    =  (*clusterA)["plane"].asInt();
  one["channel_number"]  =  (*clusterA)["ChanNoW"].asDouble();
  two["plane_number"]    =  (*clusterB)["plane"].asInt();
  two["channel_number"]  =  (*clusterB)["ChanNoW"].asDouble();
  channels.append(one);
  channels.append(two);
*/
  _npe = clusterA->get_npe()+clusterB->get_npe();

  Hep3Vector p1 = crossing_pos(clusterA, clusterB);

  // This is the position of the space-point!
  _position = p1;

  Hep3Vector p;

  // Because it's a doublet, we can't
  // evaluate the goodness of the spacepoint
  _chi2 = -1;

  _tracker = clusterA->get_tracker();
  _station = clusterA->get_station();

  // Determine time
  double time_A = clusterA->get_time();
  double time_B = clusterB->get_time();

  _time = (time_A+time_B)/2.0;
  _time_error = 0.0;
  _time_error += (time_A-_time)*time_A;
  _time_error += (time_B-_time)*time_B;
  _time_error = sqrt(_time_error);

  _time_res = time_A - _time;
}

// This function calculates the intersection position of two clusters.
// The position of a space-point will be the mean
// of all 3 possible intersections.
Hep3Vector SciFiSpacePoint::crossing_pos(SciFiCluster* c1,
                                         SciFiCluster* c2) {
    Hep3Vector d1 = c1->get_direction();

    Hep3Vector d2 = c2->get_direction();

    Hep3Vector c1_pos(c1->get_position());

    Hep3Vector c2_pos(c2->get_position());

    CLHEP::HepMatrix m1(3, 3, 0);

    m1[0][0] = (c2_pos-c1_pos).x();
    m1[0][1] = (c2_pos-c1_pos).y();
    m1[0][2] = (c2_pos-c1_pos).z();

    m1[1][0] = d2.x();
    m1[1][1] = d2.y();
    m1[1][2] = d2.z();

    m1[2][0] = (d1.cross(d2)).x();
    m1[2][1] = (d1.cross(d2)).y();
    m1[2][2] = (d1.cross(d2)).z();

    CLHEP::HepMatrix m2(3, 3, 0);

    m2[0][0] = (c2_pos-c1_pos).x();
    m2[0][1] = (c2_pos-c1_pos).y();
    m2[0][2] = (c2_pos-c1_pos).z();

    m2[1][0] = d1.x();
    m2[1][1] = d1.y();
    m2[1][2] = d1.z();

    m2[2][0] = (d1.cross(d2)).x();
    m2[2][1] = (d1.cross(d2)).y();
    m2[2][2] = (d1.cross(d2)).z();

    double t1 = m1.determinant() / pow((d1.cross(d2)).mag(), 2.);
    double t2 = m2.determinant() / pow((d1.cross(d2)).mag(), 2.);

    Hep3Vector p1 = c1_pos+t1*d1;
    Hep3Vector p2 = c2_pos+t2*d2;

    Hep3Vector an_intersection = (p1+p2)/2.;

    return an_intersection;
}
