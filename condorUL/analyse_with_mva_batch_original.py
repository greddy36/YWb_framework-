#!/usr/bin/env python
import ROOT
import math
import array
import sys
import argparse
import numpy as np
from ROOT import TLorentzVector, AddressOf, TString
from lib2to3 import pytree
import random

from optparse import OptionParser
parser = OptionParser()
parser.add_option('--inputfile', metavar='P', type='string', action='store',
                  default= "",
                  dest='inputFile',
                  help='input file to process')

parser.add_option('--datayear', metavar='P', type='string', action='store',
                  default= "",
                  dest='dataYear',
                  help='data year to process')

parser.add_option('--outputfile', metavar='P', type='string', action='store',
                  default= "",
                  dest='outputFile',
                  help='output file to process')


parser.add_option('--nque', metavar='P', type='string', action='store',
                  default= '1',
                  dest='nQueue',
                  help='number of condor queue for the process')

parser.add_option('--npro', metavar='P', type='string', action='store',
                  default= '0',
                  dest='nProcess',
                  help='Condor process number')

(options,args) = parser.parse_args()
iFile = options.inputFile
oFile = options.outputFile
year  = str(options.dataYear)
print "year ", year
applyTopPtRew =  False
useDeepCSV = False
#sfFilePath  = "root://cmsxrootd.fnal.gov//store/user/amodak//store/user/amodak/toConndor"
doSys = True
doSkim = True

def Jet_pt(fname, tree, idx, type):
    if ('SingleMuon' in str(fname) or 'SingleElectron' in str(fname) or 'EGamma' in str(fname)):
      return tree.Jet_pt[idx]
    else: 
      if ('nominal' in str(type)):   return tree.Jet_pt_nom[idx]
      elif ('jesUp' in str(type)):   return tree.Jet_pt_jesTotalUp[idx]
      elif ('jesDown' in str(type)): return tree.Jet_pt_jesTotalDown[idx]
      elif ('jerUp' in str(type)):   return tree.Jet_pt_jerUp[idx]
      elif ('jerDown' in str(type)): return tree.Jet_pt_jerDown[idx]

def Jet_mass(fname, tree, idx, type):
    if ('SingleMuon' in str(fname) or 'SingleElectron' in str(fname) or 'EGamma' in str(fname)):
      return tree.Jet_mass[idx]
    else: 
      if ('nominal' in str(type)):   return tree.Jet_mass_nom[idx]
      elif ('jesUp' in str(type)):   return tree.Jet_mass_jesTotalUp[idx]
      elif ('jesDown' in str(type)): return tree.Jet_mass_jesTotalDown[idx]
      elif ('jerUp' in str(type)):   return tree.Jet_mass_jerUp[idx]
      elif ('jerDown' in str(type)): return tree.Jet_mass_jerDown[idx]

def MET_pt(fname, tree, type):
    if ('SingleMuon' in str(fname) or 'SingleElectron' in str(fname) or 'EGamma' in str(fname)):
      return tree.MET_pt
    else: 
      if (year == "2018" or year == "2017" or year == "2016"): 
        if ('nominal' in str(type)):    return tree.MET_pt_nom #jer for smeared 
        elif ('jesUp' in str(type)):    return tree.MET_pt_jesTotalUp
        elif ('jesDown' in str(type)):  return tree.MET_pt_jesTotalDown
        elif ('jerUp' in str(type)):    return tree.MET_pt_jerUp
        elif ('jerDown' in str(type)):  return tree.MET_pt_jerDown
      else :
        if ('nominal' in str(type)):    return tree.MET_nom_pt
        elif ('jesUp' in str(type)):    return tree.MET_jesTotalUp_pt
        elif ('jesDown' in str(type)):  return tree.MET_jesTotalDown_pt
        elif ('jerUp' in str(type)):    return tree.MET_jerUp_pt
        elif ('jerDown' in str(type)):  return tree.MET_jerDown_pt

def MET_phi(fname, tree, type):
    if ('SingleMuon' in str(fname) or 'SingleElectron' in str(fname) or 'EGamma' in str(fname)):
      return tree.MET_phi
    else: 
      if (year == "2018" or year == "2017" or year == "2016"): 
        if ('nominal' in str(type)):    return tree.MET_phi_nom
        elif ('jesUp' in str(type)):    return tree.MET_phi_jesTotalUp
        elif ('jesDown' in str(type)):  return tree.MET_phi_jesTotalDown
        elif ('jerUp' in str(type)):    return tree.MET_phi_jerUp
        elif ('jerDown' in str(type)):  return tree.MET_phi_jerDown
      else :
        if ('nominal' in str(type)):    return tree.MET_nom_phi
        elif ('jesUp' in str(type)):    return tree.MET_jesTotalUp_phi
        elif ('jesDown' in str(type)):  return tree.MET_jesTotalDown_phi
        elif ('jerUp' in str(type)):    return tree.MET_jerUp_phi
        elif ('jerDown' in str(type)):  return tree.MET_jerDown_phi
  
def isMC(fname):
    if ("SingleMuon" in str(fname) or "SingleElectron" in str(fname) or 'EGamma' in str(fname)): 
      return False
    else: 
      return True

def isTTJet(fname):
    if ("TTJets" in str(fname) or "TT_Tune" in str(fname) or "TTTo" in  str(fname)): 
      return True
    else: return False

def btagDeepCSV(btagval, wp):
    flg = 0
    if (year == "2016"): 
        if (wp == "Medium" and btagval > 0.6321): flg = 1
        elif (wp == "Tight" and btagval > 0.8953): flg = 1
    elif (year == "2017"): 
        if (wp == "Medium" and btagval > 0.4941): flg = 1
        elif (wp == "Tight" and btagval > 0.8001): flg = 1
    elif (year == "2018"):
        if (wp == "Medium" and btagval > 0.4184): flg = 1
        elif (wp == "Tight" and btagval > 0.7527): flg = 1
    else: flg = 0
    return flg

def btagDeepFLV(btagval, wp):
    flg = 0
    if (year == "2016"): 
        if (wp == "Medium" and btagval > 0.3093): flg = 1
        elif (wp == "Tight" and btagval > 0.7221): flg = 1
    elif (year == "2017"): 
        if (wp == "Medium" and btagval > 0.3033): flg = 1
        elif (wp == "Tight" and btagval > 0.7489): flg = 1
    elif (year == "2018"):
        if (wp == "Medium" and btagval > 0.2770): flg = 1
        elif (wp == "Tight" and btagval > 0.7264): flg = 1
    else: flg = 0
    return flg

#1: loose (not in 2017), 2:tight, 4: tight lepton veto
def  JetID(idval):
    flag = 0
    if (year == "2016"):
      if (idval >= 2): flag = 1
    elif (year == "2017" or year == "2018"):
      if (idval >= 2): flag = 1
    return flag

def Jet_flavour(entry, idx):
    flavour = -1
    if (abs (entry.Jet_partonFlavour[idx]) ==  5): flavour  = 0
    elif (abs (entry.Jet_partonFlavour[idx]) ==  4): flavour  = 1
    else : flavour  = 2
    return flavour

def Jet_bTagEff(entry, idx):
    eff = 0
    if (abs (entry.Jet_partonFlavour[idx]) ==  5): eff  = 0.80
    elif (abs (entry.Jet_partonFlavour[idx]) ==  4): eff  = 0.20
    else : eff  = 0.01
    return eff

def JetPUID(puid, pt, wp):
    flag = 0
    if (wp == "Loose"): cut = 4
    elif (wp == "Medium"): cut = 6
    elif (wp == "Tight"): cut = 7
    if ((float(puid) >= cut) or (float(pt) > 50)): flag = 1
    return flag

def top_Scaling(ch, nJets):
  #if (year  == "2018" and ch == "Muon"): return (1.0509 - (0.0368*nJets))
  #elif (year  == "2018" and ch == "Electron"): return (1.053 - (0.0421*nJets))
  #else: return 1
  return 1

def st_WJets_Scaling(file, tree):
  weight = 1
  if (isMC(file) and "WJets" in str(file)):
    st = tree._event_ST_v2
    weight = 1.22 - 0.000219*st
  #return weight
  return 1

def wpt_WJets_Scaling(file, tree):
  weight = 1.0
  if (isMC(file) and "WJets" in str(file)):
    wpt = tree.event_Lepton_pt + MET_pt(file, tree, "nominal")
    #weight = 1.152 - 0.000321*wpt
    #new sf after aligning baseline for MT, ST, HT between CRs
    weight = 1.184 - 0.000372*wpt
  #return weight
  return 1

def tm_WJets_Scaling(file, tree, tm):
  weight = 1
  if (isMC(file) and "WJets" in str(file)):
    weight = 1.10 - 0.000197*tm
  #return weight
  return 1

def st_TTJets_Scaling(file, tree):
  weight = 1
  if (isMC(file) and ("TTJets" in str(file) or "TT_Tune" in str(file) or "TTTo" in  str(file))):
    st = tree.event_ST
    #weight = 1.178 - 0.000298*st
    #weight = 1.141 - 0.000231*st
    weight = 1.1789 - 0.0002532*st
  return weight

def topPt_Reweighting(file, tree, alpha, beta):
    weight = 1
    if (isMC(file)):
      tops = []
      for i in range(0, tree.nGenPart):
        if (abs(tree.GenPart_pdgId[i]) == 6 and abs(tree.GenPart_status[i]) == 22):
          top_4vec = ROOT.TLorentzVector()
          top_4vec.SetPtEtaPhiM(tree.GenPart_pt[i], tree.GenPart_eta[i], tree.GenPart_phi[i], tree.GenPart_mass[i])
          tops.append(top_4vec)
      if (len(tops) == 2):
         pt1 = tops[0].Pt()
         pt2 = tops[1].Pt()
         weight = 0.89*1.34*math.sqrt(math.exp(alpha - beta * pt1) * math.exp(alpha - beta * pt2))
    return weight

def W_GeneratorP4_weight(file, tree, alpha, beta):
    weight = 1
    if (isMC(file)):
      for i in range(0, tree.nGenPart):
        if (abs(tree.GenPart_pdgId[i]) == 24 and tree.GenPart_status[i] == 22):
          w_4vec = ROOT.TLorentzVector()
          w_4vec.SetPtEtaPhiM(tree.GenPart_pt[i], tree.GenPart_eta[i], tree.GenPart_phi[i], tree.GenPart_mass[i])
          #new_momentum =  beta*w_4vec.P()
          #new_pt = new_momentum/math.cosh(tree.GenPart_eta[i])
          weight = alpha + beta*w_4vec.P()
    return weight

def applyHEM(file, entry, j):
  flag = 1
  if (year == "2018"):
    if ((entry.Jet_eta[j] < -1.3 and entry.Jet_eta[j] > -3.0 and entry.Jet_phi[j] > -1.57 and entry.Jet_phi[j]  < -0.87) or (entry.event_Lepton_eta < -1.3 and entry.event_Lepton_eta > -3.0 and entry.event_Lepton_phi > -1.57 and entry.event_Lepton_phi  < -0.87)): foundLJ = 1
    else: foundLJ = 0
    #Found Lepton or Jet in HEM 15/16
    if (foundLJ):
      #print "Jet phi ", entry.event_Jet_phi[j], " Jet eta ", entry.event_Jet_eta[j], " Lep phi ", entry.event_Lepton_phi, " Lep eta", entry.event_Lepton_eta
      if (isMC(file) and float(entry.event_Event_flag) == 11):
        #Affected lumi 39/60 = 0.65
        #ran_proportion = random.uniform(0, 1)
        #print "ran_proportion", ran_proportion
        #if (ran_proportion < 0.65):
        flag = 0
      else:
        #if ("SingleMuon_2018C" in str(file) or "SingleMuon_2018D" in str(file) or "EGamma_2018C" in str(file) or "EGamma_2018D" in str(file)):
        #if ("EGamma_2018C" in str(file) or "EGamma_2018D" in str(file)):
        if ("EGamma" in str(file) and ( "Run2018C" in str(file) or "Run2018D" in str(file))):
          flag = 0
  return flag

def trig_SF(file, tree, pt, eta, channel):
  sf = 1
  if (channel  == "Muon" and (isMC(file))):
    if (year == "2018"):
      sf = tree.GetBinContent(tree.GetXaxis().FindBin(pt), tree.GetYaxis().FindBin(abs(eta)))
  if (channel == "Ele" and (isMC(file))):
    if (year == "2016" or year == "2017" or year == "2018"):
      sf = tree.Eval(eta)
  return sf

def MET_Filters(file, tree):
    if (year == "2016"):
      if (isMC(file)):
        if (tree.Flag_goodVertices > 0 and tree.Flag_globalSuperTightHalo2016Filter > 0 and tree.Flag_HBHENoiseFilter > 0 and  tree.Flag_HBHENoiseIsoFilter > 0 and tree.Flag_EcalDeadCellTriggerPrimitiveFilter > 0 and tree.Flag_BadPFMuonFilter > 0): 
          return 1
        else:
          return 0
      else:
        if (tree.Flag_goodVertices > 0 and tree.Flag_globalSuperTightHalo2016Filter > 0 and tree.Flag_HBHENoiseFilter > 0 and  tree.Flag_HBHENoiseIsoFilter > 0 and tree.Flag_EcalDeadCellTriggerPrimitiveFilter > 0 and tree.Flag_BadPFMuonFilter > 0 and tree.Flag_eeBadScFilter > 0): 
          return 1
        else:
          return 0
    elif (year == "2017" or year == "2018"):
      if (isMC(file)):
        if (tree.Flag_goodVertices > 0 and tree.Flag_globalSuperTightHalo2016Filter > 0 and tree.Flag_HBHENoiseFilter > 0 and  tree.Flag_HBHENoiseIsoFilter > 0 and tree.Flag_EcalDeadCellTriggerPrimitiveFilter > 0 and tree.Flag_BadPFMuonFilter > 0): 
          return 1
        else:
          return 0
      else:
        if (tree.Flag_goodVertices > 0 and tree.Flag_globalSuperTightHalo2016Filter > 0 and tree.Flag_HBHENoiseFilter > 0 and  tree.Flag_HBHENoiseIsoFilter > 0 and tree.Flag_EcalDeadCellTriggerPrimitiveFilter > 0 and tree.Flag_BadPFMuonFilter > 0 and tree.Flag_eeBadScFilter > 0): 
          return 1
        else:
          return 0
    else: return 0

def applyBTag (isBTagged, sf, eff, ran):
    btagFlag = isBTagged
    if (sf == 1): return btagFlag
    if (sf > 1):
      if not (isBTagged == 1):
        #fraction of jets that need to be upgraded
        mistagPercent = (1.0 - sf)/(1.0 - (1.0/eff))
        #upgrade to tagged
        if (ran < mistagPercent): btagFlag = 1 
    else:
      #downgrade tagged to untagged
      if (isBTagged == 1 and ran > sf): btagFlag = 0
    return btagFlag

def applyHighPtMuonSF (file, entry, pt, eta):
    sf = 1
    data_eff = 1
    mc_eff  = 1
    if (abs (eta) < 1.6 and pt < 100):
      mc_eff = 0.989 - 2.399*(10.0**-6)*pt 
    if (abs (eta) < 1.6 and pt > 100):
      data_eff = 0.9828 - 1.947*(10.0**-5)*pt 
    if (abs (eta) > 1.6 and abs (eta) < 2.4 and pt < 275):
      mc_eff = 0.9974 - 1.721*(10.0**-5)*pt 
    if (abs (eta) > 1.6 and abs (eta) < 2.4 and pt > 275):
      data_eff = 0.9893 - 3.666*(10.0**-5)*pt 
    sf = float(data_eff/mc_eff)
    return sf

def cutFlow(file, entry, crType, leadjetBTag, HT_Central, nBTag, MassT, LepCJ_pt, LepCJ_eta, LepCJ_btag, M_Top, Mjj_lJ_CJ, DR_lJ_CJ, HT_Full_pu_var, reader, MET_pt, LeadJet_pt, dR_lepClosestJet_v2, nVeto):

  lepPt_bdt[0] = entry.event_Lepton_pt
  leadjetPt_bdt[0] = LeadJet_pt
  #leadjetBtagFLV_bdt[0] = entry.event_DPHI_LepMet    
  met_bdt[0] = MET_pt
  dphiLepmet_bdt[0] = entry.event_DPHI_LepMet
  dphiLepleadjet_bdt[0] = entry.event_DPHI_LepleadJet
  drLepclosestJet_bdt[0] = entry.event_DR_LepClosestJet
  bvswRatio_bdt[0] = entry.event_bVsW_ratio
  angleMujetMet_bdt[0] = entry.event_Angle_MuJet_Met
  angleMuZJet_bdt[0] = entry.event_Angle_MuZ_Jet
  dphiJetmet_bdt[0] = entry.event_DPHI_JetMet
  HT_o_LeadJetPt_bdt[0] = HT_Full_pu_var/LeadJet_pt
  LepClosestJet_v2_pt_bdt[0] = LepCJ_pt
  LepClosestJet_v2_eta_bdt[0] = LepCJ_eta
  LepClosestJet_v2_btagDeepFLV_bdt[0] = LepCJ_btag
  M_top_bdt[0] = M_Top
  Mjj_leadJet_CloJet_bdt[0] = Mjj_lJ_CJ
  DR_leadJet_CloJet_bdt[0] = DR_lJ_CJ

  bdtOutput = reader.EvaluateMVA("BDT")
  #print "BDT: ", bdtOutput
  if (crType == "WJets"):
    if not (entry.PV_npvsGood >= 1 and entry.event_Lepton_pt > 40 and abs(entry.event_Lepton_eta) < 2.1): return bdtOutput, 1
    if not (MET_pt > 60): return bdtOutput, 2
    if not (MET_Filters(file, entry) > 0): return bdtOutput, 3
    if not (LeadJet_pt > 200 and abs(entry.event_LeadJet_eta) < 2.4): return bdtOutput, 4
    if not (leadjetBTag < 1): return bdtOutput, 5
    if not (entry.event_DR_LepleadJet > 2.0): return bdtOutput, 6 #backtoback
    if not (entry.event_DR_LepClosestJet > -1): return bdtOutput, 7
    if not (MassT > 40): return bdtOutput, 8 
    if not (nBTag == 0): return bdtOutput, 9
    if not (entry.event_ST > 500): return bdtOutput, 10
    if not (HT_Central > 500): return bdtOutput, 11
    if not (abs(entry.event_DPHI_LepMet) < 1.0): return bdtOutput, 12 #backtoback
    if not (abs(entry.event_DPHI_LepleadJet) > -999): return bdtOutput, 13
    if not (entry.event_bVsW_ratio < 2.0): return bdtOutput, 14  #backtoback
    #if not ((entry.event_nVetoMuons + entry.event_nVetoElectrons) == 1): return bdtOutput, 15
    if not (nVeto == 1): return bdtOutput, 15
    if not (entry.event_FwdJetPt > -999 and abs(entry.event_FwdJetEta) > -999): return bdtOutput, 16  ##changed
    return bdtOutput, 17

  if (crType == "Signal"):
    if not (entry.PV_npvsGood >= 1 and entry.event_Lepton_pt > 40 and abs(entry.event_Lepton_eta) < 2.1): return bdtOutput, 1
    if not (MET_pt > 60): return bdtOutput, 2
    if not (MET_Filters(file, entry) > 0): return bdtOutput, 3
    if not (LeadJet_pt > 200 and abs(entry.event_LeadJet_eta) < 2.4): return bdtOutput, 4
    if not (leadjetBTag >= 1): return bdtOutput, 5
    if not (entry.event_DR_LepleadJet > 2.0): return bdtOutput, 6
    if not (entry.event_DR_LepClosestJet > 1.5): return bdtOutput, 7
    #if not (dR_lepClosestJet_v2 > 1.5): return bdtOutput, 7
    if not (MassT > 0): return bdtOutput, 8 #changed from 40
    if not (nBTag >= 1): return bdtOutput, 9
    if not (entry.event_ST > 700): return bdtOutput, 10
    if not (HT_Central > 500): return bdtOutput, 11
    if not (abs(entry.event_DPHI_LepMet) < 0.5): return bdtOutput, 12
    if not (abs(entry.event_DPHI_LepleadJet) > 2.0): return bdtOutput, 13
    if not (entry.event_bVsW_ratio < 1.4): return bdtOutput, 14
    #if not ((entry.event_nVetoMuons + entry.event_nVetoElectrons) == 1): return bdtOutput, 15
    if not (nVeto == 1): return bdtOutput, 15
    if not (entry.event_FwdJetPt > 30 and abs(entry.event_FwdJetEta) > 2.4): return bdtOutput, 16 ##Changed
    return bdtOutput, 17
  if (crType == "TTJets"):
    if not (entry.PV_npvsGood >= 1 and entry.event_Lepton_pt > 40 and abs(entry.event_Lepton_eta) < 2.1): return bdtOutput, 1
    if not (MET_pt > 60): return bdtOutput, 2
    if not (MET_Filters(file, entry) > 0): return bdtOutput, 3
    if not (LeadJet_pt > 200 and abs(entry.event_LeadJet_eta) < 2.4): return bdtOutput, 4
    if not (leadjetBTag >= 1): return bdtOutput, 5
    if not (entry.event_DR_LepleadJet > -1): return bdtOutput, 6
    if not (entry.event_DR_LepClosestJet > 0.5 and entry.event_DR_LepClosestJet < 1.5): return bdtOutput, 7
    #if not (dR_lepClosestJet_v2 > 0.5 and dR_lepClosestJet_v2 < 1.5): return bdtOutput, 7
    if not (MassT > 0): return bdtOutput, 8
    if not (nBTag >= 2): return bdtOutput, 9
    if not (entry.event_ST > 300): return bdtOutput, 10
    if not (HT_Central > 500): return bdtOutput, 11
    if not (abs(entry.event_DPHI_LepMet) < 999): return bdtOutput, 12
    if not (abs(entry.event_DPHI_LepleadJet) > -999): return bdtOutput, 13
    if not (entry.event_bVsW_ratio < 999): return bdtOutput, 14
    #if not ((entry.event_nVetoMuons + entry.event_nVetoElectrons) == 1): return bdtOutput, 15
    if not (nVeto == 1): return bdtOutput, 15
    if not (entry.event_FwdJetPt > -99 and abs(entry.event_FwdJetEta) > -99): return bdtOutput, 16  ##Changed
    return bdtOutput, 17
  if (crType == "MultiJets"):
    if not (entry.PV_npvsGood >= 1 and entry.event_Lepton_pt > 40 and abs(entry.event_Lepton_eta) < 2.1): return bdtOutput, 1
    if not (MET_pt > 60): return bdtOutput, 2
    if not (MET_Filters(file, entry) > 0): return bdtOutput, 3
    if not (LeadJet_pt > 200 and abs(entry.event_LeadJet_eta) < 2.4): return bdtOutput, 4
    if not (leadjetBTag >= 1): return bdtOutput, 5
    if not (entry.event_DR_LepleadJet > -1): return bdtOutput, 6
    if not (entry.event_DR_LepClosestJet > -1.0): return bdtOutput, 7
    if not (MassT < 9999): return bdtOutput, 8
    if not (nBTag >= 1): return bdtOutput, 9
    if not (entry.event_ST > 300): return bdtOutput, 10
    if not (HT_Central > 200): return bdtOutput, 11
    if not (abs(entry.event_DPHI_LepMet) < 999): return bdtOutput, 12
    if not (abs(entry.event_DPHI_LepleadJet) > -999): return bdtOutput, 13
    if not (entry.event_bVsW_ratio < 999): return bdtOutput, 14
    #if not ((entry.event_nVetoMuons + entry.event_nVetoElectrons) == 1): return bdtOutput, 15
    if not (nVeto == 1): return bdtOutput, 15
    #if not (entry.event_FwdJetPt > -99 and abs(entry.event_FwdJetEta) > -99): return bdtOutput, 16
    if not (entry.event_nCentralJets >= 4): return bdtOutput, 16
    return bdtOutput, 17
  if (crType == "PreSig"):
    if not (entry.PV_npvsGood >= 1 and entry.event_Lepton_pt > 40 and abs(entry.event_Lepton_eta) < 2.1): return bdtOutput, 1
    if not (MET_pt > 60): return bdtOutput, 2
    if not (MET_Filters(file, entry) > 0): return bdtOutput, 3
    if not (LeadJet_pt > 200 and abs(entry.event_LeadJet_eta) < 2.4): return bdtOutput, 4
    if not (leadjetBTag >= 1): return bdtOutput, 5
    if not (entry.event_DR_LepleadJet > 2.0): return bdtOutput, 6 #backtoback
    if not (entry.event_DR_LepClosestJet > -1): return bdtOutput, 7
    if not (MassT > 0): return bdtOutput, 8 #changed from 40
    if not (nBTag >= 1): return bdtOutput, 9
    if not (entry.event_ST > 500): return bdtOutput, 10
    if not (HT_Central > 500): return bdtOutput, 11
    if not (abs(entry.event_DPHI_LepMet) < 1.0): return bdtOutput, 12 #back-to-back, changed from 999
    if not (abs(entry.event_DPHI_LepleadJet) > -999): return bdtOutput, 13
    if not (entry.event_bVsW_ratio < 2.0): return bdtOutput, 14 #back-to-back, changed from 999
    #if not ((entry.event_nVetoMuons + entry.event_nVetoElectrons) == 1): return bdtOutput, 15
    if not (nVeto == 1): return bdtOutput, 15
    if not (entry.event_FwdJetPt > -99 and abs(entry.event_FwdJetEta) > -99): return bdtOutput, 16
    return bdtOutput, 17
  if (crType == "SignalBDT"):
    if not (entry.PV_npvsGood >= 1 and entry.event_Lepton_pt > 40 and abs(entry.event_Lepton_eta) < 2.1): return bdtOutput, 1
    if not (MET_pt > 60): return bdtOutput, 2
    if not (MET_Filters(file, entry) > 0): return bdtOutput, 3
    if not (LeadJet_pt > 200 and abs(entry.event_LeadJet_eta) < 2.4): return bdtOutput, 4
    if not (leadjetBTag >= 1): return bdtOutput, 5
    if not (entry.event_DR_LepleadJet > -1): return bdtOutput, 6
    if not (entry.event_DR_LepClosestJet > -1): return bdtOutput, 7
    if not (MassT > 0): return bdtOutput, 8 
    if not (nBTag >= 1): return bdtOutput, 9
    if not (entry.event_ST > 0): return bdtOutput, 10
    if not (HT_Central > 0): return bdtOutput, 11
    if not (abs(entry.event_DPHI_LepMet) < 999.0): return bdtOutput, 12 
    if not (abs(entry.event_DPHI_LepleadJet) > -999): return bdtOutput, 13
    if not (entry.event_bVsW_ratio < 999): return bdtOutput, 14
    #if not ((entry.event_nVetoMuons + entry.event_nVetoElectrons) == 1): return bdtOutput, 15
    if not (nVeto == 1): return bdtOutput, 15
    if not (entry.event_FwdJetPt > 30 and abs(entry.event_FwdJetEta) > 2.4 and bdtOutput > 0.19): return bdtOutput, 16 # 0.39 for 1800, 0.18 for 1200, 0.19 for 1200 with bdtlong17var
    return bdtOutput, 17

def fillHisto(cntR, channeL, file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt, MET_phi, LeadJet_pt, sys, evtwt):
    iso03  = 999
    iso04  = 999
    lepCharge = 0
    if (channeL == "Mu"):
      iso03  = entry.Muon_pfRelIso03_all[entry.event_Lepton_idx]
      iso04  = entry.Muon_pfRelIso04_all[entry.event_Lepton_idx]
      lepCharge = entry.Muon_charge[entry.event_Lepton_idx]
    elif (channeL == "Ele"):
      iso03  = entry.Electron_pfRelIso03_all[entry.event_Lepton_idx]
      lepCharge = entry.Electron_charge[entry.event_Lepton_idx]

    hmap[str(channeL)+"_"+str(cntR)+"_LepPt_"+str(sys)+"_"+str(itr)].Fill(entry.event_Lepton_pt, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_LepPhi_"+str(sys)+"_"+str(itr)].Fill(entry.event_Lepton_phi, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_LepEta_"+str(sys)+"_"+str(itr)].Fill(entry.event_Lepton_eta, evtwt)
    if (HEMwt == 0):
      hmap[str(channeL)+"_"+str(cntR)+"_LepPhi_HEMf_"+str(sys)+"_"+str(itr)].Fill(entry.event_Lepton_phi, evtwt)
      hmap[str(channeL)+"_"+str(cntR)+"_LepEta_HEMf_"+str(sys)+"_"+str(itr)].Fill(entry.event_Lepton_eta, evtwt)
      hmap[str(channeL)+"_"+str(cntR)+"_JetPhi_HEMf_"+str(sys)+"_"+str(itr)].Fill(entry.event_LeadJet_phi, evtwt)
      hmap[str(channeL)+"_"+str(cntR)+"_JetEta_HEMf_"+str(sys)+"_"+str(itr)].Fill(entry.event_LeadJet_eta, evtwt)
    elif (HEMwt == 1):
      hmap[str(channeL)+"_"+str(cntR)+"_LepPhi_HEMp_"+str(sys)+"_"+str(itr)].Fill(entry.event_Lepton_phi, evtwt)
      hmap[str(channeL)+"_"+str(cntR)+"_LepEta_HEMp_"+str(sys)+"_"+str(itr)].Fill(entry.event_Lepton_eta, evtwt)
      hmap[str(channeL)+"_"+str(cntR)+"_JetPhi_HEMp_"+str(sys)+"_"+str(itr)].Fill(entry.event_LeadJet_phi, evtwt)
      hmap[str(channeL)+"_"+str(cntR)+"_JetEta_HEMp_"+str(sys)+"_"+str(itr)].Fill(entry.event_LeadJet_eta, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_MET_"+str(sys)+"_"+str(itr)].Fill(MET_pt, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_METphi_"+str(sys)+"_"+str(itr)].Fill(MET_phi, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_ST_v2_"+str(sys)+"_"+str(itr)].Fill(entry.event_ST_v2, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_TranMom_"+str(sys)+"_"+str(itr)].Fill((entry.event_ST_v2+HT_Central-LeadJet_pt), evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_ST_"+str(sys)+"_"+str(itr)].Fill(entry.event_ST, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_WPt_"+str(sys)+"_"+str(itr)].Fill(entry.event_Lepton_pt + MET_pt, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_MT_"+str(sys)+"_"+str(itr)].Fill(MassT, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_HT_"+str(sys)+"_"+str(itr)].Fill(HT_Central, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_DPHI_"+str(sys)+"_"+str(itr)].Fill(entry.event_DPHI_LepleadJet, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_DPHILepMet_"+str(sys)+"_"+str(itr)].Fill(entry.event_DPHI_LepMet, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_DR_"+str(sys)+"_"+str(itr)].Fill(entry.event_DR_LepClosestJet, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_DRpu_"+str(sys)+"_"+str(itr)].Fill(dR_lepClosestJet_v2, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_DR_LepleadJet_"+str(sys)+"_"+str(itr)].Fill(entry.event_DR_LepleadJet, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_JetPt_"+str(sys)+"_"+str(itr)].Fill(LeadJet_pt, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_JetPhi_"+str(sys)+"_"+str(itr)].Fill(entry.event_LeadJet_phi, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_JetEta_"+str(sys)+"_"+str(itr)].Fill(entry.event_LeadJet_eta, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_NBTags_DeepCSV_"+str(sys)+"_"+str(itr)].Fill(entry.event_nBTagMed_DeepCSV, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_NBTags_DeepFLV_"+str(sys)+"_"+str(itr)].Fill(nBTag, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_NJets_"+str(sys)+"_"+str(itr)].Fill(entry.event_nJets, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_NCentralJets_"+str(sys)+"_"+str(itr)].Fill(nCentralJets_v2, evtwt)
    #hmap[str(channeL)+"_"+str(cntR)+"_NCentralJets_"+str(sys)+"_"+str(itr)].Fill(entry.event_nCentralJets, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_NFwdJets_"+str(sys)+"_"+str(itr)].Fill(entry.event_nFwdJets, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_bVsW_ratio_"+str(sys)+"_"+str(itr)].Fill(entry.event_bVsW_ratio, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_DPHIMetJet_"+str(sys)+"_"+str(itr)].Fill(entry.event_DPHI_JetMet, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_FwdJetEta_"+str(sys)+"_"+str(itr)].Fill(entry.event_FwdJetEta, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_FwdJetPt_"+str(sys)+"_"+str(itr)].Fill(entry.event_FwdJetPt, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_FwdJetPt_L_"+str(sys)+"_"+str(itr)].Fill(FwdJetPt_L, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_FwdJetPt_M_"+str(sys)+"_"+str(itr)].Fill(FwdJetPt_M, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_FwdJetPt_T_"+str(sys)+"_"+str(itr)].Fill(FwdJetPt_T, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_Mass_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_Mass_v2_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass_v2, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_RelIso03_"+str(sys)+"_"+str(itr)].Fill(iso03, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_RelIso04_"+str(sys)+"_"+str(itr)].Fill(iso04, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_BDTOutput_"+str(sys)+"_"+str(itr)].Fill(bdtOutput, evtwt)
    hmap[str(channeL)+"_"+str(cntR)+"_nPVGood_"+str(sys)+"_"+str(itr)].Fill(entry.PV_npvsGood, evtwt)
    if (nBTag == 1):
       if (lepCharge > 0):
         hmap[str(channeL)+"_"+str(cntR)+"_Mass1b_CP_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_Mass1b_v2_CP_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass_v2, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_Mass1b_js_CP_"+str(sys)+"_"+str(itr)].Fill(Mt_js, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_BDTOutput1b_CP_"+str(sys)+"_"+str(itr)].Fill(bdtOutput, evtwt)
         if (leadjetBTagTig >= 1): 
           hmap[str(channeL)+"_"+str(cntR)+"_MassTight1b_CP_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_MassTight1b_v2_CP_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass_v2, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_MassTight1b_js_CP_"+str(sys)+"_"+str(itr)].Fill(Mt_js, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_BDTOutputTight1b_CP_"+str(sys)+"_"+str(itr)].Fill(bdtOutput, evtwt)
         else: 
           hmap[str(channeL)+"_"+str(cntR)+"_MassMedium1b_CP_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_MassMedium1b_v2_CP_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass_v2, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_MassMedium1b_js_CP_"+str(sys)+"_"+str(itr)].Fill(Mt_js, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_BDTOutputMedium1b_CP_"+str(sys)+"_"+str(itr)].Fill(bdtOutput, evtwt)

       elif (lepCharge < 0):
         hmap[str(channeL)+"_"+str(cntR)+"_Mass1b_CM_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_Mass1b_v2_CM_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass_v2, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_Mass1b_js_CM_"+str(sys)+"_"+str(itr)].Fill(Mt_js, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_BDTOutput1b_CM_"+str(sys)+"_"+str(itr)].Fill(bdtOutput, evtwt)
         if (leadjetBTagTig >= 1): 
           hmap[str(channeL)+"_"+str(cntR)+"_MassTight1b_CM_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_MassTight1b_v2_CM_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass_v2, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_MassTight1b_js_CM_"+str(sys)+"_"+str(itr)].Fill(Mt_js, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_BDTOutputTight1b_CM_"+str(sys)+"_"+str(itr)].Fill(bdtOutput, evtwt)
         else: 
           hmap[str(channeL)+"_"+str(cntR)+"_MassMedium1b_CM_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_MassMedium1b_v2_CM_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass_v2, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_MassMedium1b_js_CM_"+str(sys)+"_"+str(itr)].Fill(Mt_js, evtwt)
           hmap[str(channeL)+"_"+str(cntR)+"_BDTOutputMedium1b_CM_"+str(sys)+"_"+str(itr)].Fill(bdtOutput, evtwt)

    elif (nBTag > 1):
       if (lepCharge > 0):
         hmap[str(channeL)+"_"+str(cntR)+"_Mass2b_CP_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_Mass2b_v2_CP_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass_v2, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_Mass2b_js_CP_"+str(sys)+"_"+str(itr)].Fill(Mt_js, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_BDTOutput2b_CP_"+str(sys)+"_"+str(itr)].Fill(bdtOutput, evtwt)
       elif (lepCharge < 0):
         hmap[str(channeL)+"_"+str(cntR)+"_Mass2b_CM_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_Mass2b_v2_CM_"+str(sys)+"_"+str(itr)].Fill(entry.event_Mass_v2, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_Mass2b_js_CM_"+str(sys)+"_"+str(itr)].Fill(Mt_js, evtwt)
         hmap[str(channeL)+"_"+str(cntR)+"_BDTOutput2b_CM_"+str(sys)+"_"+str(itr)].Fill(bdtOutput, evtwt)


def fillSkim(skimName):
    #print "SkimTree: ", skimName
    sktree = ROOT.TTree( skimName, 'Skim Tree' )
    '''
    Event_genwtsum = array.array('f', [0])
    Event_flag = array.array('i', [0])
    Lepton_pt = array.array('f', [0])
    Lepton_eta = array.array('f', [0])
    Lepton_phi = array.array('f', [0])
    Lepton_mass = array.array('f', [0]) 
    Lepton_charge = array.array('f', [0]) 
    LeadJet_pt = array.array('f', [0]) 
    LeadJet_eta = array.array('f', [0])
    LeadJet_phi = array.array('f', [0])
    LeadJet_mass = array.array('f', [0])
    LeadJet_btagDeepCSV = array.array('f', [0])  
    LeadJet_btagDeepFLV = array.array('f', [0])  
    LeadJet_btag = array.array('f', [0])  
    MET = array.array('f', [0])
    MT = array.array('f', [0])
    HT_Central_pu = array.array('f', [0])
    HT_Central_nopu = array.array('f', [0])
    HT_Full_pu = array.array('f', [0])
    HT_Full_nopu = array.array('f', [0])
    ST = array.array('f', [0])
    ST_v2 = array.array('f', [0])
    nVetoElectrons = array.array('i', [0])
    nVetoMuons = array.array('i', [0])
    Mass = array.array('f', [0])
    Mass_v2 = array.array('f', [0])
    DR_LepClosestJet = array.array('f', [0])
    DR_LepClosestJet_pu = array.array('f', [0])
    DR_LepleadJet = array.array('f', [0])
    DPHI_LepleadJet = array.array('f', [0])
    DPHI_LepMet = array.array('f', [0])
    DPHI_JetMet = array.array('f', [0])
    Angle_MuZ_Jet = array.array('f', [0])
    Angle_MuJet_Met = array.array('f', [0])
    bVsW_ratio = array.array('f', [0])
    nJets = array.array('i', [0])
    nCentralJets = array.array('i', [0])
    nFwdJets = array.array('i', [0])
    nBTagMed_DeepCSV = array.array('i', [0])
    nBTagTig_DeepCSV = array.array('i', [0])
    nBTagMed_DeepFLV = array.array('i', [0])
    nBTagTig_DeepFLV = array.array('i', [0])
    FwdJetPt = array.array('f', [0])
    FwdJetEta = array.array('f', [0])
    FwdJetPhi = array.array('f', [0])
    STScale_TT = array.array('f', [0])
    WPTScale_WJ = array.array('f', [0])
    EvtWt = array.array('f', [0])
    EvtWtv2 = array.array('f', [0])
    LepClosestJet_v2_pt = array.array('f', [0])
    LepClosestJet_v2_eta = array.array('f', [0])
    LepClosestJet_v2_btagDeepFLV = array.array('f', [0])
    M_top = array.array('f', [0])
    Mjj_leadJet_CloJet = array.array('f', [0])
    DR_leadJet_CloJet = array.array('f', [0])
    LeadJet_btagTight = array.array('f', [0])
    Lepton_charge = array.array('f', [0])
    Mjj_leaddJet_fwdJet_puT = array.array('f', [0])
    DR_leaddJet_fwdJet_puT = array.array('f', [0])
    DR_Lep_fwdJet_puT = array.array('f', [0])
    FwdJetPt_puT = array.array('f', [0])
    FwdJetEta_puT = array.array('f', [0])
    Mt_JS = array.array('f', [0])
    Mw_JS = array.array('f', [0])
    '''
    sktree.Branch("Event_genwtsum", Event_genwtsum,'Event_genwtsum/F')
    sktree.Branch("Event_flag", Event_flag,'Event_flag/I')
    sktree.Branch("Lepton_pt", Lepton_pt, 'Lepton_pt/F')
    sktree.Branch("Lepton_eta", Lepton_eta, 'Lepton_eta/F')
    sktree.Branch("Lepton_phi", Lepton_phi, 'Lepton_phi/F')
    sktree.Branch("Lepton_charge", Lepton_charge, 'Lepton_charge/F')
    sktree.Branch("Lepton_mass", Lepton_mass, 'Lepton_mass/F')
    sktree.Branch("LeadJet_pt", LeadJet_pt, 'LeadJet_pt/F')
    sktree.Branch("LeadJet_eta", LeadJet_eta, 'LeadJet_eta/F')
    sktree.Branch("LeadJet_phi", LeadJet_phi, 'LeadJet_phi/F')
    sktree.Branch("LeadJet_mass", LeadJet_mass, 'LeadJet_mass/F')
    sktree.Branch("LeadJet_btagDeepCSV", LeadJet_btagDeepCSV, 'LeadJet_btagDeepCSV/F')
    sktree.Branch("LeadJet_btagDeepFLV", LeadJet_btagDeepFLV, 'LeadJet_btagDeepFLV/F')
    sktree.Branch("LeadJet_btag", LeadJet_btag, 'LeadJet_btag/F')
    sktree.Branch("MET", MET, 'MET/F')
    sktree.Branch("MT", MT, 'MT/F')
    sktree.Branch("HT_Central_pu", HT_Central_pu, 'HT_Central_pu/F')
    sktree.Branch("HT_Central_nopu", HT_Central_nopu, 'HT_Central_nopu/F')
    sktree.Branch("HT_Full_pu", HT_Full_pu, 'HT_Full_pu/F')
    sktree.Branch("HT_Full_nopu", HT_Full_nopu, 'HT_Full_nopu/F')
    sktree.Branch("ST", ST, 'ST/F')
    sktree.Branch("ST_v2", ST_v2, 'ST_v2/F')
    sktree.Branch("nVetoElectrons", nVetoElectrons, 'nVetoElectrons/I')
    sktree.Branch("nVetoMuons", nVetoMuons, 'nVetoMuons/I')
    sktree.Branch("Mass", Mass, 'Mass/F')
    sktree.Branch("Mass_v2", Mass_v2, 'Mass_v2/F')
    sktree.Branch("DR_LepClosestJet", DR_LepClosestJet, 'DR_LepClosestJet/F')
    sktree.Branch("DR_LepClosestJet_pu", DR_LepClosestJet_pu, 'DR_LepClosestJet_pu/F')
    sktree.Branch("DR_LepleadJet", DR_LepleadJet, 'DR_LepleadJet/F')
    sktree.Branch("DPHI_LepleadJet", DPHI_LepleadJet, 'DPHI_LepleadJet/F')
    sktree.Branch("DPHI_LepMet", DPHI_LepMet, 'DPHI_LepMet/F')
    sktree.Branch("DPHI_JetMet", DPHI_JetMet, 'DPHI_JetMet/F')
    sktree.Branch("Angle_MuZ_Jet", Angle_MuZ_Jet, 'Angle_MuZ_Jet/F')
    sktree.Branch("Angle_MuJet_Met", Angle_MuJet_Met, 'Angle_MuJet_Met/F')
    sktree.Branch("bVsW_ratio", bVsW_ratio, 'bVsW_ratio/F')
    sktree.Branch("nJets", nJets, 'nJets/I')
    sktree.Branch("nCentralJets", nCentralJets, 'nCentralJets/I')
    sktree.Branch("nFwdJets", nFwdJets, 'nFwdJets/I')
    sktree.Branch("nBTagMed_DeepCSV", nBTagMed_DeepCSV, 'nBTagMed_DeepCSV/I')
    sktree.Branch("nBTagTig_DeepCSV", nBTagTig_DeepCSV, 'nBTagTig_DeepCSV/I')
    sktree.Branch("nBTagMed_DeepFLV", nBTagMed_DeepFLV, 'nBTagMed_DeepFLV/I')
    sktree.Branch("nBTagTig_DeepFLV", nBTagTig_DeepFLV, 'nBTagTig_DeepFLV/I')
    sktree.Branch("FwdJetPt", FwdJetPt, 'FwdJetPt/F')
    sktree.Branch("FwdJetEta", FwdJetEta, 'FwdJetEta/F')
    sktree.Branch("FwdJetPhi", FwdJetPhi, 'FwdJetPhi/F')
    sktree.Branch("STScale_TT", STScale_TT, 'STScale_TT/F')
    sktree.Branch("WPTScale_WJ", WPTScale_WJ, 'WPTScale_WJ/F')
    sktree.Branch("EvtWt", EvtWt, 'EvtWt/F')
    sktree.Branch("EvtWt_PU_Up", EvtWt_PU_Up, 'EvtWt_PU_Up/F')
    sktree.Branch("EvtWt_PU_Down", EvtWt_PU_Down, 'EvtWt_PU_Down/F')
    sktree.Branch("EvtWt_BTagHF_Up", EvtWt_BTagHF_Up, 'EvtWt_BTagHF_Up/F')
    sktree.Branch("EvtWt_BTagHF_Down", EvtWt_BTagHF_Down, 'EvtWt_BTagHF_Down/F')
    sktree.Branch("EvtWt_BTagLF_Up", EvtWt_BTagLF_Up, 'EvtWt_BTagLF_Up/F')
    sktree.Branch("EvtWt_BTagLF_Down", EvtWt_BTagLF_Down, 'EvtWt_BTagLF_Down/F')
    sktree.Branch("EvtWt_LHEScale_Up", EvtWt_LHEScale_Up, 'EvtWt_LHEScale_Up/F')
    sktree.Branch("EvtWt_LHEScale_Down", EvtWt_LHEScale_Down, 'EvtWt_LHEScale_Down/F')
    sktree.Branch("EvtWt_LHEPdf_Up", EvtWt_LHEPdf_Up, 'EvtWt_LHEPdf_Up/F')
    sktree.Branch("EvtWt_LHEPdf_Down", EvtWt_LHEPdf_Down, 'EvtWt_LHEPdf_Down/F')
    sktree.Branch("EvtWt_STScale_Up", EvtWt_STScale_Up, 'EvtWt_STScale_Up/F')
    sktree.Branch("EvtWt_STScale_Down", EvtWt_STScale_Down, 'EvtWt_STScale_Down/F')
    sktree.Branch("LepClosestJet_v2_pt", LepClosestJet_v2_pt, 'LepClosestJet_v2_pt/F')    
    sktree.Branch("LepClosestJet_v2_eta", LepClosestJet_v2_eta, 'LepClosestJet_v2_eta/F')    
    sktree.Branch("LepClosestJet_v2_btagDeepFLV", LepClosestJet_v2_btagDeepFLV, 'LepClosestJet_v2_btagDeepFLV/F')    
    sktree.Branch("M_top", M_top, 'M_top/F')    
    sktree.Branch("Mjj_leadJet_CloJet", Mjj_leadJet_CloJet, 'Mjj_leadJet_CloJet/F')    
    sktree.Branch("DR_leadJet_CloJet", DR_leadJet_CloJet, 'DR_leadJet_CloJet/F')    
    sktree.Branch("LeadJet_btagTight", LeadJet_btagTight, 'LeadJet_btagTight/F')    
    sktree.Branch("Lepton_charge", Lepton_charge, 'Lepton_charge/F')    
    sktree.Branch("Mjj_leaddJet_fwdJet_puT", Mjj_leaddJet_fwdJet_puT, 'Mjj_leaddJet_fwdJet_puT/F')    
    sktree.Branch("DR_leaddJet_fwdJet_puT", DR_leaddJet_fwdJet_puT, 'DR_leaddJet_fwdJet_puT/F')    
    sktree.Branch("DR_Lep_fwdJet_puT", DR_Lep_fwdJet_puT, 'DR_Lep_fwdJet_puT/F')    
    sktree.Branch("FwdJetPt_puT", FwdJetPt_puT, 'FwdJetPt_puT/F')
    sktree.Branch("FwdJetEta_puT", FwdJetEta_puT, 'FwdJetEta_puT/F')
    sktree.Branch("Mt_JS", Mt_JS, 'Mt_JS/F')
    sktree.Branch("Mw_JS", Mw_JS, 'Mw_JS/F')
    return sktree

def loadSkimTree(file, entry, event_genwt, leadjetBTag, MassT, HT_Central, HT_Full_pu_var, HT_Full_nopu_var, neles, nmuons, evtwt, evtwt_PileupUp, evtwt_PileupDown, evtwt_BTaghfSFUp, evtwt_BTaghfSFDown, evtwt_BTaglfSFUp, evtwt_BTaglfSFDown, evtwt_LHEScaleUpWeight, evtwt_LHEScaleDownWeight, evtwt_LHEPdfUpWeight, evtwt_LHEPdfDownWeight, evtwt_TTJets_ST_ScalingUp, evtwt_TTJets_ST_ScalingDown, lepClosestJet_v2_4vec, Lep4vec, Met4vec, leadJet_4vec, leadjetBTagTig, FwdJetPt_T, fwdJeteta_T, Mt_js, Mw_js, met, jetpt):
    '''
    Event_genwtsum = array.array('f', [0])
    Event_flag = array.array('i', [0])
    Lepton_pt = array.array('f', [0])
    Lepton_eta = array.array('f', [0])
    Lepton_phi = array.array('f', [0])
    Lepton_mass = array.array('f', [0]) 
    Lepton_charge = array.array('f', [0]) 
    LeadJet_pt = array.array('f', [0]) 
    LeadJet_eta = array.array('f', [0])
    LeadJet_phi = array.array('f', [0])
    LeadJet_mass = array.array('f', [0])
    LeadJet_btagDeepCSV = array.array('f', [0])  
    LeadJet_btagDeepFLV = array.array('f', [0])  
    LeadJet_btag = array.array('f', [0])  
    MET = array.array('f', [0])
    MT = array.array('f', [0])
    HT_Central_pu = array.array('f', [0])
    HT_Central_nopu = array.array('f', [0])
    HT_Full_pu = array.array('f', [0])
    HT_Full_nopu = array.array('f', [0])
    ST = array.array('f', [0])
    ST_v2 = array.array('f', [0])
    nVetoElectrons = array.array('i', [0])
    nVetoMuons = array.array('i', [0])
    Mass = array.array('f', [0])
    Mass_v2 = array.array('f', [0])
    DR_LepClosestJet = array.array('f', [0])
    DR_LepClosestJet_pu = array.array('f', [0])
    DR_LepleadJet = array.array('f', [0])
    DPHI_LepleadJet = array.array('f', [0])
    DPHI_LepMet = array.array('f', [0])
    DPHI_JetMet = array.array('f', [0])
    Angle_MuZ_Jet = array.array('f', [0])
    Angle_MuJet_Met = array.array('f', [0])
    bVsW_ratio = array.array('f', [0])
    nJets = array.array('i', [0])
    nCentralJets = array.array('i', [0])
    nFwdJets = array.array('i', [0])
    nBTagMed_DeepCSV = array.array('i', [0])
    nBTagTig_DeepCSV = array.array('i', [0])
    nBTagMed_DeepFLV = array.array('i', [0])
    nBTagTig_DeepFLV = array.array('i', [0])
    FwdJetPt = array.array('f', [0])
    FwdJetEta = array.array('f', [0])
    FwdJetPhi = array.array('f', [0])
    STScale_TT = array.array('f', [0])
    WPTScale_WJ = array.array('f', [0])
    EvtWt = array.array('f', [0])
    EvtWtv2 = array.array('f', [0])
    LepClosestJet_v2_pt = array.array('f', [0])
    LepClosestJet_v2_eta = array.array('f', [0])
    LepClosestJet_v2_btagDeepFLV = array.array('f', [0])
    M_top = array.array('f', [0])
    Mjj_leadJet_CloJet = array.array('f', [0])
    DR_leadJet_CloJet = array.array('f', [0])
    LeadJet_btagTight = array.array('f', [0])
    Lepton_charge = array.array('f', [0])
    Mjj_leaddJet_fwdJet_puT = array.array('f', [0])
    DR_leaddJet_fwdJet_puT = array.array('f', [0])
    DR_Lep_fwdJet_puT = array.array('f', [0])
    FwdJetPt_puT = array.array('f', [0])
    FwdJetEta_puT = array.array('f', [0])
    Mt_JS = array.array('f', [0])
    Mw_JS = array.array('f', [0])
    '''
    #Load tree containers
    Event_genwtsum[0] = event_genwt
    Event_flag[0] = entry.event_Event_flag
    Lepton_pt[0] = entry.event_Lepton_pt
    Lepton_eta[0] = entry.event_Lepton_eta
    Lepton_phi[0] = entry.event_Lepton_phi
    Lepton_mass[0] = entry.event_Lepton_mass
    #LeadJet_pt[0] = entry.event_LeadJet_pt
    LeadJet_pt[0] = jetpt
    LeadJet_eta[0] = entry.event_LeadJet_eta
    LeadJet_phi[0] = entry.event_LeadJet_mass
    LeadJet_btagDeepCSV[0] = entry.event_LeadJet_btag_DeepCSV
    LeadJet_btagDeepFLV[0] = entry.event_LeadJet_btag_DeepFLV
    LeadJet_btag[0] = leadjetBTag
    #MET[0] = MET_pt(file, entry, "nominal")
    MET[0] = met
    MT[0] = MassT
    HT_Central_pu[0] = HT_Central
    HT_Central_nopu[0] = entry.event_HT
    HT_Full_pu[0] = HT_Full_pu_var
    HT_Full_nopu[0] = HT_Full_nopu_var
    ST[0] = entry.event_ST
    ST_v2[0] = entry.event_ST_v2
    #nVetoElectrons[0] = entry.event_nVetoElectrons
    #nVetoMuons[0] = entry.event_nVetoMuons
    nVetoElectrons[0] = neles
    nVetoMuons[0] = nmuons
    Mass[0] = entry.event_Mass
    Mass_v2[0] = entry.event_Mass_v2
    DR_LepClosestJet[0] = entry.event_DR_LepClosestJet
    DR_LepClosestJet_pu[0] = dR_lepClosestJet_v2
    DR_LepleadJet[0] = entry.event_DR_LepleadJet
    DPHI_LepleadJet[0] = entry.event_DPHI_LepleadJet
    DPHI_LepMet[0] = entry.event_DPHI_LepMet
    DPHI_JetMet[0] = entry.event_DPHI_JetMet
    Angle_MuZ_Jet[0] = entry.event_Angle_MuZ_Jet
    Angle_MuJet_Met[0] = entry.event_Angle_MuJet_Met
    bVsW_ratio[0] = entry.event_bVsW_ratio
    nJets[0] = entry.event_nJets
    #nCentralJets[0] = entry.nCentralJets
    nCentralJets[0] = nCentralJets_v2
    nFwdJets[0] = entry.event_nFwdJets
    nBTagMed_DeepCSV[0] = entry.event_nBTagMed_DeepCSV
    nBTagTig_DeepCSV[0] = entry.event_nBTagTig_DeepCSV
    nBTagMed_DeepFLV[0] = nBTag
    nBTagTig_DeepFLV[0] = entry.event_nBTagTig_DeepFLV
    FwdJetPt[0] = entry.event_FwdJetPt
    FwdJetEta[0] = entry.event_FwdJetEta
    FwdJetPhi[0] = entry.event_FwdJetPhi
    STScale_TT[0] = float(st_TTJets_Scaling(file, entry))
    WPTScale_WJ[0] = float(wpt_WJets_Scaling(file, entry))
    EvtWt[0] = evtwt
    EvtWt_PU_Up[0] = evtwt_PileupUp
    EvtWt_PU_Down[0] = evtwt_PileupDown
    EvtWt_BTagHF_Up[0] = evtwt_BTaghfSFUp
    EvtWt_BTagHF_Down[0] = evtwt_BTaghfSFDown
    EvtWt_BTagLF_Up[0] = evtwt_BTaglfSFUp
    EvtWt_BTagLF_Down[0] = evtwt_BTaglfSFDown
    EvtWt_LHEScale_Up[0] = evtwt_LHEScaleUpWeight
    EvtWt_LHEScale_Down[0] = evtwt_LHEScaleDownWeight
    EvtWt_LHEPdf_Up[0] = evtwt_LHEPdfUpWeight
    EvtWt_LHEPdf_Down[0] = evtwt_LHEPdfDownWeight
    EvtWt_STScale_Up[0] = evtwt_TTJets_ST_ScalingUp
    EvtWt_STScale_Down[0] = evtwt_TTJets_ST_ScalingDown
    LepClosestJet_v2_pt[0] = lepClosestJet_v2_4vec.Pt()
    LepClosestJet_v2_eta[0] = lepClosestJet_v2_4vec.Eta()
    if (lepClosestJet_v2_idx >= 0): LepClosestJet_v2_btagDeepFLV[0] = entry.Jet_btagDeepFlavB[lepClosestJet_v2_idx]
    else: LepClosestJet_v2_btagDeepFLV[0] = 0
    M_top[0] = (Lep4vec + Met4vec + lepClosestJet_v2_4vec).M()
    Mjj_leadJet_CloJet[0] = (leadJet_4vec + lepClosestJet_v2_4vec).M()
    DR_leadJet_CloJet[0] = leadJet_4vec.DeltaR(lepClosestJet_v2_4vec)
    LeadJet_btagTight[0] = leadjetBTagTig
    if ("Mu" in str(ch)): Lepton_charge[0] = entry.Muon_charge[entry.event_Lepton_idx]
    elif ("Ele" in str(ch)): Lepton_charge[0] = entry.Electron_charge[entry.event_Lepton_idx]
    Mjj_leaddJet_fwdJet_puT[0] = (leadJet_4vec + fwdJet_T_4vec).M()
    DR_leaddJet_fwdJet_puT[0] = leadJet_4vec.DeltaR(fwdJet_T_4vec)
    DR_Lep_fwdJet_puT[0] = Lep4vec.DeltaR(fwdJet_T_4vec)
    FwdJetPt_puT[0] = FwdJetPt_T
    FwdJetEta_puT[0] = fwdJeteta_T
    Mt_JS[0] = Mt_js
    Mw_JS[0] = Mw_js
    #print "Mw_JS[0] ", Mw_JS[0], " Mw_js ", Mw_js

for i in  range(1):
  file = ROOT.TFile.Open(iFile)
  print "Opened EOS file to process ..."
  tree = file.Get("Events")
  #canv = ROOT.TCanvas()
  outfile = oFile
  outputfile = ROOT.TFile(outfile, 'RECREATE')
  outputfile.cd()

  Event_genwtsum = array.array('f', [0])
  Event_flag = array.array('i', [0])
  Lepton_pt = array.array('f', [0])
  Lepton_eta = array.array('f', [0])
  Lepton_phi = array.array('f', [0])
  Lepton_mass = array.array('f', [0]) 
  Lepton_charge = array.array('f', [0]) 
  LeadJet_pt = array.array('f', [0]) 
  LeadJet_eta = array.array('f', [0])
  LeadJet_phi = array.array('f', [0])
  LeadJet_mass = array.array('f', [0])
  LeadJet_btagDeepCSV = array.array('f', [0])  
  LeadJet_btagDeepFLV = array.array('f', [0])  
  LeadJet_btag = array.array('f', [0])  
  MET = array.array('f', [0])
  MT = array.array('f', [0])
  HT_Central_pu = array.array('f', [0])
  HT_Central_nopu = array.array('f', [0])
  HT_Full_pu = array.array('f', [0])
  HT_Full_nopu = array.array('f', [0])
  ST = array.array('f', [0])
  ST_v2 = array.array('f', [0])
  nVetoElectrons = array.array('i', [0])
  nVetoMuons = array.array('i', [0])
  Mass = array.array('f', [0])
  Mass_v2 = array.array('f', [0])
  DR_LepClosestJet = array.array('f', [0])
  DR_LepClosestJet_pu = array.array('f', [0])
  DR_LepleadJet = array.array('f', [0])
  DPHI_LepleadJet = array.array('f', [0])
  DPHI_LepMet = array.array('f', [0])
  DPHI_JetMet = array.array('f', [0])
  Angle_MuZ_Jet = array.array('f', [0])
  Angle_MuJet_Met = array.array('f', [0])
  bVsW_ratio = array.array('f', [0])
  nJets = array.array('i', [0])
  nCentralJets = array.array('i', [0])
  nFwdJets = array.array('i', [0])
  nBTagMed_DeepCSV = array.array('i', [0])
  nBTagTig_DeepCSV = array.array('i', [0])
  nBTagMed_DeepFLV = array.array('i', [0])
  nBTagTig_DeepFLV = array.array('i', [0])
  FwdJetPt = array.array('f', [0])
  FwdJetEta = array.array('f', [0])
  FwdJetPhi = array.array('f', [0])
  STScale_TT = array.array('f', [0])
  WPTScale_WJ = array.array('f', [0])
  EvtWt = array.array('f', [0])
  EvtWtv2 = array.array('f', [0])
  EvtWt_PU_Up = array.array('f', [0])
  EvtWt_PU_Down = array.array('f', [0])
  EvtWt_BTagHF_Up = array.array('f', [0])
  EvtWt_BTagHF_Down = array.array('f', [0])
  EvtWt_BTagLF_Up = array.array('f', [0])
  EvtWt_BTagLF_Down = array.array('f', [0])
  EvtWt_LHEScale_Up = array.array('f', [0])
  EvtWt_LHEScale_Down = array.array('f', [0])
  EvtWt_LHEPdf_Up = array.array('f', [0])
  EvtWt_LHEPdf_Down = array.array('f', [0])
  EvtWt_STScale_Up = array.array('f', [0])
  EvtWt_STScale_Down = array.array('f', [0])
  LepClosestJet_v2_pt = array.array('f', [0])
  LepClosestJet_v2_eta = array.array('f', [0])
  LepClosestJet_v2_btagDeepFLV = array.array('f', [0])
  M_top = array.array('f', [0])
  Mjj_leadJet_CloJet = array.array('f', [0])
  DR_leadJet_CloJet = array.array('f', [0])
  LeadJet_btagTight = array.array('f', [0])
  Lepton_charge = array.array('f', [0])
  Mjj_leaddJet_fwdJet_puT = array.array('f', [0])
  DR_leaddJet_fwdJet_puT = array.array('f', [0])
  DR_Lep_fwdJet_puT = array.array('f', [0])
  FwdJetPt_puT = array.array('f', [0])
  FwdJetEta_puT = array.array('f', [0])
  Mt_JS = array.array('f', [0])
  Mw_JS = array.array('f', [0])

  #Skim Tree
  if (doSkim):
    sktree                = fillSkim ("nominal")
    sktree_jerUp          = fillSkim ("jerUp")
    sktree_jerDown        = fillSkim ("jerDown")
    sktree_jesUp          = fillSkim ("jesUp")
    sktree_jesDown        = fillSkim ("jesDown")

  #Histograms
  Entry           = ROOT.TH1D("Entry", "", 1, 0.5, 1.5)
  Entry_nowt      = ROOT.TH1D("Entry_nowt", "", 1, 0.5, 1.5)
  Entry_genwt      = ROOT.TH1D("Entry_genwt", "", 1, 0.5, 1.5)
  effmap_pt_eta_b_all = ROOT.TH2D("effmap_pt_eta_b_all", "", 100, 30, 1030, 10, -3, 3)
  effmap_pt_eta_c_all = ROOT.TH2D("effmap_pt_eta_c_all", "", 100, 30, 1030, 10, -3, 3)
  effmap_pt_eta_l_all = ROOT.TH2D("effmap_pt_eta_l_all", "", 100, 30, 1030, 10, -3, 3)
  effmap_pt_eta_b_btagged = ROOT.TH2D("effmap_pt_eta_b_btagged", "", 100, 30, 1030, 10, -3, 3)
  effmap_pt_eta_c_btagged = ROOT.TH2D("effmap_pt_eta_c_btagged", "", 100, 30, 1030, 10, -3, 3)
  effmap_pt_eta_l_btagged = ROOT.TH2D("effmap_pt_eta_l_btagged", "", 100, 30, 1030, 10, -3, 3)
  #Mu_TTJets_Mass_lead_sublead_bin0 = ROOT.TH2D("Mu_TTJets_Mass_lead_sublead_bin0", "", 1000, 0, 1000, 1000, 0, 1000)
  #Mu_TTJets_Mass_lead_sublead_bin1 = ROOT.TH2D("Mu_TTJets_Mass_lead_sublead_bin1", "", 1000, 0, 1000, 1000, 0, 1000)
  #Mu_TTJets_Mass_lead_sublead_bin2 = ROOT.TH2D("Mu_TTJets_Mass_lead_sublead_bin2", "", 1000, 0, 1000, 1000, 0, 1000)
  #Mu_TTJets_Mass_lead_sublead_bin3 = ROOT.TH2D("Mu_TTJets_Mass_lead_sublead_bin3", "", 1000, 0, 1000, 1000, 0, 1000)
  #Ele_TTJets_Mass_lead_sublead_bin0 = ROOT.TH2D("Ele_TTJets_Mass_lead_sublead_bin0", "", 1000, 0, 1000, 1000, 0, 1000)
  #Ele_TTJets_Mass_lead_sublead_bin1 = ROOT.TH2D("Ele_TTJets_Mass_lead_sublead_bin1", "", 1000, 0, 1000, 1000, 0, 1000)
  #Ele_TTJets_Mass_lead_sublead_bin2 = ROOT.TH2D("Ele_TTJets_Mass_lead_sublead_bin2", "", 1000, 0, 1000, 1000, 0, 1000)
  #Ele_TTJets_Mass_lead_sublead_bin3 = ROOT.TH2D("Ele_TTJets_Mass_lead_sublead_bin3", "", 1000, 0, 1000, 1000, 0, 1000)
  Mu_WJets_HT_vs_ST = ROOT.TH2D("Mu_WJets_HT_vs_ST", "", 300, 0, 3000, 300, 0, 3000)
  Ele_WJets_HT_vs_ST = ROOT.TH2D("Ele_WJets_HT_vs_ST", "", 300, 0, 3000, 300, 0, 3000)
  Jet_eta_phi_2D = ROOT.TH2D("Jet_eta_phi_2D", "", 50, -2.5, 2.5, 70, -3.5, 3.5)
  Lep_eta_phi_2D = ROOT.TH2D("Lep_eta_phi_2D", "", 50, -2.5, 2.5, 70, -3.5, 3.5)

  hmap = {}
  channels = ["Mu", "Ele"]
  controlR = ["WJets", "TTJets", "Signal", "SignalBDT", "PreSig"]
  variables = {"LepPt":  [100, 0, 1000], "LepPhi":  [100, -5, 5], "LepPhi_HEMf":  [100, -5, 5], "LepPhi_HEMp":  [100, -5, 5], "MET": [100, 0, 1000], "METphi": [100, -5, 5], "LepEta": [120, -3, 3], "LepEta_HEMf": [120, -3, 3], "LepEta_HEMp": [120, -3, 3], "ST": [300, 0, 3000], "ST_v2": [300, 0, 3000], "MT": [200, 0, 200], "HT": [300, 0, 3000], "DPHI": [100, -5, 5], "DPHILepMet": [100, -5, 5], "DRpu":  [100, 0, 5], "DR": [100, 0, 5], "DR_LepleadJet": [100, 0, 5], "JetPt": [200, 0, 2000], "JetPhi": [100, -5, 5], "JetEta": [100, -5, 5], "JetPhi_HEMp": [100, -5, 5], "JetEta_HEMp": [100, -5, 5], "JetPhi_HEMf": [100, -5, 5], "JetEta_HEMf": [100, -5, 5], "NBTags_DeepCSV": [5, -0.5, 4.5], "NBTags_DeepFLV": [5, -0.5, 4.5], "NJets": [20, 0, 20], "NCentralJets": [20, 0, 20], "NFwdJets": [20, 0, 20], "DPHIMetJet": [100, -5, 5], "FwdJetEta": [100, -5, 5], "FwdJetPt": [500, 0, 500], "Mass": [300, 0, 3000], "Mass_v2": [300, 0, 3000], "Mass1b": [300, 0, 3000], "Mass1b_v2": [300, 0, 3000], "MassTight1b": [300, 0, 3000], "MassMedium1b": [300, 0, 3000], "MassTight1b_v2": [300, 0, 3000], "MassMedium1b_v2": [300, 0, 3000], "Mass2b": [300, 0, 3000], "Mass2b_v2": [300, 0, 3000], "RelIso03": [100, 0, 0.5], "RelIso04": [100, 0, 0.5], "WPt": [200, 0, 2000], "TranMom": [600, 0, 6000], "FwdJetPt_L": [500, 0, 500], "FwdJetPt_M": [500, 0, 500], "FwdJetPt_T": [500, 0, 500], "bVsW_ratio": [100, 0, 10], "BDTOutput": [100, -1, 1], "nPVGood": [100, 0, 100], "BDTOutput1b_CP": [100, -1, 1], "BDTOutputTight1b_CP": [100, -1, 1], "BDTOutputMedium1b_CP": [100, -1, 1], "BDTOutput2b_CP": [100, -1, 1], "BDTOutputTight1b_CM": [100, -1, 1], "BDTOutputMedium1b_CM": [100, -1, 1], "BDTOutput1b_CM": [100, -1, 1], "BDTOutput2b_CM": [100, -1, 1], "Mass1b_CP": [300, 0, 3000], "MassTight1b_CP": [300, 0, 3000], "MassMedium1b_CP": [300, 0, 3000], "Mass2b_CP": [300, 0, 3000], "MassTight1b_CM": [300, 0, 3000], "MassMedium1b_CM": [300, 0, 3000], "Mass1b_CM":[300, 0, 3000], "Mass2b_CM": [300, 0, 3000], "Mass1b_v2_CP": [300, 0, 3000], "MassTight1b_v2_CP": [300, 0, 3000], "MassMedium1b_v2_CP": [300, 0, 3000], "Mass2b_v2_CP": [300, 0, 3000], "MassTight1b_v2_CM": [300, 0, 3000], "MassMedium1b_v2_CM": [300, 0, 3000], "Mass1b_v2_CM":[300, 0, 3000], "Mass2b_v2_CM": [300, 0, 3000], "Mass1b_js_CP": [300, 0, 3000], "MassTight1b_js_CP": [300, 0, 3000], "MassMedium1b_js_CP": [300, 0, 3000], "Mass2b_js_CP": [300, 0, 3000], "MassTight1b_js_CM": [300, 0, 3000], "MassMedium1b_js_CM": [300, 0, 3000], "Mass1b_js_CM":[300, 0, 3000], "Mass2b_js_CM": [300, 0, 3000]}

  #variables_top = {"Pt_lmj_select": [100, 0, 1000], "Pt_lmj_select2": [100, 0, 1000], "Pt_lmj_select_alpha_up": [100, 0, 1000], "Pt_lmj_select_alpha_down": [100, 0, 1000], "Pt_lmj_select_beta_up": [100, 0, 1000], "Pt_lmj_select_beta_down": [100, 0, 1000], "Top_Score": [4, -0.5, 3.5], "Pt_lmj_select_beta_up2": [100, 0, 1000], "Pt_lmj_select_beta_down2": [100, 0, 1000], "ST_v2_def2": [300, 0, 3000], "ST_v2_beta_up2": [300, 0, 3000], "ST_v2_beta_down2": [300, 0, 3000]}

  #levels = [4, 11, 16]
  levels = [16]
  systematicS = ["nominal"]
  if (doSys):
    systematicS = ["nominal", "PileupUp", "PileupDown", "BTaghfSFUp", "BTaghfSFDown", "BTaglfSFUp", "BTaglfSFDown", "topptweightUp", "topptweightDown", "jerUp", "jerDown", "jesUp", "jesDown", "LHEScale1", "LHEScale2", "LHEScale3", "LHEScale4", "LHEScale6", "LHEScale8", "LHEScaleUpWeight", "LHEScaleDownWeight", "LHEPdfUpWeight", "LHEPdfDownWeight","TTJets_ST_ScalingUp", "TTJets_ST_ScalingDown", "NoPileup"]
    
  for ch in range(len(channels)):
    for cr in range(len(controlR)):
      hname = channels[ch]+"_"+controlR[cr]+"_Counter"
      hnamestr = hname
      hname = ROOT.TH1D(str(hnamestr), "", 21, -0.5, 20.5)
      hmap.update({str(hnamestr) : hname})
      #for key in variables_top:
      #  hname = channels[ch]+"_"+controlR[cr]+"_"+key
      #  hnamestr = hname
      #  hname = ROOT.TH1D(str(hnamestr), "", variables_top[key][0], variables_top[key][1], variables_top[key][2])
      #  hmap.update({str(hnamestr) : hname})
      for hist in levels:
        for key in variables:
          for item in systematicS:
            hname = channels[ch]+"_"+controlR[cr]+"_"+key+"_"+str(item)+"_"+str(hist)
            hnamestr = hname
            hname = ROOT.TH1D(str(hnamestr), "", variables[key][0], variables[key][1], variables[key][2])
            hmap.update({str(hnamestr) : hname})

  event_genwt = 1
  if (isMC(file)): 
    histo = file.Get("autoPU")
    Entry_nowt.Fill(1, float(histo.GetEntries()))
    runs = file.Get("Runs")
    for itr in runs:
      if (year == "2018"):
        #Entry_genwt.Fill(1, itr.genEventSumw_)
        Entry_genwt.Fill(1, itr.genEventSumw)
        event_genwt = itr.genEventSumw
      else :
        Entry_genwt.Fill(1, itr.genEventSumw)
        event_genwt = itr.genEventSumw
    #Open SF root files here
    if (year == "2018"): 
      file_muTrigBefore = ROOT.TFile.Open("root://cmsxrootd.fnal.gov//store/user/greddy/condorAtanu/EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root")
      tree_muTrigBefore = file_muTrigBefore.Get("IsoMu24_PtEtaBins/pt_abseta_ratio")
      file_muTrigAfter = ROOT.TFile.Open("root://cmsxrootd.fnal.gov//store/user/greddy/condorAtanu/EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root")
      tree_muTrigAfter = file_muTrigAfter.Get("IsoMu24_PtEtaBins/pt_abseta_ratio")

  # create TMVA::Reader object
  lepPt_bdt, leadjetPt_bdt, met_bdt, dphiLepmet_bdt, dphiLepleadjet_bdt, drLepclosestJet_bdt, bvswRatio_bdt, angleMujetMet_bdt, angleMuZJet_bdt, dphiJetmet_bdt, HT_o_LeadJetPt_bdt, LepClosestJet_v2_pt_bdt, LepClosestJet_v2_eta_bdt, LepClosestJet_v2_btagDeepFLV_bdt, M_top_bdt, Mjj_leadJet_CloJet_bdt, DR_leadJet_CloJet_bdt = array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0]), array.array('f',[0])

  reader = ROOT.TMVA.Reader()
  reader.AddVariable( "Lepton_pt", lepPt_bdt )
  reader.AddVariable( "LeadJet_pt", leadjetPt_bdt )
  #reader.AddVariable( "LeadJet_btagDeepFLV", leadjetBtagFLV_bdt )
  reader.AddVariable( "MET", met_bdt )
  reader.AddVariable( "DPHI_LepMet",  dphiLepmet_bdt)
  reader.AddVariable( "DPHI_LepleadJet", dphiLepleadjet_bdt)
  reader.AddVariable( "DR_LepClosestJet",  drLepclosestJet_bdt)
  reader.AddVariable( "bVsW_ratio",  bvswRatio_bdt)
  reader.AddVariable( "Angle_MuJet_Met",  angleMujetMet_bdt)
  reader.AddVariable( "Angle_MuZ_Jet",  angleMuZJet_bdt)
  #dataloader->AddVariable( "Ext_Jet_TransPt", 'F' );
  #dataloader->AddVariable( "ST", 'F' );
  #dataloader->AddVariable( "HT", 'F' );
  #dataloader->AddVariable( "nBTagMed_DeepFLV", 'F' );
  #dataloader->AddVariable( "nCentralJets", 'F' );
  #dataloader->AddVariable( "NFwdJets", 'F' );
  #dataloader->AddVariable( "MT", 'F' );
  reader.AddVariable( "DPHI_JetMet",  dphiJetmet_bdt)
  reader.AddVariable( "HT_o_LeadJetPt := HT_Full_pu/LeadJet_pt",  HT_o_LeadJetPt_bdt)
  #dataloader->AddVariable( "myvar2 := (HT - LeadJet_pt)", 'F' );
  #dataloader->AddVariable( "var3",                "Variable 3", "units", 'F' );
  #dataloader->AddVariable( "var4",                "Variable 4", "units", 'F' );
  reader.AddVariable( "LepClosestJet_v2_pt", LepClosestJet_v2_pt_bdt );
  reader.AddVariable( "LepClosestJet_v2_eta", LepClosestJet_v2_eta_bdt );
  reader.AddVariable( "LepClosestJet_v2_btagDeepFLV", LepClosestJet_v2_btagDeepFLV_bdt );
  reader.AddVariable( "M_top", M_top_bdt );
  reader.AddVariable( "Mjj_leadJet_CloJet", Mjj_leadJet_CloJet_bdt );
  reader.AddVariable( "DR_leadJet_CloJet", DR_leadJet_CloJet_bdt );
        
  #reader.BookMVA(TString("TMVA::BDT"), TString("root://cmseos.fnal.gov//store/user/amodak/toConndor/TMVAClassification_BDT.weights.xml"))
  reader.BookMVA(TString("BDT"), TString("TMVAClassification_BDT.weights.xml"))

  #Event loop starts
  nent = tree.GetEntries()
  nQUE = int(options.nQueue)
  PRCS = int(options.nProcess)
  evtidx = 0
  elist = []
  #print nent
  #print "NQue, nPro", nQUE, PRCS
  for steps in range(0, nQUE):
    elist.append (steps*(nent//nQUE))  
  elist.append(nent)
  #print elist
  for entry in tree:
    if (nQUE != 1):
      #print elist[PRCS], elist[PRCS+1]
      #print evtidx
      if not (evtidx >= elist[PRCS] and evtidx < elist[PRCS+1]):
        #print evtidx
        evtidx += 1
        continue
      evtidx += 1
    if (isMC(file)): 
      Entry.Fill(1, float(entry.genWeight)*float(entry.puWeight))
      #runs = file.Get("Runs")
      #for itr in runs:
      #  if (year == "2018"):
      #    for nScale in range(itr.nLHEScaleSumw_):
      #      print "Index: ", nScale, ", nLHEScale: ", itr.LHEScaleSumw_[nScale]
    else: 
      Entry.Fill(1, 1)
    MassT = -1
    Lep4vec, Met4vec, leadJet_4vec = ROOT.TLorentzVector(), ROOT.TLorentzVector(), ROOT.TLorentzVector()
    Lep4vec.SetPtEtaPhiM(entry.event_Lepton_pt, entry.event_Lepton_eta, entry.event_Lepton_phi, entry.event_Lepton_mass)
    Met4vec.SetPtEtaPhiM(MET_pt(file, entry, "nominal"), 0, MET_phi(file, entry, "nominal"), 0)
    MassT = math.sqrt(2*entry.event_Lepton_pt*MET_pt(file, entry, "nominal")*(1 - math.cos(Lep4vec.DeltaPhi(Met4vec))))
 
    leadJet_4vec.SetPtEtaPhiM(entry.event_LeadJet_pt, entry.event_LeadJet_eta, entry.event_LeadJet_phi, entry.event_LeadJet_mass)
    #Add JS variables 
    MV = ROOT.TLorentzVector()
    MV = Lep4vec + leadJet_4vec
    Mt_js  = math.sqrt(MV.M()**2 + 2*(math.sqrt(MV.M()**2 + MV.Pt()**2)*Met4vec.Pt() - (MV.Px()*Met4vec.Px() + MV.Py()*Met4vec.Py())))
    Mw_js  = math.sqrt((2*(Lep4vec.E()*MV.E() - Lep4vec.Pz()*MV.Pz())*(Met4vec.Pt()/math.sqrt(MV.M()**2 + MV.Pt()**2))) + Lep4vec.M2() -2*(Lep4vec.Px()*Met4vec.Px() + Lep4vec.Py()*Met4vec.Py()))
    #print Mt_js, Mw_js

    leadjetBTag = -999
    leadjetBTagTig = -999
    nBTag  = 0
    #Tuned to "Medium" WP, change here if needed
    if (useDeepCSV): 
      nBTag = entry.event_nBTagMed_DeepCSV
      leadjetBTag = btagDeepCSV(entry.event_LeadJet_btag_DeepCSV, "Medium")
      leadjetBTagTig = btagDeepCSV(entry.event_LeadJet_btag_DeepCSV, "Tight")
    else:
      nBTag = entry.event_nBTagMed_DeepFLV
      leadjetBTag = btagDeepFLV(entry.event_LeadJet_btag_DeepFLV, "Medium")
      leadjetBTagTig = btagDeepFLV(entry.event_LeadJet_btag_DeepFLV, "Tight")

    HT_Central =  0
    HT_Full_nopu_var =  0
    HT_Full_pu_var =  0
    nCentralJets_v2 = 0
    dR_lepClosestJet_v2 = 999
    lepClosestJet_v2_idx = -1
    fwdJet_T_idx = -1
    lepton4vec = ROOT.TLorentzVector()
    lepton4vec.SetPtEtaPhiM(entry.event_Lepton_pt, entry.event_Lepton_eta, entry.event_Lepton_phi, entry.event_Lepton_mass)
    HEMwt  = 1
    FwdJetPt_L = -999
    FwdJetPt_M = -999
    FwdJetPt_T = -999
    fwdJeteta_L =  0
    fwdJeteta_M =  0
    fwdJeteta_T =  0

    #print "Ele Trig from store: ", entry.event_EleTrigFlag, " direct: ", (entry.HLT_Photon175 or entry.HLT_Ele27_WPTight_Gsf)
    for j in range(0, entry.nJet):
       if (Jet_pt(file, entry, j, "nominal") > 30 and JetID(entry.Jet_jetId[j]) >= 1 and abs(entry.Jet_eta[j]) < 5.0): ##Changed
         #Check HEM for all central jets
         if (HEMwt == 1 and applyHEM(file, entry, j) == 0): HEMwt = 0
         jet4vec = ROOT.TLorentzVector()
         jet4vec.SetPtEtaPhiM(Jet_pt(file, entry, j, "nominal"), entry.Jet_eta[j], entry.Jet_phi[j], Jet_mass(file, entry, j, "nominal"))
         if (jet4vec.DeltaR(Lep4vec) > 0.5 and abs(entry.Jet_eta[j]) < 5.0 and JetPUID(entry.Jet_puId[j], Jet_pt(file, entry, j, "nominal"), "Tight") >= 1):
           HT_Full_pu_var += jet4vec.Pt()
         if (jet4vec.DeltaR(Lep4vec) > 0.5 and abs(entry.Jet_eta[j]) < 5.0):
           HT_Full_nopu_var += jet4vec.Pt()
         if (jet4vec.DeltaR(Lep4vec) > 0.5 and abs(entry.Jet_eta[j]) < 2.4 and JetPUID(entry.Jet_puId[j], Jet_pt(file, entry, j, "nominal"), "Tight") >= 1):
           HT_Central += jet4vec.Pt()
           nCentralJets_v2 += 1

         if (jet4vec.DeltaR(Lep4vec) > 0.5 and jet4vec.DeltaR(lepton4vec) < dR_lepClosestJet_v2 and Jet_pt(file, entry, j, "nominal") > 40 and JetPUID(entry.Jet_puId[j], Jet_pt(file, entry, j, "nominal"), "Tight") >= 1): 
           dR_lepClosestJet_v2 = jet4vec.DeltaR(lepton4vec)
           lepClosestJet_v2_idx = j

         if (jet4vec.DeltaR(Lep4vec) > 0.5 and abs(entry.Jet_eta[j]) > abs(fwdJeteta_L)  and JetPUID(entry.Jet_puId[j], Jet_pt(file, entry, j, "nominal"), "Loose") >= 1):
           fwdJeteta_L = entry.Jet_eta[j]
           FwdJetPt_L = Jet_pt(file, entry, j, "nominal")
         if (jet4vec.DeltaR(Lep4vec) > 0.5 and abs(entry.Jet_eta[j]) > abs(fwdJeteta_M)  and JetPUID(entry.Jet_puId[j], Jet_pt(file, entry, j, "nominal"), "Medium") >= 1):
           fwdJeteta_M = entry.Jet_eta[j]
           FwdJetPt_M = Jet_pt(file, entry, j, "nominal")
         if (jet4vec.DeltaR(Lep4vec) > 0.5 and abs(entry.Jet_eta[j]) > abs(fwdJeteta_T)  and JetPUID(entry.Jet_puId[j], Jet_pt(file, entry, j, "nominal"), "Tight") >= 1):
           fwdJeteta_T = entry.Jet_eta[j]
           FwdJetPt_T = Jet_pt(file, entry, j, "nominal")
           fwdJet_T_idx = j

    lepClosestJet_v2_4vec = ROOT.TLorentzVector()
    if (lepClosestJet_v2_idx >= 0): 
      lepClosestJet_v2_4vec.SetPtEtaPhiM(Jet_pt(file, entry, lepClosestJet_v2_idx, "nominal"), entry.Jet_eta[lepClosestJet_v2_idx], entry.Jet_phi[lepClosestJet_v2_idx], Jet_mass(file, entry, lepClosestJet_v2_idx, "nominal"))
    fwdJet_T_4vec = ROOT.TLorentzVector()
    if (fwdJet_T_idx >= 0):
      fwdJet_T_4vec.SetPtEtaPhiM(FwdJetPt_T, fwdJeteta_T, entry.Jet_phi[fwdJet_T_idx], Jet_mass(file, entry, fwdJet_T_idx, "nominal"))

    #Recalculate additional leptons
    nmuons =  0
    for mu in range(0, entry.nMuon):
      if (entry.Muon_pt[mu] > 10 and abs(entry.Muon_eta[mu]) < 2.1 and abs (entry.Muon_dxy[mu]) < 0.05 and 
          (entry.Muon_dz[mu]) < 0.2 and entry.Muon_looseId[mu] > 0 and entry.Muon_pfRelIso04_all[mu] < 0.25): nmuons += 1

    neles = 0
    for el in range(0, entry.nElectron):
      etaSC = entry.Electron_eta[el] + entry.Electron_deltaEtaSC[el]
      if (abs(etaSC) <=  1.479):
        if (entry.Electron_pt[el] > 20 and abs(entry.Electron_dxy[el]) < 0.05 and abs(entry.Electron_dz[el]) < 0.1
            and entry.Electron_cutBased[el] >= 1): neles += 1
      elif (abs(etaSC) >  1.479 and abs(etaSC) < 2.1):
        if (entry.Electron_pt[el] > 20 and abs(entry.Electron_dxy[el]) < 0.10 and abs(entry.Electron_dz[el]) < 0.2
            and entry.Electron_cutBased[el] >= 1): neles += 1
      
    nVeto = nmuons + neles

    btagSF_evt_tree = 1
    btagSF_evt_tree_hf = 1
    btagSF_evt_tree_lf = 1
    btagSF_evt_treeUp_hf = 1
    btagSF_evt_treeDown_hf = 1
    btagSF_evt_treeUp_lf = 1
    btagSF_evt_treeDown_lf = 1

    #Event weights initialized
    evtwt = 1
    evtwt_nominal = 1
    evtwt_nominal_nopu = 1
    evtwt_nominalv2 = 1
    evtwt_PileupUp = 1
    evtwt_PileupDown = 1
    evtwt_BTaghfSFUp = 1
    evtwt_BTaghfSFDown = 1
    evtwt_BTaglfSFUp = 1
    evtwt_BTaglfSFDown = 1
    evtwt_topptweightUp = 1
    evtwt_topptweightDown = 1
    evtwt_lhescale1 = 1   
    evtwt_lhescale2 = 1   
    evtwt_lhescale3 = 1   
    evtwt_lhescale4 = 1   
    evtwt_lhescale6 = 1   
    evtwt_lhescale8 = 1   
    evtwt_LHEScaleDownWeight  = 1
    evtwt_LHEScaleUpWeight = 1
    evtwt_LHEPdfDownWeight  = 1
    evtwt_LHEPdfUpWeight = 1
    evtwt_TTJets_ST_ScalingUp = 1
    evtwt_TTJets_ST_ScalingDown = 1

    if (HEMwt == 0): 
      Jet_eta_phi_2D.Fill(entry.event_LeadJet_eta, entry.event_LeadJet_phi, evtwt)
      Lep_eta_phi_2D.Fill(entry.event_Lepton_eta, entry.event_Lepton_phi, evtwt)

    #Apply HEM
    if (isMC(file) and year == "2018" and HEMwt == 0):
      #ran_proportion = random.uniform(0, 1)
      #print "ran_proportion", ran_proportion                                                                                               
      #if (ran_proportion > 0.65): HEMwt = 1
      HEMwt = 0.35
    evtwt  *= HEMwt

    #Event weights: this makes sure evtwt is getting applied on data as well, needed for HEM
    if not (isMC(file)):
     evtwt_nominal *= evtwt
     evtwt_nominal_nopu *= evtwt
     evtwt_nominalv2 *= evtwt
     evtwt_PileupUp *= evtwt
     evtwt_PileupDown *= evtwt
     evtwt_BTaghfSFUp *= evtwt
     evtwt_BTaghfSFDown *= evtwt
     evtwt_BTaglfSFUp *= evtwt
     evtwt_BTaglfSFDown *= evtwt
     evtwt_topptweightUp *= evtwt
     evtwt_topptweightDown *= evtwt
     evtwt_LHEScaleUpWeight *= evtwt
     evtwt_LHEScaleDownWeight *= evtwt
     evtwt_LHEPdfUpWeight *= evtwt
     evtwt_LHEPdfDownWeight *= evtwt
     evtwt_TTJets_ST_ScalingUp *=evtwt
     evtwt_TTJets_ST_ScalingDown *=evtwt

    if (isMC(file)):
      #random.seed(0)
      #nBTag  = 0
      for j in range(0, entry.nJet):
         if (Jet_pt(file, entry, j, "nominal") > 50 and abs(entry.Jet_eta[j]) < 2.4 and JetID(entry.Jet_jetId[j]) >= 1):  ##Changed
          jet4vec = ROOT.TLorentzVector()
          jet4vec.SetPtEtaPhiM(Jet_pt(file, entry, j, "nominal"), entry.Jet_eta[j], entry.Jet_phi[j], Jet_mass(file, entry, j, "nominal"))
          if (jet4vec.DeltaR(Lep4vec) > 0.5):
            
            #ran = random.uniform(0, 1)
            if (useDeepCSV): 
                if (btagDeepCSV(entry.Jet_btagDeepB[j], "Medium") >= 1): 
                  btagSF_evt_tree *= entry.Jet_btagSF_deepcsv[j]
                else:
                  btagSF_evt_tree *= (1 - min(1.0 , Jet_bTagEff(entry, j)*entry.Jet_btagSF_deepcsv[j]))/(1 - Jet_bTagEff(entry, j))
            else: 
                sfUpAbs = entry.Jet_btagSF_deepjet_M[j] + (entry.Jet_btagSF_deepjet_M_up[j] - entry.Jet_btagSF_deepjet_M[j])/entry.Jet_btagSF_deepjet_M[j]
                sfDownAbs = entry.Jet_btagSF_deepjet_M[j] + (entry.Jet_btagSF_deepjet_M_down[j] - entry.Jet_btagSF_deepjet_M[j])/entry.Jet_btagSF_deepjet_M[j]
                if (sfUpAbs < 0): 
                  sfUpAbs = 0
                if (sfDownAbs < 0): 
                  sfDownAbs = 0
                #print "per jet sf nom, up, down", entry.Jet_btagSF_deepjet_M[j], entry.Jet_btagSF_deepjet_M_up[j], entry.Jet_btagSF_deepjet_M_down[j]
                #print "sfUpAbs, sfDownAbs, flav", sfUpAbs, sfDownAbs, Jet_flavour(entry, j)
                if (btagDeepFLV(entry.Jet_btagDeepFlavB[j], "Medium") >= 1): 
                  btagSF_evt_tree *= entry.Jet_btagSF_deepjet_M[j]
                  if (Jet_flavour(entry, j) == 0 or Jet_flavour(entry, j) == 1):
                    btagSF_evt_tree_hf *= entry.Jet_btagSF_deepjet_M[j]
                    btagSF_evt_treeUp_hf *= sfUpAbs
                    btagSF_evt_treeDown_hf *= sfDownAbs
                  else:
                    btagSF_evt_tree_lf *= entry.Jet_btagSF_deepjet_M[j]
                    btagSF_evt_treeUp_lf *= sfUpAbs
                    btagSF_evt_treeDown_lf *= sfDownAbs
                else:
                  btagSF_evt_tree *= (1 - min(1.0 , Jet_bTagEff(entry, j)*entry.Jet_btagSF_deepjet_M[j]))/(1 - Jet_bTagEff(entry, j))
                  if (Jet_flavour(entry, j) == 0 or Jet_flavour(entry, j) == 1):
                    btagSF_evt_treeUp_hf *= (1 - min(1.0 , Jet_bTagEff(entry, j)*sfUpAbs))/(1 - Jet_bTagEff(entry, j))
                    btagSF_evt_treeDown_hf *= (1 - min(1.0 , Jet_bTagEff(entry, j)*sfDownAbs))/(1 - Jet_bTagEff(entry, j))
                    btagSF_evt_tree_hf *= (1 - min(1.0 , Jet_bTagEff(entry, j)*entry.Jet_btagSF_deepjet_M[j]))/(1 - Jet_bTagEff(entry, j))
                  else:
                    btagSF_evt_treeUp_lf *= (1 - min(1.0 , Jet_bTagEff(entry, j)*sfUpAbs))/(1 - Jet_bTagEff(entry, j))
                    btagSF_evt_treeDown_lf *= (1 - min(1.0 , Jet_bTagEff(entry, j)*sfDownAbs))/(1 - Jet_bTagEff(entry, j))
                    btagSF_evt_tree_lf *= (1 - min(1.0 , Jet_bTagEff(entry, j)*entry.Jet_btagSF_deepjet_M[j]))/(1 - Jet_bTagEff(entry, j))

      #print "btagSF_nom, nom_hf, nom_lf, hf_up, hf_down, lf_up, lf_down", btagSF_evt_tree, btagSF_evt_tree_hf, btagSF_evt_tree_lf, btagSF_evt_treeUp_hf, btagSF_evt_treeDown_hf, btagSF_evt_treeUp_lf, btagSF_evt_treeDown_lf
      #Define weights here 
      #muHPtSF = applyHighPtMuonSF (file, entry, Lep4vec.P(), Lep4vec.Eta())
      #evtwt *= muHPtSF
      topptweight=1
      topptweightUp=1
      topptweightDown=1
      LHEScaleUpWeight  = 1
      LHEScaleDownWeight  = 1
      LHEPdfUpWeight  = 1
      LHEPdfDownWeight  = 1
      TTJets_ST_ScalingUp = 1
      TTJets_ST_ScalingDown = st_TTJets_Scaling(file, entry)*st_TTJets_Scaling(file, entry)

      if (doSys):
        #Scale
        lhe = []
        if (hasattr(entry, 'nLHEScaleWeight') and hasattr(entry, 'LHEScaleWeight')):
          for nScale in range(entry.nLHEScaleWeight):
            if not (nScale == 0 or nScale == 5 or nScale == 7):
              #print "LHEScale : ", nScale, ", weight ", entry.LHEScaleWeight[nScale]
              lhe.append(entry.LHEScaleWeight[nScale])
          lhe.sort()
          #print len(lhe)
          if (len(lhe)): 
            LHEScaleDownWeight = lhe[0]
            LHEScaleUpWeight = lhe[len(lhe) - 1]
          #print "LHEScaleDownWeight: ", LHEScaleDownWeight, ", LHEScaleUpWeight: ", LHEScaleUpWeight
        #PDF
        if (hasattr(entry, 'nLHEPdfWeight') and hasattr(entry, 'LHEPdfWeight') and entry.nLHEPdfWeight > 0):
          PDFWeightPlus, PDFWeightMinus = 0, 0
          #print entry.nLHEPdfWeight, entry.LHEPdfWeight[0]
          for nPdf in range((entry.nLHEPdfWeight -1)/2):
            PDFWeightPlus += max ((entry.LHEPdfWeight[2*nPdf+2] - entry.LHEPdfWeight[0]), (entry.LHEPdfWeight[2*nPdf+1] - entry.LHEPdfWeight[0]), 0)**2
            PDFWeightMinus += max ((entry.LHEPdfWeight[0] - entry.LHEPdfWeight[2*nPdf+2]), (entry.LHEPdfWeight[0] - entry.LHEPdfWeight[2*nPdf+1]), 0)**2
          if (entry.LHEPdfWeight[0] != 0): 
            LHEPdfUpWeight = 1 + (math.sqrt(PDFWeightPlus)/entry.LHEPdfWeight[0])
          if (entry.LHEPdfWeight[0] != 0): 
            LHEPdfDownWeight = 1 - (math.sqrt(PDFWeightMinus)/entry.LHEPdfWeight[0])
          #print "LHEPDFUpWeight, LHEPDFDownWeight", LHEPdfUpWeight, LHEPdfDownWeight


      if (year  == "2016"):
          if (float(entry.event_Event_flag) == 13 and float(entry.event_MuonTrigFlag) >= 1):
            idsf=float((0.45*float(entry.Muon_ID_GH_SF[entry.event_Lepton_idx]))+(0.55*float(entry.Muon_ID_BCDEF_SF[entry.event_Lepton_idx])))
            isosf=np.add(np.multiply(0.45,entry.Muon_ISO_GH_SF[entry.event_Lepton_idx]), np.multiply(0.55,entry.Muon_ISO_BCDEF_SF[entry.event_Lepton_idx]))
            trigsf=np.add(np.multiply(0.45,entry.Muon_Trigger_GH_SF[entry.event_Lepton_idx]), np.multiply(0.55,entry.Muon_Trigger_BCDEF_SF[entry.event_Lepton_idx]))
            evtwt *= (float(idsf)*float(isosf)*float(trigsf))
          elif (entry.event_Event_flag == 11 and entry.event_EleTrigFlag >= 1):
            idisosf     = float(entry.Electron_IDISO_BCDEFGH_SF[entry.event_Lepton_idx])
            recosf      = float(entry.Electron_RECO_BCDEFGH_SF[entry.event_Lepton_idx])
            #trigsf      = float(entry.Electron_Trigger_BCDEFGH_SF[entry.event_Lepton_idx])
            trigsf = 1
            evtwt  *= (float(idisosf)*float(recosf)*float(trigsf))
          if (isTTJet(file) and applyTopPtRew): 
            #topptweight=float(entry.TopPt_Weight_Nominal)
            #Fit with M_lmj classification
            #topptweight= topPt_Reweighting(file, entry, 0.0615*2.11, 0.0005*0.514)
            #Fit with M_lj classification
            #topptweight= topPt_Reweighting(file, entry, 0.0615*1.333, 0.0005*0.0266)
            #Fit with M_lmj classification and only beta (v2)
            #topptweight= topPt_Reweighting(file, entry, 0.0615*0.0, -0.0005*0.0976)
            #based on ST_v2 fit, only beta
            #topptweight= topPt_Reweighting(file, entry, 0.0615*0.0, -0.0005*0.99)
            #based on ST_v2 fit, norm and beta
            topptweight= topPt_Reweighting(file, entry, 0.0615*0.0, 0.0005*0.877)
            topptweightDown= topptweight*topptweight
      elif (year == "2017"):
          if (float(entry.event_Event_flag) == 13 and float(entry.event_MuonTrigFlag) >= 1):
            idsf=float(entry.Muon_ID_BCDEF_SF[entry.event_Lepton_idx])
            isosf=float(entry.Muon_ISO_BCDEF_SF[entry.event_Lepton_idx])
            trigsf=float(entry.Muon_Trigger_BCDEF_SF[entry.event_Lepton_idx])
            evtwt *= (float(idsf)*float(isosf)*float(trigsf))
          elif (entry.event_Event_flag == 11 and entry.event_EleTrigFlag >= 1):
            idisosf     = float(entry.Electron_IDISO_BCDEF_SF[entry.event_Lepton_idx])
            recosf      = float(entry.Electron_RECO_BCDEF_SF[entry.event_Lepton_idx])
            #trigsf      = float(entry.Electron_Trigger_BCDEF_SF[entry.event_Lepton_idx])
            evtwt  *= (float(idisosf)*float(recosf))
          if (isTTJet(file) and applyTopPtRew): 
            #topptweight=float(entry.TopPt_Weight_Nominal)
            #topptweight= topPt_Reweighting(file, entry, 0.0615*2.11, 0.0005*0.514)
            #topptweight= topPt_Reweighting(file, entry, 0.0615*1.333, 0.0005*0.0266)
            #topptweight= topPt_Reweighting(file, entry, 0.0615*0.0, -0.0005*0.0976)
            #based on ST_v2 fit
            #topptweight= topPt_Reweighting(file, entry, 0.0615*0.0, -0.0005*0.99)
            #based on ST_v2 fit, norm and beta
            topptweight= topPt_Reweighting(file, entry, 0.0615*0.0, 0.0005*0.877)
            topptweightDown= topptweight*topptweight
      elif (year == "2018"):
          if (float(entry.event_Event_flag) == 13 and float(entry.event_MuonTrigFlag) >= 1):
            idsf=float(entry.Muon_ID_ABCD_SF[entry.event_Lepton_idx])
            isosf=float(entry.Muon_ISO_ABCD_SF[entry.event_Lepton_idx])
            #trigsfBefore=float(trig_SF(file, tree_muTrigBefore, entry.event_Lepton_pt, entry.event_Lepton_eta, "Muon"))
            #trigsfAfter=float(trig_SF(file, tree_muTrigAfter, entry.event_Lepton_pt, entry.event_Lepton_eta, "Muon"))
            trigsfBefore = entry.Muon_TriggerBefore_ABCD_SF[entry.event_Lepton_idx]
            trigsfAfter  = entry.Muon_TriggerAfter_ABCD_SF[entry.event_Lepton_idx]
            trigsf = float(((trigsfBefore*7.0) + (trigsfAfter*53.0))/60.0)
            #print "idsf ", idsf, " isosf ", isosf, " trigsf", trigsf
            evtwt *= (float(idsf)*float(isosf)*float(trigsf))
          elif (entry.event_Event_flag == 11 and entry.event_EleTrigFlag >= 1):
            idisosf     = float(entry.Electron_IDISO_ABCD_SF[entry.event_Lepton_idx])
            recosf      = float(entry.Electron_RECO_ABCD_SF[entry.event_Lepton_idx])
            #trigsf      = float(entry.Electron_Trigger_ABCD_SF[entry.event_Lepton_idx])
            evtwt  *= (float(idisosf)*float(recosf))
          if (isTTJet(file) and applyTopPtRew): 
            #topptweight=float(entry.TopPt_Weight_Nominal)
            #topptweight= topPt_Reweighting(file, entry, 0.0615*2.11, 0.0005*0.514)
            #topptweight= topPt_Reweighting(file, entry, 0.0615*1.333, 0.0005*0.0266)
            #topptweight= topPt_Reweighting(file, entry, 0.0615*0.0, -0.0005*0.0976)
            #based on ST_v2 fit
            #topptweight= topPt_Reweighting(file, entry, 0.0615*0.0, -0.0005*0.99)
            #based on ST_v2 fit, norm and beta
            topptweight= topPt_Reweighting(file, entry, 0.0615*0.0, 0.0005*0.877)
            topptweightDown= topptweight*topptweight

      #evtwt_nominal = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree)*float(topptweight)*tm_WJets_Scaling(file, tree, (entry.ST_v2+HT_Central-entry.LeadJet_pt))
      evtwt_nominal = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
      evtwt_nominal_nopu = evtwt*float(entry.genWeight)*float(btagSF_evt_tree)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
      evtwt_nominalv2 = evtwt*float(entry.puWeight)*float(btagSF_evt_tree)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
      if (doSys):
        evtwt_PileupUp = evtwt*float(entry.genWeight)*float(entry.puWeightUp)*float(btagSF_evt_tree)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
        evtwt_PileupDown = evtwt*float(entry.genWeight)*float(entry.puWeightDown)*float(btagSF_evt_tree)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
        evtwt_BTaghfSFUp = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree_lf)*float(btagSF_evt_treeUp_hf)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
        evtwt_BTaghfSFDown = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree_lf)*float(btagSF_evt_treeDown_hf)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
        evtwt_BTaglfSFUp = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree_hf)*float(btagSF_evt_treeUp_lf)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
        evtwt_BTaglfSFDown = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree_hf)*float(btagSF_evt_treeDown_lf)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
        evtwt_topptweightUp = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree)*float(topptweightUp)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
        evtwt_topptweightDown = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree)*float(topptweightDown)*float(wpt_WJets_Scaling(file, entry))*st_TTJets_Scaling(file, entry)
        evtwt_LHEScaleDownWeight =  evtwt_nominal*LHEScaleDownWeight
        evtwt_LHEScaleUpWeight =  evtwt_nominal*LHEScaleUpWeight
        evtwt_LHEPdfDownWeight =  evtwt_nominal*LHEPdfDownWeight
        evtwt_LHEPdfUpWeight =  evtwt_nominal*LHEPdfUpWeight
        evtwt_TTJets_ST_ScalingUp = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*TTJets_ST_ScalingUp
        evtwt_TTJets_ST_ScalingDown = evtwt*float(entry.genWeight)*float(entry.puWeight)*float(btagSF_evt_tree)*float(topptweight)*float(wpt_WJets_Scaling(file, entry))*TTJets_ST_ScalingDown

        #print "evtwt_nominal, evtwt_BTaghfSFUp, evtwt_BTaghfSFDown, evtwt_BTaglfSFUp, evtwt_BTaglfSFDown", evtwt_nominal, evtwt_BTaghfSFUp, evtwt_BTaghfSFDown, evtwt_BTaglfSFUp, evtwt_BTaglfSFDown
        if (hasattr(entry, 'LHEScaleWeight')  and entry.nLHEScaleWeight): 
          evtwt_lhescale1 =  evtwt_nominal*entry.LHEScaleWeight[1]  
          evtwt_lhescale2 =  evtwt_nominal*entry.LHEScaleWeight[2]  
          evtwt_lhescale3 =  evtwt_nominal*entry.LHEScaleWeight[3]  
          evtwt_lhescale4 =  evtwt_nominal*entry.LHEScaleWeight[4]  
          evtwt_lhescale6 =  evtwt_nominal*entry.LHEScaleWeight[6]  
          evtwt_lhescale8 =  evtwt_nominal*(entry.LHEScaleWeight[8] if 8 < entry.nLHEScaleWeight else 1)

    #WJets Selection
    #if (MET_Filters(file, entry) >= 1 and entry.Lepton_pt > 40 and abs(entry.Lepton_eta) < 2.1 and entry.LeadJet_pt > 200 and abs(entry.LeadJet_eta) < 2.4 and entry.DR_LepleadJet > 0.5 and nBTag == 0 and MET_pt(file, entry) > 60 and entry.ST_v2 > 500 and  HT_Central > 400 and MassT > 40 and abs(entry.DPHI_LepMet) < 0.5 and abs(entry.DPHI_LepleadJet) > 2.0 and (entry.event_nVetoMuons + entry.event_nVetoElectrons) == 1):


    #Signal Pre Selection
    #BTag Eff Map after a preselection
    if (isMC(file)):
      if (MET_Filters(file, entry) >= 1 and entry.event_Lepton_pt > 40 and abs(entry.event_Lepton_eta) < 2.1 and entry.event_LeadJet_pt > 200 and abs(entry.event_LeadJet_eta) < 2.4 and entry.event_DR_LepleadJet > 0.5 and MET_pt(file, entry, "nominal") > 60 and entry.event_ST_v2 > 500 and  HT_Central > 500 and MassT < 130):
        if (entry.event_nJets > 0):
          for j in range(0, entry.event_nJets):
            if (Jet_pt(file, entry, j, "nominal") > 30 and abs(entry.Jet_eta[j]) < 2.4 and JetID(entry.Jet_jetId[j]) >= 1):
              if (Jet_flavour(entry, j) == 0): effmap_pt_eta_b_all.Fill(Jet_pt(file, entry, j, "nominal"), entry.Jet_eta[j])
              elif (Jet_flavour(entry, j) == 1): effmap_pt_eta_c_all.Fill(Jet_pt(file, entry, j, "nominal"), entry.Jet_eta[j]) 
              elif (Jet_flavour(entry, j) == 2): effmap_pt_eta_l_all.Fill(Jet_pt(file, entry, j, "nominal"), entry.Jet_eta[j]) 
          
              if (Jet_pt(file, entry, j, "nominal") > 30 and abs(entry.Jet_eta[j]) < 2.4 and JetID(entry.Jet_jetId[j]) >= 1 and btagDeepFLV(entry.Jet_btagDeepFlavB[j], "Medium") >= 1):
                if (Jet_flavour(entry, j) == 0): effmap_pt_eta_b_btagged.Fill(Jet_pt(file, entry, j, "nominal"), entry.Jet_eta[j]) 
                elif (Jet_flavour(entry, j) == 1): effmap_pt_eta_c_btagged.Fill(Jet_pt(file, entry, j, "nominal"), entry.Jet_eta[j]) 
                elif (Jet_flavour(entry, j) == 2): effmap_pt_eta_l_btagged.Fill(Jet_pt(file, entry, j, "nominal"), entry.Jet_eta[j]) 


    LepCJ_pt = lepClosestJet_v2_4vec.Pt()
    LepCJ_eta = lepClosestJet_v2_4vec.Eta()
    if (lepClosestJet_v2_idx >= 0): LepCJ_btag = entry.Jet_btagDeepFlavB[lepClosestJet_v2_idx]
    else: LepCJ_btag = 0
    M_Top = (Lep4vec + Met4vec + lepClosestJet_v2_4vec).M()
    Mjj_lJ_CJ = (leadJet_4vec + lepClosestJet_v2_4vec).M()
    DR_lJ_CJ = leadJet_4vec.DeltaR(lepClosestJet_v2_4vec)
    
    for cr in range(len(controlR)):
      bdtOutput, cut = cutFlow(file, entry, str(controlR[cr]), leadjetBTag, HT_Central, nBTag, MassT, LepCJ_pt, LepCJ_eta, LepCJ_btag, M_Top, Mjj_lJ_CJ, DR_lJ_CJ, HT_Full_pu_var, reader, MET_pt(file, entry, "nominal"), float(entry.event_LeadJet_pt), dR_lepClosestJet_v2, nVeto)
      for itr in range (0, int(cut)):
          ch = ""
          if (float(entry.event_Event_flag) == 13 and float(entry.event_MuonTrigFlag) >= 1):
            ch = "Mu"
          elif (entry.event_Event_flag == 11 and entry.event_EleTrigFlag >= 1):
            ch = "Ele"
          else:
            continue
          hmap[str(ch)+"_"+str(controlR[cr])+"_Counter"].Fill(itr, evtwt_nominal)
          if (itr == 16):
            fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "nominal", evtwt_nominal)
            if (doSys):
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "NoPileup", evtwt_nominal_nopu)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,"topptweightUp", evtwt_topptweightUp)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,"topptweightDown", evtwt_topptweightDown)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt, "BTaghfSFUp", evtwt_BTaghfSFUp)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt, "BTaghfSFDown", evtwt_BTaghfSFDown)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt, "BTaglfSFUp", evtwt_BTaglfSFUp)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt, "BTaglfSFDown", evtwt_BTaglfSFDown)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "PileupUp", evtwt_PileupUp)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "PileupDown", evtwt_PileupDown)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEScale1", evtwt_lhescale1)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEScale2", evtwt_lhescale2)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEScale3", evtwt_lhescale3)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEScale4", evtwt_lhescale4)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEScale6", evtwt_lhescale6)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEScale8", evtwt_lhescale8)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEScaleDownWeight", evtwt_LHEScaleDownWeight)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEScaleUpWeight", evtwt_LHEScaleUpWeight)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEPdfDownWeight", evtwt_LHEPdfDownWeight)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "LHEPdfUpWeight", evtwt_LHEPdfUpWeight)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "TTJets_ST_ScalingUp", evtwt_TTJets_ST_ScalingUp)
              fillHisto(str(controlR[cr]), str(ch), file, entry, itr, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutput, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "nominal"), MET_phi(file, entry, "nominal"), entry.event_LeadJet_pt,  "TTJets_ST_ScalingDown", evtwt_TTJets_ST_ScalingDown)


          if (itr == 4 and doSkim and "PreSig" in str(controlR[cr])):
            #Nominal 
            loadSkimTree(file, entry, event_genwt, leadjetBTag, MassT, HT_Central, HT_Full_pu_var, HT_Full_nopu_var, neles, nmuons, evtwt_nominal, evtwt_PileupUp, evtwt_PileupDown, evtwt_BTaghfSFUp, evtwt_BTaghfSFDown, evtwt_BTaglfSFUp, evtwt_BTaglfSFDown, evtwt_LHEScaleUpWeight, evtwt_LHEScaleDownWeight, evtwt_LHEPdfUpWeight, evtwt_LHEPdfDownWeight, evtwt_TTJets_ST_ScalingUp, evtwt_TTJets_ST_ScalingDown, lepClosestJet_v2_4vec, Lep4vec, Met4vec, leadJet_4vec, leadjetBTagTig, FwdJetPt_T, fwdJeteta_T, Mt_js, Mw_js, MET_pt(file, entry, "nominal"), entry.event_LeadJet_pt)
            sktree.Fill()

    #Signal Selection
    #if (MET_Filters(file, entry) >= 1 and entry.event_Lepton_pt > 40 and abs(entry.event_Lepton_eta) < 2.1 and entry.event_LeadJet_pt > 200 and abs(entry.event_LeadJet_eta) < 2.4 and entry.event_DR_LepleadJet > 0.5 and entry.event_DR_LepClosestJet > 1.5 and abs(entry.event_DPHI_LepMet) < 0.5 and entry.event_bVsW_ratio < 1.4 and abs(entry.event_DPHI_LepleadJet) > 2.0 and nBTag_30 >= 1 and MET_pt(file, entry) > 60 and entry.event_ST_v2 > 700 and  HT_Central > 500 and MassT < 130 and leadjetBTag >= 1 and (entry.event_nVetoMuons + entry.event_nVetoElectrons) == 1 and entry.event_FwdJetPt > 30 and abs(entry.event_FwdJetEta) > 2.4):


    #TTJets Selection
    #if (MET_Filters(file, entry) >= 1 and entry.event_Lepton_pt > 40 and abs(entry.event_Lepton_eta) < 2.1 and entry.event_LeadJet_pt > 200 and abs(entry.event_LeadJet_eta) < 2.4 and entry.event_DR_LepClosestJet <  1.5 and nBTag >= 2 and MET_pt(file, entry) > 60 and entry.event_ST_v2 > 200 and leadjetBTag  >= 1):


    #Systematics for JES and JER
    if (doSys):
      for cr in range(len(controlR)):
        bdtOutputjerUp, cutjerUp = cutFlow(file, entry, str(controlR[cr]), leadjetBTag, HT_Central, nBTag, MassT, LepCJ_pt, LepCJ_eta, LepCJ_btag, M_Top, Mjj_lJ_CJ, DR_lJ_CJ, HT_Full_pu_var, reader, MET_pt(file, entry, "jerUp"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jerUp"), dR_lepClosestJet_v2, nVeto)
        bdtOutputjerDown, cutjerDown = cutFlow(file, entry, str(controlR[cr]), leadjetBTag, HT_Central, nBTag, MassT, LepCJ_pt, LepCJ_eta, LepCJ_btag, M_Top, Mjj_lJ_CJ, DR_lJ_CJ, HT_Full_pu_var, reader, MET_pt(file, entry, "jerDown"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jerDown"), dR_lepClosestJet_v2, nVeto)
        bdtOutputjesUp, cutjesUp = cutFlow(file, entry, str(controlR[cr]), leadjetBTag, HT_Central, nBTag, MassT, LepCJ_pt, LepCJ_eta, LepCJ_btag, M_Top, Mjj_lJ_CJ, DR_lJ_CJ, HT_Full_pu_var, reader, MET_pt(file, entry, "jesUp"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jesUp"), dR_lepClosestJet_v2, nVeto)
        bdtOutputjesDown, cutjesDown = cutFlow(file, entry, str(controlR[cr]), leadjetBTag, HT_Central, nBTag, MassT, LepCJ_pt, LepCJ_eta, LepCJ_btag, M_Top, Mjj_lJ_CJ, DR_lJ_CJ, HT_Full_pu_var, reader, MET_pt(file, entry, "jesDown"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jesDown"), dR_lepClosestJet_v2, nVeto)
        CHL = ""
        if (float(entry.event_Event_flag) == 13 and float(entry.event_MuonTrigFlag) >= 1):
          CHL = "Mu"
        elif (entry.event_Event_flag == 11 and entry.event_EleTrigFlag >= 1):
          CHL = "Ele"
        if ('Mu' in CHL or 'Ele' in CHL):
          if (int(cutjerUp) == 17):   fillHisto(str(controlR[cr]), CHL, file, entry, int(cutjerUp)-1, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutputjerUp, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "jerUp"), MET_phi(file, entry, "jerUp"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jerUp"),  "jerUp", evtwt_nominal)
          if (int(cutjerDown) == 17): fillHisto(str(controlR[cr]), CHL, file, entry, int(cutjerDown)-1, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutputjerDown, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "jerDown"), MET_phi(file, entry, "jerDown"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jerDown"), "jerDown", evtwt_nominal)
          if (int(cutjesUp) == 17):   fillHisto(str(controlR[cr]), CHL, file, entry, int(cutjesUp)-1, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutputjesUp, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "jesUp"), MET_phi(file, entry, "jesUp"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jesUp"), "jesUp", evtwt_nominal)
          if (int(cutjesDown) == 17): fillHisto(str(controlR[cr]), CHL, file, entry, int(cutjesDown)-1, nBTag, MassT, Mt_js, HT_Central, leadjetBTagTig, bdtOutputjesDown, HEMwt, nCentralJets_v2, dR_lepClosestJet_v2, FwdJetPt_L, FwdJetPt_M, FwdJetPt_T, MET_pt(file, entry, "jesDown"), MET_phi(file, entry, "jesDown"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jesDown"),"jesDown", evtwt_nominal)

          if (int(cutjerUp) >= 5 and doSkim and "PreSig" in str(controlR[cr])):
            loadSkimTree(file, entry, event_genwt, leadjetBTag, MassT, HT_Central, HT_Full_pu_var, HT_Full_nopu_var, neles, nmuons, evtwt_nominal, evtwt_PileupUp, evtwt_PileupDown, evtwt_BTaghfSFUp, evtwt_BTaghfSFDown, evtwt_BTaglfSFUp, evtwt_BTaglfSFDown, evtwt_LHEScaleUpWeight, evtwt_LHEScaleDownWeight, evtwt_LHEPdfUpWeight, evtwt_LHEPdfDownWeight, evtwt_TTJets_ST_ScalingUp, evtwt_TTJets_ST_ScalingDown, lepClosestJet_v2_4vec, Lep4vec, Met4vec, leadJet_4vec, leadjetBTagTig, FwdJetPt_T, fwdJeteta_T, Mt_js, Mw_js, MET_pt(file, entry, "jerUp"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jerUp"))
            sktree_jerUp.Fill()
          if (int(cutjerDown) >= 5 and doSkim and "PreSig" in str(controlR[cr])):
            loadSkimTree(file, entry, event_genwt, leadjetBTag, MassT, HT_Central, HT_Full_pu_var, HT_Full_nopu_var, neles, nmuons, evtwt_nominal, evtwt_PileupUp, evtwt_PileupDown, evtwt_BTaghfSFUp, evtwt_BTaghfSFDown, evtwt_BTaglfSFUp, evtwt_BTaglfSFDown, evtwt_LHEScaleUpWeight, evtwt_LHEScaleDownWeight, evtwt_LHEPdfUpWeight, evtwt_LHEPdfDownWeight, evtwt_TTJets_ST_ScalingUp, evtwt_TTJets_ST_ScalingDown, lepClosestJet_v2_4vec, Lep4vec, Met4vec, leadJet_4vec, leadjetBTagTig, FwdJetPt_T, fwdJeteta_T, Mt_js, Mw_js, MET_pt(file, entry, "jerDown"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jerDown"))
            sktree_jerDown.Fill()
          if (int(cutjesUp) >= 5 and doSkim and "PreSig" in str(controlR[cr])):
            loadSkimTree(file, entry, event_genwt, leadjetBTag, MassT, HT_Central, HT_Full_pu_var, HT_Full_nopu_var, neles, nmuons, evtwt_nominal, evtwt_PileupUp, evtwt_PileupDown, evtwt_BTaghfSFUp, evtwt_BTaghfSFDown, evtwt_BTaglfSFUp, evtwt_BTaglfSFDown, evtwt_LHEScaleUpWeight, evtwt_LHEScaleDownWeight, evtwt_LHEPdfUpWeight, evtwt_LHEPdfDownWeight, evtwt_TTJets_ST_ScalingUp, evtwt_TTJets_ST_ScalingDown, lepClosestJet_v2_4vec, Lep4vec, Met4vec, leadJet_4vec, leadjetBTagTig, FwdJetPt_T, fwdJeteta_T, Mt_js, Mw_js, MET_pt(file, entry, "jesUp"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jesUp"))
            sktree_jesUp.Fill()
          if (int(cutjesDown) >= 5 and doSkim and "PreSig" in str(controlR[cr])):
            loadSkimTree(file, entry, event_genwt, leadjetBTag, MassT, HT_Central, HT_Full_pu_var, HT_Full_nopu_var, neles, nmuons, evtwt_nominal, evtwt_PileupUp, evtwt_PileupDown, evtwt_BTaghfSFUp, evtwt_BTaghfSFDown, evtwt_BTaglfSFUp, evtwt_BTaglfSFDown, evtwt_LHEScaleUpWeight, evtwt_LHEScaleDownWeight, evtwt_LHEPdfUpWeight, evtwt_LHEPdfDownWeight, evtwt_TTJets_ST_ScalingUp, evtwt_TTJets_ST_ScalingDown, lepClosestJet_v2_4vec, Lep4vec, Met4vec, leadJet_4vec, leadjetBTagTig, FwdJetPt_T, fwdJeteta_T, Mt_js, Mw_js, MET_pt(file, entry, "jesDown"), Jet_pt(file, entry, entry.event_LeadJet_idx, "jesDown"))
            sktree_jesDown.Fill()

  #canv.Modified()
  #canv.Update()
  outputfile.Write()
  outputfile.Close()
  print "Processing completed ... DONE"
