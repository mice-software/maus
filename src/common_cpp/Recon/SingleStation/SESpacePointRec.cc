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

#include "src/common_cpp/Recon/SingleStation/SESpacePointRec.hh"

// namespace MAUS {

SESpacePointRec::SESpacePointRec() {}

SESpacePointRec::~SESpacePointRec() {}

void SESpacePointRec::process(SEEvent &evt) {
  int tracker, station, plane;
  int clusters_size = evt.clusters().size();
  // Store clusters in a vector.
  std::vector<SECluster*> clusters[3];
  for ( int cl = 0; cl < clusters_size; cl++ ) {
    SECluster* a_cluster = evt.clusters()[cl];
    plane   = a_cluster->get_plane();
    clusters[plane].push_back(a_cluster);
  }

  // Make all possible combinations of doublet
  // clusters from each of the 3 views...
  // looping over all clusters in plane 0 (view v)
  int numb_clusters_in_v = clusters[0].size();
  int numb_clusters_in_w = clusters[1].size();
  int numb_clusters_in_u = clusters[2].size();
  for ( int cla = 0; cla < numb_clusters_in_v; cla++ ) {
    SECluster* candidate_A = (clusters[0])[cla];

    // Looping over all clusters in plane 1 (view w)
    for ( int clb = 0; clb < numb_clusters_in_w; clb++ ) {
      SECluster* candidate_B = (clusters[1])[clb];

      // Looping over all clusters in plane 2 (view u)
      for ( int clc = 0; clc < numb_clusters_in_u; clc++ ) {
        SECluster* candidate_C = (clusters[2])[clc];

        if ( kuno_accepts(candidate_A, candidate_B, candidate_C) &&
             clusters_are_not_used(candidate_A, candidate_B, candidate_C) ) {
          SESpacePoint* triplet = new SESpacePoint(candidate_A, candidate_B, candidate_C);
          build_triplet(triplet);
          evt.add_spacepoint(triplet);
        }
      }  // ends plane 2
    }  // ends plane 1
  }  // ends plane 0

  // Run over left-overs and make duplets without any selection criteria
  for ( int a_plane = 0; a_plane < 2; a_plane++ ) {
    for ( int another_plane = a_plane+1; another_plane < 3; another_plane++ ) {
      // Make all possible combinations of doublet clusters from views 0 & 1
      // looping over all clusters in view 0, then 1
      for ( unsigned int cla = 0;
            cla < clusters[a_plane].size(); cla++ ) {
      SECluster* candidate_A =
                      (clusters[a_plane])[cla];

        // Looping over all clusters in view 1,2, then 2
        for ( unsigned int clb = 0;
              clb < clusters[another_plane].size();
              clb++ ) {
          SECluster* candidate_B =
                       (clusters[another_plane])[clb];

          if ( clusters_are_not_used(candidate_A, candidate_B) &&
               candidate_A->get_plane() != candidate_B->get_plane() &&
               duplet_within_radius(candidate_A, candidate_B) ) {
            SESpacePoint* duplet = new SESpacePoint(candidate_A, candidate_B);
            build_duplet(duplet);
            evt.add_spacepoint(duplet);
          }
        }
      }
    }
  }
}

bool SESpacePointRec::duplet_within_radius(SECluster* candidate_A,
                                           SECluster* candidate_B) {
  Hep3Vector pos = crossing_pos(candidate_A, candidate_B);
  double radius = pow(pow(pos.x(), 2.0)+pow(pos.y(), 2.0), 0.5);
  if ( radius < _acceptable_radius ) {
    return true;
  } else {
    return false;
  }
}

bool SESpacePointRec::kuno_accepts(SECluster* cluster1,
                                   SECluster* cluster2,
                                   SECluster* cluster3) {
  // The 3 clusters passed to the kuno_accepts function belong
  // to the same station, only the planes are different
  double uvwSum = cluster1->get_channel() +
                  cluster2->get_channel() +
                  cluster3->get_channel();

  if (    (uvwSum < (_kuno+_kuno_toler))
       && (uvwSum > (_kuno-_kuno_toler)) ) {
    return true;
  } else {
    return false;
  }
}

bool SESpacePointRec::clusters_are_not_used(SECluster* candidate_A,
                                            SECluster* candidate_B) {
  if ( candidate_A->is_used() || candidate_B->is_used() ) {
    return false;
  } else {
    return true;
  }
}

bool SESpacePointRec::clusters_are_not_used(SECluster* candidate_A,
                                            SECluster* candidate_B,
                                            SECluster* candidate_C) {
  if ( candidate_A->is_used() || candidate_B->is_used() || candidate_C->is_used() ) {
    return false;
  } else {
    return true;
  }
}

// Given 3 input clusters, this function computes all the triplet variables,
// like position and respective standard deviation
void SESpacePointRec::build_triplet(SESpacePoint* triplet) {
  std::vector<SECluster*> channels = triplet->get_channels();
  SECluster *xcluster = channels[0];
  SECluster *wcluster = channels[1];
  SECluster *vcluster = channels[2];

  // This is the position of the space-point
  Hep3Vector p1 = crossing_pos(vcluster, xcluster);
  Hep3Vector p2 = crossing_pos(vcluster, wcluster);
  Hep3Vector p3 = crossing_pos(xcluster, wcluster);
  Hep3Vector position = (p1+p2+p3)/3.0;
  triplet->set_position(position);

  // Vector p stores the crossing position of views v and w.
  Hep3Vector p(p2);

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

  double chi2 = (dist*dist)/0.064;
  triplet->set_chi2(chi2);
/*
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
  _time_res = time_A - _time;*/
}

void SESpacePointRec::build_duplet(SESpacePoint* duplet) {
  std::vector<SECluster*> channels = duplet->get_channels();
  SECluster *clusterA = channels[0];
  SECluster *clusterB = channels[1];

  Hep3Vector p1 = crossing_pos(clusterA, clusterB);

  // This is the position of the space-point.
  Hep3Vector position(p1);
  duplet->set_position(position);

/*
  // Determine time
  double time_A = clusterA->get_time();
  double time_B = clusterB->get_time();
  _time = (time_A+time_B)/2.0;
  _time_error = 0.0;
  _time_error += (time_A-_time)*time_A;
  _time_error += (time_B-_time)*time_B;
  _time_error = sqrt(_time_error);
  _time_res = time_A - _time;*/
}

// This function calculates the intersection position of two clusters.
// The position of a space-point will be the mean
// of all 3 possible intersections.
Hep3Vector SESpacePointRec::crossing_pos(SECluster* c1,
                                         SECluster* c2) {
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
// } // ~namespace MAUS
