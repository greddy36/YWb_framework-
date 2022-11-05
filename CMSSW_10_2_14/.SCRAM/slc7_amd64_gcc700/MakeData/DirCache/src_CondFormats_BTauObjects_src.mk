ifeq ($(strip $(CondFormats/BTauObjects)),)
ALL_COMMONRULES += src_CondFormats_BTauObjects_src
src_CondFormats_BTauObjects_src_parent := CondFormats/BTauObjects
src_CondFormats_BTauObjects_src_INIT_FUNC := $$(eval $$(call CommonProductRules,src_CondFormats_BTauObjects_src,src/CondFormats/BTauObjects/src,LIBRARY))
CondFormatsBTauObjects := self/CondFormats/BTauObjects
CondFormats/BTauObjects := CondFormatsBTauObjects
CondFormatsBTauObjects_files := $(patsubst src/CondFormats/BTauObjects/src/%,%,$(wildcard $(foreach dir,src/CondFormats/BTauObjects/src ,$(foreach ext,$(SRC_FILES_SUFFIXES),$(dir)/*.$(ext)))))
CondFormatsBTauObjects_BuildFile    := $(WORKINGDIR)/cache/bf/src/CondFormats/BTauObjects/BuildFile
CondFormatsBTauObjects_LOC_USE := self  CondFormats/Common CondFormats/Serialization FWCore/MessageLogger FWCore/Utilities boost CondFormats/PhysicsToolsObjects
CondFormatsBTauObjects_LCGDICTS  := x 
CondFormatsBTauObjects_PRE_INIT_FUNC += $$(eval $$(call LCGDict,CondFormatsBTauObjects,src/CondFormats/BTauObjects/src/classes.h,src/CondFormats/BTauObjects/src/classes_def.xml,$(SCRAMSTORENAME_LIB), --fail_on_warnings,))
CondFormatsBTauObjects_PRE_INIT_FUNC += $$(eval $$(call CondSerialization,CondFormatsBTauObjects,src/CondFormats/BTauObjects/src,src/CondFormats/BTauObjects/src/headers.h))
CondFormatsBTauObjects_EX_LIB   := CondFormatsBTauObjects
CondFormatsBTauObjects_EX_USE   := $(foreach d,$(CondFormatsBTauObjects_LOC_USE),$(if $($(d)_EX_FLAGS_NO_RECURSIVE_EXPORT),,$d))
CondFormatsBTauObjects_PACKAGE := self/src/CondFormats/BTauObjects/src
ALL_PRODS += CondFormatsBTauObjects
CondFormatsBTauObjects_CLASS := LIBRARY
CondFormats/BTauObjects_forbigobj+=CondFormatsBTauObjects
CondFormatsBTauObjects_INIT_FUNC        += $$(eval $$(call Library,CondFormatsBTauObjects,src/CondFormats/BTauObjects/src,src_CondFormats_BTauObjects_src,$(SCRAMSTORENAME_BIN),,$(SCRAMSTORENAME_LIB),$(SCRAMSTORENAME_LOGS)))
endif
