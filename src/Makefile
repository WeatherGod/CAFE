SHELL = /bin/sh

##############################################
##### Prevents any built-in suffix rules from
##### accidentally interfering
.SUFFIXES:
##############################################

# Contains some useful information regarding
# third-party packages
include ../include.mk

INCLUDEDIR = ../include
BINDIR = ../bin
LIBDIR = ../lib

BGRIB_INC = ../../BGrib-0.1

CXXFLAGS = -I $(INCLUDEDIR) $(BUTILS_INC) $(MYSQLPP_INC) $(PROJECTION_INC) -I $(BGRIB_INC)
LDFLAGS = -L $(LIBDIR) $(BUTILS_LIB) $(MYSQLPP_LIB) $(PROJECTION_LIB) \
          -lCAFE -lBUtils -lmysqlpp -lProjection
DEFFLAGS = -D _DO_LINKING_

CAFE_SOURCES = AnomalyCollection.C \
               CalcCorrelation.C \
               ClusterWork.C \
               DatabaseMaker.C \
               FindGammaMaxChi.C \
               GetThresholds.C \
               Hindcast.C \
               ResetTraining.C \
               SpecialHindcast.C \
               TableMaker.C \
               TotalRecall.C
CAFE_BINS = $(CAFE_SOURCES:.C=)
CAFE_DEPENDS = $(CAFE_SOURCES:.C=.d)

UTLY_SOURCES = DrawStdAnomHist.C \
               EventCount.C \
               FilterEvents.C \
               FindPercentPass.C \
               LonLatAnomExtract.C \
               MakeFile_HtmlView.C \
               MakeFile_RandomNonEventDates.C \
               DrawFingerprintMaps.C \
               MakeScript_ObjectiveAnalysis.C \
               MakeTruthTables.C \
               PrepareCrossFold.C \
               QuickCompare.C \
	       CAFEStateTest.C \
	       DisplayConfig.C
UTLY_BINS = $(UTLY_SOURCES:.C=)
UTLY_DEPENDS = $(UTLY_SOURCES:.C=.d)

PEAKVAL_SOURCES = PeakValleyPicker.C
PEAKVAL_BINS = PeakValleyPicker
PEAKVAL_DEPENDS = PeakValleyPicker.d

UTIL_OBJECTS := $(addprefix Utils/, CAFEEqtns.o \
				    CAFEUtly.o \
				    CAFE_CmdLine.o \
				    CAFE_SQLStructs.o \
				    CAFE_SQLUtly.o \
				    Cluster_CmdLine.o \
				    SamplingUtly.o \
				    PeakValleyFile.o \
				    ClusterFile.o \
				    EventScoreFile.o \
				    FieldMeasFile.o \
				    LonLatAnom.o \
				    FieldMeasure.o)
UTIL_DEPENDS := $(UTIL_OBJECTS:.o=.d)
SPA_OBJECTS := $(addprefix SPAnalysis/, BoardConvertor.o \
					BoardType.o \
					ClusterBoard.o \
					ClusterConfig.o \
					StrongPointAnalysis.o)
SPA_DEPENDS := $(SPA_OBJECTS:.o=.d)
CONF_OBJECTS := $(addprefix Config/, CAFEDomain.o \
				     CAFEVar.o \
				     Configuration.o \
				     DataSource.o \
				     DataVar.o \
				     EventType.o \
				     Variable.o \
				     CAFEState.o \
				     CAFEParam.o)
CONF_DEPENDS := $(CONF_OBJECTS:.o=.d)


all : libCAFE.so CAFE Utly PeakVal
.PHONY : all clean clean-depends CAFE Utly PeakVal \
         $(CAFE_BINS) $(UTLY_BINS) $(PEAKVAL_BINS) \
         libCAFE.so

#######################################
CAFE : libCAFE.so $(CAFE_BINS)

Utly : libCAFE.so $(UTLY_BINS)

PeakVal : $(PEAKVAL_BINS)
#######################################



###################################################################################
# Pattern rules for generating a depenency file for each source file
# From http://www.gnu.org/software/make/manual/html_node/Automatic-Prerequisites.html
# This one was modified slightly for use with C++.
# An output file is also explicity given (-MF).
%.d : %.C
	@ echo "Making dependency file for target '$(DEP_TARGET)'"
	@ $(CXX) -MM $(CXXFLAGS) -MT $(DEP_TARGET) -MT $@ $< -MF $@

$(UTIL_DEPENDS) $(SPA_DEPENDS) $(CONF_DEPENDS) : DEP_TARGET = $*.o
$(CAFE_DEPENDS) $(UTLY_DEPENDS) $(PEAKVAL_DEPENDS) : DEP_TARGET = $(BINDIR)/$*
###########################################################################


#############################################################
#        --- Shared Library file ---
libCAFE.so : $(LIBDIR)/libCAFE.so

$(LIBDIR)/libCAFE.so : $(CONF_OBJECTS) $(SPA_OBJECTS) $(UTIL_OBJECTS)
	$(CXX) $(LDFLAGS) -shared $^ -o $@

%.o : %.C
	$(CXX) -fPIC -c $< -o $@ $(DEFFLAGS) $(CXXFLAGS)
##############################################################

include $(CAFE_DEPENDS)
include $(UTLY_DEPENDS)
include $(PEAKVAL_DEPENDS)
include $(UTIL_DEPENDS)
include $(SPA_DEPENDS)
include $(CONF_DEPENDS)

################################################################
# Pattern rules for compiling an executable from a .C file
$(CAFE_BINS) $(UTLY_BINS) $(PEAKVAL_BINS) : % : $(BINDIR)/%

$(addprefix $(BINDIR)/, $(CAFE_BINS) $(UTLY_BINS) $(PEAKVAL_BINS)) : $(BINDIR)/% : %.C $(LIBDIR)/libCAFE.so
	$(CXX) $< -o $@ $(DEFFLAGS) $(CXXFLAGS) $(LDFLAGS)
#################################################################

### Additional information given for specific executables ###
CalcCorrelation : CXXFLAGS += -O3
ClusterWork : CXXFLAGS += -O3
FindGammaMaxChi : CXXFLAGS += -O3
PeakValleyPicker : CXXFLAGS += -O3
#############################################################

### Additional information given for specific objects ###
$(SPA_OBJECTS) : CXXFLAGS += -O3
$(CONF_OBJECTS) : CXXFLAGS += -O2
$(UTIL_OBJECTS) : CXXFLAGS += -O2
#########################################################

clean :
	-rm -f $(LIBDIR)/libCAFE.so
	-rm -f $(addprefix $(BINDIR)/, $(CAFE_BINS) $(UTLY_BINS) $(PEAKVAL_BINS))
	-rm -f $(UTIL_OBJECTS)
	-rm -f $(SPA_OBJECTS)
	-rm -f $(CONF_OBJECTS)

clean-depends :
	-rm -f $(CAFE_DEPENDS)
	-rm -f $(UTLY_DEPENDS)
	-rm -f $(PEAKVAL_DEPENDS)
	-rm -f $(UTIL_DEPENDS)
	-rm -f $(SPA_DEPENDS)
	-rm -f $(CONF_DEPENDS)

