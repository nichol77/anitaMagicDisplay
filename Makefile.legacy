#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
include Makefile.arch


### Package subdirectories
LIBDIR=lib
BUILDDIR=build
INCLUDEDIR=include
BINDIR=bin

#Site Specific  Flags
SYSINCLUDES	=
SYSLIBS         = 
DLLSUF = ${DllSuf}
OBJSUF = ${ObjSuf}
SRCSUF = ${SrcSuf}

ifdef ANITA_UTIL_INSTALL_DIR
ANITA_UTIL_LIB_DIR=${ANITA_UTIL_INSTALL_DIR}/lib
ANITA_UTIL_INC_DIR=${ANITA_UTIL_INSTALL_DIR}/include
LD_ANITA_UTIL=-L$(ANITA_UTIL_LIB_DIR)
INC_ANITA_UTIL=-I$(ANITA_UTIL_INC_DIR)
ANITA_UTIL_MAP_DIR=$(ANITA_UTIL_INSTALL_DIR)/share/anitaMap
else
ANITA_UTIL_LIB_DIR=/usr/local/lib
ANITA_UTIL_INC_DIR=/usr/local/include
ANITA_UTIL_MAP_DIR=/usr/local/share/anitaMap
ifdef EVENT_READER_DIR
LD_ANITA_UTIL=-L$(EVENT_READER_DIR)
INC_ANITA_UTIL=-I$(EVENT_READER_DIR)
endif
endif

#Toggles the FFT functions on and off
USE_FFT_TOOLS=1

ifdef USE_FFT_TOOLS
FFTLIBS = -lRootFftwWrapper -lfftw3
FFTFLAG = -DUSE_FFT_TOOLS
else
FFTLIBS =
FFTFLAG =
endif

#Generic and Site Specific Flags
CXXFLAGS     += $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) $(INC_ANITA_UTIL) #-DANITA2
LDFLAGS      += -g $(ROOTLDFLAGS) 

LIBS          = $(ROOTLIBS) -lMathMore -lMinuit -lGeom -lGraf3d $(SYSLIBS) $(LD_ANITA_UTIL) $(FFTLIBS) -lGui
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

#Now the bits we're actually compiling
ROOT_LIBRARY = ${LIBDIR}/libMagicDisplay.${DLLSUF}
DICT  = magicDict
LIB_OBJS =  $(addprefix $(BUILDDIR)/,  AnitaCanvasMaker.o WaveformGraph.o MagicDisplay.o MagicDisplayConventions.o AnitaRFCanvasMaker.o MagicControlPanel.o FFTGraph.o CorrelationFactory.o  AnitaGpsCanvasMaker.o $(DICT).o )
CLASS_HEADERS = $(addprefix $(INCLUDEDIR)/, AnitaCanvasMaker.h AnitaRFCanvasMaker.h WaveformGraph.h MagicDisplay.h MagicDisplayConventions.h MagicControlPanel.h FFTGraph.h CorrelationFactory.h AnitaGpsCanvasMaker.h )


all : $(ROOT_LIBRARY)


$(LIB_OBJS): | $(BUILDDIR) 

$(BINDIR): 
	mkdir -p $(BINDIR)

$(BUILDDIR): 
	mkdir -p $(BUILDDIR)

$(LIBDIR): 
	mkdir -p $(LIBDIR) | $(LIBDIR)

#The library
$(ROOT_LIBRARY) : $(LIB_OBJS)
	@echo "Linking $@ ..."
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@ $(LDFLAGS) $(LIBS) $^ $(OutPutOpt) $@
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
else
		$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $^ \
		   $(OutPutOpt) $(subst .$(DllSuf),.so,$@)
endif
endif
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(LIBS) $(LIB_OBJS) -o $@
endif
	@if [ $(shell root-config --version | cut -c1) -ge 6 ]; then \
	cp $(BUILDDIR)/*.pcm $(LIBDIR); \
	fi; # Additional install command for ROOTv6


$(BUILDDIR)/%.$(OBJSUF) : src/%.$(SRCSUF) $(CLASS_HEADERS) Makefile | $(BUILDDIR) 
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILDDIR)/%.$(OBJSUF) : $(BUILDDIR)/%.C
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) $ -c $< -o  $@


#eventDict.C: $(CLASS_HEADERS)
$(BUILDDIR)/$(DICT).C: $(CLASS_HEADERS)
	@echo "Generating dictionary ..."
	@ rm -f *Dict* 
	rootcint $@ -c -p $(CINTFLAGS) -I./ $(INC_ANITA_UTIL) $(CLASS_HEADERS) LinkDef.h

install: $(ROOT_LIBRARY)
	install -d $(ANITA_UTIL_LIB_DIR)
	install -d $(ANITA_UTIL_INC_DIR)
ifeq ($(PLATFORM),macosx)
	install -c -m 755 $(ROOT_LIBRARY) $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY)) $(ANITA_UTIL_LIB_DIR)
else
	install -c -m 755 $(ROOT_LIBRARY) $(ANITA_UTIL_LIB_DIR)
endif
	install -c -m 644  $(CLASS_HEADERS) $(ANITA_UTIL_INC_DIR)
	install -d $(ANITA_UTIL_MAP_DIR)

	@if [ $(shell root-config --version | cut -c1) -ge 6 ]; then \
	install -c -m 755 $(BUILDDIR)/$(DICT)_rdict.pcm $(ANITA_UTIL_LIB_DIR) ;\
	fi # Additional install command for ROOTv6

	install -c -m 644 antarcticaIceMap.png $(ANITA_UTIL_MAP_DIR)
	install -c -m 644 anitageom.root $(ANITA_UTIL_MAP_DIR)

clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(LIBRARY)
	@rm -f $(ROOT_LIBRARY)
	@rm -f $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY))	
	@rm -f $(TEST)
#############################################################################



