.SUFFIXES: .cc .dep .hh .ll .o .so .yy

$(outdir)/%.o: %.cc
	$(call ly_progress,Making,$@,< cc)
	$(DO_O_DEP) $(CXX) -c $(ALL_CXXFLAGS) -o $@ $<

$(outdir)/%.o: $(outdir)/%.cc
	$(call ly_progress,Making,$@,< cc)
	$(DO_O_DEP) $(CXX) -c $(ALL_CXXFLAGS) -o $@ $<

$(outdir)/%.lo: %.cc
	$(call ly_progress,Making,$@,< cc)
	$(DO_LO_DEP) $(CXX) -c $(ALL_CXXFLAGS) $(PIC_FLAGS) -o $@ $<

$(outdir)/%.lo: $(outdir)/%.cc
	$(call ly_progress,Making,$@,< cc)
	$(DO_LO_DEP) $(CXX) -c $(ALL_CXXFLAGS) $(PIC_FLAGS) -o $@ $<

$(outdir)/%.cc $(outdir)/%.hh: %.yy
	$(call ly_progress,Making,$@,< yy)
	$(BISON) -d -o $(outdir)/$*.cc $<

$(outdir)/%.cc: %.ll
	$(call ly_progress,Making,$@,< ll)
	$(FLEX) -Cfe -p -p -o$@ $<

$(outdir)/%.rc.o: $(outdir)/%.rc
	$(call ly_progress,Making,$@,< rc)
	$(WINDRES) $(WINDRES_FLAGS) -o$@ $<
