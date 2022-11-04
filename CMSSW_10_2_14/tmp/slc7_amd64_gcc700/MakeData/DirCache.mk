ALL_SUBSYSTEMS+=CondFormats
subdirs_src_CondFormats = src_CondFormats_BTauObjects
ALL_PACKAGES += CondFormats/BTauObjects
subdirs_src_CondFormats_BTauObjects := src_CondFormats_BTauObjects_interface src_CondFormats_BTauObjects_src src_CondFormats_BTauObjects_test
ifeq ($(strip $(testBTagCalibration)),)
testBTagCalibration := self/src/CondFormats/BTauObjects/test
testBTagCalibration_files := $(patsubst src/CondFormats/BTauObjects/test/%,%,$(foreach file,testBTagCalibration.cpp,$(eval xfile:=$(wildcard src/CondFormats/BTauObjects/test/$(file)))$(if $(xfile),$(xfile),$(warning No such file exists: src/CondFormats/BTauObjects/test/$(file). Please fix src/CondFormats/BTauObjects/test/BuildFile.))))
testBTagCalibration_TEST_RUNNER_CMD :=  testBTagCalibration 
testBTagCalibration_BuildFile    := $(WORKINGDIR)/cache/bf/src/CondFormats/BTauObjects/test/BuildFile
testBTagCalibration_LOC_USE := self  CondFormats/PhysicsToolsObjects CondFormats/BTauObjects
testBTagCalibration_PACKAGE := self/src/CondFormats/BTauObjects/test
ALL_PRODS += testBTagCalibration
testBTagCalibration_INIT_FUNC        += $$(eval $$(call Binary,testBTagCalibration,src/CondFormats/BTauObjects/test,src_CondFormats_BTauObjects_test,$(SCRAMSTORENAME_BIN),,$(SCRAMSTORENAME_TEST),test,$(SCRAMSTORENAME_LOGS)))
testBTagCalibration_CLASS := TEST
else
$(eval $(call MultipleWarningMsg,testBTagCalibration,src/CondFormats/BTauObjects/test))
endif
ifeq ($(strip $(testSerializationBTauObjects)),)
testSerializationBTauObjects := self/src/CondFormats/BTauObjects/test
testSerializationBTauObjects_files := $(patsubst src/CondFormats/BTauObjects/test/%,%,$(foreach file,testSerializationBTauObjects.cpp,$(eval xfile:=$(wildcard src/CondFormats/BTauObjects/test/$(file)))$(if $(xfile),$(xfile),$(warning No such file exists: src/CondFormats/BTauObjects/test/$(file). Please fix src/CondFormats/BTauObjects/test/BuildFile.))))
testSerializationBTauObjects_TEST_RUNNER_CMD :=  testSerializationBTauObjects 
testSerializationBTauObjects_BuildFile    := $(WORKINGDIR)/cache/bf/src/CondFormats/BTauObjects/test/BuildFile
testSerializationBTauObjects_LOC_USE := self  CondFormats/PhysicsToolsObjects CondFormats/BTauObjects
testSerializationBTauObjects_PACKAGE := self/src/CondFormats/BTauObjects/test
ALL_PRODS += testSerializationBTauObjects
testSerializationBTauObjects_INIT_FUNC        += $$(eval $$(call Binary,testSerializationBTauObjects,src/CondFormats/BTauObjects/test,src_CondFormats_BTauObjects_test,$(SCRAMSTORENAME_BIN),,$(SCRAMSTORENAME_TEST),test,$(SCRAMSTORENAME_LOGS)))
testSerializationBTauObjects_CLASS := TEST
else
$(eval $(call MultipleWarningMsg,testSerializationBTauObjects,src/CondFormats/BTauObjects/test))
endif
ifeq ($(strip $(testBTagEntry)),)
testBTagEntry := self/src/CondFormats/BTauObjects/test
testBTagEntry_files := $(patsubst src/CondFormats/BTauObjects/test/%,%,$(foreach file,testBTagEntry.cpp,$(eval xfile:=$(wildcard src/CondFormats/BTauObjects/test/$(file)))$(if $(xfile),$(xfile),$(warning No such file exists: src/CondFormats/BTauObjects/test/$(file). Please fix src/CondFormats/BTauObjects/test/BuildFile.))))
testBTagEntry_TEST_RUNNER_CMD :=  testBTagEntry 
testBTagEntry_BuildFile    := $(WORKINGDIR)/cache/bf/src/CondFormats/BTauObjects/test/BuildFile
testBTagEntry_LOC_USE := self  CondFormats/PhysicsToolsObjects CondFormats/BTauObjects
testBTagEntry_PACKAGE := self/src/CondFormats/BTauObjects/test
ALL_PRODS += testBTagEntry
testBTagEntry_INIT_FUNC        += $$(eval $$(call Binary,testBTagEntry,src/CondFormats/BTauObjects/test,src_CondFormats_BTauObjects_test,$(SCRAMSTORENAME_BIN),,$(SCRAMSTORENAME_TEST),test,$(SCRAMSTORENAME_LOGS)))
testBTagEntry_CLASS := TEST
else
$(eval $(call MultipleWarningMsg,testBTagEntry,src/CondFormats/BTauObjects/test))
endif
ALL_COMMONRULES += src_CondFormats_BTauObjects_test
src_CondFormats_BTauObjects_test_parent := CondFormats/BTauObjects
src_CondFormats_BTauObjects_test_INIT_FUNC += $$(eval $$(call CommonProductRules,src_CondFormats_BTauObjects_test,src/CondFormats/BTauObjects/test,TEST))
ALL_SUBSYSTEMS+=PhysicsTools
subdirs_src_PhysicsTools = src_PhysicsTools_NanoAODTools
ALL_PACKAGES += PhysicsTools/NanoAODTools
subdirs_src_PhysicsTools_NanoAODTools := src_PhysicsTools_NanoAODTools_python src_PhysicsTools_NanoAODTools_scripts src_PhysicsTools_NanoAODTools_src src_PhysicsTools_NanoAODTools_standalone
ifeq ($(strip $(PyPhysicsToolsNanoAODTools)),)
PyPhysicsToolsNanoAODTools := self/src/PhysicsTools/NanoAODTools/python
src_PhysicsTools_NanoAODTools_python_parent := 
ALL_PYTHON_DIRS += $(patsubst src/%,%,src/PhysicsTools/NanoAODTools/python)
PyPhysicsToolsNanoAODTools_files := $(patsubst src/PhysicsTools/NanoAODTools/python/%,%,$(wildcard $(foreach dir,src/PhysicsTools/NanoAODTools/python ,$(foreach ext,$(SRC_FILES_SUFFIXES),$(dir)/*.$(ext)))))
PyPhysicsToolsNanoAODTools_LOC_USE := self  
PyPhysicsToolsNanoAODTools_PACKAGE := self/src/PhysicsTools/NanoAODTools/python
ALL_PRODS += PyPhysicsToolsNanoAODTools
PyPhysicsToolsNanoAODTools_INIT_FUNC        += $$(eval $$(call PythonProduct,PyPhysicsToolsNanoAODTools,src/PhysicsTools/NanoAODTools/python,src_PhysicsTools_NanoAODTools_python,1,1,$(SCRAMSTORENAME_PYTHON),$(SCRAMSTORENAME_LIB),,))
else
$(eval $(call MultipleWarningMsg,PyPhysicsToolsNanoAODTools,src/PhysicsTools/NanoAODTools/python))
endif
ALL_COMMONRULES += src_PhysicsTools_NanoAODTools_python
src_PhysicsTools_NanoAODTools_python_INIT_FUNC += $$(eval $$(call CommonProductRules,src_PhysicsTools_NanoAODTools_python,src/PhysicsTools/NanoAODTools/python,PYTHON))
src_PhysicsTools_NanoAODTools_scripts_files := $(filter-out \#% %\#,$(notdir $(wildcard $(foreach dir,$(LOCALTOP)/src/PhysicsTools/NanoAODTools/scripts,$(dir)/*))))
$(eval $(call Src2StoreCopy,src_PhysicsTools_NanoAODTools_scripts,src/PhysicsTools/NanoAODTools/scripts,$(SCRAMSTORENAME_BIN),*))
