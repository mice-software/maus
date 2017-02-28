#!/usr/bin/python
#pylint: skip-file
import ROOT
import csv
import math

ROOT.gROOT.Reset()
ROOT.gStyle.SetOptStat(1000110110)
ROOT.gStyle.SetNumberContours(100)

c1 = ROOT.TCanvas("c1","Canvas1",1500,1000)
c1.SetCanvasSize(1500, 1000)
c1.Divide(3,2)

tof0range = 1
tof1range = 100
tof2range = 100
klrange = 100
emrrange = 50

hTOF0 = ROOT.TH1I("tof0", "TOF0", 100, -tof0range, tof0range)
hTOF1 = ROOT.TH2I("tof1", "TOF1", 100, -tof1range, tof1range, 100, -tof1range, tof1range)
hTOF2 = ROOT.TH2I("tof2", "TOF2", 100, -tof2range, tof2range, 100, -tof2range, tof2range)
hKL = ROOT.TH1I("kl", "KL", 100, -klrange, klrange)
hEMR = ROOT.TH2I("emr", "EMR", 100, -emrrange, emrrange, 100, -emrrange, emrrange)
hThrough = ROOT.TH1I("through", "Through", 100, 0.2, 1.0)

with open("match_tof0.csv", "rb") as tof0file:
  reader = csv.reader(tof0file, delimiter=" ")
  for row in reader:
    hTOF0.Fill(float(row[0]))

with open("match_tof1.csv", "rb") as tof1file:
  reader = csv.reader(tof1file, delimiter=" ")
  for row in reader:
    hTOF1.Fill(float(row[0]), float(row[1]))

with open("match_tof2.csv", "rb") as tof2file:
  reader = csv.reader(tof2file, delimiter=" ")
  for row in reader:
    hTOF2.Fill(float(row[0]), float(row[1]))

with open("match_kl.csv", "rb") as klfile:
  reader = csv.reader(klfile, delimiter=" ")
  for row in reader:
    hKL.Fill(float(row[0]))

with open("match_emr.csv", "rb") as emrfile:
  reader = csv.reader(emrfile, delimiter=" ")
  for row in reader:
    hEMR.Fill(float(row[0]), float(row[1]))

with open("match_through.csv", "rb") as throughfile:
  reader = csv.reader(throughfile, delimiter=" ")
  for row in reader:
    hThrough.Fill(float(row[0]))

pad = c1.cd(1)
pad.SetLogy()
hTOF0.Draw()
hTOF0.GetXaxis().SetTitle("\Delta t \ [ns]")
c1.cd(2)
hTOF1.Draw("COLZ")
hTOF1.GetXaxis().SetTitle("\Delta x \ [mm]")
hTOF1.GetYaxis().SetTitle("\Delta y \ [mm]")
tofbox = ROOT.TBox(-30, -30, 30, 30)
tofbox.SetLineWidth(2)
tofbox.SetLineColor(2)
tofbox.SetFillStyle(0)
tofbox2 = tofbox.Clone()
tofbox2.SetLineColor(9)
tofbox3 = tofbox.Clone()
tofbox3.SetLineColor(8)
tofbox.Draw()
tofbox2.DrawBox(-40, -40, 40, 40)
tofbox3.DrawBox(-50, -50, 50, 50)
c1.cd(3)
hTOF2.Draw("COLZ")
hTOF2.GetXaxis().SetTitle("\Delta x \ [mm]")
hTOF2.GetYaxis().SetTitle("\Delta y \ [mm]")
tofbox.Draw()
tofbox2.DrawBox(-40, -40, 40, 40)
tofbox3.DrawBox(-50, -50, 50, 50)
pad = c1.cd(4)
pad.SetLogy()
hKL.Draw()
hKL.GetXaxis().SetTitle("\Delta y \ [mm]")
c1.cd(5)
hEMR.Draw("COLZ")
hEMR.GetXaxis().SetTitle("\Delta x \ [mm]")
hEMR.GetYaxis().SetTitle("\Delta y \ [mm]")
pad = c1.cd(6)
pad.SetLogy()
hThrough.Draw()
hThrough.GetXaxis().SetTitle("#beta_{z}")
c1.Update()
raw_input()
