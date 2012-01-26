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

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/Utils/CppErrorHandler.hh"
#include "Interface/Squeak.hh"
#include "Interface/Squeal.hh"
#include "Interface/dataCards.hh"

#include "src/reduce/ReduceCppTrackerErrorLog/ReduceCppTrackerErrorLog.hh"


bool ReduceCppTrackerErrorLog::birth(std::string argJsonConfigDocument) {
  // Check if the JSON document can be parsed, else return error only
  _classname = "ReduceCppTrackerErrorLog";
  _filename = "cosmics.root";
  _nSpills = 0;

  project4();
  // JsonCpp setup
  Json::Value configJSON;
  try {
    configJSON = JsonWrapper::StringToJson(argJsonConfigDocument);
    // this will contain the configuration
    return true;
  } catch(Squeal squee) {
    MAUS::CppErrorHandler::getInstance()->HandleSquealNoJson(squee, _classname);
  } catch(std::exception exc) {
    MAUS::CppErrorHandler::getInstance()->HandleStdExcNoJson(exc, _classname);
  }

  return false;
}

std::string  ReduceCppTrackerErrorLog::process(std::string document) {
  //  JsonCpp setup
  Json::FastWriter writer;
  Json::Value root;
  Json::Value xEventType;
  // Check if the JSON document can be parsed, else return error only

  try {
    root = JsonWrapper::StringToJson(document);}
  catch(...) {
    Json::Value errors;
    std::stringstream ss;
    ss << _classname << " says: Failed to parse input document";
    errors["bad_json_document"] = ss.str();
    root["errors"] = errors;
    return writer.write(root);
  }
  try {
    xEventType = JsonWrapper::GetProperty(root,
                                          "daq_event_type",
                                          JsonWrapper::stringValue);

    if (xEventType == "physics_event" && root.isMember("space_points")) {
      // Light Yield plots.

      int n_events = root["space_points"]["tracker1"].size();

      // Loop over events.
      for (int PartEvent = 0; PartEvent < n_events; PartEvent++) {
        Json::Value xPartEventTracker1_SP = GetPartEvent(root, "space_points", "tracker1", PartEvent);
        Json::Value xPartEventTracker2_SP = GetPartEvent(root, "space_points", "tracker2", PartEvent);

        int n_sp_tracker1 = xPartEventTracker1_SP.size();
        int n_sp_tracker2 = xPartEventTracker2_SP.size();

      } // ends loop over particle events
    }
  } catch(Squeal squee) {
     Squeak::mout(Squeak::error) << squee.GetMessage() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleSqueal(root, squee, _classname);
  } catch(std::exception exc) {
    Squeak::mout(Squeak::error) << exc.what() << std::endl;
    root = MAUS::CppErrorHandler::getInstance()->HandleStdExc(root, exc, _classname);
  }

//  TGLabel *event_number = (TGLabel*) gROOT->FindObject("fLabel722");
//  event_number->ChangeText("hello");

  _nSpills++;
  // Display light yield.
  if (!(_nSpills%25)) {
 /*   c1->cd(1);
    _digits.Draw("npe","tracker==1");
    c1->cd(2);
    _digits.Draw("npe","tracker==2");
    c1->Modified();
    c1->Update();*/
    Save();
  }

  return document;
}

bool ReduceCppTrackerErrorLog::death()  {
  Save();
  return true;
}

void ReduceCppTrackerErrorLog::Save() {
  TFile datafile(_filename.c_str(), "recreate" );
  datafile.cd();

  _digits.Write();
  _spacepoints.Write();
  _events.Write();

  datafile.ls();
  datafile.Close();
  Squeak::mout(Squeak::info) << _filename << " is updated." << std::endl;
}

Json::Value ReduceCppTrackerErrorLog::GetPartEvent(Json::Value root, std::string entry_type,
                         std::string detector, int part_event) {
  Json::Value xPartEvent;
  Json::Value xHitDoc = JsonWrapper::GetProperty(root,
                                                 entry_type ,
                                                 JsonWrapper::objectValue);

  xPartEvent = xHitDoc[detector][part_event];
  return xPartEvent;
}

void ReduceCppTrackerErrorLog::project4() {
   // main frame
   TGMainFrame *fMainFrame1586 = new TGMainFrame(0,10,10,kMainFrame | kVerticalFrame);

   // composite frame
   TGCompositeFrame *fCompositeFrame709 = new TGCompositeFrame(fMainFrame1586,486,372,kVerticalFrame);
   fCompositeFrame709->SetLayoutBroken(kTRUE);

   TGFont *ufont;         // will reflect user font changes
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-14-*-*-*-*-*-*-*");

   TGGC   *uGC;           // will reflect user GC changes
   // graphics context changes
   GCValues_t vall710;
   vall710.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall710.fForeground);
   gClient->GetColorByName("#c0c0c0",vall710.fBackground);
   vall710.fFillStyle = kFillSolid;
   vall710.fFont = ufont->GetFontHandle();
   vall710.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall710, kTRUE);
   TGLabel *fLabel710 = new TGLabel(fCompositeFrame709,"SciFi Error Log",uGC->GetGC(),ufont->GetFontStruct());
   fLabel710->SetTextJustify(36);
   fLabel710->SetMargins(0,0,0,0);
   fLabel710->SetWrapLength(-1);
   fCompositeFrame709->AddFrame(fLabel710, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel710->MoveResize(2,2,464,16);

   // "DAQ" group frame
   TGGroupFrame *fGroupFrame711 = new TGGroupFrame(fCompositeFrame709,"DAQ");
   fGroupFrame711->SetLayoutBroken(kTRUE);

   ufont = gClient->GetFont("-*-helvetica-(null)-*-*-0-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall712;
   vall712.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall712.fForeground);
   gClient->GetColorByName("#c0c0c0",vall712.fBackground);
   vall712.fFillStyle = kFillSolid;
   vall712.fFont = ufont->GetFontHandle();
   vall712.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall712, kTRUE);
   TGLabel *fLabel712 = new TGLabel(fGroupFrame711,"Tracker 0 Missing",uGC->GetGC(),ufont->GetFontStruct());
   fLabel712->SetTextJustify(33);
   fLabel712->SetMargins(0,0,0,0);
   fLabel712->SetWrapLength(-1);
   fGroupFrame711->AddFrame(fLabel712, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,34,2));
   fLabel712->MoveResize(10,50,110,14);

   ufont = gClient->GetFont("-*-helvetica-(null)-*-*-0-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall713;
   vall713.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall713.fForeground);
   gClient->GetColorByName("#c0c0c0",vall713.fBackground);
   vall713.fFillStyle = kFillSolid;
   vall713.fFont = ufont->GetFontHandle();
   vall713.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall713, kTRUE);
   TGLabel *fLabel713 = new TGLabel(fGroupFrame711,"Tracker 1 Missing",uGC->GetGC(),ufont->GetFontStruct());
   fLabel713->SetTextJustify(33);
   fLabel713->SetMargins(0,0,0,0);
   fLabel713->SetWrapLength(-1);
   fGroupFrame711->AddFrame(fLabel713, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,40,2));
   fLabel713->MoveResize(10,106,110,14);

   ufont = gClient->GetFont("-*-helvetica-(null)-*-*-0-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall714;
   vall714.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall714.fForeground);
   gClient->GetColorByName("#c0c0c0",vall714.fBackground);
   vall714.fFillStyle = kFillSolid;
   vall714.fFont = ufont->GetFontHandle();
   vall714.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall714, kTRUE);
   TGLabel *fLabel714 = new TGLabel(fGroupFrame711,"Diff numb evts in trackers",uGC->GetGC(),ufont->GetFontStruct());
   fLabel714->SetTextJustify(33);
   fLabel714->SetMargins(0,0,0,0);
   fLabel714->SetWrapLength(-1);
   fGroupFrame711->AddFrame(fLabel714, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,40,2));
   fLabel714->MoveResize(10,162,164,14);

   ufont = gClient->GetFont("-*-helvetica-(null)-*-*-0-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall715;
   vall715.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall715.fForeground);
   gClient->GetColorByName("#c0c0c0",vall715.fBackground);
   vall715.fFillStyle = kFillSolid;
   vall715.fFont = ufont->GetFontHandle();
   vall715.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall715, kTRUE);
   TGLabel *fLabel715 = new TGLabel(fGroupFrame711,"Bank Lenghts Mismatch",uGC->GetGC(),ufont->GetFontStruct());
   fLabel715->SetTextJustify(33);
   fLabel715->SetMargins(0,0,0,0);
   fLabel715->SetWrapLength(-1);
   fGroupFrame711->AddFrame(fLabel715, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,2,40,9));
   fLabel715->MoveResize(10,218,132,14);
   TGLabel *fLabel716 = new TGLabel(fGroupFrame711,"661");
   fLabel716->SetTextJustify(36);
   fLabel716->SetMargins(0,0,0,0);
   fLabel716->SetWrapLength(-1);
   fGroupFrame711->AddFrame(fLabel716, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel716->MoveResize(192,48,28,16);

   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*- 8-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall717;
   vall717.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#666666",vall717.fForeground);
   gClient->GetColorByName("#c0c0c0",vall717.fBackground);
   vall717.fFillStyle = kFillSolid;
   vall717.fFont = ufont->GetFontHandle();
   vall717.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall717, kTRUE);
   TGLabel *fLabel717 = new TGLabel(fGroupFrame711,"% of events",uGC->GetGC(),ufont->GetFontStruct());
   fLabel717->SetTextJustify(36);
   fLabel717->SetMargins(0,0,0,0);
   fLabel717->SetWrapLength(-1);
   fGroupFrame711->AddFrame(fLabel717, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel717->MoveResize(176,24,44,12);
   TGLabel *fLabel718 = new TGLabel(fGroupFrame711,"662");
   fLabel718->SetTextJustify(36);
   fLabel718->SetMargins(0,0,0,0);
   fLabel718->SetWrapLength(-1);
   fGroupFrame711->AddFrame(fLabel718, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel718->MoveResize(192,104,28,16);
   TGLabel *fLabel719 = new TGLabel(fGroupFrame711,"663");
   fLabel719->SetTextJustify(36);
   fLabel719->SetMargins(0,0,0,0);
   fLabel719->SetWrapLength(-1);
   fGroupFrame711->AddFrame(fLabel719, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel719->MoveResize(190,160,32,18);
   TGLabel *fLabel720 = new TGLabel(fGroupFrame711,"664");
   fLabel720->SetTextJustify(36);
   fLabel720->SetMargins(0,0,0,0);
   fLabel720->SetWrapLength(-1);
   fGroupFrame711->AddFrame(fLabel720, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel720->MoveResize(184,216,32,18);

   fGroupFrame711->SetLayoutManager(new TGVerticalLayout(fGroupFrame711));
   fGroupFrame711->Resize(224,268);
   fCompositeFrame709->AddFrame(fGroupFrame711, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fGroupFrame711->MoveResize(8,80,224,268);

   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall721;
   vall721.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall721.fForeground);
   gClient->GetColorByName("#c0c0c0",vall721.fBackground);
   vall721.fFillStyle = kFillSolid;
   vall721.fFont = ufont->GetFontHandle();
   vall721.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall721, kTRUE);
   TGLabel *fLabel721 = new TGLabel(fCompositeFrame709,"Run number: 12345",uGC->GetGC(),ufont->GetFontStruct());
   fLabel721->SetTextJustify(33);
   fLabel721->SetMargins(0,0,0,0);
   fLabel721->SetWrapLength(-1);
   fCompositeFrame709->AddFrame(fLabel721, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel721->MoveResize(8,24,224,18);

   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall722;
   vall722.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall722.fForeground);
   gClient->GetColorByName("#c0c0c0",vall722.fBackground);
   vall722.fFillStyle = kFillSolid;
   vall722.fFont = ufont->GetFontHandle();
   vall722.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall722, kTRUE);
   TGLabel *fLabel722 = new TGLabel(fCompositeFrame709,"Numb events processed: 666",uGC->GetGC(),ufont->GetFontStruct());
   fLabel722->SetTextJustify(33);
   fLabel722->SetMargins(0,0,0,0);
   fLabel722->SetWrapLength(-1);
   fCompositeFrame709->AddFrame(fLabel722, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel722->MoveResize(8,56,224,18);

   // "Reconstruction" group frame
   TGGroupFrame *fGroupFrame723 = new TGGroupFrame(fCompositeFrame709,"Reconstruction");
   fGroupFrame723->SetLayoutBroken(kTRUE);

   ufont = gClient->GetFont("-*-helvetica-(null)-*-*-0-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall724;
   vall724.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall724.fForeground);
   gClient->GetColorByName("#c0c0c0",vall724.fBackground);
   vall724.fFillStyle = kFillSolid;
   vall724.fFont = ufont->GetFontHandle();
   vall724.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall724, kTRUE);
   TGLabel *fLabel724 = new TGLabel(fGroupFrame723,"Massive event",uGC->GetGC(),ufont->GetFontStruct());
   fLabel724->SetTextJustify(33);
   fLabel724->SetMargins(0,0,0,0);
   fLabel724->SetWrapLength(-1);
   fGroupFrame723->AddFrame(fLabel724, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,34,2));
   fLabel724->MoveResize(10,50,110,14);

   ufont = gClient->GetFont("-*-helvetica-(null)-*-*-0-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall725;
   vall725.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall725.fForeground);
   gClient->GetColorByName("#c0c0c0",vall725.fBackground);
   vall725.fFillStyle = kFillSolid;
   vall725.fFont = ufont->GetFontHandle();
   vall725.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall725, kTRUE);
   TGLabel *fLabel725 = new TGLabel(fGroupFrame723,"Tracker 1 empty",uGC->GetGC(),ufont->GetFontStruct());
   fLabel725->SetTextJustify(33);
   fLabel725->SetMargins(0,0,0,0);
   fLabel725->SetWrapLength(-1);
   fGroupFrame723->AddFrame(fLabel725, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,40,2));
   fLabel725->MoveResize(10,106,110,14);

   ufont = gClient->GetFont("-*-helvetica-(null)-*-*-0-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall726;
   vall726.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall726.fForeground);
   gClient->GetColorByName("#c0c0c0",vall726.fBackground);
   vall726.fFillStyle = kFillSolid;
   vall726.fFont = ufont->GetFontHandle();
   vall726.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall726, kTRUE);
   TGLabel *fLabel726 = new TGLabel(fGroupFrame723,"Tracker 2 empty",uGC->GetGC(),ufont->GetFontStruct());
   fLabel726->SetTextJustify(33);
   fLabel726->SetMargins(0,0,0,0);
   fLabel726->SetWrapLength(-1);
   fGroupFrame723->AddFrame(fLabel726, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,40,2));
   fLabel726->MoveResize(10,162,164,14);
   TGLabel *fLabel727 = new TGLabel(fGroupFrame723,"551");
   fLabel727->SetTextJustify(36);
   fLabel727->SetMargins(0,0,0,0);
   fLabel727->SetWrapLength(-1);
   fGroupFrame723->AddFrame(fLabel727, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel727->MoveResize(192,48,28,16);

   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*- 8-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall728;
   vall728.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#666666",vall728.fForeground);
   gClient->GetColorByName("#c0c0c0",vall728.fBackground);
   vall728.fFillStyle = kFillSolid;
   vall728.fFont = ufont->GetFontHandle();
   vall728.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall728, kTRUE);
   TGLabel *fLabel728 = new TGLabel(fGroupFrame723,"% of events",uGC->GetGC(),ufont->GetFontStruct());
   fLabel728->SetTextJustify(36);
   fLabel728->SetMargins(0,0,0,0);
   fLabel728->SetWrapLength(-1);
   fGroupFrame723->AddFrame(fLabel728, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel728->MoveResize(176,24,44,12);
   TGLabel *fLabel729 = new TGLabel(fGroupFrame723,"552");
   fLabel729->SetTextJustify(36);
   fLabel729->SetMargins(0,0,0,0);
   fLabel729->SetWrapLength(-1);
   fGroupFrame723->AddFrame(fLabel729, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel729->MoveResize(192,104,28,16);
   TGLabel *fLabel730 = new TGLabel(fGroupFrame723,"553");
   fLabel730->SetTextJustify(36);
   fLabel730->SetMargins(0,0,0,0);
   fLabel730->SetWrapLength(-1);
   fGroupFrame723->AddFrame(fLabel730, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel730->MoveResize(190,160,32,18);

   fGroupFrame723->SetLayoutManager(new TGVerticalLayout(fGroupFrame723));
   fGroupFrame723->Resize(224,268);
   fCompositeFrame709->AddFrame(fGroupFrame723, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fGroupFrame723->MoveResize(240,80,224,268);
   char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
   std::string fname = std::string(pMAUS_ROOT_DIR)+"/src/reduce/ReduceCppTrackerErrorLog/mice";
   const char * file = fname.c_str();
   TGIcon *fIcon1077 = new TGIcon(fCompositeFrame709,file);
   fCompositeFrame709->AddFrame(fIcon1077, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fIcon1077->MoveResize(392,16,70,70);

   fMainFrame1586->AddFrame(fCompositeFrame709, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

   fMainFrame1586->SetWindowName("project6.C");
   fMainFrame1586->SetIconName("project6.C");
   fMainFrame1586->SetIconPixmap("bld_rgb.xpm");
   fMainFrame1586->SetClassHints("project6.C","project6.C");
   fMainFrame1586->SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputFullApplicationModal);
   fMainFrame1586->SetWMSize(486,372);
   fMainFrame1586->SetWMSizeHints(486,372,10000,10000,0,0);
   fMainFrame1586->MapSubwindows();

   fMainFrame1586->Resize(fMainFrame1586->GetDefaultSize());
   fMainFrame1586->MapWindow();
   fMainFrame1586->Resize(486,372);
}
