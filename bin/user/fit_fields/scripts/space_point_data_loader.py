import ROOT
import libMausCpp

class SpacePointDataLoader(object):
    def __init__(self, config):
        self.config = config
        self.tof_cut_count = 0
        self.scifi_cluster_cut_count = 0
        self.scifi_space_point_cut_count = 0
        self.event_count = 0
        self.accepted_count = 0
        self.events = []

    def load_data(self, max_number_of_events):
        file_name = self.config.reco_file
        print "Loading ROOT file", file_name
        root_file = ROOT.TFile(file_name, "READ") # pylint: disable = E1101

        print "Setting up data tree"
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)

        print "Getting some data"
        for i in range(tree.GetEntries()):
            if i % 100 == 0 or i == tree.GetEntries()-1 or i == 0:
                print "Spill", i+1, "/", tree.GetEntries(),
                print "ev:", self.event_count, "tof:", self.tof_cut_count,
                print "clusters:", self.scifi_cluster_cut_count,
                print "space points:", self.scifi_space_point_cut_count,
                print "accepted:", self.accepted_count
            tree.GetEntry(i)
            spill = data.GetSpill()
            if spill.GetDaqEventType() == "physics_event":
                for reco_event in spill.GetReconEvents():
                    self.event_count += 1
                    event = self.load_reco_event(reco_event)
                    if event != None:
                        event["spill"] = spill.GetSpillNumber()
                        self.accepted_count += 1
                        self.events.append(event)
                    tof_sp = reco_event.GetTOFEvent().GetTOFEventSpacePoint()
                    try:
                        tof1 = tof_sp.GetTOF1SpacePointArray()[0].GetTime()
                        tof2 = tof_sp.GetTOF2SpacePointArray()[0].GetTime()
                    except Exception:
                        pass

            if len(self.events) >= max_number_of_events:
                break

    def load_tof_event(self, tof_event):
        space_points = tof_event.GetTOFEventSpacePoint()
        if self.will_cut_on_tof(tof_event):
            return None
        tof1_sp = space_points.GetTOF1SpacePointArray()[0]
        tof1 = {
            "x":tof1_sp.GetGlobalPosX(),
            "y":tof1_sp.GetGlobalPosY(),
            "z":tof1_sp.GetGlobalPosZ(),
            "t":tof1_sp.GetTime(),
            "detector":"tof1"
        }
        tof2_sp = space_points.GetTOF2SpacePointArray()[0]
        tof2 = {
            "x":tof2_sp.GetGlobalPosX(),
            "y":tof2_sp.GetGlobalPosY(),
            "z":tof2_sp.GetGlobalPosZ(),
            "t":tof2_sp.GetTime(),
            "detector":"tof2"
        }
        delta_t = tof2_sp.GetTime() - tof1_sp.GetTime()
        return [tof1, tof2]

    def will_cut_on_tof(self, tof_event):
        if self.config.will_require_tof12 == False:
            return False
        space_points = tof_event.GetTOFEventSpacePointPtr()
        if space_points.GetTOF1SpacePointArray().size() != 1:
            self.tof_cut_count += 1
            return True
        if space_points.GetTOF2SpacePointArray().size() != 1:
            self.tof_cut_count += 1
            return True
        
        return False

    def load_scifi_event(self, scifi_event):
        space_points = scifi_event.spacepoints()
        if self.will_cut_on_scifi_clusters(scifi_event):
            return None
        if self.will_cut_on_scifi_space_points(scifi_event):
            return None
        space_points_out = []
        for space_point in scifi_event.spacepoints():
            # if we require triplets and space point is not a triplet, skip it
            if self.config.will_require_triplets and \
                space_point.get_channels().GetEntries() != 3:
                continue
            position = space_point.get_global_position()
            if space_point.get_tracker() == 0:
                space_points_out.append({
                  "x":position.x(),
                  "y":position.y(),
                  "z":position.z(),
                  "t":None,
                  "detector":"tku"
                })
            else:
                space_points_out.append({
                  "x":position.x(),
                  "y":position.y(),
                  "z":position.z(),
                  "t":None,
                  "detector":"tkd"
                })
        space_points_out = sorted(space_points_out, key = lambda sp: sp["z"])
        return space_points_out

    def will_cut_on_scifi_clusters(self, scifi_event):
        """Require exactly one cluster in each plane"""
        if self.config.will_require_clusters == False:
            return False
        n_clusters = [[[0 for k in range(3)] for j in range(5)] for i in range(2)]
        for cluster in scifi_event.clusters():
            tracker = cluster.get_tracker()
            station = cluster.get_station()-1
            plane = cluster.get_plane()
            n_clusters[tracker][station][plane] += 1
        print n_clusters[1]
        for i, tracker_cluster in enumerate(n_clusters):
            for station_cluster in tracker_cluster:
                if station_cluster != [1, 1, 1]:
                    self.scifi_cluster_cut_count += 1
                    return True
        return False

    def will_cut_on_scifi_space_points(self, scifi_event):
        """Require exactly one space point in each station"""
        if self.config.will_require_space_points == False:
            return False
        n_space_points = [[0 for j in range(5)] for i in range(2)]
        for space_point in scifi_event.spacepoints():
            tracker = space_point.get_tracker()
            station = space_point.get_station()-1
            if not self.config.will_require_triplets:
                n_space_points[tracker][station] += 1
            # require triplet space point
            elif space_point.get_channels().GetEntries() == 3:
                n_space_points[tracker][station] += 1
        #print n_space_points
        for i, tracker_space_point in enumerate(n_space_points):
            if i in self.config.required_trackers:
                if tracker_space_point != [1, 1, 1, 1, 1]:
                    self.scifi_space_point_cut_count += 1
                    return True
        return False

    def load_reco_event(self, reco_event):
        tof_event = reco_event.GetTOFEvent()
        tof_sp = reco_event.GetTOFEvent().GetTOFEventSpacePoint()
        try:
            tof1 = tof_sp.GetTOF1SpacePointArray()[0].GetTime()
            tof2 = tof_sp.GetTOF2SpacePointArray()[0].GetTime()
        except Exception:
            pass
        scifi_event = reco_event.GetSciFiEvent()
        tof_loaded = self.load_tof_event(tof_event)
        if tof_loaded == None:
            return None
        scifi_loaded = self.load_scifi_event(scifi_event)
        if scifi_loaded == None:
            return None
        event = {"data":scifi_loaded+tof_loaded}
        event["particle_number"] = reco_event.GetPartEventNumber()
        return event


